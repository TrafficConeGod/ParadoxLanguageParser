#include "Parser.h"
#include <sstream>
#include <iostream>
// #include <regex>

std::string ParadoxLanguage::StringToCode(std::string string) {
    // std::regex spaceRegex("(\\s+)");
    bool hasSpace = false;
    for (auto ch : string) {
        if (ch == ' ') {
            hasSpace = true;
            break;
        }
    }
    // if (std::regex_match(string, spaceRegex)) {
    if (hasSpace) {
        std::stringstream stream;
        stream << "\"";
        stream << string;
        stream << "\"";
        return stream.str();
    } else {
        return string;
    }
}

using namespace ParadoxLanguage;

std::variant<std::string, Object, Array> Value::Variant(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    enum class Stage {
        None,
        Compound,
        CompoundLiteral
    };
    Stage stage = Stage::None;
    auto compoundFirstTokenIterator = it;

    // std::cout << it->String() << "\n";
    
    for (; it != tokens.end(); it++) {
        auto type = it->TokenType();
        switch (stage) {
            case Stage::None: {
                switch (type) {
                    case Token::Type::Literal: {
                        return std::string(std::string(it->Literal()));
                    } break;
                    case Token::Type::OpenBracket: {
                        stage = Stage::Compound;
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected literal or \"{\" for value");
                    } break;
                }
            } break;
            case Stage::Compound: {
                switch (type) {
                    case Token::Type::OpenBracket: {
                        return Array(tokens, it);
                    } break;
                    case Token::Type::CloseBracket: {
                        return Object(tokens, it);
                    } break;
                    case Token::Type::Literal: {
                        stage = Stage::CompoundLiteral;
                        compoundFirstTokenIterator = it;
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected literal, \"{\", or \"}\" after \"{\"");
                    } break;
                }
            } break;
            case Stage::CompoundLiteral: {
                switch (type) {
                    case Token::Type::Assignment: {
                        it = compoundFirstTokenIterator;
                        return Object(tokens, it);
                    } break;
                    case Token::Type::OpenBracket:
                    case Token::Type::CloseBracket:
                    case Token::Type::Literal: {
                        it = compoundFirstTokenIterator;
                        return Array(tokens, it);
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected \"=\", literal, \"{\", or \"}\" after literal");
                    } break;
                }
            } break;
            default: {
                throw InvalidTokenException(it, "Unknown value error");
            } break;
        }
    }
    throw InvalidTokenException(it, "Unknown value error");
}

Value::Value(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin) : variant{Variant(tokens, begin)} {}

std::string Value::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    if (CanCast<std::string>()) {
        stream << StringToCode(Cast<std::string>());
    } else if (CanCast<Object>()) {
        stream << "{\n" << Cast<Object>().Code(std::string(frontAppend.size() + 1, '\t')) << frontAppend << "}";
    } else if (CanCast<Array>()) {
        stream << "{ " << Cast<Array>().Code(frontAppend) << "}";
    } else {
        stream << "UNDEFINED";
    }
    return stream.str();
}

Object::Object() {}

Object::Object(std::string code) {
    // split into tokens
    std::vector<Token> tokens;
    std::stringstream stream;

    Token::Position tokenPosition;
    bool isInQuote = false;
    bool isInComment = false;
    bool wasKeyCharacter = false;
    code += "\n";
    for (char ch : code) {
        if (ch == '#') {
            isInComment = true;
        }
        if (isInComment) {
            if (ch == '\n') {
                isInComment = false;
            }
        } else {
            if (wasKeyCharacter) {
                wasKeyCharacter = false;
                tokens.push_back(Token(stream.str(), tokenPosition));
                stream.str(std::string());
            }
            
            if (isInQuote) {
                if (ch == '"') {
                    isInQuote = false;
                    std::string literal = stream.str();
                    if (literal.size() > 0) {
                        tokens.push_back(Token(stream.str(), tokenPosition));
                        stream.str(std::string());
                    }
                } else {
                    stream << ch;
                }
            } else {
                switch (ch) {
                    case '=':
                    case '{':
                    case '}':
                    case '\n':
                    case 0xD:
                    case ' ':
                    case '\t': {
                        if (!isInQuote) {
                            std::string literal = stream.str();
                            if (literal.size() > 0) {
                                tokens.push_back(Token(stream.str(), tokenPosition));
                                stream.str(std::string());
                            }
                        }
                    } break;
                    case '"': {
                        isInQuote = true;
                    } break;
                    default: {
                        stream << ch;
                    } break;
                }
                switch (ch) {
                    case '=':
                    case '{':
                    case '}': {
                        wasKeyCharacter = true;
                        stream << ch;
                    } break;
                    default: {

                    } break;
                }
            }
        }
        if (ch == '\n') {
            tokenPosition.column = 1;
            tokenPosition.row++;
        } else {
            tokenPosition.column++;
        }
    }

    // for (const auto& token : tokens) {
    //     std::cout << token.String() << "\n";
    // }
    // std::cout << tokens.size() << "\n";

    std::vector<Token>::const_iterator it = tokens.begin();
    Parse(tokens, it);
}

Object::Object(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin) {
    Parse(tokens, begin);
}

void Object::Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    enum class Stage {
        None,
        Identifier,
        Assignment,
        Value
    };
    std::string_view key;
    Stage stage = Stage::None;
    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        // std::cout << "object: " << it->String() << ", stage: " << (int)stage << "\n";
        switch (stage) {
            case Stage::None: {
                if (it->TokenType() == Token::Type::Literal) {
                    stage = Stage::Identifier;
                    key = it->Literal();
                } else {
                    throw InvalidTokenException(it, "Expected literal");
                }
            } break;
            case Stage::Identifier: {
                if (it->TokenType() == Token::Type::Assignment) {
                    stage = Stage::Assignment;
                } else {
                    throw InvalidTokenException(it, "Expected \"=\"");
                }
            } break;
            case Stage::Assignment: {
                stage = Stage::None;
                AllAt(std::string(key)).push_back(Value(tokens, it));
            } break;
            default: {
                throw InvalidTokenException(it, "Unknown object error");
            } break;
        }
    }
}

std::vector<std::string> Object::Keys() const {
    std::vector<std::string> keys;
    for (auto it = map.begin(); it != map.end(); it++) {
        keys.push_back(it->first);
    }
    return keys;
}

std::vector<Value>& Object::AllAt(std::string key) {
    if (!map.count(key)) {
        map[key] = std::vector<Value>();
    }
    return map[key];
}

const std::vector<Value>& Object::AllAt(std::string key) const {
    return map.at(key);
}

Value& Object::At(std::string key) {
    return map.at(key).at(0);
}

const Value& Object::At(std::string key) const {
    return map.at(key).at(0);
}

std::string Object::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& key : Keys()) {
        for (auto& value : AllAt(key)) {
            stream << frontAppend << key << " = ";
            stream << value.Code(frontAppend);
            stream << "\n";
        }
    }
    return stream.str();
}

Array::Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        // std::cout << "array: " << it->String() << ", stage: " << (int)stage << "\n";
        Values().push_back(Value(tokens, it));
    }
}

std::vector<Value>& Array::Values() {
    return vector;
}
            
const std::vector<Value>& Array::Values() const {
    return vector;
}
            
std::string Array::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& value : Values()) {
        stream << value.Code(frontAppend);
        stream << " ";
    }
    return stream.str();
}

std::string InvalidTokenException::Message(std::string literal, const Token::Position& position, std::string_view reason) {
    std::stringstream stream;
    stream << "Invalid token \"" << literal << "\" at row: " << position.row << ", column: " << position.column << "\nReason: " << reason << "\n";
    return stream.str();
}


InvalidTokenException::InvalidTokenException(std::vector<Token>::const_iterator iterator, std::string_view reason) : message{Message(std::string(iterator->Literal()), iterator->TokenPosition(), reason)} {}
InvalidTokenException::~InvalidTokenException() throw() {}

const char* InvalidTokenException::what() const throw() {
    return message.c_str();
}
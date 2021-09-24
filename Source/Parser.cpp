#include "Parser.h"
#include <sstream>
#include <iostream>
// #include <regex>

using namespace ParadoxLanguage;

std::string ParadoxLanguage::StringToStringLiteral(std::string string) {
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

std::string ParadoxLanguage::ValueToString(const std::any& value, std::string_view frontAppend) {
    std::stringstream stream;
    auto& type = value.type();
    if (type == typeid(std::string)) {
        stream << StringToStringLiteral(std::any_cast<const std::string&>(value));
    } else if (type == typeid(Object)) {
        stream << "{\n" << std::any_cast<const Object&>(value).Code(std::string(frontAppend.size() + 1, '\t')) << frontAppend << "}";
    } else if (type == typeid(Array)) {
        stream << "{ " << std::any_cast<const Array&>(value).Code(frontAppend) << "}";
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
                tokens.push_back(Token(stream.str()));
                stream.str(std::string());
            }
            
            if (isInQuote) {
                if (ch == '"') {
                    isInQuote = false;
                    std::string literal = stream.str();
                    if (literal.size() > 0) {
                        tokens.push_back(Token(stream.str()));
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
                                tokens.push_back(Token(stream.str()));
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
    }

    // for (const auto& token : tokens) {
    //     std::cout << token.Literal() << " " << (int)token.TokenType() << "\n";
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
        Value,
        Compound,
        CompoundLiteral
    };
    std::string_view key;
    auto compoundFirstTokenIterator = it;
    Stage stage = Stage::None;
    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        switch (stage) {
            case Stage::None: {
                if (it->TokenType() == Token::Type::Literal) {
                    stage = Stage::Identifier;
                    key = it->Literal();
                }
            } break;
            case Stage::Identifier: {
                if (it->TokenType() == Token::Type::Assignment) {
                    stage = Stage::Assignment;
                }
            } break;
            case Stage::Assignment: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::Literal: {
                        stage = Stage::None;
                        AllAt(std::string(key)).push_back(std::string(it->Literal()));
                    } break;
                    case Token::Type::OpenBracket: {
                        stage = Stage::Compound;
                        auto nextIterator = it;
                        nextIterator++;
                        if (nextIterator == tokens.end() || nextIterator->TokenType() == Token::Type::CloseBracket) {
                            stage = Stage::None;
                            AllAt(std::string(key)).push_back(Object(tokens, it));
                        }
                    } break;
                    default: {
                        throw InvalidTokenException(it, "object");
                    } break;
                }
            } break;
            case Stage::Compound: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::Literal: {
                        stage = Stage::CompoundLiteral;
                        compoundFirstTokenIterator = it;

                        auto nextIterator = it;
                        nextIterator++;
                        if (nextIterator == tokens.end() || nextIterator->TokenType() == Token::Type::CloseBracket) {
                            stage = Stage::None;
                            AllAt(std::string(key)).push_back(Array(tokens, it));
                        }
                    } break;
                    default: {
                        throw InvalidTokenException(it, "object");
                    } break;
                }
            } break;
            case Stage::CompoundLiteral: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::CloseBracket:
                    case Token::Type::Literal: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        AllAt(std::string(key)).push_back(Array(tokens, it));
                    } break;
                    case Token::Type::Assignment: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        AllAt(std::string(key)).push_back(Object(tokens, it));
                    } break;
                    default: {
                        throw InvalidTokenException(it, "object");
                    } break;
                }
            } break;
            default: {
                throw InvalidTokenException(it, "object");
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

std::vector<std::any>& Object::AllAt(std::string key) {
    if (!map.count(key)) {
        map[key] = std::vector<std::any>();
    }
    return map[key];
}

const std::vector<std::any>& Object::AllAt(std::string key) const {
    return map.at(key);
}

std::any& Object::At(std::string key) {
    return map.at(key).at(0);
}

const std::any& Object::At(std::string key) const {
    return map.at(key).at(0);
}

std::string Object::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& key : Keys()) {
        for (auto& value : AllAt(key)) {
            stream << frontAppend << key << " = ";
            stream << ValueToString(value, frontAppend);
            stream << "\n";
        }
    }
    return stream.str();
}

Array::Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    enum class Stage {
        None,
        Compound,
        CompoundLiteral
    };
    auto compoundFirstTokenIterator = it;
    Stage stage = Stage::None;

    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        switch (stage) {
            case Stage::None: {
                switch (it->TokenType()) {
                    case Token::Type::Literal: {
                        Values().push_back(std::string(it->Literal()));
                    } break;
                    case Token::Type::OpenBracket: {
                        stage = Stage::Compound;
                    } break;
                    default: {
                        throw InvalidTokenException(it, "array");
                    } break;
                }
            } break;
            case Stage::Compound: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::Literal: {
                        stage = Stage::CompoundLiteral;
                        compoundFirstTokenIterator = it;

                        auto nextIterator = it;
                        nextIterator++;
                        if (nextIterator == tokens.end() || nextIterator->TokenType() == Token::Type::CloseBracket) {
                            stage = Stage::None;
                            Values().push_back(Array(tokens, it));
                        }
                    } break;
                    default: {
                        throw InvalidTokenException(it, "array");
                    } break;
                }
            } break;
            case Stage::CompoundLiteral: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::CloseBracket:
                    case Token::Type::Literal: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        Values().push_back(Array(tokens, it));
                    } break;
                    case Token::Type::Assignment: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        Values().push_back(Object(tokens, it));
                    } break;
                    default: {
                        throw InvalidTokenException(it, "array");
                    } break;
                }
            } break;
            default: {
                throw InvalidTokenException(it, "array");
            } break;
        }
    }
}

std::vector<std::any>& Array::Values() {
    return vector;
}
            
const std::vector<std::any>& Array::Values() const {
    return vector;
}
            
std::string Array::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& value : Values()) {
        stream << ValueToString(value, frontAppend);
        stream << " ";
    }
    return stream.str();
}

InvalidTokenException::InvalidTokenException(const Token& token, std::string_view area, std::string_view reason) {
    std::stringstream stream;
    stream << "Invalid token \"" << token.Literal() << "\" in " << area << " for " << reason << "\n";
    message = stream.str();
}
InvalidTokenException::InvalidTokenException(std::vector<Token>::const_iterator iterator, std::string_view area, std::string_view reason) {
    std::stringstream stream;
    stream << "Invalid token \"" << iterator->Literal() << "\" in " << area << " for " << reason << "\n";
    message = stream.str();
}
InvalidTokenException::~InvalidTokenException() throw() {}

const char* InvalidTokenException::what() const throw() {
    return message.c_str();
}
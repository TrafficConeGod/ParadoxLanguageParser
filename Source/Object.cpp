#include "Object.h"
#include "InvalidTokenException.h"
#include "Array.h"
#include "Value.h"
#include <sstream>

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

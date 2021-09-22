#include "Parser.h"
#include <sstream>
#include <iostream>

using namespace ParadoxLanguage;

Object::Object(std::string code) {
    // split into tokens
    std::vector<Token> tokens;
    std::stringstream stream;

    bool isInQuote = false;
    bool wasKeyCharacter = false;
    code += "\n";
    for (char ch : code) {
        if (wasKeyCharacter) {
            wasKeyCharacter = false;
            std::string literal = stream.str();
            tokens.push_back(Token(stream.str()));
            stream.str("");
        }
        
        if (isInQuote) {
            if (ch == '"') {
                isInQuote = false;
                std::string literal = stream.str();
                if (literal.size() > 0) {
                    tokens.push_back(Token(stream.str()));
                    stream.str("");
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
                case ' ':
                case '\t': {
                    if (!isInQuote) {
                        std::string literal = stream.str();
                        if (literal.size() > 0) {
                            tokens.push_back(Token(stream.str()));
                            stream.str("");
                        }
                        break;
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
            }
        }
    }

    std::vector<Token>::const_iterator it = tokens.begin();
    Parse(tokens, it);
}

Object::Object(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin) {
    Parse(tokens, begin);
}

void Object::Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    data = std::shared_ptr<ObjectData>(new ObjectData());
    
    enum class Stage {
        Invalid,
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
                    } break;
                    default: {
                        stage = Stage::Invalid;
                    } break;
                }
            } break;
            case Stage::Compound: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::Literal: {
                        stage = Stage::CompoundLiteral;
                        compoundFirstTokenIterator = it;
                    } break;
                    case Token::Type::CloseBracket: {
                        stage = Stage::None;
                        // AllAt(std::string(key)).push_back(Object());
                    } break;
                    default: {
                        stage = Stage::Invalid;
                    } break;
                }
            } break;
            case Stage::CompoundLiteral: {
                auto type = it->TokenType();
                switch (type) {
                    case Token::Type::CloseBracket:
                    case Token::Type::Literal: {
                        stage = Stage::None;
                        // std::cout << it->Literal() << "\n";
                        // AllAt(std::string(key)).push_back(Array(tokens, it));
                        // std::cout << it->Literal() << "\n";
                    } break;
                    case Token::Type::Assignment: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        std::cout << it->Literal() << "\n";
                        AllAt(std::string(key)).push_back(Object(tokens, it));
                        std::cout << it->Literal() << "\n";
                    } break;
                    default: {
                        stage = Stage::Invalid;
                    } break;
                }
            } break;
            default: {
                stage = Stage::Invalid;
            } break;
        }
        if (stage == Stage::Invalid) {
            std::cout << "Unexpected token: " << it->Literal() << "\n";
        }
    }
}

std::vector<std::string> Object::Keys() const {
    std::vector<std::string> keys;
    for (auto it = (*data).map.begin(); it != (*data).map.end(); it++) {
        keys.push_back(it->first);
    }
    return keys;
}

std::vector<std::any>& Object::AllAt(std::string key) {
    if (!(*data).map.count(key)) {
        (*data).map[key] = std::vector<std::any>();
    }
    return (*data).map[key];
}

const std::vector<std::any>& Object::AllAt(std::string key) const {
    return (*data).map.at(key);
}

std::any& Object::At(std::string key) {
    return (*data).map.at(key).at(0);
}

const std::any& Object::At(std::string key) const {
    return (*data).map.at(key).at(0);
}
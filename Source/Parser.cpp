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

void Object::Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin) {
    data = std::shared_ptr<ObjectData>(new ObjectData());
    
    enum class Stage {
        None,
        Identifier,
        Assignment,
        Value,
        Compound
    };
    std::string_view key;
    Stage stage = Stage::None;
    for (auto it = begin; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
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
                    default: {

                    } break;
                }
            } break;
            default: {

            } break;
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
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
    data = std::shared_ptr<Data>(new Data());
    
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
                        std::cout << it->Literal() << "\n";
                        AllAt(std::string(key)).push_back(Array(tokens, it));
                        std::cout << it->Literal() << "\n";
                    } break;
                    case Token::Type::Assignment: {
                        stage = Stage::None;
                        it = compoundFirstTokenIterator;
                        AllAt(std::string(key)).push_back(Object(tokens, it));
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

std::string Object::Code() const {
    return GenerateCode("");
}

std::string Object::GenerateCode(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& key : Keys()) {
        for (auto& value : AllAt(key)) {
            stream << frontAppend << key << " = ";
            auto& type = value.type();
            if (type == typeid(std::string)) {
                stream << std::any_cast<std::string>(value);
            } else if (type == typeid(Object)) {
                stream << "{\n" << std::any_cast<Object>(value).GenerateCode(std::string(frontAppend.size() + 1, '\t')) << "}";
            } else if (type == typeid(Array)) {
                stream << "{ " << std::any_cast<Array>(value).Code() << "}";
            } else {
                stream << "UNDEFINED";
            }
            stream << "\n";
        }
    }
    return stream.str();
}

Array::Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    data = std::shared_ptr<Data>(new Data());

    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        switch (it->TokenType()) {
            case Token::Type::Literal: {
                Values().push_back(std::string(it->Literal()));
            } break;
            case Token::Type::CloseBracket: {
                return;
            } break;
            default: {
                std::cout << "Invalid token in array: " << it->Literal() << "\n";
            } break;
        }
    }
}

std::vector<std::any>& Array::Values() {
    return (*data).array;
}
            
const std::vector<std::any>& Array::Values() const {
    return (*data).array;
}
            
std::string Array::Code() const {
    std::stringstream stream;
    for (auto& value : Values()) {
        stream << std::any_cast<std::string>(value) << " ";
    }
    return stream.str();
}
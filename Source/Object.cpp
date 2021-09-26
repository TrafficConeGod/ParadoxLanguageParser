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
    std::stringstream tokenBuilder;

    Token::Position tokenPosition;
    bool inComment = false;


    code += "\n";

    auto Tokenize = [tokenPosition](std::vector<Token>& tokens, std::stringstream& tokenBuilder) {
        std::string literal = tokenBuilder.str();
        if (literal.size() > 0) {
            tokens.push_back(Token(std::string(literal), tokenPosition));
            tokenBuilder.str(std::string());
        }
    };

    enum class Mode {
        Literal,
        Symbol,
        String
    };

    Mode mode = Mode::Literal;

    for (char ch : code) {
        if (ch == '#') {
            inComment = true;
        }
        if (inComment) {
            if (ch == '\n') {
                inComment = false;
            }
        } else {
            
            switch (mode) {
                case Mode::Literal: {
                    switch (ch) {
                        case '"': {
                            mode = Mode::String;
                            Tokenize(tokens, tokenBuilder);
                        } break;
                        case 0xD: // carriage return moment
                        case ' ':
                        case '\n':
                        case '\t': {
                            Tokenize(tokens, tokenBuilder);
                        } break;
                        case '=':
                        case '{':
                        case '}': {
                            mode = Mode::Symbol;
                            Tokenize(tokens, tokenBuilder);
                        } // flows down
                        default: {
                            tokenBuilder << ch;
                        } break;
                    }
                } break;
                case Mode::Symbol: {
                    switch (ch) {
                        case '"': {
                            mode = Mode::String;
                            Tokenize(tokens, tokenBuilder);
                        } break;
                        case 0xD: // carriage return 2
                        case ' ':
                        case '\n':
                        case '\t': {
                            mode = Mode::Literal;
                            Tokenize(tokens, tokenBuilder);
                        } break;
                        case '=':
                        case '{':
                        case '}': {
                            tokenBuilder << ch;
                        } // if this does flow down then you can't get multi-character symbol tokens (like ==, {{ etc), however in this case we don't want that so it does flow down
                        default: {
                            mode = Mode::Literal;
                            Tokenize(tokens, tokenBuilder);
                            tokenBuilder << ch;
                        } break;
                    }
                } break;
                case Mode::String: {
                    switch (ch) {
                        case '"': {
                            mode = Mode::Literal;
                            Tokenize(tokens, tokenBuilder);
                        } break;
                        default: {
                            tokenBuilder << ch;
                        } break;
                    }
                } break;
                default: break;
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

bool Object::HasAt(std::string key) const {
    return map.count(key);
}

std::vector<Value>& Object::AllAt(std::string key) {
    if (!HasAt(key)) {
        map[key] = std::vector<Value>();
    }
    return map[key];
}

const std::vector<Value>& Object::AllAt(std::string key) const {
    return map.at(key);
}

Value& Object::At(std::string key) {
    auto& values = AllAt(key);
    if (values.size() == 0) {
        values.push_back(std::string());
    }
    return values[0];
}

const Value& Object::At(std::string key) const {
    return AllAt(key).at(0);
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

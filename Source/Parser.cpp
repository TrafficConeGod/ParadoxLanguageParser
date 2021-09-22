#include "Parser.h"
#include "Token.h"
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
    for (const auto& token : tokens) {
        std::cout << token.Literal() << " " << (int)token.TokenType() << "\n";
    }
}
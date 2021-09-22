#include "Token.h"

using namespace ParadoxLanguage;

Token::Token(std::string_view vLiteral) : literal{std::string(vLiteral)} {
    switch (vLiteral.at(0)) {
        case '=': {
            type = Type::Assignment;
        } break;
        case '{': {
            type = Type::OpenBracket;
        } break;
        case '}': {
            type = Type::CloseBracket;
        } break;
        default: {
            type = Type::StringLiteral;
        } break;
    }
}

Token::Type Token::TokenType() const {
    return type;
}

std::string_view Token::Literal() const {
    return literal;
}
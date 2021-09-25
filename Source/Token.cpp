#include "Token.h"
#include <sstream>

using namespace ParadoxLanguage;

Token::Token(std::string_view vLiteral, Position vPosition) : literal{std::string(vLiteral)}, position{vPosition} {
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
            type = Type::Literal;
        } break;
    }
}

Token::Type Token::TokenType() const {
    return type;
}

const Token::Position& Token::TokenPosition() const {
    return position;
}

std::string_view Token::Literal() const {
    return literal;
}

std::string Token::String() const {
    std::stringstream stream;
    stream << "literal: " << Literal() << ", type: " << (int)TokenType() << ", row: " << TokenPosition().row << ", column: " << TokenPosition().column;
    return stream.str();
}
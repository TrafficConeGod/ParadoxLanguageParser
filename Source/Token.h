#pragma once
#include <string>

namespace ParadoxLanguage {
    class Token {
        public:
            enum class Type {
                Invalid,
                Literal,
                Assignment,
                OpenBracket,
                CloseBracket
            };

            struct Position {
                uint row = 1;
                uint column = 1;
            };
        private:
            Type type = Type::Invalid;
            std::string literal;
            Position position;
        public:
            Token(std::string_view literal, Position position);

            Type TokenType() const;
            const Position& TokenPosition() const;
            std::string_view Literal() const;
            std::string String() const;
    };
}
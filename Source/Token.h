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
        private:
            Type type = Type::Invalid;
            std::string literal;
        public:
            Token(std::string_view literal);

            Type TokenType() const;
            std::string_view Literal() const;
    };
}
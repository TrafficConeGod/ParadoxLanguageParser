#pragma once
#include <exception>
#include <vector>
#include "Token.h"

namespace ParadoxLanguage {

    class InvalidTokenException : public std::exception {
        private:
            std::string message;

            std::string Message(std::string literal, const Token::Position& position, std::string_view reason);
        public:
            // explicit InvalidTokenException(const Token& token, std::string_view reason = "no reason");
            explicit InvalidTokenException(std::vector<Token>::const_iterator iterator, std::string_view reason = "no reason");
            virtual ~InvalidTokenException() throw();

            const char* what() const throw();
    };

}
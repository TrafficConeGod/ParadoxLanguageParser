#pragma once
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <exception>
#include "Token.h"

namespace ParadoxLanguage {

    class Value;

    std::string StringToCode(std::string string);

    class Object {
        private:
            std::map<std::string, std::vector<Value>> map;

            void Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
        public:
            Object();
            Object(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
            Object(std::string code);

            std::vector<std::string> Keys() const;

            std::vector<Value>& AllAt(std::string key);
            const std::vector<Value>& AllAt(std::string key) const;

            Value& At(std::string key);
            const Value& At(std::string key) const;

            std::string Code(std::string_view frontAppend = "") const;
    };

    class Array {
        private:
            std::vector<Value> vector;
        public:
            Array();
            Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);

            std::vector<Value>& Values();
            const std::vector<Value>& Values() const;

            std::string Code(std::string_view frontAppend = "") const;
    };

    class Value {
        private:
            std::variant<std::string, Object, Array> variant;

            std::variant<std::string, Object, Array> Variant(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
        public:
            Value(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
            std::string Code(std::string_view frontAppend = "") const;

            template<typename T>
            bool CanCast() const { return std::holds_alternative<T>(variant); }
            template<typename T>
            const T& Cast() const { return std::get<T>(variant); }
            template<typename T>
            T& Cast() { return std::get<T>(variant); }
    };

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
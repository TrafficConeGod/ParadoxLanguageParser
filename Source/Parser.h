#pragma once
#include <string>
#include <map>
#include <vector>
#include <any>
#include <memory>
#include "Token.h"

namespace ParadoxLanguage {

    std::string StringToStringLiteral(std::string string);

    std::string ValueToString(const std::any& value, std::string_view frontAppend);

    class Object {
        private:
            std::map<std::string, std::vector<std::any>> map;

            void Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
        public:
            Object();
            Object(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
            Object(std::string code);

            std::vector<std::string> Keys() const;

            std::vector<std::any>& AllAt(std::string key);
            const std::vector<std::any>& AllAt(std::string key) const;

            std::any& At(std::string key);
            const std::any& At(std::string key) const;

            std::string Code(std::string_view frontAppend = "") const;
    };

    class Array {
        private:
            std::vector<std::any> vector;
        public:
            Array();
            Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);

            std::vector<std::any>& Values();
            const std::vector<std::any>& Values() const;

            std::string Code(std::string_view frontAppend = "") const;
    };

}
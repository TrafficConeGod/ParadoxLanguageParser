#pragma once
#include <string>
#include <map>
#include <vector>
#include <any>
#include <memory>
#include "Token.h"

namespace ParadoxLanguage {

    class Object {
        private:
            struct Data {
                std::map<std::string, std::vector<std::any>> map;
            };

            std::shared_ptr<Data> data;

            void Parse(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);

            Object(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);

            std::string GenerateCode(std::string_view frontAppend) const;
        public:
            Object();
            Object(std::string code);

            std::vector<std::string> Keys() const;

            std::vector<std::any>& AllAt(std::string key);

            const std::vector<std::any>& AllAt(std::string key) const;

            std::any& At(std::string key);

            const std::any& At(std::string key) const;

            std::string Code() const;
    };

    class Array {
        private:
            struct Data {
                std::vector<std::any> array;
            };

            std::shared_ptr<Data> data;
        public:
            Array();
            Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);

            std::vector<std::any>& Values();
            
            const std::vector<std::any>& Values() const;

            std::string Code() const;
    };

}
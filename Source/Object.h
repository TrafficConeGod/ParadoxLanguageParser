#pragma once
#include <fifo_map.hpp>
#include <vector>
#include "Token.h"

namespace ParadoxLanguage {

    class Value;

    std::string StringToCode(std::string string);

    class Object {
        private:
            nlohmann::fifo_map<std::string, std::vector<Value>> map;

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

}
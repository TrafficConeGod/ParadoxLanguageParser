#pragma once
#include <map>
#include <vector>
#include "Token.h"

namespace ParadoxLanguage {

    class Value;

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

};
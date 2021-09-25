#pragma once
#include <variant>
#include "Object.h"
#include "Array.h"

namespace ParadoxLanguage {
    class Value {
        private:
            std::variant<std::string, Object, Array> variant;

            std::variant<std::string, Object, Array> Variant(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
        public:
            template<typename T>
            Value(const T& val) : variant{val} {}
            
            Value(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin);
            std::string Code(std::string_view frontAppend = "") const;

            template<typename T>
            bool CanCast() const { return std::holds_alternative<T>(variant); }
            template<typename T>
            const T& Cast() const { return std::get<T>(variant); }
            template<typename T>
            T& Cast() { return std::get<T>(variant); }
    };
}
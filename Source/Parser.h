#pragma once
#include <string>
#include <map>
#include <vector>
#include <any>
#include <memory>

namespace ParadoxLanguage {

    class Object {
        private:
            struct ObjectData {
                std::map<std::string, std::vector<std::any>> dataMap;
            };

            std::shared_ptr<ObjectData> reference;
        public:
            Object(std::string code);

            std::vector<std::string_view> Keys() const;

            std::vector<std::any> AllAt(std::string_view key);

            std::vector<const std::any> AllAt(std::string_view key) const;

            std::any At(std::string_view key);

            const std::any At(std::string_view key) const;

            template<typename T>
            void SetAt(std::string_view key, std::vector<T> value);
    };

    class Array {
        private:
        public:
            std::vector<std::any> Values();
            
            std::vector<const std::any> Values() const;

            void SetValues(std::vector<std::any> values);
    };

}
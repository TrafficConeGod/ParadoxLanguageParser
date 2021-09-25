#include "Parser.h"
#include <sstream>
#include <fstream>

bool IsNumber(const std::string& string) {
    for (auto ch : string) {
        if (!std::isdigit(ch)) {
            return false;
        }
    }
    return true;
}

int main() {
    std::stringstream stream;
    stream << std::ifstream("SaveGameExample.txt").rdbuf();
    ParadoxLanguage::Object object(stream.str());
    stream.str(std::string());

    std::string owner = "BRZ";

    for (const auto& key : object.Keys()) {
        if (IsNumber(key)) {
            auto& provinceObject = object.At(key).Cast<ParadoxLanguage::Object>();
            provinceObject.At("owner") = owner;
            provinceObject.At("controller") = owner;
        }
    }

    std::string code = object.Code();

    std::ofstream("SaveGameExampleReconstructed.out").write(code.c_str(), code.size());

    return 0;
}
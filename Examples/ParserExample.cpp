#include "Parser.h"
#include <iostream>
#include <sstream>
#include <fstream>

int main() {
    std::stringstream stream;
    stream << std::ifstream("ParserExample.txt").rdbuf();
    ParadoxLanguage::Object object(stream.str());

    for (auto key : object.Keys()) {
        std::cout << key << ":\n";
        for (auto value : object.AllAt(key)) {
            std::cout << "\t" << std::any_cast<std::string>(value) << "\n";
        }
    }

    return 0;
}
#include "Parser.h"
#include <sstream>
#include <fstream>

int main() {
    std::stringstream stream;
    stream << std::ifstream("SaveGameExample.txt").rdbuf();
    ParadoxLanguage::Object object(stream.str());
    stream.str(std::string());

    std::string code = object.Code();

    std::ofstream("SaveGameExampleReconstructed.o").write(code.c_str(), code.size());

    return 0;
}
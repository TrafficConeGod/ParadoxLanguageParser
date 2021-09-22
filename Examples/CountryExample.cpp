#include "Parser.h"
#include <iostream>
#include <sstream>
#include <fstream>

class Country {
    private:
        ParadoxLanguage::Object object;
    public:
        Country(std::string_view code) : object{std::string(code)} {}

        std::string GraphicalCulture() const {
            auto graphicalCulture = std::any_cast<std::string>(object.At("graphical_culture"));
            return graphicalCulture;
        }
};

int main() {
    std::stringstream stream;
    stream << std::ifstream("CountryExample.txt").rdbuf();
    Country country(stream.str());

    std::cout << country.GraphicalCulture() << "\n";

    return 0;
}
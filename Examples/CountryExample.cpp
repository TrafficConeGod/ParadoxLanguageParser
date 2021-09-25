#include "Parser.h"
#include <iostream>
#include <sstream>
#include <fstream>

class Party {
    private:
        ParadoxLanguage::Object& object;
    public:
        Party(ParadoxLanguage::Object& vObject) : object{vObject} {}

        std::string& Name() {
            return object.At("name").Cast<std::string>();
        }
};

class Country {
    private:
        ParadoxLanguage::Object object;
    public:
        Country(std::string_view code) : object{std::string(code)} {}

        std::string& GraphicalCulture() {
            return object.At("graphical_culture").Cast<std::string>();
        }

        std::vector<Party> Parties() {
            std::vector<Party> parties;
            auto& partyObjects = object.AllAt("party");
            for (auto& partyObject : partyObjects) {
                parties.push_back(partyObject.Cast<ParadoxLanguage::Object>());
            }
            return parties;
        }
};

int main() {
    std::stringstream stream;
    stream << std::ifstream("CountryExample.txt").rdbuf();
    Country country(stream.str());

    std::cout << country.GraphicalCulture() << "\n";
    country.GraphicalCulture() = "FrenchGC";
    std::cout << country.GraphicalCulture() << "\n";

    for (auto& party : country.Parties()) {
        std::cout << party.Name() << "\n";
        party.Name() = "default_party";
        std::cout << party.Name() << "\n";
    }

    return 0;
}
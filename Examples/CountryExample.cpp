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
            return std::any_cast<std::string&>(object.At("name"));
        }
};

class Country {
    private:
        ParadoxLanguage::Object object;
    public:
        Country(std::string_view code) : object{std::string(code)} {}

        std::string& GraphicalCulture() {
            return std::any_cast<std::string&>(object.At("graphical_culture"));
        }

        std::vector<Party> Parties() {
            std::vector<Party> parties;
            auto& partyObjects = object.AllAt("party");
            for (auto& partyObject : partyObjects) {
                parties.push_back(std::any_cast<ParadoxLanguage::Object&>(partyObject));
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
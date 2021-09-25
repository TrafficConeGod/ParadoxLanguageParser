#include "Array.h"
#include "Value.h"
#include <sstream>

using namespace ParadoxLanguage;

Array::Array(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    for (; it != tokens.end() && it->TokenType() != Token::Type::CloseBracket; it++) {
        // std::cout << "array: " << it->String() << ", stage: " << (int)stage << "\n";
        Values().push_back(Value(tokens, it));
    }
}

std::vector<Value>& Array::Values() {
    return vector;
}
            
const std::vector<Value>& Array::Values() const {
    return vector;
}
            
std::string Array::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    for (auto& value : Values()) {
        stream << value.Code(frontAppend);
        stream << " ";
    }
    return stream.str();
}

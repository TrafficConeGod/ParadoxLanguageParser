#include "Value.h"
#include "InvalidTokenException.h"
#include <sstream>

using namespace ParadoxLanguage;

std::variant<std::string, Object, Array> Value::Variant(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it) {
    enum class Stage {
        None,
        Compound,
        CompoundLiteral
    };
    Stage stage = Stage::None;
    auto compoundFirstTokenIterator = it;

    // std::cout << it->String() << "\n";
    
    for (; it != tokens.end(); it++) {
        auto type = it->TokenType();
        switch (stage) {
            case Stage::None: {
                switch (type) {
                    case Token::Type::Literal: {
                        return std::string(std::string(it->Literal()));
                    } break;
                    case Token::Type::OpenBracket: {
                        stage = Stage::Compound;
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected literal or \"{\" for value");
                    } break;
                }
            } break;
            case Stage::Compound: {
                switch (type) {
                    case Token::Type::OpenBracket: {
                        return Array(tokens, it);
                    } break;
                    case Token::Type::CloseBracket: {
                        return Object(tokens, it);
                    } break;
                    case Token::Type::Literal: {
                        stage = Stage::CompoundLiteral;
                        compoundFirstTokenIterator = it;
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected literal, \"{\", or \"}\" after \"{\"");
                    } break;
                }
            } break;
            case Stage::CompoundLiteral: {
                switch (type) {
                    case Token::Type::Assignment: {
                        it = compoundFirstTokenIterator;
                        return Object(tokens, it);
                    } break;
                    case Token::Type::OpenBracket:
                    case Token::Type::CloseBracket:
                    case Token::Type::Literal: {
                        it = compoundFirstTokenIterator;
                        return Array(tokens, it);
                    } break;
                    default: {
                        throw InvalidTokenException(it, "Expected \"=\", literal, \"{\", or \"}\" after literal");
                    } break;
                }
            } break;
            default: {
                throw InvalidTokenException(it, "Unknown value error");
            } break;
        }
    }
    throw InvalidTokenException(it, "Unknown value error");
}

Value::Value(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& begin) : variant{Variant(tokens, begin)} {}

std::string Value::Code(std::string_view frontAppend) const {
    std::stringstream stream;
    if (CanCast<std::string>()) {
        stream << StringToCode(Cast<std::string>());
    } else if (CanCast<Object>()) {
        stream << "{\n" << Cast<Object>().Code(std::string(frontAppend.size() + 1, '\t')) << frontAppend << "}";
    } else if (CanCast<Array>()) {
        stream << "{ " << Cast<Array>().Code(frontAppend) << "}";
    } else {
        stream << "UNDEFINED";
    }
    return stream.str();
}

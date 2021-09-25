#include "InvalidTokenException.h"
#include <sstream>
#include <iostream>

using namespace ParadoxLanguage;

std::string InvalidTokenException::Message(std::string literal, const Token::Position& position, std::string_view reason) {
    std::stringstream stream;
    stream << "Invalid token \"" << literal << "\" at row: " << position.row << ", column: " << position.column << "\nReason: " << reason << "\n";
    return stream.str();
}


InvalidTokenException::InvalidTokenException(std::vector<Token>::const_iterator iterator, std::string_view reason) : message{Message(std::string(iterator->Literal()), iterator->TokenPosition(), reason)} {}
InvalidTokenException::~InvalidTokenException() throw() {}

const char* InvalidTokenException::what() const throw() {
    return message.c_str();
}
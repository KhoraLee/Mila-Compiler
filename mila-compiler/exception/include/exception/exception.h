#pragma once

#include <iostream>
#include <string>

class Exception : std::exception {
public:
  Exception(std::string msg) : message(msg) {}

  const char* what() const throw() {
      return message.c_str();
  }

protected:
std::string message;
};

class InvalidSymbolException : Exception {
public:
  InvalidSymbolException(std::string msg) : Exception(msg) {}
};
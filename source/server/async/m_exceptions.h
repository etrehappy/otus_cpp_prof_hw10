#pragma once
#include <stdexcept>

class TooManyArguments : public std::runtime_error
{
public:
    TooManyArguments(const std::string& msg)
        : std::runtime_error(msg) {}
};

class NoArguments : public std::runtime_error
{
public:
    NoArguments(const std::string& msg)
        : std::runtime_error(msg) {}
};

class UnknownCommand: public std::runtime_error
{
public:
    UnknownCommand(const std::string& msg)
        : std::runtime_error(msg) {}
};
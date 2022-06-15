#ifndef AEACUS_ARGUMENT_PARSER_H
#define AEACUS_ARGUMENT_PARSER_H

#pragma once

#include <util/argument.h>
#include <unordered_map>
#include <string>

class ArgumentParser {
private:

public:
    virtual void registerArgument(Argument arg) = 0;
    virtual std::unordered_map<std::string, std::string> parse(int argc, const char** argv) = 0;
};

#endif //AEACUS_ARGUMENT_PARSER_H

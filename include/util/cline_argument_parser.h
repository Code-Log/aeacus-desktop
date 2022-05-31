#ifndef AEACUS_CLINE_ARGUMENT_PARSER_H
#define AEACUS_CLINE_ARGUMENT_PARSER_H

#pragma once
#include <util/argument_parser.h>

class CommandLineArgumentParser : public ArgumentParser {
public:
    std::unordered_map<std::string, std::string> parse(int argc, const char **argv) override;
    void registerArgument(Argument arg) override;
};

#endif //AEACUS_CLINE_ARGUMENT_PARSER_H

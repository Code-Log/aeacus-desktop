#ifndef AEACUS_CLINE_ARGUMENT_PARSER_H
#define AEACUS_CLINE_ARGUMENT_PARSER_H

#pragma once
#include <util/argument_parser.h>
#include <vector>

class CommandLineArgumentParser : public ArgumentParser {
private:
    std::vector<Argument>m_RegisteredArguments;
    std::unordered_map<std::string, std::string>m_Hash;
public:
    CommandLineArgumentParser();
    std::unordered_map<std::string, std::string> parse(int argc, const char **argv) override;
    void registerArgument(Argument arg) override;
    bool isRegistered(Argument arg);
    static int dashCount(std::string str);
    static std::string undash(std::string str);
    static bool isValidFlag(const std::string& flag);
};

#endif //AEACUS_CLINE_ARGUMENT_PARSER_H

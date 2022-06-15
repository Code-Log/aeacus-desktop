#ifndef AEACUS_ARGUMENT_H
#define AEACUS_ARGUMENT_H

#pragma once
#include <string>
#include <utility>
#include <vector>
class Argument
{
private:
    std::vector<std::string> m_Aliases;
    std::string m_KeyName;
    bool m_IsFlag;
public:
    Argument(const std::string& alias,std::string key,bool isFlag);
    Argument(std::string arrAlias[], int arrSize, std::string keyName, bool isFlag);
    void addAlias(const std::string& alias);
    std::string getKey();
    std::vector<std::string> getAliases();
    [[nodiscard]] bool isFlag() const;
};

#endif //AEACUS_ARGUMENT_H

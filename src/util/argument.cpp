#include <util/argument.h>

#include <utility>

Argument::Argument(const std::string& alias, std::string keyName, bool isFlag)
    :m_IsFlag(isFlag), m_KeyName(std::move(keyName))
{
    m_Aliases.emplace_back(alias);
}

Argument::Argument(std::string arrAlias[], int arrSize, std::string keyName, bool isFlag)
    :m_IsFlag(isFlag), m_KeyName(std::move(keyName))
{
    for (int i = 0;i < arrSize; i++)
        m_Aliases.emplace_back(arrAlias[i]);
}

std::string Argument::getKey()
{
    return m_KeyName;
}

void Argument::addAlias(const std::string& alias)
{
    m_Aliases.emplace_back(alias);
}

std::vector<std::string> Argument::getAliases()
{
    return m_Aliases;
}

bool Argument::isFlag() const
{
    return m_IsFlag;
}
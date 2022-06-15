#include <util/cline_argument_parser.h>
#include <util/InvalidArgumentException.h>

CommandLineArgumentParser::CommandLineArgumentParser()
{
    m_RegisteredArguments = {};
    m_Hash = {};
}
std::unordered_map<std::string, std::string> CommandLineArgumentParser::parse(int argc, const char** argv)
{
    for (int i = 0; i < argc; i++)  //loop through argument values received
    {
        std::string newArgV = argv[i];
        bool registered = false;
        if (isValidFlag(newArgV))
            for (Argument arg: m_RegisteredArguments)  //loop through registered arguments
            {
                for (const std::string &alias: arg.getAliases()) // loop through all aliases of argument
                {
                    if (alias == undash(newArgV))
                    {
                        registered = true;
                        if ((i == argc - 1) || (isValidFlag(argv[i + 1])))
                            m_Hash[arg.getKey()] = "true";
                        else if (arg.isFlag())
                            throw InvalidArgumentException("Attempted to assign a value to a flag!");
                        else
                            m_Hash[arg.getKey()] = argv[i + 1];
                    }
                    if (registered)
                        break;
                }
            if (registered)
                break;
            }
        if ((!registered) && (dashCount(newArgV) > 0))
            throw InvalidArgumentException("An unregistered or invalid argument was specified");
    }

    return m_Hash;
}

bool CommandLineArgumentParser::isRegistered(Argument arg)
{
    bool isRegistered = false;

    for (Argument registered : m_RegisteredArguments)
        if (arg.getKey() == registered.getKey())
        {
            isRegistered = true;
            break;
        }

    return isRegistered;
}
void CommandLineArgumentParser::registerArgument(Argument arg)
{
    if (!isRegistered(arg))
        m_RegisteredArguments.emplace_back(arg);
}

std::string CommandLineArgumentParser::undash(std::string str)
{
    std::string undashed;

    if (dashCount(str) > 2)
        throw InvalidArgumentException("Too many dashes on argument!");
    else
        for (int i = dashCount(str); i < str.length(); i++)
            undashed += str[i];

    return undashed;
}

int CommandLineArgumentParser::dashCount(std::string str)
{
    int i = 0;
    while (str[i] == '-')
        i++;

    return i;
}

bool CommandLineArgumentParser::isValidFlag(const std::string& flag)
{
    if (dashCount(flag)<3)
    {
        if ((dashCount(flag) == 2) && (flag.length() < 4)) //2 dashes but not more than 1 non_dash char long
            return false;
        if ((dashCount(flag) == 1) && (flag.length() > 2)) //1 dash but more than 1 non_dash char long
            return false;
        if (dashCount(flag) == 0) //not a flag, does not contain dash
            return false;
    }
    else
        throw InvalidArgumentException("Too many dashes on argument!");

    return true;
}



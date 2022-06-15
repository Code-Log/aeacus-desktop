#include <iostream>
#include <util/argument.h>
#include <util/cline_argument_parser.h>

int main()
{
    const char* argVals[5] = { "-l", "3", "--write", "-s", "good"};

    Argument arg1("l","reader", false);
    Argument arg2("write","writer",false);
    Argument arg3("s","seer", false);
    arg1.addAlias("listen");
    std::cout << arg1.isFlag() << std::endl;

    CommandLineArgumentParser parser;
    parser.registerArgument(arg1);
    parser.registerArgument(arg2);
    parser.registerArgument(arg3);


    std::unordered_map result = parser.parse(5, argVals);

    for (auto & it : result)
        std::cout << " " << it.first << ":" << it.second;


    return 0;
}
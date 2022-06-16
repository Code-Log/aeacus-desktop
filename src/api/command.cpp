#include <api/command.h>
#include <iostream>

namespace aeacus
{
    void ShutdownCommand::execute()
    {
        std::cout << "Shutdown command received!" << std::endl;
    }
}
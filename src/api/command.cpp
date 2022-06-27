#include <api/command.h>
#include <iostream>
#include <glog/logging.h>

namespace aeacus
{
    void ShutdownCommand::execute()
    {
        LOG(INFO) << "Shutdown command received!" << std::endl;
    }
}
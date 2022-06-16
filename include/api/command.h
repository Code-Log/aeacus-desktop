#ifndef AEACUS_COMMAND_H
#define AEACUS_COMMAND_H

#pragma once

#include <string>

namespace aeacus
{
    class Command
    {
    public:
        virtual void execute() = 0;
    };

    class ShutdownCommand : public Command
    {
    public:
        ShutdownCommand() = default;
        void execute() override;
    };
}

#endif //AEACUS_COMMAND_H

#pragma once
#include "Cmd.h"

#include <vector>
#include <functional>
#include <tuple>

namespace rclib
{
    class CmdShell
    {
    public:
        CmdShell();
        ~CmdShell();
        template <class U>
        void AddCmd(Cmd<U> newcmd);
        void Run();
    private:

    };
}


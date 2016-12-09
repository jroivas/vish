#include "builtin.hpp"

#include <iostream>
#include <unistd.h>

Builtin::Builtin()
    : m_status(0)
{
}

bool Builtin::run(const std::vector<std::string> &args)
{
    m_status = -1;
    if (args.empty()) return false;

    std::string val = args[0];
    if (val == "cd") {
        m_status = _cd(args);
        return true;
    }
    return false;
}

int Builtin::_cd(const std::vector<std::string> &args)
{
    if (args.size() <= 1) {
        std::cout << "Expected argument to \"cd\"\n";
        return 1;
    }

    if (chdir(args[1].c_str()) != 0) {
        perror("vish");
    }

    return 0;
}

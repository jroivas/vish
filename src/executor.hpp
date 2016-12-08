#pragma once

#include <string>
#include <vector>

class Executor
{
public:
    Executor();
    ~Executor();

    void execute(const std::vector<std::string> &cmdline);

    int returnCode() const
    {
        return m_return_code;
    }

private:
    int m_return_code;
};

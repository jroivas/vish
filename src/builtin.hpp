#pragma once

#include <string>
#include <vector>

class Builtin
{
public:
    Builtin();

    bool run(const std::vector<std::string> &args);
    int status() const
    {
        return m_status;
    }

protected:
    int _cd(const std::vector<std::string> &);

private:
    int m_status;
};

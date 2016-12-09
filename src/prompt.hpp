#pragma once

#include <string>
#include <vector>
#include "builtin.hpp"

class Prompt
{
public:
    Prompt();
    void loop();
    void resetLoop();

private:
    std::vector<std::string> parseLine(const std::string &line);
    std::vector<std::string> tokenize(const std::string &str, const std::string &delimiters = " ");
    void executeCommand(const std::vector<std::string> &command);
    Builtin m_builtin;

    bool m_running;
    bool m_invalid;
    std::string m_prompt;
    int m_return_code;
};

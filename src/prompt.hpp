#pragma once

#include <string>
#include <vector>

class Prompt
{
public:
    Prompt();
    void loop();

private:
    std::vector<std::string> parseLine(const std::string &line);
    std::vector<std::string> tokenize(const std::string &str, const std::string &delimiters = " ");
    void executeCommand(const std::vector<std::string> &command);

    bool running;
    std::string prompt;
    int m_return_code;
};

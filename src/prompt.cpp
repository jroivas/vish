#include "prompt.hpp"
#include "executor.hpp"
#include <iostream>

Prompt::Prompt()
    : running(true),
    prompt("$ ")
{
}

void Prompt::loop()
{
    std::string line;
    while (running) {
        std::cout << prompt;
        std::getline(std::cin, line);
        auto res = parseLine(line);
        executeCommand(res);
    }
}

std::vector<std::string> Prompt::tokenize(const std::string &str, const std::string &delimiters)
{
    std::vector<std::string> tokens;

    std::string part = "";
    bool quote = false;
    bool escape = false;
    for (char c : str) {
        if (!escape && quote && c == '"') {
            quote = false;
        } else if (quote) {
            part += c;
        } else if (delimiters.find(c) != std::string::npos) {
            if (!part.empty()) {
                tokens.push_back(part);
            }
            part = "";
        } else if (c == '\\') {
            escape = true;
        } else if (c == '"') {
            if (escape) {
                part += c;
                escape = false;
            } else {
                quote = true;
            }
        } else if (escape) {
            part += '\\';
            part += c;
        } else {
            part += c;
        }
    }
    if (!part.empty()) {
        tokens.push_back(part);
    }

    return tokens;
}

std::vector<std::string> Prompt::parseLine(const std::string &line)
{
    std::vector<std::string> res;

    res = tokenize(line, " \t\r\n\a");
    // TODO: Combine quoted in res

    return res;
}

void Prompt::executeCommand(const std::vector<std::string> &command)
{
    if (command.empty()) return;
    if (command[0] == "exit") {
        running = false;
        return;
    }

    std::cout << "CMD:\n";
    for (std::string part : command) {
        std::cout << " " << part << "\n";
    }

    Executor e;
    e.execute(command);
    m_return_code = e.returnCode();
    if (m_return_code != 0) {
        std::cout << "-- Return code: " << m_return_code << "\n";
    }
}

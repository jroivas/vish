#include "prompt.hpp"
#include "executor.hpp"
#include <iostream>

Prompt::Prompt()
    : m_running(true),
    m_prompt("$ ")
{
}

void Prompt::loop()
{
    std::string line;
    while (m_running) {
        m_invalid = false;
        std::cout << m_prompt;
        // FIXME alternative controls, arrows, etc.
        std::getline(std::cin, line);
        if (!m_invalid) {
            auto res = parseLine(line);
            executeCommand(res);
        }
    }
}

void Prompt::resetLoop()
{
    // FIXME
    m_invalid = true;
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
        m_running = false;
        return;
    }

    if (m_builtin.run(command)) {
        m_return_code = m_builtin.status();
    } else {
        Executor e;
        e.execute(command);
        m_return_code = e.returnCode();
    }

    if (m_return_code != 0) {
        std::cout << "-- Return code: " << m_return_code << "\n";
    }
}

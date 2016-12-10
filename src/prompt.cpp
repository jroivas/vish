#include "prompt.hpp"
#include "executor.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

Prompt::Prompt()
    : m_running(true),
    m_got_eof(false),
    m_prompt("$ "),
    m_history_pos(0)
{
}

Prompt::~Prompt()
{
    resetCanonical();
}

void Prompt::setupCanonical()
{
    memset(&m_term, 0, sizeof(m_term));
    if (tcgetattr(0, &m_term) < 0) {
        perror("vish, terminal info");
    }

    // Disable canonical mode and echo, to get raw terminal control
    m_term.c_lflag &= ~ICANON;
    m_term.c_lflag &= ~ECHO;
    m_term.c_cc[VMIN] = 1;
    m_term.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &m_term) < 0) {
        perror("vish, terminal set");
    }
}

void Prompt::resetCanonical()
{
    // Return default terminal control
    m_term.c_lflag |= ICANON;
    m_term.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &m_term) < 0) {
        perror("vish, terminal reset");
    }
}

void Prompt::loop()
{
    std::string line;
    while (m_running) {
        m_invalid = false;
        std::cout << m_prompt;
        // FIXME alternative controls, arrows, etc.
        //std::getline(std::cin, line);
        m_history_pos = 0;
        line = readLine();
        if (!m_invalid) {
            m_history.push_back(line);
            auto res = parseLine(line);
            executeCommand(res);
        }
    }
}

void Prompt::eraseChar()
{
    std::cout << '\b' << ' ' << '\b';
}

void Prompt::restoreLine(std::string line)
{
    for (auto c : line) {
        (void)c;
        eraseChar();
    }
}

std::string Prompt::readLine()
{
    std::string res;
    m_got_eof = false;

    std::vector<int> special;
    setupCanonical();

    std::ofstream ff("log.txt", std::ios::out | std::ios::app);
    ff.write("LINE\n", 5);
    m_line_pos = 0;
    while (!m_got_eof) {
        bool do_append = true;
        int c = std::cin.get();
#if 1
        std::stringstream ss;
        ss << " C: " << std::hex << c << " " << std::dec << (char)c << "\n";
        ff.write(ss.str().c_str(), ss.str().size());
#endif
        if (c == CHR_BACKSPACE) {
            if (m_line_pos < res.size()) {
                if (m_line_pos > 0) {
                    --m_line_pos;
                }
                res = res.erase(m_line_pos, 1);

                std::string rest = res.substr(m_line_pos) + " ";
                std::cout << '\b';
                std::cout << rest;
                for (auto c : rest) {
                    std::cout << '\b';
                }
            } else {
                if (!res.empty()) {
                    res = res.substr(0, res.size() - 1);
                }
                eraseChar();
                --m_line_pos;
            }
            do_append = false;
        } else if (c == CHR_SPECIAL || !special.empty()) {
            special.push_back(c);
            if (special.size() >= 3) {
                if (special[1] == CHR_SPECIAL_ARR) {
                    if (special[2] == CHR_ARR_UP) {
                        if (m_history.size() >= 1) {
                            ++m_history_pos;
                            if (m_history_pos >= m_history.size()) {
                                m_history_pos = m_history.size();
                            }
                            restoreLine(res);
                            res = m_history[m_history.size() - m_history_pos];
                            std::cout << res;
                            m_line_pos = res.size();
                        } else {
                            m_history_pos = 0;
                        }
                    } else if (special[2] == CHR_ARR_DOWN) {
                        if (m_history_pos > 0) {
                            --m_history_pos;
                        } else {
                            m_history_pos = 0;
                        }
                        restoreLine(res);
                        if (m_history_pos == 0) {
                            res = "";
                        } else {
                            res = m_history[m_history.size() - m_history_pos];
                        }
                        std::cout << res;
                        m_line_pos = res.size();
                    } else if (special[2] == CHR_ARR_LEFT) {
                        if (m_line_pos > 0) {
                            --m_line_pos;
                            std::cout << '\b';
                        }
                    } else if (special[2] == CHR_ARR_RIGHT) {
                        if (m_line_pos < res.size()) {
                            std::cout << res[m_line_pos];
                            ++m_line_pos;
                        }
                        //std::cout << CHR_SPECIAL << CHR_SPECIAL_ARR << CHR_ARR_RIGHT;
                    }
                }
                special.erase(special.begin(), special.end());
            }
            do_append = false;
        } else if (c == CHR_EOL) {
            std::cout << "\n";
            break;
        }
        if (do_append) {
            std::cout.put(c);
            if (m_line_pos < res.size()) {
                res.replace(m_line_pos, 1, 1, c);
            } else {
                res += c;
            }
            ++m_line_pos;
        }
    }

    return res;
}

void Prompt::resetLoop()
{
    // FIXME
    m_invalid = true;
    m_got_eof = true;
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

    std::cout << "CMD:\n";
    for (auto c : command) {
        std::cout << c << " ";
    }
    std::cout << "\n";

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

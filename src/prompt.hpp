#pragma once

#include <string>
#include <vector>
#include <termios.h>

#include "builtin.hpp"

class Prompt
{
public:
    Prompt();
    ~Prompt();
    void loop();
    void resetLoop();

private:
    static const int CHR_BACKSPACE = 0x7f;
    static const int CHR_EOL = 0x0a;
    static const int CHR_SPECIAL = 0x1b;
    static const int CHR_SPECIAL_ARR = 0x5b;
    static const int CHR_ARR_UP = 0x41;
    static const int CHR_ARR_DOWN = 0x42;
    static const int CHR_ARR_LEFT = 0x43;
    static const int CHR_ARR_RIGHT = 0x44;

    std::vector<std::string> parseLine(const std::string &line);
    std::vector<std::string> tokenize(const std::string &str, const std::string &delimiters = " ");
    void executeCommand(const std::vector<std::string> &command);
    std::string readLine();
    void restoreLine(std::string line);
    void eraseChar();

    Builtin m_builtin;

    bool m_running;
    bool m_invalid;
    bool m_got_eof;
    std::string m_prompt;
    int m_return_code;
    struct termios m_term;
    std::vector<std::string> m_history;
    unsigned int m_history_pos;
};

#include "prompt.hpp"
#include <iostream>
#include <csignal>

Prompt prompt;

void break_signal(int)
{
    std::cout << "Break\n";
    prompt.resetLoop();
}


int main(int argc, char **argv)
{
    std::signal(SIGINT, break_signal);

    // TODO Config

    prompt.loop();

    return EXIT_SUCCESS;
}

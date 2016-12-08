#include "executor.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

Executor::Executor()
    : m_return_code(0)
{
}

Executor::~Executor()
{
}

void Executor::execute(const std::vector<std::string> &cmdline)
{
    const char ** items = new const char *[cmdline.size()];
    unsigned int i = 0;
    for (std::string item : cmdline) {
        items[i++] = item.c_str();
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child
        if (execvp(items[0], (char * const *)items) == -1) {
            perror("vish");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Failed to fork
        perror("vish");
    } else {
        int status = 0;

        do {
            /*pid_t wait_pid = */ waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        m_return_code = WEXITSTATUS(status);
    }
}

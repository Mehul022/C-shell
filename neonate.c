#include "header.h"
struct termios orig_termios;
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode()
{
    // in normal mode it wait for newline from user to read the input by removing the canonical mode from original now we can read the input without having newline from the user..
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void last_pid(int time)
{
    enableRawMode();
    int pid = fork();
    if (pid == 0)
    {
        while (1)
        {
            FILE *file = fopen("/proc/sys/kernel/ns_last_pid", "r");
            if (!file)
            {
                perror("Error opening /proc/sys/kernel/ns_last_pid");
                exit(EXIT_FAILURE);
            }

            int last_pid;
            if (fscanf(file, "%d", &last_pid) != 1)
            {
                perror("Error reading last PID");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            fclose(file);
            printf("%d\n", last_pid);
            sleep(time);
        }
    }
    else if (pid > 0)
    {
        char c;
        while (read(STDIN_FILENO, &c, 1) == 1 && c != 'x')
        {
            continue;
        }
        kill(pid, SIGKILL);
    }
    disableRawMode();
}

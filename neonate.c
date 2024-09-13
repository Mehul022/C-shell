#include "header.h"
struct termios orig_termios;
int child_pid;
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void last_pid(int time)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    sigset_t new_set, old_set;
    sigemptyset(&new_set);
    sigaddset(&new_set, SIGINT);
    sigaddset(&new_set, SIGTSTP);
    sigprocmask(SIG_BLOCK, &new_set, &old_set) ;
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
        child_pid = pid;
        fd_set readfds;
        char c;
        while (1)
        {
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds); 
            struct timeval timeout;
            timeout.tv_sec = time; 
            timeout.tv_usec = 0;
            int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
            if (result == -1)
            {
                perror("select() error");
                break;
            }
            else if (result > 0)
            {
                if (FD_ISSET(STDIN_FILENO, &readfds))
                {
                    if (read(STDIN_FILENO, &c, 1) == 1 && c == 'x')
                    {
                        kill(pid, SIGKILL);
                        disableRawMode();
                        break;
                    }
                }
            }
        }
    }
}

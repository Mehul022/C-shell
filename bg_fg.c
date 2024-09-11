#include"header.h"

void fg(int pid,int* flag)
{
    if (kill(pid, 0) == 0)
    {
        if (kill(pid, SIGCONT) == -1)
        {
            perror("kill failed");
            return;
        }
        char* input_command2=remove_process(&process_list,pid);
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        strcpy(foregroung_command, input_command2);
        int status;
        foreground_pid = pid;
        while (1)
        {
            pid_t result = waitpid(pid, &status, WNOHANG | WUNTRACED);

            if (result == -1)
            {
                perror("waitpid");
                break;
            }

            if (result == 0)
            {
                sleep(0.1);
            }
            else if (WIFSTOPPED(status))
            {
                break;
            }
            else if (WIFEXITED(status) || WIFSIGNALED(status))
            {
                break;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        foreground_pid = -1;
        if (!status)
        {
            double elapsed = end.tv_sec - start.tv_sec;
            elapsed += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
            int t = elapsed;
            if (t > 2)
            {
                if (*flag)
                {
                    strcat(time_exceed_command, " ");
                    strcat(time_exceed_command, "|");
                    strcat(time_exceed_command, input_command2);
                }
                else
                {
                    strcpy(time_exceed_command, input_command2);
                    *flag = 1;
                }
            }
        }
    }
    else
    {
        printf("No such process found!!\n");
        return;
    }
}

void b_g(int pid)
{
    if(kill(pid, 0) == 0)
    {
        kill(pid, SIGCONT);
        return;
    }
}
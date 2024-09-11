#include "header.h"

bool proclore_pro(char *command)
{
    int pid;
    if (command == NULL)
    {
        pid = getpid();
    }
    else
    {
        pid = atoi(command);
    }
    if (kill(pid, 0) == 0)
    {
        int group_id = getpgid(pid);
        pid_t fg_pgid = tcgetpgrp(STDIN_FILENO);
        char process_path[PATH];
        char state;
        snprintf(process_path, sizeof(process_path), "/proc/%d/status", pid);
        int fd = open(process_path, O_RDONLY);
        if (fd == -1)
        {
            perror("Failed to open status file");
            return 1;
        }
        else
        {
            char buffer[PATH];
            ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
            if (bytesRead == -1)
            {
                perror("Failed to read status file");
                close(fd);
                return 1;
            }
            buffer[bytesRead] = '\0';
            char state;
            char *state_ptr = strstr(buffer, "State:\t");
            if (state_ptr != NULL)
            {
                sscanf(state_ptr, "State:\t%c", &state);
            }
            else
            {
                printf("Failed to find process state in the status file\n");
                return 1;
            }
            int vm_size = 0;
            char *line = strtok(buffer, "\n");
            while (line != NULL)
            {
                if (strncmp(line, "VmSize:", 7) == 0)
                {
                    sscanf(line + 7, "%d", &vm_size);
                    break;
                }
                line = strtok(NULL, "\n");
            }
            close(fd);
            char process_state[3];
            if (state == 'R' || state == 'Z')
            {
                process_state[0] = state;
            }
            else
            {
                process_state[0] = 'S';
            }
            if (fg_pgid == group_id)
            {
                process_state[1] = '+';
                process_state[2] = '\0';
            }
            else
            {
                process_state[1] = '\0';
            }
            char path[PATH];
            char exe_path[PATH];
            snprintf(path, sizeof(path), "/proc/%d/exe", pid);
            ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
            exe_path[len] = '\0';
            printf("pid             : %d\n", pid);
            printf("process status  : %s\n", process_state);
            printf("Process Group   : %d\n", group_id);
            printf("Virtual memory  : %d\n", vm_size);
            printf("executable path : %s\n", exe_path);
        }
    }
    else
    {
        if (errno == ESRCH)
        {

            printf("Process with PID %d does not exist.\n", pid);
        }
        else if (errno == EPERM)
        {
            printf("Process with PID %d exists, but you don't have permission to signal it.\n", pid);
        }
        else
        {
            perror("Error checking process existence");
        }
    }
}

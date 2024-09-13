#include "header.h"

char home_path[PATH_MAX];
char previous_dir[PATH_MAX];
char log_path[PATH_MAX];
char time_exceed_command[COMMAND_PATH];
char foregroung_command[COMMAND_PATH];
char bgends_path[PATH_MAX];
char myshrc_path[PATH];
Process *process_list = NULL;
int foreground_pid;
char *decode_path(char *command)
{
    if (strcmp(command, "~") == 0)
    {
        return home_path;
    }
    else if (strcmp(command, "-") == 0)
    {
        return previous_dir;
    }
    else if (strcmp(command, "..") == 0 || strcmp(command, ".") == 0)
    {
        return command;
    }
    else
    {
        if (command[0] == '~')
        {
            char *expanded_path = (char *)malloc(sizeof(char) * PATH);
            strcpy(expanded_path, home_path);
            strcat(expanded_path, command + 1);
            return expanded_path;
        }
        else
        {
            return command;
        }
    }
}

int check_ampersand_pipe(char *str)
{
    const char *p = str;
    int found_amp = 0;
    while (*p)
    {
        if (*p == '&')
        {
            found_amp = 1;
        }
        else if (found_amp && *p == '|')
        {
            return 0;
        }
        else if (found_amp && !isspace(*p))
        {
            found_amp = 0;
        }
        p++;
    }
    return 1;
}

void sigint_handler(int sig)
{
    if (foreground_pid != -1)
    {
        kill(-foreground_pid, SIGINT);
        // perror("kill");
    }
    else
    {
        printf("\n");
        print_prompt(home_path, time_exceed_command);
        fflush(stdout);
    }
}

void ctrl_d_handler()
{
    while (process_list != NULL)
    {
        int pid = process_list->pid;
        process_list = process_list->next;
        kill(pid, SIGKILL);
    }
    kill(getpid(), SIGKILL);
    exit(0);
}

void sigtstp_handler(int sig)
{
    if (foreground_pid > 0)
    {
        add_process(&process_list, foreground_pid, foregroung_command);
        if (kill(foreground_pid, SIGTSTP) == -1)
        {
            perror("Error sending SIGTSTP to the process");
        }
        foreground_pid = -1;
        tcsetpgrp(STDIN_FILENO, getpid());
    }
    else
    {
        printf("\n");
        print_prompt(home_path, time_exceed_command);
        fflush(stdout);
    }
}

void pipe_caller(char *input, int bg, int *flag)
{
    if (strchr(input, '|'))
    {
        int bg_count = 0;
        int flag_pipe_invalid = 1;
        for (int y = 0; y < strlen(input); y++)
        {
            if (input[y] == '|')
            {
                if (!flag_pipe_invalid)
                {
                    bg_count++;
                    flag_pipe_invalid = 1;
                }
                else
                {
                    printf("Invalid use of pipe\n");
                    return;
                }
            }
            else if (input[y] != ' ' && input[y] != '\t')
            {
                flag_pipe_invalid = 0;
            }
        }
        if (flag_pipe_invalid)
        {
            printf("Invalid use of pipe\n");
            return;
        }
        char inp2[500][COMMAND_PATH];
        int count2 = 0;
        char *inp = strtok(input, "|");
        while (inp != NULL)
        {
            strcpy(inp2[count2++], inp);
            inp = strtok(NULL, "|");
        }
        int read_store = -1;
        for (int y = 0; y < count2; y++)
        {
            if (y == 0)
            {
                read_store = caller(inp2[y], 0, flag, 1, 0, read_store);
                if (read_store == -1)
                {
                    break;
                }
            }
            else if (y == count2 - 1 && bg_count != count2)
            {
                read_store = caller(inp2[y], bg, flag, 0, 1, read_store);
                if (read_store == -1)
                {
                    break;
                }
            }
            else if (y == count2 - 1)
            {
                read_store = caller(inp2[y], bg, flag, 1, 1, read_store);
                if (read_store == -1)
                {
                    break;
                }
            }
            else
            {
                read_store = caller(inp2[y], 0, flag, 1, 1, read_store);
                if (read_store == -1)
                {
                    break;
                }
            }
        }
    }
    else
    {
        caller(input, bg, flag, 0, 0, -1);
    }
}

void command_handler(char *input)
{
    char delimiters[] = ";";
    char *Input = strtok(input, delimiters);
    char com[500][COMMAND_PATH];
    int count = 0;
    int flag = 0;
    char buffer[COMMAND_PATH];
    FILE *file = fopen(bgends_path, "r");
    if (file == NULL)
    {
        perror("Failed to open file for reading");
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        printf("%s", buffer);
    }
    fclose(file);
    file = fopen(bgends_path, "w");
    if (file == NULL)
    {
        perror("Failed to open file for writing");
    }
    fclose(file);
    while (Input != NULL)
    {
        strcpy(com[count++], Input);
        Input = strtok(NULL, delimiters);
    }
    for (int x = 0; x < count; x++)
    {
        if (strchr(com[x], '&'))
        {
            int bg_count = 0;
            for (int y = 0; y < strlen(com[x]); y++)
            {
                if (com[x][y] == '&')
                {
                    bg_count++;
                }
            }
            char inp2[500][COMMAND_PATH];
            int count2 = 0;
            char *inp = strtok(com[x], "&");
            while (inp != NULL)
            {
                strcpy(inp2[count2++], inp);
                inp = strtok(NULL, "&");
            }
            for (int y = 0; y < count2; y++)
            {
                if (y == count2 - 1 && bg_count != count2)
                {
                    pipe_caller(inp2[y], 0, &flag);
                }
                else
                    pipe_caller(inp2[y], 1, &flag);
            }
        }
        else
        {
            pipe_caller(com[x], 0, &flag);
        }
    }
}

void begin()
{
    getcwd(home_path, sizeof(home_path));
    strncpy(previous_dir, home_path, PATH_MAX);
    strcpy(log_path, home_path);
    strcat(log_path, "/log.txt");
    strcpy(myshrc_path, home_path);
    strcat(myshrc_path, "/.myshrc");
    strcpy(bgends_path, home_path);
    strcat(bgends_path, "/.bg_ends.txt");
    time_exceed_command[0] = '\0';
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    FILE* file = fopen(bgends_path, "w");
    if (file == NULL)
    {
        perror("Failed to open file for writing");
    }
    fclose(file);
}

void sigchld_handler(int signo)
{
    int status;
    pid_t pid;
    FILE *file = fopen(bgends_path, "a");
    if (file == NULL)
    {
        perror("Error opening file in append mode");
        return;
    }
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        Process *temp = process_list;
        while (temp != NULL)
        {
            if (temp->pid == pid)
            {
                if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
                {
                    fprintf(file,"%s exited abnormally (%d) \n", temp->command, temp->pid);
                }
                else if (WIFSIGNALED(status))
                {
                    fprintf(file,"%s exited abnormally (%d)\n", temp->command, temp->pid);
                }
                else if (WIFEXITED(status))
                {
                    fprintf(file,"%s exited normally (%d) \n", temp->command, temp->pid);
                }
                else
                {
                    fprintf(file,"%s exited abnormally (%d)\n", temp->command, temp->pid);
                }
                remove_process(&process_list, temp->pid);
            }
            temp = temp->next;
        }
    }
    fclose(file);
}

void nano_handler(char *input,int bg,int *flag)
{
    char *command=strtok(input," \t");
    int pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
    }
    else if (pid == 0)
    {
        char *args[COMMAND_PATH];
        int i = 0;
        args[i++] = command;
        char *arg;
        while ((arg = strtok(NULL, " \t")) != NULL && strcmp(arg, ">>") != 0 && strcmp(arg, ">") != 0 && strcmp(arg, "<") != 0)
        {
            args[i++] = arg;
        }
        args[i] = NULL;
        if (execvp(command, args) < 0)
        {
            perror("Execution failed");
        }
        exit(1);
    }
    else
    {
        if (!bg)
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            strcpy(foregroung_command, input);
            int status;
            foreground_pid = pid;
            while (1)
            {
                pid_t result = waitpid(pid, &status, WNOHANG | WUNTRACED);

                if (result == -1)
                {
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
            setpgid(pid, pid);
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
                        strcat(time_exceed_command, input);
                    }
                    else
                    {
                        strcpy(time_exceed_command, input);
                        *flag = 1;
                    }
                }
            }
        }
        else
        {
            if (setpgid(pid, pid) < 0)
            {
                perror("Failed to set process group ID");
            }
            add_process(&process_list, pid, input);
            printf("%d\n", pid);
        }
    }
}
void vim_handler(char *input, int bg, int *flag)
{
    char *command = strtok(input, " \t");
    int pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
    }
    else if (pid == 0)
    {
        char *args[COMMAND_PATH];
        int i = 0;
        args[i++] = command;
        char *arg;
        while ((arg = strtok(NULL, " \t")) != NULL && strcmp(arg, ">>") != 0 && strcmp(arg, ">") != 0 && strcmp(arg, "<") != 0)
        {
            args[i++] = arg;
        }
        args[i] = NULL;
        if (execvp(command, args) < 0)
        {
            perror("Execution failed");
        }
        exit(1);
    }
    else
    {
        if (!bg)
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            strcpy(foregroung_command, input);
            int status;
            foreground_pid = pid;
            while (1)
            {
                pid_t result = waitpid(pid, &status, WNOHANG | WUNTRACED);

                if (result == -1)
                {
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
                        strcat(time_exceed_command, input);
                    }
                    else
                    {
                        strcpy(time_exceed_command, input);
                        *flag = 1;
                    }
                }
            }
        }
        else
        {
            if (setpgid(pid, pid) < 0)
            {
                perror("Failed to set process group ID");
            }
            add_process(&process_list, pid, input);
            printf("%d\n", pid);
        }
    }
}
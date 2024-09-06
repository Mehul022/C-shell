#include "header.h"

int caller(char *pipe, int bg, int *flag, int pipe_flag_write, int pipe_flag_read)
{
    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);
    int flag_write = 0, flag_append = 0, flag_read = 0;
    char *out_file = NULL;
    char *in_file = NULL;
    int fd_out = -1, fd_in = -1;
    char pipe3[COMMAND_PATH];
    strcpy(pipe3, pipe);
    FILE *file = fopen(pipe_file, "w");
    if (file == NULL)
    {
        perror("Error opening file");
    }
    else
    {
        fclose(file);
    }
    if (strstr(pipe3, ">>"))
    {
        flag_append = 1;
        out_file = strtok(strstr(pipe3, ">>") + 2, " \t");
    }
    else if (strstr(pipe3, ">"))
    {
        flag_write = 1;
        out_file = strtok(strstr(pipe3, ">") + 1, " \t");
    }
    else if (pipe_flag_write)
    {
        flag_write = 1;
        out_file = pipe_file;
    }
    if (strstr(pipe3, "<"))
    {
        flag_read = 1;
        in_file = strtok(strstr(pipe3, "<") + 1, " \t");
    }
    else if (pipe_file_read)
    {

        flag_read = 1;
        in_file = pipe_file_read;
    }
    if (flag_read && in_file)
    {
        fd_in = open(in_file, O_RDONLY);
        if (fd_in < 0)
        {
            perror("open input file");
            return 0;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }

    else if (flag_append && out_file)
    {
        fd_out = open(out_file, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
        if (fd_out < 0)
        {
            perror("open output file");
            return 0;
        }
        dup2(fd_out, STDOUT_FILENO);
    }

    if (flag_write && out_file)
    {
        fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (fd_out < 0)
        {
            perror("open output file");
            return 0;
        }
        dup2(fd_out, STDOUT_FILENO);
    }
    char pipe2[COMMAND_PATH];
    strcpy(pipe2, pipe);
    char *command = strtok(pipe, " \t"); // divide pipe with space and /t simple put \0 in place of that; if we pass null instead of pipe then it will take the previous_dirious pipeay;it returns the first string formed after insertion of the \0;
    if (strcmp(command, "hop") == 0)
    {
        hop_handler(pipe2, flag_read, in_file);
    }
    else if (strcmp(command, "reveal") == 0)
    {
        reveal_handler(pipe2, flag_read, in_file);
    }
    else if (strcmp(command, "log") == 0)
    {
        log_handler(pipe2, flag_read, in_file);
    }
    else if (strcmp(command, "proclore") == 0)
    {
        command = strtok(NULL, " \t");
        if (strcmp(command, ">>") != 0 && strcmp(command, ">") != 0 && strcmp(command, "<") != 0)
            proclore_pro(command);
        else
            proclore_pro(NULL);
    }
    else if (strcmp(command, "seek") == 0)
    {
        seek_handler(pipe2, flag_read, in_file);
    }
    else if (strcmp(command, "cd") == 0)
    {
        cd_handler(pipe2, flag_read, in_file);
    }
    else if (strcmp(command, "activities") == 0)
    {
        update_and_print_processes();
    }
    else
    {
        int pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
        }
        else if (pid == 0)
        {

            if (bg)
            {
                int pid2 = fork();
                if (pid2 < 0)
                {
                    perror("fork failed");
                }
                else if (pid2 == 0)
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
                        exit(1);
                    }
                    exit(0);
                }
                else
                {
                    int status;
                    waitpid(pid2, &status, 0);
                    FILE *file = fopen(bgends_path, "a");
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 0)
                    {
                        fprintf(file, "Command: %s with PID: %d executed successfully\n", pipe, getpid());
                    }
                    else
                    {
                        fprintf(file, "Command: %s with PID: %d is failed\n", pipe, getpid());
                    }
                    fflush(file);
                    fclose(file);
                    exit(0);
                }
            }
            else
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
        }
        else
        {
            if (!bg)
            {
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                int status;
                waitpid(pid, &status, 0); // status will gwt 0 if child ends normally else status will terminate on 1;
                clock_gettime(CLOCK_MONOTONIC, &end);
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
                            strcat(time_exceed_command, " |");
                            strcat(time_exceed_command, pipe2);
                        }
                        else
                        {
                            strcpy(time_exceed_command, pipe2);
                            *flag = 1;
                        }
                    }
                }
            }
            else
            {
                add_process(&process_list, pid, pipe2);
                printf("%d\n", pid);
            }
        }
    }
    if (fd_out != -1)
    {
        dup2(original_stdout, STDOUT_FILENO);
        close(fd_out);
    }
    if (fd_in != -1)
    {
        dup2(original_stdin, STDIN_FILENO);
        close(fd_in);
    }

    close(original_stdout);
    close(original_stdin);
    FILE *src = fopen(pipe_file, "r");
    if (src == NULL)
    {
        perror("Error opening source file");
        return 1;
    }
    FILE *dest = fopen(pipe_file_read, "w");
    if (dest == NULL)
    {
        perror("Error opening destination file");
        fclose(src);
        return 1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        fwrite(buffer, 1, bytes, dest);
    }
    fclose(src);
    fclose(dest);
    src = fopen(pipe_file, "w");
    if (src == NULL)
    {
        perror("Error opening source file to clear");
        return 1;
    }
    fclose(src);
    return 1;
}

int main()
{
    begin();
    while (1)
    {
        print_prompt(home_path, time_exceed_command);
        time_exceed_command[0] = '\0';
        char input[COMMAND_PATH];
        scanf("%[^\n]", input);
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        input[strlen(input)] = '\0';
        int len = strlen(input);
        int is_only_whitespace = 1;
        for (int i = 0; i < len; i++)
        {
            if (!isspace(input[i]))
            {
                is_only_whitespace = 0;
                break;
            }
        }
        if (len == 0 || is_only_whitespace)
        {
            continue;
        }
        if (strstr(input, "log") == NULL)
        {
            write_log(input, log_path);
        }
        command_handler(input);
    }
    return 0;
}
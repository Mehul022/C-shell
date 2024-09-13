#include "header.h"
int caller(char *input_command, int bg, int *flag, int pipe_flag_write, int pipe_flag_read, int fd_read)
{
    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);
    int flag_write = 0, flag_append = 0, flag_read = 0;
    char *out_file = NULL;
    int fd_pipe[2];
    char *in_file = NULL;
    int fd_out = -1, fd_in = -1;
    char input_command3[COMMAND_PATH];
    int has_pipe = 0;
    strcpy(input_command3, input_command);
    if (pipe_flag_write)
    {
        if (pipe(fd_pipe) == -1)
        {
            perror("Pipe failed");
            return -1;
        }
        has_pipe = 1;
    }
    if (strstr(input_command3, ">>"))
    {
        flag_append = 1;
        out_file = strtok(strstr(input_command3, ">>") + 2, " \t");
    }
    else if (strstr(input_command3, ">"))
    {
        flag_write = 1;
        out_file = strtok(strstr(input_command3, ">") + 1, " \t");
    }
    if (strstr(input_command3, "<"))
    {
        flag_read = 1;
        in_file = strtok(strstr(input_command3, "<") + 1, " \t");
    }
    if (flag_read && in_file)
    {
        fd_in = open(in_file, O_RDONLY);
        if (fd_in < 0)
        {
            perror("open input file");
            return -1;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    else if (pipe_flag_read)
    {
        // printf("read\n");
        fd_in = fd_read;
        dup2(fd_read, STDIN_FILENO);
        close(fd_read);
    }

    if (flag_append && out_file)
    {
        fd_out = open(out_file, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
        if (fd_out < 0)
        {
            perror("open output file");
            return -1;
        }
        dup2(fd_out, STDOUT_FILENO);
    }

    else if (flag_write && out_file)
    {
        fd_out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (fd_out < 0)
        {
            perror("open output file");
            return 0;
        }
        dup2(fd_out, STDOUT_FILENO);
    }
    else if (pipe_flag_write)
    {
        fd_out = fd_pipe[1];
        dup2(fd_pipe[1], STDOUT_FILENO);
    }
    char input_command2[COMMAND_PATH];
    strcpy(input_command2, input_command);
    char *command = strtok(input_command, " \t"); // divide input_command with space and /t simple put \0 in place of that; if we pass null instead of input_command then it will take the previous_dirious input_commanday;it returns the first string formed after insertion of the \0;
    if (strcmp(command, "hop") == 0)
    {
        hop_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "reveal") == 0)
    {
        reveal_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "log") == 0)
    {
        log_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "proclore") == 0)
    {
        command = strtok(NULL, " \t");
        if (command != NULL && strcmp(command, ">>") != 0 && strcmp(command, ">") != 0 && strcmp(command, "<") != 0)
            proclore_pro(command);
        else if (flag_read)
        {
            printf("Invalid use of redirection!!\n");
        }
        else
            proclore_pro(NULL);
    }
    else if (strcmp(command, "seek") == 0)
    {
        seek_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "cd") == 0)
    {
        cd_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "activities") == 0)
    {
        update_and_print_processes();
    }
    else if (strcmp(command, "ping") == 0)
    {
        ping_handler(input_command2, flag_read, in_file);
    }
    else if (strcmp(command, "bg") == 0)
    {
        char *temp = strtok(NULL, " \t");
        b_g(atoi(temp));
    }
    else if (strcmp(command, "fg") == 0)
    {
        char *temp = strtok(NULL, " \t");
        fg(atoi(temp), flag);
    }
    else if (strcmp(command, "neonate") == 0)
    {
        strtok(NULL, " \t");
        char* time=strtok(NULL," \t");
        last_pid(atoi(time));
    }
    else if(strcmp(command,"iMan")==0)
    {
        fetch_and_display_man_page(strtok(NULL, " \t"));
    }
    else if(strcmp(command,"nano")==0)
    {
        nano_handler(input_command2,bg,flag);
    }
    else if (strcmp(command, "vim") == 0)
    {
        vim_handler(input_command2,bg,flag);
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
            setpgid(0, 0);
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
            setpgid(pid, pid);
            if (!bg)
            {
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
                add_process(&process_list, pid, input_command2);
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
    if(pipe_flag_write)
    return fd_pipe[0];
    return -1;
}

int main()
{

    begin();
    foreground_pid = -1;
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    while (1)
    {
        print_prompt(home_path, time_exceed_command);
        time_exceed_command[0] = '\0';
        char input[COMMAND_PATH]={0};
        if (scanf("%[^\n]", input) == EOF)
        {
            ctrl_d_handler();
        }
        char c;
        while ((c = getchar()) != '\n' && c != EOF)
        {
            if (c == EOF)
            {
                ctrl_d_handler();
            }
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
        
        if(check_ampersand_pipe(input))
        {
            char* input3=replace_word_in_line(input);
            if(input3!=NULL)
            command_handler(input3);
            free(input3);
        }
        else
        {
            printf("Invalid use of pipe\n");
            continue;
        }
    }
    return 0;
}
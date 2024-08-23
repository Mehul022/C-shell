#include "header.h"

void caller(char *arr, int bg, char *home, char *prev, char *log_path, char *time, int *flag, char *bg_ends)
{
    char arr2[1024];
    strcpy(arr2, arr);
    char *command = strtok(arr, " \t"); // divide arr with space and /t simple put \0 in place of that; if we pass null instead of arr then it will take the previous array;it returns the first string formed after insertion of the \0;

    if (strcmp(command, "hop") == 0)
    {
        char com_store[1024];
        strcpy(com_store, command);
        char temp[PATH_MAX];
        strcpy(temp, prev);
        command = strtok(NULL, " \t");
        while (command != NULL)
        {
            getcwd(prev, PATH_MAX);
            hop(command, home, temp);
            command = strtok(NULL, " \t");
        }
        strcpy(temp, prev);
    }
    else if (strcmp(command, "reveal") == 0)
    {
        char *flags = NULL;
        char *path = NULL;
        char *arg = strtok(NULL, " \t");
        int check = 1;
        while (arg != NULL)
        {
            if (arg[0] == '-' && strlen(arg) > 1)
            {
                for (int x = 0; x < strlen(arg); x++)
                {
                    if (arg[x] != '-' && arg[x] != 'a' && arg[x] != 'l')
                    {
                        check = 0;
                        printf("Invalid flag!!\n");
                        break;
                    }
                }
                if (check)
                {
                    if (flags == NULL)
                    {
                        flags = strdup(arg);
                    }
                    else
                    {
                        char *temp_flags = (char *)malloc(strlen(flags) + strlen(arg) + 1);
                        strcpy(temp_flags, flags);
                        strcat(temp_flags, arg);
                        free(flags);
                        flags = temp_flags;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                path = arg;
            }
            arg = strtok(NULL, " \t");
        }
        if (check)
        {
            handle_reveal(path, flags, home, prev);
        }
        if (flags != NULL)
        {
            free(flags);
        }
    }
    else if (strcmp(command, "log") == 0)
    {
        print_log(log_path);
    }
    else if (strcmp(command, "proclore") == 0)
    {
        command = strtok(NULL, " \t");
        if (command == NULL)
        {
            proclore_self();
        }
        else
        {
            proclore_pro(command);
        }
    }
    else if (strcmp(command, "seek") == 0)
    {
        char *arg = strtok(NULL, " \t");
        int d = 0, f = 0, e = 0;
        while (arg != NULL)
        {
            if (arg[0] == '-' && strlen(arg) > 1)
            {
                if (arg[1] == 'd')
                {
                    d = 1;
                }
                else if (arg[1] == 'f')
                {
                    f = 1;
                }
                else if (arg[1] == 'e')
                {
                    e = 1;
                }
                else
                {
                    printf("Invalid Flag!!\n");
                    return;
                }

                if (d && f)
                {
                    printf("Invalid Flags!!\n");
                    return;
                }
            }
            else
            {
                char search[1024];
                strcpy(search, arg);
                arg = strtok(NULL, " \t");
                if (arg != NULL)
                {
                    char directrix_path[1024];
                    strcpy(directrix_path, arg);
                    char ret[1024];

                    if (seek(d, f, e, search, directrix_path, ret))
                    {
                        getcwd(prev, PATH_MAX);
                        hop(ret, home, prev);
                    }
                }
                else
                {
                    char ret[1024];

                    if (seek(d, f, e, search, NULL, ret))
                    {
                        // printf("%s\n",ret);
                        getcwd(prev,PATH_MAX);
                        hop(ret,home,prev);
                    }
                }
            }
            arg = strtok(NULL, " \t");
        }
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
                // setsid();
                int pid2 = fork();
                if (pid2 < 0)
                {
                    perror("fork failed");
                }
                else if (pid2 == 0)
                {
                    char *args[1024];
                    int i = 0;
                    args[i++] = command;
                    char *arg;
                    while ((arg = strtok(NULL, " \t")) != NULL)
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
                    FILE *file = fopen(bg_ends, "a");
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 0)
                    {
                        fprintf(file, "Command: %s with PID: %d executed successfully\n", arr, getpid());
                    }
                    else
                    {
                        fprintf(file, "Command: %s with PID: %d is failed\n", arr, getpid());
                    }
                    fflush(file);
                    fclose(file);
                    exit(0);
                }
            }
            else
            {
                char *args[1024];
                int i = 0;
                args[i++] = command;
                char *arg;
                while ((arg = strtok(NULL, " \t")) != NULL)
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
                            strcat(time, " / ");
                            strcat(time, arr2);
                        }
                        else
                        {
                            strcpy(time, arr2);
                            *flag = 1;
                        }
                    }
                }
            }
            else
            {
                printf("%d\n", pid);
            }
        }
    }
}
int main()
{

    char arr[PATH_MAX];
    getcwd(arr, sizeof(arr));
    char previous_dir[PATH_MAX];
    strncpy(previous_dir, arr, PATH_MAX);
    char log_path[PATH_MAX];
    snprintf(log_path, sizeof(log_path), "%s%s", arr, "/log.txt");
    char bgends_path[PATH_MAX];
    snprintf(bgends_path, sizeof(bgends_path), "%s%s", arr, "/bg_ends.txt");
    char time_exceed_command[1024];
    time_exceed_command[0] = '\0';
    FILE *file2 = fopen(bgends_path, "w");
    if (file2 == NULL)
    {
        perror("Failed to open file for writing");
    }
    fclose(file2);
    while (1)
    {
        print_prompt(arr, time_exceed_command);
        time_exceed_command[0] = '\0';
        char input[1024];
        scanf("%[^\n]%*c", input);
        input[strlen(input)] = '\0';
        if (strstr(input, "log") == NULL)
        {
            write_log(input, log_path);
        }
        char delimiters[] = ";";
        char *Input = strtok(input, delimiters);
        char com[500][1024];
        int count = 0;
        int flag = 0;
        char buffer[1024];
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
                char inp2[500][1024];
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
                        caller(inp2[y], 0, arr, previous_dir, log_path, time_exceed_command, &flag, bgends_path);
                    }
                    else
                        caller(inp2[y], 1, arr, previous_dir, log_path, time_exceed_command, &flag, bgends_path);
                }
            }
            else
            {
                caller(com[x], 0, arr, previous_dir, log_path, time_exceed_command, &flag, bgends_path);
            }
        }
    }
    return 0;
}

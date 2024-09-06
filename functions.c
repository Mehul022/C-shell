#include "header.h"

char home_path[PATH_MAX];
char previous_dir[PATH_MAX];
char log_path[PATH_MAX];
char bgends_path[PATH_MAX];
char time_exceed_command[COMMAND_PATH];
char myshrc_path[PATH];
char pipe_file[PATH];
char pipe_file_read[PATH];
Process *process_list = NULL;
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
                    flag_pipe_invalid=1;
                }
                else
                {
                    printf("Invalid use of pipe\n");
                    return;
                }
            }
            else if(input[y]!=' ' && input[y]!='\t')
            {
                flag_pipe_invalid=0;
            }
        }
        if(flag_pipe_invalid)
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
        for (int y = 0; y < count2; y++)
        {
            if (y == 0)
            {
                // printf("%s  00\n",inp2[y]);
                if (!caller(inp2[y], bg, flag, 1, 0))
                {
                    break;
                }
            }
            else if (y == count2 - 1 && bg_count != count2)
            {
                // printf("%s  01\n", inp2[y]);
                if (!caller(inp2[y], bg, flag, 0, 1))
                {
                    break;
                }
            }
            else
            {
                // printf("%s  02\n", inp2[y]);
                if (!caller(inp2[y], bg, flag, 1, 1))
                {
                    break;
                }
            }
        }
    }
    else
    {
        caller(input, bg, flag, 0, 0);
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
    strcpy(bgends_path, home_path);
    strcat(bgends_path, "/bg_ends.txt");
    strcpy(myshrc_path, home_path);
    strcat(myshrc_path, "/.myshrc");
    strcpy(pipe_file, home_path);
    strcat(pipe_file, "/.pipe_output.txt");
    strcpy(pipe_file_read, home_path);
    strcat(pipe_file_read, "/.pipe_read.txt");
    time_exceed_command[0] = '\0';
    FILE *file2 = fopen(bgends_path, "w");
    if (file2 == NULL)
    {
        perror("Failed to open file for writing");
    }
    fclose(file2);
}

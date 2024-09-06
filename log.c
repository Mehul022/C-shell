#include "header.h"

void print_log(char* log_path)
{
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }
    char line[COMMAND_PATH];
    while (fgets(line, sizeof(line), file)) // reads untill encounter \0,EOF or sizeof(line) ends; also inlude the \n character in the string
    {
        printf("%s", line);
    }
    fclose(file);
}

void read_commands(char commands[15][COMMAND_PATH], int *count, char* log_path)
{
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        *count = 0;
        return;
    }
    *count = 0;
    while (fgets(commands[*count], COMMAND_PATH, file) != NULL)
    {
        // Remove the newline character
        commands[*count][strcspn(commands[*count], "\n")] = '\0';
        (*count)++;
        if (*count >= 15)
        {
            break;
        }
    }
    fclose(file);
}

char *log_execute(char *log_path, int count)
{
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        return NULL;
    }
    char *command = (char *)malloc(sizeof(char) * COMMAND_PATH);
    if (command == NULL)
    {
        fclose(file);
        return NULL;
    }
    count=15-count +1;
    int flag = 0;
    while (fgets(command, COMMAND_PATH, file) != NULL)
    {
        command[strcspn(command, "\n")] = '\0';
        count--;
        if (count == 0)
        {
            flag = 1;
            break;
        }
    }
    fclose(file);
    if (flag)
    {
        return command; 
    }
    else
    {
        free(command);
        return NULL;
    }
}

void write_commands(char commands[15][COMMAND_PATH], int count,char *log_path)
{
    FILE *file = fopen(log_path, "w");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s\n", commands[i]);
    }
    fflush(file);
    fclose(file);
}

void write_log(char *command, char* log_path)
{
    char commands[15][COMMAND_PATH];
    int count;
    read_commands(commands, &count,log_path);
    if (count > 0 && strcmp(commands[count - 1], command) == 0)
    {
        return;
    }
    if (count >= 15)
    {
        for (int i = 1; i < count; i++)
        {
            strcpy(commands[i - 1], commands[i]);
        }
        count--;
    }
    strcpy(commands[count], command);
    count++;
    write_commands(commands, count,log_path);
}


void log_handler(char *pipe,int flag_read,char* in_file)
{
    char* command=strtok(pipe," \t");
    command = strtok(NULL, " \t");
    if (command == NULL || strcmp(command, ">>") != 0 || strcmp(command, ">") != 0 || strcmp(command, "<") != 0)
        print_log(log_path);
    else if (strcmp("execute", command) == 0)
    {
        command = strtok(NULL, " \t");
        int d = strtod(command, NULL);
        if (d <= 15 && d > 0)
        {
            char *log_command = log_execute(log_path, d);
            command_handler(log_command);
        }
        else
        {
            printf("Invalid number\n");
        }
    }
    else if (strcmp(command, "purge") == 0)
    {
        FILE *fd = fopen(log_path, "w");
        fclose(fd);
    }
    else if (strcmp(command, ">>") != 0 && strcmp(command, ">") != 0 && strcmp(command, "<") != 0)
    {
        printf("Invalid Command\n");
    }
}
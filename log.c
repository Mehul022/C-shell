#include "header.h"

void print_log(char* log_path)
{
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        perror("fopen");
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) // reads untill encounter \0,EOF or sizeof(line) ends; also inlude the \n character in the string
    {
        printf("%s", line);
    }
    fclose(file);
}

void read_commands(char commands[15][1024], int *count, char* log_path)
{
    FILE *file = fopen(log_path, "r");
    if (file == NULL)
    {
        *count = 0;
        return;
    }
    *count = 0;
    while (fgets(commands[*count], 1024, file) != NULL)
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

void write_commands(char commands[15][1024], int count,char *log_path)
{
    FILE *file = fopen(log_path, "w"); // make the file empty as we are opening in w mode not a;
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
    char commands[15][1024];
    int count;
    read_commands(commands, &count,log_path);
    if (count > 0 && strcmp(commands[count - 1], command) == 0)
    {
        return; // as we dont want to append if last command is same as this;
    }
    if (count >= 15)
    {
        for (int i = 1; i < count; i++)
        {
            strcpy(commands[i - 1], commands[i]); // make all the commands shift oneline above
        }
        count--;
    }
    strcpy(commands[count], command);
    count++;
    write_commands(commands, count,log_path);
}

#include "header.h"

char *trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';
    return str;
}
char *replace_word_in_line(char *line)
{
    FILE *file = fopen(myshrc_path, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }
    char new_line[COMMAND_PATH];
    char file_line[COMMAND_PATH];
    new_line[0] = '\0';
    char *token = strtok(line, " \t");
    while (token != NULL)
    {
        char *trimmed_token = trim_whitespace(token);
        char alias[COMMAND_PATH];
        snprintf(alias, sizeof(alias), "alias %s =", trimmed_token);
        char alias2[COMMAND_PATH];
        snprintf(alias2, sizeof(alias2), "%s =", trimmed_token);
        int found = 0;
        rewind(file);
        while (fgets(file_line, sizeof(file_line), file))
        {
            char *line_trimmed = trim_whitespace(file_line);
            if (strncmp(line_trimmed, alias, strlen(alias)) == 0 || strncmp(line_trimmed, alias2, strlen(alias2)) == 0)
            {
                char *equals_sign = strchr(line_trimmed, '=');
                if (equals_sign)
                {
                    char *value = equals_sign + 1;
                    value[strcspn(value, "\n")] = '\0';
                    strcat(new_line, value);
                    strcat(new_line, " ");
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            strcat(new_line, token);
            strcat(new_line, " ");
        }

        token = strtok(NULL, " \t");
    }
    char *final_result = (char *)malloc(strlen(new_line) + 2);
    strcpy(final_result, new_line);
    final_result[strlen(new_line) - 1] = '\0';
    // printf("%s\n", new_line);
    return final_result;
}

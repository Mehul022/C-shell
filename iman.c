#include "header.h"

void fetch_and_display_man_page(char *command)
{
    char url[256];
    snprintf(url, sizeof(url), "http://man.he.net/?topic=%s&section=all", command);
    if (fork() == 0)
    {
        char *args[] = {"wget", "-q", "-O", "/tmp/manpage.html", url, NULL};
        execvp(args[0], args);
        perror("Error: exec failed for wget");
        exit(1);
    }
    wait(NULL);
    FILE *file = fopen("/tmp/manpage.html", "r");
    if (!file)
    {
        perror("Error opening manpage file");
        return;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *html_content = malloc(file_size + 1);
    if (!html_content)
    {
        perror("Memory allocation error");
        fclose(file);
        return;
    }
    fread(html_content, 1, file_size, file);
    fclose(file);
    html_content[file_size] = '\0';
    int inside_tag = 0, last_was_space = 0;
    for (char *src = html_content; *src; src++)
    {
        if (*src == '<')
        {
            inside_tag = 1;
        }
        else if (*src == '>')
        {
            inside_tag = 0;
        }
        else if (!inside_tag)
        {
            if (*src == '\n' || *src == '\r')
            {
                if (!last_was_space)
                    putchar('\n');
                last_was_space = 0;
            }
            else if (*src == ' ' || *src == '\t')
            {
                if (!last_was_space)
                    putchar(' ');
                last_was_space = 1;
            }
            else
            {
                putchar(*src);
                last_was_space = 0;
            }
        }
    }
    free(html_content);
}

#include "header.h"

bool hop(char *command, char *home, char *temp)
{

    if (strcmp(command, "~") == 0)
    {
        if (chdir(home) != 0)
        {
            perror("hop");
            return false;
        }
        else
        {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("getcwd");
            }
            return true;
        }
    }
    else if (strcmp(command, "-") == 0)
    {
        if (chdir(temp) != 0)
        {
            perror("hop");
            return false;
        }
        else
        {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("getcwd");
            }
            return true;
        }
    }
    else if (strcmp(command, "..") == 0 || strcmp(command, ".") == 0)
    {
        if (chdir(command) != 0)
        {
            perror("hop");
            return false;
        }
        else
        {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("getcwd");
            }
            return true;
        }
    }
    else
    {
        if (command[0] == '~')
        {
            char expanded_path[1024];
            snprintf(expanded_path, sizeof(expanded_path), "%s%s", home, command + 1); // combine 2 string and store it in the first argument;
            if (chdir(expanded_path) != 0)
            {
                perror("hop");
                return false;
            }
            else
            {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                {
                    printf("%s\n", cwd);
                }
                else
                {
                    perror("getcwd");
                }
                return true;
            }
        }
        else
        {
            if (chdir(command) != 0)
            {
                perror("hop");
                return false;
            }
            else
            {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL)
                {
                    printf("%s\n", cwd);
                }
                else
                {
                    perror("getcwd");
                }
                return true;
            }
        }
    }
}
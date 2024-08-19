#include "header.h"
#include <sys/utsname.h>

void print_prompt(char *arr,char *time)
{
    char cwd[PATH_MAX];      // defining a array of pathmax size which is the maximum size of any directrix;
    char *user = getlogin(); // returns a pointer to string which contain current terminal login id.
    struct utsname sys_info; // defining a variable of utsname struct type which contain sysname,nodename,version ,machine,relaese;
    if (getcwd(cwd, sizeof(cwd)) != NULL && user != NULL && uname(&sys_info) == 0)
    {
        if (strlen(cwd) >= strlen(arr))
        {
            char check[strlen(arr) + 1];
            int x;
            for (x = 0; x < strlen(arr); x++)
            {
                check[x] = cwd[x];
            }
            check[x] = '\0';
            if (strcmp(check, arr) == 0)
            {
                char p[PATH_MAX];
                int h = 0;
                for (; x < strlen(cwd); x++)
                {
                    p[h++] = cwd[x];
                }
                p[h++] = '\0';
                printf("\033[31m%s\033[0m@"
                       "\033[32m%s\033[0m:"
                       "\033[34m~%s\033[0m%s> ",
                       user, sys_info.sysname, p,time);
            }
            else
                printf("\033[31m%s\033[0m@"
                       "\033[32m%s\033[0m:~"
                       "\033[34m%s\033[0m%s> ",
                       user, sys_info.sysname, cwd,time);
        }
        else
            printf("\033[31m%s\033[0m@"
                   "\033[32m%s\033[0m:"
                   "\033[34m%s\033[0m%s> ",
                   user, sys_info.sysname, cwd,time);
    }
    else
    {
        perror("Error retrieving user or system name or directory");
    }
}

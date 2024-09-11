#include "header.h"

void ping_handler(char *command, int flag_read, char *in_file)
{
    if (!flag_read)
    {
        char *arg = strtok(command, " \t");
        arg = strtok(NULL, " \t");
        pid_t pid;
        if (arg != NULL)
        {
            pid = atoi(arg);
        }
        else
        {
            printf("Incomplete Command!!\n");
            return;
        }
        arg = strtok(NULL, " \t");
        int signal;
        if (arg != NULL)
        {
            signal = atoi(arg);
        }
        else
        {
            printf("Incomplete Command!!\n");
            return;
        }
        signal = signal % 32;
        if (kill(pid, 0) == -1)
        {
            if (errno == ESRCH)
            {
                printf("Process with PID %d does not exist.\n", pid);
            }
            else
            {
                printf("Process with PID %d exists, but you do not have permission to send a signal.\n", pid);
            }
        }
        else
        {
            // printf("%d\n",signal);
            if (kill(pid, signal) == -1)
            {
                perror("Error sending signal");
            }
        }
    }
    else
    {
        // FILE *file;
        // char line[COMMAND_PATH];
        // file = fopen(in_file, "r");
        // if (file == NULL)
        // {
        //     perror("Error opening file");
        //     return;
        // }
        // else
        // {
        //     while (fgets(line, sizeof(line), file))
        //     {
        //         if (line[strlen(line) - 1] == '\n')
        //         {
        //             line[strlen(line) - 1] = '\0';
        //         }
        //         else
        //         {
        //             line[strlen(line)] = '\0';
        //         }
        //         char *line2 = strtok(line, " \t");
        //         while (line2 != NULL)
        //         {
        //             pid_t pid;
        //             if (line2 != NULL)
        //             {
        //                 pid = atoi(line2);
        //             }
        //             else
        //             {
        //                 printf("Incomplete Command!!\n");
        //                 return;
        //             }
        //             line2 = strtok(NULL, " \t");
        //             int signal;
        //             if (line2 != NULL)
        //             {
        //                 signal = atoi(line2);
        //             }
        //             else
        //             {
        //                 printf("Incomplete Command!!\n");
        //                 return;
        //             }
        //             signal = signal % 32;
        //             if (kill(pid, 0) == -1)
        //             {
        //                 if (errno == ESRCH)
        //                 {
        //                     printf("Process with PID %d does not exist.\n", pid);
        //                 }
        //                 else
        //                 {
        //                     printf("Process with PID %d exists, but you do not have permission to send a signal.\n", pid);
        //                 }
        //             }
        //             else
        //             {
        //                 if (kill(pid, signal) == -1)
        //                 {
        //                     perror("Error sending signal");
        //                 }
        //             }
        //         }
        //     }
        // }
        printf("Invalid use of redirection!!\n");
    }
}
#ifndef ACTIVITIES_H
#define ACTIVITIES_H
#include"header.h"
typedef struct Process
{
    int pid;
    char command[4097];
    struct Process *next;
} Process;
Process *create_process(int pid, const char *command);
void add_process(Process **head, int pid, const char *command);
char* remove_process(Process **head, int pid);
int process_exists(int pid, char *state);
void update_and_print_processes();
bool process_exist_list(int pid);
#endif
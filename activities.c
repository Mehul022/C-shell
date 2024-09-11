#include "header.h"

Process *create_process(int pid, const char *command)
{
    Process *new_process = (Process *)malloc(sizeof(Process));
    if (!new_process)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    new_process->pid = pid;
    strncpy(new_process->command, command, COMMAND_PATH);
    new_process->command[COMMAND_PATH - 1] = '\0';
    new_process->next = NULL;
    return new_process;
}

void add_process(Process **head, int pid, const char *command)
{
    // printf("%d\n",pid);
    Process *new_process = create_process(pid, command);
    if (*head == NULL || (*head)->pid > pid)
    {
        new_process->next = *head;
        *head = new_process;
        return;
    }
    Process *current = *head;
    while (current->next != NULL && current->next->pid < pid)
    {
        current = current->next;
    }
    new_process->next = current->next;
    current->next = new_process;
}

char *remove_process(Process **head, int pid)
{
    Process *current = *head;
    Process *previous = NULL;

    while (current != NULL && current->pid != pid)
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("Process with PID %d not found.\n", pid);
        return NULL;
    }

    char *command = strdup(current->command); 
    if (previous == NULL)
    {
        *head = current->next;
    }
    else
    {
        previous->next = current->next;
    }

    free(current);
    return command;
}

bool process_exist_list(int pid)
{
    // printf("%d\n",pid);
    Process *temp=process_list;
    while(temp!=NULL)
    {
        if(temp->pid==pid)
        {
            return true;
        }
        printf("%d\n",temp->pid);
        fflush(stdout);
        temp=temp->next;
    }
    return false;
}

int process_exists(int pid, char *state)
{
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *status_file = fopen(path, "r");
    if (status_file == NULL)
    {
        return 0;
    }
    char line[256];
    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            sscanf(line, "State: %s", state);
            break;
        }
    }
    fclose(status_file);
    return 1;
}

void update_and_print_processes()
{
    Process *current = process_list;
    Process *previous = NULL;
    char state[32]; 
    while (current != NULL)
    {
        if (process_exists(current->pid, state))
        {
            if (strcmp(state, "T") == 0)
            {
                printf("%d : %s - Stopped\n", current->pid, current->command);
                previous = current;
                current = current->next;
            }
            else if(strcmp(state,"Z")==0)
            {
                // printf("maa ki chut\n");
                int pid=current->pid;
                current = current->next;
                remove_process(&process_list, pid);
            }
            else
            {
                printf("%d : %s - Running\n", current->pid, current->command);
                previous = current;
                current = current->next;
            }
        }
        else
        {
            int pid = current->pid;
            current = current->next;
            remove_process(&process_list,pid);
        }
    }
}

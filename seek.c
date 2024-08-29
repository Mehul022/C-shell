#include "header.h"

typedef struct Node
{
    char path[1024];
    struct Node *next;
} Node;

void enqueue(Node **head, const char *path)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    strcpy(new_node->path, path);
    new_node->next = NULL;
    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        Node *temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

int is_executable(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
    {
        return 0;
    }
    return (statbuf.st_mode & S_IXUSR);
}

char *dequeue(Node **head)
{
    if (*head == NULL)
        return NULL;
    Node *temp = *head;
    *head = (*head)->next;
    char *path = (char *)malloc(1024 * sizeof(char));
    strcpy(path, temp->path);
    free(temp);
    return path;
}
int is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
    {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

bool seek(int l, int f, int e, char *search, char *direc ,char* store_path)
{
    int n = strlen(search);
    if (l)
    {
        Node *queue = NULL;
        char start_path[1024];
        if (direc == NULL)
        {
            getcwd(start_path, sizeof(start_path));
        }
        else
        {
            char temp[1024];
            getcwd(temp,sizeof(temp));
            snprintf(start_path,sizeof(start_path),"%s%s",temp,direc+1);
        }
        enqueue(&queue, start_path);
        int count = 0;
        while (queue != NULL)
        {
            char *current_path = dequeue(&queue);
            DIR *dp = opendir(current_path);
            if (dp == NULL)
            {
                perror("opendir");
                free(current_path);
                continue;
            }
            struct dirent *entry;
            while ((entry = readdir(dp)) != NULL)
            {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (is_directory(new_path) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    enqueue(&queue, new_path);
                    if (strncmp(search, entry->d_name, n) == 0)
                    {
                        count++;
                        char relative_path[1024];
                        snprintf(store_path,1024, "%s/%s", current_path, entry->d_name);
                        // printf("%s\n",current_path);
                        if (strcmp(current_path, start_path) == 0)
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s", entry->d_name);
                        }
                        else
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s/%s", current_path + strlen(start_path) + 1, entry->d_name);
                        }
                        printf("\033[34m%s\033[0m\n", relative_path);
                    }
                }
            }
            closedir(dp);
            free(current_path);
        }
        // printf("%d\n",count);
        if (e && count == 1)
        {
            return true ;
        }
    }
    else if (f)
    {
        Node *queue = NULL;
        char start_path[1024];
        getcwd(start_path, sizeof(start_path));
        enqueue(&queue, start_path);
        int count = 0;
        while (queue != NULL)
        {
            char *current_path = dequeue(&queue);
            DIR *dp = opendir(current_path);
            if (dp == NULL)
            {
                perror("opendir");
                free(current_path);
                continue;
            }
            struct dirent *entry;
            while ((entry = readdir(dp)) != NULL)
            {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    if (is_directory(new_path))
                    {
                        enqueue(&queue, new_path);
                    }
                    else if (strncmp(search, entry->d_name, n) == 0)
                    {
                        char relative_path[1024];
                        if (strcmp(current_path, start_path) == 0)
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s", entry->d_name);
                        }
                        else
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s/%s", current_path + strlen(start_path) + 1, entry->d_name);
                        }
                        if (!is_executable(new_path))
                        {
                            snprintf(store_path, 1024, "%s/%s", current_path, entry->d_name);
                            count++;
                            printf("\033[32m%s\033[0m\n", relative_path);
                        }
                    }
                }
            }
            closedir(dp);
            free(current_path);
        }
        if (e && count == 1)
        {
            FILE *file = fopen(store_path, "r");
            if (file == NULL)
            {
                perror("fopen");
                return false;
            }
            char line[1024];
            while (fgets(line, sizeof(line), file) != NULL)
            {
                printf("%s", line);
            }
            fclose(file);
        }
    }
    else
    {
        Node *queue = NULL;
        char start_path[1024];
        getcwd(start_path, sizeof(start_path));
        enqueue(&queue, start_path);
        int count = 0;
        while (queue != NULL)
        {
            char *current_path = dequeue(&queue);
            DIR *dp = opendir(current_path);
            if (dp == NULL)
            {
                perror("opendir");
                free(current_path);
                continue;
            }
            struct dirent *entry;
            while ((entry = readdir(dp)) != NULL)
            {
                char new_path[1024];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    if (is_directory(new_path))
                    {
                        enqueue(&queue, new_path);
                    }
                    if (strncmp(search, entry->d_name, n) == 0)
                    {
                        char relative_path[1024];
                        if (strcmp(current_path, start_path) == 0)
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s", entry->d_name);
                        }
                        else
                        {
                            snprintf(relative_path, sizeof(relative_path), "./%s/%s", current_path + strlen(start_path) + 1, entry->d_name);
                        }
                        if (is_directory(new_path))
                        {
                            snprintf(store_path, 1024, "%s/%s", current_path, entry->d_name);
                            count++;
                            printf("\033[34m%s\033[0m\n", relative_path);
                        }
                        else if (!is_executable(new_path))
                        {
                            snprintf(store_path, 1024, "%s/%s", current_path, entry->d_name);
                            count++;
                            printf("\033[32m%s\033[0m\n", relative_path);
                        }
                    }
                }
            }
            closedir(dp);
            free(current_path);
        }
        if (e && count == 1)
        {
            if (!is_directory(store_path))
            {
                FILE *file = fopen(store_path, "r");
                if (file == NULL)
                {
                    perror("fopen");
                    return false;
                }
                char line[1024];
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    printf("%s", line);
                }
                fclose(file);
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}

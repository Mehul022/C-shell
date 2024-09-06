#include "header.h"

typedef struct Node
{
    char path[PATH];
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
    char *path = (char *)malloc(PATH * sizeof(char));
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

bool seek(int l, int f, int e, char *search, char *direc, char *store_path)
{
    int n = strlen(search);
    char start_path[PATH];
    if (direc == NULL)
    {
        getcwd(start_path, sizeof(start_path));
    }
    else
    {
        if (strcmp(direc, "~") == 0)
        {
            getcwd(start_path, sizeof(start_path));
        }
        else if (strcmp(direc, "-") == 0)
        {
            strcpy(start_path, previous_dir);
        }
        else if (strcmp(direc, "..") == 0 || strcmp(direc, ".") == 0)
        {
            strcpy(start_path, direc);
        }
        else
        {
            char temp[PATH];
            getcwd(temp, sizeof(temp));
            snprintf(start_path, sizeof(start_path), "%s%s", temp, direc + 1);
        }
    }
    if (l)
    {
        Node *queue = NULL;
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
                char new_path[PATH];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (is_directory(new_path) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    enqueue(&queue, new_path);
                    if (strncmp(search, entry->d_name, n) == 0)
                    {
                        count++;
                        char relative_path[PATH];
                        snprintf(store_path, PATH, "%s/%s", current_path, entry->d_name);
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
        if (e && count == 1)
        {
            return true;
        }
        else if (count == 0)
        {
            printf("No Match found!\n");
        }
    }
    else if (f)
    {
        Node *queue = NULL;
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
                char new_path[PATH];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    if (is_directory(new_path))
                    {
                        enqueue(&queue, new_path);
                    }
                    else if (strncmp(search, entry->d_name, n) == 0)
                    {
                        char relative_path[PATH];
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
                            snprintf(store_path, PATH, "%s/%s", current_path, entry->d_name);
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
                printf("Missing permissions for task!\n");
                return false;
            }
            char line[PATH];
            while (fgets(line, sizeof(line), file) != NULL)
            {
                printf("%s", line);
            }
            fclose(file);
        }
        else if (count == 0)
        {
            printf("No Match found!\n");
        }
    }
    else
    {
        Node *queue = NULL;
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
                char new_path[PATH];
                snprintf(new_path, sizeof(new_path), "%s/%s", current_path, entry->d_name);
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    if (is_directory(new_path))
                    {
                        enqueue(&queue, new_path);
                    }
                    if (strncmp(search, entry->d_name, n) == 0)
                    {
                        char relative_path[PATH];
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
                            snprintf(store_path, PATH, "%s/%s", current_path, entry->d_name);
                            count++;
                            printf("\033[34m%s\033[0m\n", relative_path);
                        }
                        else if (!is_executable(new_path))
                        {
                            snprintf(store_path, PATH, "%s/%s", current_path, entry->d_name);
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
                    printf("Missing permissions for task!\n");
                    return false;
                }
                char line[PATH];
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
        else if (count == 0)
        {
            printf("No Match found!\n");
        }
    }
    return false;
}

void seek_handler(char *pipe, int flag_read, char *in_file)
{
    int d = 0, f = 0, e = 0;
    if (!flag_read)
    {
        char *arg = strtok(pipe, " \t");
        arg = strtok(NULL, " \t");
        while (arg != NULL && strcmp(arg, ">>") != 0 && strcmp(arg, ">") != 0 && strcmp(arg, "<") != 0)
        {
            if (arg[0] == '-' && strlen(arg) > 1)
            {
                if (arg[1] == 'd')
                {
                    d = 1;
                }
                else if (arg[1] == 'f')
                {
                    f = 1;
                }
                else if (arg[1] == 'e')
                {
                    e = 1;
                }
                else
                {
                    printf("Invalid Flag!!\n");
                    return;
                }

                if (d && f)
                {
                    printf("Invalid Flags!!\n");
                    return;
                }
            }
            else
            {
                char search[COMMAND_PATH];
                strcpy(search, arg);
                arg = strtok(NULL, " \t");
                if (arg != NULL && strcmp(arg, ">>") != 0 && strcmp(arg, ">") != 0 && strcmp(arg, "<") != 0)
                {
                    char directrix_path[COMMAND_PATH];
                    strcpy(directrix_path, arg);
                    char ret[COMMAND_PATH];
                    if (seek(d, f, e, search, directrix_path, ret))
                    {
                        char previous_dir2[PATH];
                        getcwd(previous_dir2, PATH);
                        hop(ret);
                        char previous_dir3[PATH];
                        getcwd(previous_dir3, PATH);
                        if (strcmp(previous_dir2, previous_dir3) != 0)
                        {
                            strcpy(previous_dir, previous_dir2);
                        }
                    }
                }
                else
                {
                    char ret[COMMAND_PATH];
                    if (seek(d, f, e, search, NULL, ret))
                    {
                        char previous_dir2[PATH];
                        getcwd(previous_dir2, PATH);
                        char previous_dir3[PATH];
                        getcwd(previous_dir3, PATH);
                        if (strcmp(previous_dir2, previous_dir3) != 0)
                        {
                            strcpy(previous_dir, previous_dir2);
                        }
                    }
                }
            }
            arg = strtok(NULL, " \t");
        }
    }
    else
    {
        FILE *file;
        char line[COMMAND_PATH];
        file = fopen(in_file, "r");
        if (file == NULL)
        {
            perror("Error opening file");
            return;
        }
        else
        {
            while (fgets(line, sizeof(line), file))
            {
                if (line[strlen(line) - 1] == '\n')
                {
                    line[strlen(line) - 1] = '\0';
                }
                else
                {
                    line[strlen(line)] = '\0';
                }
                char *line2 = strtok(line, " \t");
                while (line2 != NULL)
                {
                    if (line2[0] == '-' && strlen(line2) > 1)
                    {
                        if (line2[1] == 'd')
                        {
                            d = 1;
                        }
                        else if (line2[1] == 'f')
                        {
                            f = 1;
                        }
                        else if (line2[1] == 'e')
                        {
                            e = 1;
                        }
                        else
                        {
                            printf("Invalid Flag!!\n");
                            return;
                        }

                        if (d && f)
                        {
                            printf("Invalid Flags!!\n");
                            return;
                        }
                    }
                    else
                    {
                        char search[COMMAND_PATH];
                        strcpy(search, line2);
                        line2 = strtok(NULL, " \t");
                        if (line2 != NULL && strcmp(line2, ">>") != 0 && strcmp(line2, ">") != 0 && strcmp(line2, "<") != 0)
                        {
                            char directrix_path[COMMAND_PATH];
                            strcpy(directrix_path, line2);
                            char ret[COMMAND_PATH];
                            if (seek(d, f, e, search, directrix_path, ret))
                            {
                                char previous_dir2[PATH];
                                getcwd(previous_dir2, PATH);
                                hop(ret);
                                char previous_dir3[PATH];
                                getcwd(previous_dir3, PATH);
                                if (strcmp(previous_dir2, previous_dir3) != 0)
                                {
                                    strcpy(previous_dir, previous_dir2);
                                }
                            }
                        }
                        else
                        {
                            char ret[COMMAND_PATH];
                            if (seek(d, f, e, search, NULL, ret))
                            {
                                char previous_dir2[PATH];
                                getcwd(previous_dir2, PATH);
                                char previous_dir3[PATH];
                                getcwd(previous_dir3, PATH);
                                if (strcmp(previous_dir2, previous_dir3) != 0)
                                {
                                    strcpy(previous_dir, previous_dir2);
                                }
                            }
                        }
                    }
                    line2 = strtok(NULL, " \t");
                }
            }
        }
    }
}
#include "header.h"
void print_permissions(mode_t mode)
{
    char permissions[11];
    if (S_ISDIR(mode))
        permissions[0] = 'd'; // Directory
    else if (S_ISLNK(mode))
        permissions[0] = 'l'; // Symbolic link
    else if (S_ISCHR(mode))
        permissions[0] = 'c'; // Character device
    else if (S_ISBLK(mode))
        permissions[0] = 'b'; // Block device
    else if (S_ISFIFO(mode))
        permissions[0] = 'p'; // FIFO
    else if (S_ISSOCK(mode))
        permissions[0] = 's'; // Socket
    else
        permissions[0] = '-'; // Regular file
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';

    permissions[10] = '\0';

    printf("%s ", permissions);
}

void print_file_type(const struct stat *file_info)
{
    if (S_ISDIR(file_info->st_mode))
    {
        printf("\033[34m"); // Blue
    }
    else if (S_ISREG(file_info->st_mode) && (file_info->st_mode & S_IXUSR)) // S_ISREG for checking regular file not a directrix or link and second one is checking hweather we have executing permission or not if yes then it is executable.
    {
        printf("\033[32m"); // Green
    }
    else
    {
        printf("\033[37m"); // White
    }
}
void print_extra_info(const struct stat *file_info, const char *filename)
{
    char time_buf[20];
    struct tm *tm_info;
    struct passwd *pw;
    struct group *gr;
    print_permissions(file_info->st_mode);
    printf(" %3ld ", file_info->st_nlink);
    pw = getpwuid(file_info->st_uid);
    if (pw)
    {
        printf("%-8s ", pw->pw_name);
    }
    else
    {
        printf("%-8d ", file_info->st_uid);
    }
    gr = getgrgid(file_info->st_gid);
    if (gr)
    {
        printf("%-8s ", gr->gr_name);
    }
    else
    {
        printf("%-8d ", file_info->st_gid);
    }
    printf("%6ld ", file_info->st_size);
    tm_info = localtime(&file_info->st_mtime); // st_mtime last modified time;
    time_t current_time = time(NULL);
    if (difftime(current_time, file_info->st_mtime) < 6 * 30 * 24 * 60 * 60)
    {
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", tm_info);
    }
    else
    {
        strftime(time_buf, sizeof(time_buf), "%b %d  %Y", tm_info);
    }
    printf("%s ", time_buf);
    print_file_type(file_info);
    printf("%s\033[0m\n", filename);
}

int compare_filenames(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

bool handle_reveal(char *path, char *flags, char *home, char *prev)
{
    if (flags == NULL)
    {
        if (path == NULL)
        {
            return (reveal("."));
        }
        else if (strcmp(path, "~") == 0)
        {
            return (reveal(home));
        }
        else if (strcmp(path, "-") == 0)
        {
            return (reveal(prev));
        }
        else
        {
            if (path[0] == '~')
            {
                char expanded_path[1024];
                snprintf(expanded_path, sizeof(expanded_path), "%s%s", home, path + 1);
                return (reveal(expanded_path));
            }
            else
            {
                return (reveal(path));
            }
        }
    }
    else
    {
        if (path == NULL)
        {
            return (revealf(".", flags));
        }
        else if (strcmp(path, "~") == 0)
        {
            return (revealf(home, flags));
        }
        else if (strcmp(path, "-") == 0)
        {
            return (revealf(prev, flags));
        }
        else
        {
            if (path[0] == '~')
            {
                char expanded_path[1024];
                snprintf(expanded_path, sizeof(expanded_path), "%s%s", home, path + 1);
                return (revealf(expanded_path, flags));
            }
            else
            {
                return (revealf(path, flags));
            }
        }
    }
}

bool reveal(char *dir_path)
{
    struct dirent *entry;  // contain file serial number and filename
    struct stat file_info; // conatain multiple things about file including file permission owner id modified time and many more.
    DIR *dp = opendir(dir_path);
    char *filenames[1024];
    int file_count = 0;

    if (dp == NULL)
    {
        perror("opendir");
        return false;
    }
    while ((entry = readdir(dp)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && entry->d_name[0] != '.')
        {
            filenames[file_count] = strdup(entry->d_name);
            file_count++;
        }
    }
    closedir(dp);
    
    qsort(filenames, file_count, sizeof(char *), compare_filenames);
    for (int i = 0; i < file_count; i++)
    {
        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, filenames[i]);
        if (stat(file_path, &file_info) == -1)
        {
            perror("stat");
            continue;
        }
        print_file_type(&file_info);
        printf("%s\033[0m\t", filenames[i]);
        free(filenames[i]);
    }
    printf("\n");
    return true;
}

bool revealf(char *dir_path, char *flags)
{
    if (strchr(flags, 'a') != NULL)
    {
        struct dirent *entry;
        struct stat file_info;
        DIR *dp = opendir(dir_path);
        char *filenames[1024];
        int file_count = 0;

        if (dp == NULL)
        {
            perror("opendir");
            return false;
        }
        while ((entry = readdir(dp)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                filenames[file_count] = strdup(entry->d_name);
                file_count++;
            }
        }
        closedir(dp);
        qsort(filenames, file_count, sizeof(char *), compare_filenames);
        if (strchr(flags, 'l') != NULL)
        {
            int total_blocks = 0;
            dp = opendir(dir_path);
            if (dp == NULL)
            {
                perror("opendir");
                return false;
            }
            while ((entry = readdir(dp)) != NULL)
            {
                struct stat file_stat;
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", ".", entry->d_name);
                if (stat(path, &file_stat) == 0)
                {
                    if (S_ISREG(file_stat.st_mode) || S_ISLNK(file_stat.st_mode))
                    {
                        total_blocks += file_stat.st_blocks;
                    }
                }
                else
                {
                    perror("stat");
                }
            }
            printf("total %d\n",total_blocks);
            for (int i = 0; i < file_count; i++)
            {
                char file_path[PATH_MAX];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, filenames[i]);
                if (stat(file_path, &file_info) == -1)
                {
                    perror("stat");
                    continue;
                }

                print_extra_info(&file_info, filenames[i]);
                free(filenames[i]); // Free allocated memory
            }
        }
        else
        {
            for (int i = 0; i < file_count; i++)
            {
                char file_path[PATH_MAX];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, filenames[i]);
                if (stat(file_path, &file_info) == -1)
                {
                    perror("stat");
                    continue;
                }
                print_file_type(&file_info);
                printf("%s\033[0m\t", filenames[i]); // Reset color
                free(filenames[i]);                  // Free allocated memory
            }
            printf("\n");
        }
    }
    else
    {
        struct dirent *entry;
        struct stat file_info;
        DIR *dp = opendir(dir_path);
        char *filenames[1024];
        int file_count = 0;

        if (dp == NULL)
        {
            perror("opendir");
            return false;
        }
        while ((entry = readdir(dp)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && entry->d_name[0] != '.')
            {
                filenames[file_count] = strdup(entry->d_name);
                file_count++;
            }
        }
        closedir(dp);
        qsort(filenames, file_count, sizeof(char *), compare_filenames);
        if (strchr(flags, 'l') != NULL)
        {
            int total_blocks = 0;
            dp = opendir(dir_path);
            if (dp == NULL)
            {
                perror("opendir");
                return false;
            }
            while ((entry = readdir(dp)) != NULL)
            {
                struct stat file_stat;
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", ".", entry->d_name);
                if (stat(path, &file_stat) == 0)
                {
                    if (S_ISREG(file_stat.st_mode) || S_ISLNK(file_stat.st_mode) || S_ISDIR(file_stat.st_mode))
                    {
                        total_blocks += file_stat.st_blocks;
                    }
                }
                else
                {
                    perror("stat");
                }
            }
            printf("total %d\n", total_blocks/2);
            for (int i = 0; i < file_count; i++)
            {
                char file_path[PATH_MAX];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, filenames[i]);
                if (stat(file_path, &file_info) == -1)
                {
                    perror("stat");
                    continue;
                }

                print_extra_info(&file_info, filenames[i]);
                free(filenames[i]); // Free allocated memory
            }
        }
        else
        {
            for (int i = 0; i < file_count; i++)
            {
                char file_path[PATH_MAX];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, filenames[i]);
                if (stat(file_path, &file_info) == -1)
                {
                    perror("stat");
                    continue;
                }
                print_file_type(&file_info);
                printf("%s\033[0m\t", filenames[i]); // Reset color
                free(filenames[i]);                  // Free allocated memory
            }
            printf("\n");
        }
    }
    return true;
}
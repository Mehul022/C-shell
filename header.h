#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h> 
#include <sys/wait.h>
#include <dirent.h>
#include <linux/limits.h>     
#include <time.h>   
#include <sys/utsname.h> 
#include <dirent.h>
#include<paths.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <fcntl.h>
#include "proclore.h"
#include "seek.h"
#include "ls.h"
#include "change_directrix.h"
#include "log.h"
#include "print_promt.h"
#include"main.h"
#include"functions.h"
#include"activities.h"
#define PATH 1025
#define COMMAND_PATH 4097
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define WHITE "\033[37m"
#define ORIGINAL "\033[0m"
extern char home_path[PATH_MAX];
extern char previous_dir[PATH_MAX];
extern char log_path[PATH_MAX];
extern char bgends_path[PATH_MAX];
extern char time_exceed_command[COMMAND_PATH]; // for printing the foreground command that takes more than 2 sec integral value
extern char myshrc_path[PATH];
extern char pipe_file[PATH];
extern char pipe_file_read[PATH];
extern Process *process_list;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>   // For timeval, timezone, itimerval, clock_gettime, etc.
#include <sys/wait.h>
#include <dirent.h>
#include <linux/limits.h>     // For PATH_MAX, instead of linux/limits.h
#include <time.h>         // For clock_gettime, CLOCK_MONOTONIC, localtime, strftime
#include <sys/utsname.h>  // For struct utsname and uname function
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
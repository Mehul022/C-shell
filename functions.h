#ifndef FUNCTIONS_H
#define FUNCTIONS_H
char *decode_path(char *command);
void pipe_caller(char *input, int bg, int *flag);
void command_handler(char *input);
void begin();
void sigint_handler(int sig);
void ctrl_d_handler();
void sigtstp_handler(int sig);
void sigchld_handler(int signo);
void vim_handler(char *input, int bg, int *flag);
void nano_handler(char *input, int bg, int *flag);
int check_ampersand_pipe(char *str);
#endif
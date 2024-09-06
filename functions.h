#ifndef FUNCTIONS_H
#define FUNCTIONS_H
char *decode_path(char *command);
void pipe_caller(char *input, int bg, int *flag);
void command_handler(char *input);
void begin();
#endif
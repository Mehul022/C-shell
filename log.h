#ifndef LOG_H
#define LOG_H
void log_handler(char *pipe, int flag_read, char *in_file);
void print_log(char *log_path);
void write_log(char *command,char* log_path);
char* log_execute(char* log_path,int count);
#endif

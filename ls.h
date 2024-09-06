#ifndef LS_H
#define LS_H
bool reveal(char *dir_path);
bool revealf(char *dir_path,char *flags);
bool handle_reveal(char *path, char *flags);
void reveal_handler(char *pipe, int flag_read, char *in_file);
#endif 
#ifndef CD_H
#define CD_H
bool hop(char *command);
bool cd(char *command);
void hop_handler(char *pipe, int flag_read, char *in_file);
void cd_handler(char *pipe, int flag_read, char *in_file);
#endif

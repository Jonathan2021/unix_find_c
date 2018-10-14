#ifndef _USEFUL_H_
#define _USEFUL_H_

void __attribute__((noreturn)) error(const char* msg); // exit code 1
void __attribute__((noreturn)) fail(const char* msg); // perror, exit code errno

int my_strcmp(const char *a, const char *b);
int get_size(const char *str);
void append_string(char *dest, char *first, char *second);
char *my_strdup(const char* str);
char *get_fullpath(char *path, char *file);

#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "useful.h"

void __attribute__((noreturn)) error(const char* msg) {
  fprintf(stderr, "error: %s\n", msg);
  exit(1);
}

void __attribute__((noreturn)) fail(const char* msg) {
  perror(msg);
  exit(errno);
}
void append_string(char *dest, char *first, char *second)
{
    int i = 0;
    int j = 0;
    for(; first[i] != '\0'; ++i)
        dest[i] = first[i];
    for(; second[j] != '\0'; ++j)
        dest[i+j] = second[j];
    dest[i+j] = '\0';
}

int get_size(const char *str)
{
    int i = 0;
    for(; str[i]; ++i);
    return i+1;
}
char *get_fullpath(char *path, char *file)
{
    char *res = malloc(get_size(path) + get_size(file));
    if(!res)
    {
        fail("malloc");
        return NULL;
    }
    int i = 0;
    int j = 0;
    for(; path[i] != 0; ++i)
        res[i] = path[i];
    res[i++] = '/';
    for(; file[j] != 0; ++j)
        res[i+j] = file[j];
    res[i+j] = 0;
    return res;
}

int my_strcmp(const char *a, const char *b)
{
    size_t i = 0;
    for(; a[i] != '\0' && b[i] != '\0'; ++i)
    {
        if(a[i] != b[i])
            return 1;
    }
    return !(a[i] == b[i]);
}

char* my_strdup(const char* str) {
    int size = get_size(str);
    char *res = malloc(size);
    if (!res)
        fail("malloc");
    for (int i = 0; i < size; ++i)
        res[i] = str[i];
    return res;
}

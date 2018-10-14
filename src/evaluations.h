#ifndef _EVAL_H_
#define _EVAL_H_

#include "parse_tree.h"

int my_delete(char *path);
int print_path(char *path, char *file);
int name_match(struct node *n, char *file_name);
int perm(int fd, struct node *n);
int is_user(int fd, struct node *n);
int is_group(int fd, struct node *n);
int is_newer(int fd, struct node *n);
int my_exec(struct node *n, char *path);
int my_execdir(struct node *n, char *path, char *file);
int my_type(struct node *n, int fd);

#endif

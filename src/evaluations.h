#ifndef _EVAL_H_
#define _EVAL_H_

#include "parse_tree.h"

int my_delete(char *path);
int print_path(char *path, char *file);
int name_match(struct node *n, char *file_name);
int perm(char *path, struct node *n);
int is_user(char *path, struct node *n);
int is_group(char *path, struct node *n);
int is_newer(char *path, struct node *n);
int my_exec(struct node *n, char *path);
int my_execdir(struct node *n, char *path, char *file);
int my_type(struct node *n, char *path);

#endif

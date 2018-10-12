#ifndef _PARSE_TREE_H_
#define _PARSE_TREE_H_

enum type
{
    TYPE = 0,
    NAME,
    PRINT,
    EXEC,
    EXECDIR,
    DELETE,
    PERM,
    USER,
    GROUP,
    NEWER,
    AND,
    OR,
    TRUE,
    NOT_VALID
};

struct node
{
    int barre;
    enum type type;
    char *arg;
    struct node *left; 
    struct node *right;
};

typedef int (*handler)(struct node);

#endif

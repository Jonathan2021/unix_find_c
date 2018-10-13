#ifndef _PARSE_TREE_H_
#define _PARSE_TREE_H_
#define COUNT 10
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
    const char *arg;
    struct node *left; 
    struct node *right;
    char is_plus;
};

struct node *build_tree(char *exp[], int len, int par, int *end);
void free_tree(struct node *root);
void print2D(struct node *root);

typedef int (*node_handler)(struct node);

#endif

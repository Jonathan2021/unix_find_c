#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"

//int print()

//int evaluate tree(struct node *root)
//{
    
//}

struct node *init_node(void)
{
    struct node *res = malloc(sizeof(struct node));
    if(!res)
        return NULL;
    res->barre = 0;
    res->arg = NULL;
    res->left = NULL;
    res->right = NULL;
    res->type = NOT_VALID;
    return res;
}
char *append_strings(char *strings[], int start, int stop)
{
    int size = 0;
    for(int i = start; i < stop; ++i)
    {
        size += get_size(strings[i]) - 1;
    }
    char *res = malloc(size * sizeof(char) + 1);
    int pos = 0;
    for(int i = start; i < stop; ++i)
    {
        for(int j = 0; strings[i][j] != '\0'; ++j)
        {
            res[pos] = strings[i][j];
            ++pos;
        }
        res[pos] = ' ';
        pos++;
    }
    res[size] = '\0';
    return res;
}

void free_tree(struct node *root)
{
    if(!root)
        return;
    if(root->arg)
        free(root->arg);
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

enum type get_type(char *str)
{
    if(!my_strcmp(str, "-type"))
        return TYPE;
    else if(!my_strcmp(str, "-name"))
        return NAME;
    else if(!my_strcmp(str, "-print"))
        return NAME;
    else if(!my_strcmp(str, "-delete"))
        return NAME;
    else if(!my_strcmp(str, "-perm"))
        return NAME;
    else if(!my_strcmp(str, "-user"))
        return NAME;
    else if(!my_strcmp(str, "-group"))
        return NAME;
    else if(!my_strcmp(str, "-newer"))
        return NAME;
    else if(!my_strcmp(str, "-exec"))
        return NAME;
    else if(!my_strcmp(str, "-execdir"))
        return NAME; 
    else
        return NOT_VALID;
}

struct node *create_node(char *type, int barre)
{
    struct node *res = init_node();
    if(!res)
        return NULL;
    res->barre = barre;
    res->type = get_type(type);
    return res;
}

struct node *link_nodes(struct node *left, struct node *right, int type)
{
    struct node *res = init_node();
    if(!res)
    {
        free(right);
        return left;
    }
    if(type == 1)
    {
        res->type = OR;
    }
    else
        res->type = AND;
    res->left = left;
    res->right = right;
    return res;
}

int add_arg(struct node *n, char *exp[], int i, int len)
{
    int start = -1;
    switch (n->type)
    {
        case PRINT:
        case DELETE:
            n->arg = NULL;
            return i;
        case PERM:
        case GROUP:
        case USER:
        case NEWER:
        case TYPE:
        case NAME:
            n->arg = append_strings(exp, i, i+1);
            start = i+1;
            break;
        case EXEC:
        case EXECDIR:
            start = i;
            for(; start<len && my_strcmp(exp[i], "+") && my_strcmp(exp[i], ";")\
                ; ++start);
            if(start<len)
            {
                ++start;
                n->arg = append_strings(exp, i, start);
            }
            else
            {
                char *s;
                if(n->type == EXEC)
                    s = "-exec";
                else
                    s = "-execdir";
                fprintf(stderr, "myfind: paramètre manquant pour « %s »;", s);
                start = -1;
            }
            break;
        default:
                fprintf(stderr, "myfind: prédicat inconnu « %s »", exp[i-1]);
                break;
    }
    return start;
}

void set_error(int error_number, int *error)
{
    switch (error_number)
    {
        case 1:
            perror("myfind: expression non valide; « ( »  attendue mais \
            non détectée.\n");
            break;
        case 2: 
            perror("myfind: expression non valide ; vous avez utilisé un \
            opérateur binaire « -a » non précédé d'une expression.\n");
            break;
        case 3:
            perror("myfind: expression non valide ; vous avez utilisé un \
            opérateur binaire « -o » non précédé d'une expression.\n");
            break;
        case 4:
            perror("myfind: expression non valide. « ) » était attendue m\
            ais n'a pas été détectée. Peut-être faut-il un autre prédicat \
            après « ( ».\n");
            break;
        case 5:
            perror("bash: erreur de syntaxe près du symbole inattendu « \
            ( »\n");
            break;
        case 6:
            perror("myfind: creating new node returned NULL\n");
            break;
        default:
            perror("something wrong happened\n");
    }
    *error = 1;
}
struct node *build_tree(char *exp[], int len, int par, int *end, int *error)
{
    struct node *root = init_node();
    if(!root)
        return NULL;
    root->type = TRUE;
    int gate = 0;
    int barre = 0;
    int err_number = 0;
    for(int i = 0; i<len && !(*error) && !(err_number);)
    {
        if(!strcmp(exp[i], "!"))
            barre += 1;
        if (!strcmp(exp[i], ")"))
        {
            if(barre)
                err_number = 5;
            else if(par)
                *end = i+1;
            else
                err_number = 1;
            break;
        }
        if(!my_strcmp(exp[i], "-a"))
        {
            if(gate || barre)
            {
                err_number = 2;
                break;
            }
            gate = 1;
            i++;
            continue;
        }
        struct node *new;
        if(!my_strcmp(exp[i], "-o"))
        {
            if(gate || barre)
            {
                err_number = 3;
               *error = 1;
                break;
            }
            new = build_tree(exp + i + 1, len - (i + 1), par , NULL, error);
            link_nodes(root, new, 1);
            break;
        }
        if(!strcmp(exp[i], "("))
        {
            if(i+1 < len)
            {
                int add = 0;
                new = build_tree(exp + i + 1, len - (i + 1), par + 1, &add,\
                error);
                new->barre = barre % 2;
                i = i + add;
            }
            else
                err_number = 4; 
            continue;
        }
        else
        {
            new = create_node(exp[i], barre % 2);
            if(!new)
                err_number = 6;
            else if(i+1 < len)
                i = add_arg(new, exp, i+1, len);
        }
        if(i<0)
            *error = 1;
        root = link_nodes(root, new, 0);
        gate = 0;
        barre = 0;
    }
    if(err_number)
        set_error(err_number, error);
    return root;
}

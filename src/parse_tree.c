#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"

//int print()
/*
int evaluate_node(struct node *node, char *path, char *file)
{
    if(!node)
    {
        fprintf(stderr, "myfind: in evaluate_node: empty node");
        return 0;
    }
     switch (n->type)
    {
        case NAME:
                return name_match(node, file);
                break;
        case TYPE:
                
                break;
        case PRINT:
                return print_path(path);
                break;
        case EXEC:
                break;
        case EXECDIR:
                break;
        case DELETE:
                delete(path);
                break;
        case PERM:
                perm(path, node);
                break;
        case USER:
                return is_user(path, node);
                break;
        case GROUP:
                return is_group(path, n);
                break;
        case NEWER:
                return is_newer(path, n);
                break;
        case AND:
                return (evaluate(n->left) && evaluate(n->right));
                break;
        case OR:
                return (evaluate(n->left) || evaluate(n->right));
                break;
        case TRUE:
                return 1;
                break;
        default:
                fprintf(stderr, "myfind: in evaluate_node: type NOT_VALIDE\n");
    }
    return 0;
}

// Function to print binary tree in 2D
// It does reverse inorder traversal
*/
void print_type(struct node *n)
{
    switch (n->type)
    {
        case NAME:
                printf("-name\n");
                break;
        case TYPE:
                printf("-type\n");
                break;
        case PRINT:
                printf("-print\n");
                break;
        case EXEC:
                printf("-exec\n");
                break;
        case EXECDIR:
                printf("-execdir\n");
                break;
        case DELETE:
                printf("-delete\n");
                break;
        case PERM:
                printf("-perm\n");
                break;
        case USER:
                printf("-user\n");
                break;
        case GROUP:
                printf("-group\n");
                break;
        case NEWER:
                printf("-newer\n");
                break;
        case AND:
                printf("-a\n");
                break;
        case OR:
                printf("-o\n");
                break;
        case TRUE:
                printf("true\n");
                break;
        default:
                printf("not valid\n");
    }
}
void print2DUtil(struct node *root, int space) 
{ 
    // Base case 
    if (root == NULL) 
        return; 
  
    // Increase distance between levels 
    space += COUNT; 
  
    // Process right child first 
    print2DUtil(root->right, space); 
  
    // Print current node after space 
    // count 
    printf("\n"); 
    for (int i = COUNT; i < space; i++) 
        printf(" "); 
    print_type(root);
  
    // Process left child 
    print2DUtil(root->left, space); 
} 
  
// Wrapper over print2DUtil() 
void print2D(struct node *root) 
{ 
   // Pass initial space count as 0 
   print2DUtil(root, 0); 
}

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
        size += get_size(strings[i]);
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
        return PRINT;
    else if(!my_strcmp(str, "-delete"))
        return DELETE;
    else if(!my_strcmp(str, "-perm"))
        return PERM;
    else if(!my_strcmp(str, "-user"))
        return USER;
    else if(!my_strcmp(str, "-group"))
        return GROUP;
    else if(!my_strcmp(str, "-newer"))
        return NEWER;
    else if(!my_strcmp(str, "-exec"))
        return EXEC;
    else if(!my_strcmp(str, "-execdir"))
        return EXECDIR; 
    else
    {
        fprintf(stderr, "myfind: prédicat inconnu « %s »", str);
        exit(1);
    }
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
            for(; start<len && my_strcmp(exp[start], "+") && my_strcmp(exp[start], ";")\
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
                fprintf(stderr, "myfind: paramètre manquant pour « %s ».\n", s);
                exit(1);
            }
            break;
        default:
                exit(1);
    }
    return start;
}

void set_error(int error_number)
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
    exit(1);
}
struct node *build_tree(char *exp[], int len, int par, int *end)
{
    printf("entering build_tree len: %d\n", len);
    struct node *root = init_node();
    if(!root)
        return NULL;
    root->type = TRUE;
    int gate = 0;
    int barre = 0;
    int err_number = 0;
    printf("creating true node\n");
    for(int i = 0; i<len && !(err_number);)
    {
        printf("on regarde %s\n", exp[i]);
        if(!strcmp(exp[i], "!"))
        {
            barre += 1;
            i++;
            continue;
        }
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
            i++;
            continue;
        }
        struct node *new;
        if(!my_strcmp(exp[i], "-o"))
        {
            if(gate || barre)
            {
                err_number = 3;
                break;
            }
            new = build_tree(exp + i + 1, len - (i + 1), par , end);
            root = link_nodes(root, new, 1);
            break;
        }
        if(!strcmp(exp[i], "("))
        {
            if(i+1 < len)
            {
                int add = 0;
                new = build_tree(exp + i + 1, len - (i + 1), par + 1, &add);
                i = i + 1 + add; 
            }
            else
                err_number = 4; 
        }
        else
        {
            new = create_node(exp[i], barre % 2);
            if(!new)
            {
                err_number = 6;
                break;
            }
            else if(i+1 < len)
                i = add_arg(new, exp, i+1, len);
            else
                i++;

            printf("arg is %s\n", new->arg);
        }
        root = link_nodes(root, new, 0);
        gate = 0;
        barre = 0;
    }
    if(err_number)
        set_error(err_number);
    return root;
}

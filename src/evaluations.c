#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"
#include "evaluations.h"
/*
int my_delete(char *path)
{
    int res = unlink(path);
    if(res == -1)
        fprintf(stderr, "myfind: impossible de supprimer '%s': Le dossier n'est pas \
        vide\n", path);
    return !res;
}

int print_path(char *path)
{
    printf("%s", path);
    return 1;
}

int name_match(struct node *n, char *file_name)
{
    char *pattern = n->arg;
    int res = fnmatch(pattern, file_name, FNM_PATHNAME);
    return !res;
}

int get_perm(char *str, char *tag, int perm[])
{
    int size;
    char *tmp = str;
    if((size = get_size(str)) > 5 || size < 4)
    {
        fprintf(stderr, "myfind: -perm: mode non valide « ‘%s’ »\n", str);
        return 0;
    }
    if(size == 5)
    {
        if(s[0] == '-' || s[0] == '/' || s[0] == '+')
            *tag = s[0];
        else
        {
            fprintf(stderr, "myfind: -perm: unknown %c\n", s[0]);
            return 0;
        }
        str++;
    }
    for(int i = 0; str[i] != '\0' && i < 3; ++i)
    {
        if(str[i] >= '0' && str[i] <= '7')
        {
                perm[i] = str[i] - '0';
        }
        else
        {
            fprintf(stderr, "myfind: -perm: mode non valide « ‘%s’ »\n", tmp);
            return 0;
        }
    }
    return 1;
}

int perm(char *path, struct node *n)
{
    struct stat buf;
    int all_perm[3] = { 0, 0, 0};
    char tag = 0;
    if(!get_perm(n->arg, &tag, all_perm))
        exit(1);
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -perm: couldn't get stat of %s\n", path);
        return 0;
    }
    mode_t mode = 64 * perm[0] + 8 * perm[1] + perm[2];
    mode_t file_mode = buf.st_mode;
    if(tag == '-')
        return ((file_mode & mode) == mode);
    else if(tag == '/' || tag == '+')
        return ((mode & file_mode) != 0);
    else if(!tag)
        return (mode == file_mode);
    else
        perror("myfind: in -perm mode has unknow identifier");
    return 0;
}

int is_user(char *path, struct node *n)
{
    struct stat buf;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -user: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_uid == user);
}

int is_group(char *path, struct node *n)
{
    struct stat buf;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -group: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_gid == group);
}

int is_newer(char *path, struct node *n)
{
    struct stat buf_path;
    struct stat buf_compare;
    if(stat(path, &buf_path))
    {
        fprintf(stderr, "myfind: in -newer: couldn't get stat of %s\n", path);
        return 0;
    }
    if(stat(path, &buf_compare))
    {
        fprintf(stderr, "myfind: in -groupe: couldn't get stat of %s\n", \
        compare);
        return 0;
    }
    return (buf_path.st_mtime > buf_compare.st_mtime);
}

//int exec(char *to_exec)i
*/

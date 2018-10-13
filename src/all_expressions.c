#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"

int delete(char *path)
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

int name_match(char *pattern, char *file_name)
{
    int res = fnmatch(pattern, file_name, FNM_PATHNAME);
    return !res;
}

int perm(char *path, mode_t mode, char tag)
{
    struct stat buf;
    
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -perm: couldn't get stat of %s\n", path);
        return 0;
    }
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

int is_user(char *path, uid_t user)
{
    struct stat buf;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -user: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_uid == user);
}

int is_group(char *path, gid_t group)
{
    struct stat buf;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -group: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_gid == group);
}

int is_newer(char *path, char *compare)
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


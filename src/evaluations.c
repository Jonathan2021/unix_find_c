#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"
#include "evaluations.h"

int my_delete(char *full_name)
{
    int res = unlink(full_name);
    if(res == -1)
        fprintf(stderr, "myfind: impossible de supprimer '%s': Le dossier \
n'est pas vide\n", full_name);
    return !res;
}

int print_path(char *path, char *file)
{
    if (!my_strcmp(path, "."))
        printf("./%s\n", file);
    else
        printf("%s/%s\n", path, file);
    return 1;
}

int name_match(struct node *n, char *file_name)
{
    const char *pattern = n->arg;
    int res = fnmatch(pattern, file_name, FNM_PATHNAME);
    return !res;
}

int get_perm(const char *s, char *tag, int perm[])
{
    int size;
    const char *tmp = s;
    if((size = get_size(s)) > 5 || size < 4)
    {
        fprintf(stderr, "myfind: -perm: mode non valide « ‘%s’ »\n", s);
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
        s++;
    }
    for(int i = 0; s[i] != '\0' && i < 3; ++i)
    {
        if(s[i] >= '0' && s[i] <= '7')
        {
                perm[i] = s[i] - '0';
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
    mode_t mode = 64 * all_perm[0] + 8 * all_perm[1] + all_perm[2];
    mode_t file_mode = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    if(tag == '-')
        return ((file_mode & mode) == mode);
    else if(tag == '/' || tag == '+')
        return ((mode & file_mode) != 0);
    else if(!tag)
        return (mode == file_mode);
    else
        perror("myfind: in -perm mode has unknow identifier\n");
    return 0;
}

int is_user(char *path, struct node *n)
{
    struct stat buf;
    struct passwd *pass = getpwnam(n->arg);
    if(!pass)
        return 0;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -user: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_uid == pass->pw_uid);
}

int is_group(char *path, struct node *n)
{
    struct stat buf;
    struct group *grp = getgrnam(n->arg);
    if(!grp)
        return 0;
    if(stat(path, &buf))
    {
        fprintf(stderr, "myfind: in -group: couldn't get stat of %s\n", path);
        return 0;
    }
    return (buf.st_gid == grp->gr_gid);
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
    if(stat(n->arg, &buf_compare))
    {
        fprintf(stderr, "myfind: in -groupe: couldn't get stat of %s\n", \
        n->arg);
        return 0;
    }
    return (buf_path.st_mtime > buf_compare.st_mtime);
}

char **get_args(struct node* n, char *path)
{
    int j = 0;
    int size;
    int is_replaced;
    char **res = calloc(n->elements + 1, sizeof(char *));
    for(int i = 0; i < n->elements; ++i, ++j)
    {
        is_replaced = !my_strcmp(n->arg + j, "{}");
        size = is_replaced ? get_size(path) : get_size(n->arg + j);
        char *new_arg = malloc(size);
        if (!is_replaced) {
            for(int k = 0; n->arg[j] != '\0'; ++j) {
                new_arg[k] = n->arg[j];
                k++;
            }
        } else {
          j = j + 2;
          for(int k = 0; path[k] != '\0'; ++k)
          {
              new_arg[k] = path[k];
          }
        }
        new_arg[size - 1] = '\0';
        res[i] = new_arg;
    }
    res[n->elements] = NULL;
    return res;
}

int my_exec(struct node *n, char *path)
{
    int res = 0;
    if(n->is_plus != '+' && n->is_plus != ';')
    {
        fprintf(stderr, "myfind: -exec: expected + or ;");
    }
    pid_t pid = fork();
    if(pid == -1)
    {
        fprintf(stderr, "myfind: -exec: fork failed\n");
    }
    if(!pid)
    {
        char **args = get_args(n, path);
        if(execvp(n->arg, args)  < 0)
        {
            fprintf(stderr,"execvp failed\n");
            exit(1);
        }
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if(!WIFEXITED(wstatus))
            return 0;
        res = !WEXITSTATUS(wstatus);
    }
    if(n->is_plus == '+')
        return 1;
    return res;
}

int my_execdir(struct node *n, char *path, char *file)
{
    int res = 0;
    char *new_path = malloc(2 + get_size(file));
    append_string(new_path, "./", file);
    if(n->is_plus != '+' && n->is_plus != ';')
    {
        fprintf(stderr, "myfind: -execdir: expected + or ;");
    }
    pid_t pid = fork();
    if(pid == -1)
    {
        fprintf(stderr, "myfind: -exec: fork failed\n");
    }
    if(!pid)
    {
        if (chdir(path) < 0)
        {
            fprintf(stderr,"chdir failed\n");
            exit(1);
        }
        char **args = get_args(n, new_path);
        if(execvp(n->arg, args)  < 0)
        {
            fprintf(stderr,"execvp failed\n");
            exit(1);
        }
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if(!WIFEXITED(wstatus))
            return 0;
        res = !WEXITSTATUS(wstatus);
    }
    free(new_path);
    if(n->is_plus == '+')
        return 1;
    return res;

}

int my_type(struct node *n, char *full_name)
{ 
    struct stat buf;
    if(stat(full_name, &buf))
    {
        fprintf(stderr, "myfind: -type: couldn't get stat of %s\n", full_name);
    }
    mode_t mode = buf.st_mode;
    if(!my_strcmp(n->arg, "b"))
        return (mode & S_IFBLK);
    if(!my_strcmp(n->arg, "c"))
        return (mode & S_IFCHR);
    if(!my_strcmp(n->arg, "d"))
        return (mode & S_IFDIR);
    if(!my_strcmp(n->arg, "f"))
        return (mode & S_IFREG);
    if(!my_strcmp(n->arg, "l"))
        return (mode & S_IFLNK);
    if(!my_strcmp(n->arg, "p"))
        return (mode & S_IFIFO);
    if(!my_strcmp(n->arg, "s"))
        return (mode & S_IFSOCK);
    else
        fprintf(stderr, "myfind: -type: invalide type %s\n", n->arg);
    
    return 0;
}

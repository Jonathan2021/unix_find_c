#include "evaluations.h"
#include <errno.h>
#include "libraries.h"
#include "parse_tree.h"
#include "useful.h"

int
delete_dir(const char *full_path)
{
    int res = 1;

    // Recursively delete the directory contents.
    DIR *dir = opendir(full_path);
    if (!dir)
    {
        perror(full_path);
        return 0;
    }
    struct dirent *ent;
    do
    {
        errno = 0;
        ent = readdir(dir);
        if (!ent)
        {
            if (!errno)
                break;
            perror(full_path);
            return 0;
        }
        if (!my_strcmp(ent->d_name, ".."))
            continue;
        if (!my_strcmp(ent->d_name, "."))
            continue;
        int dir_len = get_size(full_path) - 1;
        int file_len = get_size(ent->d_name) - 1;
        char *new_path = malloc(dir_len + file_len + 2);
        if (!new_path)
            fail("malloc");
        int target = 0;
        for (int i = 0; i < dir_len; ++i)
            new_path[target++] = full_path[i];
        new_path[target++] = '/';
        for (int i = 0; i < file_len; ++i)
            new_path[target++] = ent->d_name[i];
        new_path[target++] = 0;
        res = my_delete(new_path) && res;
        free(new_path);
    } while (1);
    closedir(dir);

    // Delete the directory.
    if (rmdir(full_path))
    {
        perror(full_path);
        return 0;
    }
    return res;
}

int
my_delete(const char *full_path)
{
    int res = unlink(full_path);
    if (!res)
        return 1;
    if (errno != EISDIR)
    {
        perror(full_path);
        return 0;
    }
    return delete_dir(full_path);
}

int
print_path(const char *full_path)
{
    printf("%s\n", full_path);
    return 1;
}

int
name_match(struct node *n, char *file_name)
{
    const char *pattern = n->arg;
    int res = fnmatch(pattern, file_name, FNM_PATHNAME);
    return !res;
}

int
get_perm(const char *s, char *tag, mode_t *mode)
{
    int size;
    const char *tmp = s;

    *mode = 0;
    if ((size = get_size(s)) > 5 || size < 1)
    {
        fprintf(stderr, "myfind: -perm: mode non valide « ‘%s’ »\n", s);
        return 0;
    }

    if (s[0] == '-' || s[0] == '/' || s[0] == '+')
    {
        *tag = s[0];
        ++s;
    }
    else
    {
        *tag = 0;
    }

    for (; *s; ++s)
    {
        if (*s >= '0' && *s <= '7')
        {
            *mode = 8 * *mode + (*s - '0');
        }
        else
        {
            fprintf(stderr, "myfind: -perm: mode non valide « ‘%s’ »\n", tmp);
            return 0;
        }
    }
    return 1;
}

int
perm(int fd, struct node *n)
{
    struct stat buf;
    mode_t mode;
    char tag = 0;
    if (!get_perm(n->arg, &tag, &mode))
        exit(1);
    if (fstat(fd, &buf))
        fail("fstat");
    mode_t file_mode = buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    if (tag == '-')
        return ((file_mode & mode) == mode);
    else if (tag == '/' || tag == '+')
        return ((mode & file_mode) != 0);
    else if (!tag)
        return (mode == file_mode);
    else
        perror("myfind: in -perm mode has unknow identifier\n");
    return 0;
}

int
is_user(int fd, struct node *n)
{
    struct stat buf;
    struct passwd *pass = getpwnam(n->arg);
    if (!pass)
        return 0;
    if (fstat(fd, &buf))
        fail("fstat");
    return (buf.st_uid == pass->pw_uid);
}

int
is_group(int fd, struct node *n)
{
    struct stat buf;
    struct group *grp = getgrnam(n->arg);
    if (!grp)
        return 0;
    if (fstat(fd, &buf))
        fail("fstat");
    int res = (buf.st_gid == grp->gr_gid);
    return res;
}

int
is_newer(int fd, struct node *n)
{
    struct stat buf_path;
    struct stat buf_compare;
    if (fstat(fd, &buf_path))
        fail("fstat");
    if (stat(n->arg, &buf_compare))
    {
        fprintf(
            stderr, "myfind: in -groupe: couldn't get stat of %s\n", n->arg);
        return 0;
    }
    return (buf_path.st_mtime > buf_compare.st_mtime);
}

char **
get_args(struct node *n, const char *path)
{
    int j = 0;
    int size;
    int is_replaced;
    char **res = calloc(n->elements + 1, sizeof(char *));
    for (int i = 0; i < n->elements; ++i, ++j)
    {
        is_replaced = !my_strcmp(n->arg + j, "{}");
        size = is_replaced ? get_size(path) : get_size(n->arg + j);
        char *new_arg = malloc(size);
        if (!is_replaced)
        {
            for (int k = 0; n->arg[j] != '\0'; ++j)
            {
                new_arg[k] = n->arg[j];
                k++;
            }
        }
        else
        {
            j = j + 2;
            for (int k = 0; path[k] != '\0'; ++k)
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

void
free_arg_array(char **arg)
{
    for (int i = 0; arg[i] != NULL; ++i)
    {
        free(arg[i]);
    }
    free(arg);
}

int
is_bracket(int elements, const char *str)
{
    int j = 0;
    for (int i = 0; i < elements; ++i)
    {
        if (!my_strcmp(str + j, "{}"))
            return 1;
        j += get_size(str + j);
    }
    return 0;
}

int
my_exec(struct node *n, const char *full_path)
{
    int res = 0;
    if (n->is_plus == '+' && !is_bracket(n->elements, n->arg))
    {
        fprintf(stderr, "myfind: paramètre manquant pour « -exec »");
        exit(1);
    }
    char **args = get_args(n, full_path);
    if (n->is_plus != '+' && n->is_plus != ';')
    {
        fprintf(stderr, "myfind: -exec: expected + or ;");
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "myfind: -exec: fork failed\n");
    }
    if (!pid)
    {
        if (execvp(n->arg, args) < 0)
        {
            fprintf(stderr, "execvp failed\n");
            exit(1);
        }
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        res = WIFEXITED(wstatus);
        if (res)
            res = !WEXITSTATUS(wstatus);
    }
    free_arg_array(args);
    if (n->is_plus == '+')
        return 1;
    return res;
}

int
my_execdir(struct node *n, char *path, char *file)
{
    int res = 0;
    if (n->is_plus == '+' && !is_bracket(n->elements, n->arg))
    {
        fprintf(stderr, "myfind: paramètre manquant pour « -exec »");
        exit(1);
    }
    char *new_path = malloc(2 + get_size(file));
    if (!new_path)
    {
        fail("malloc");
        return 0;
    }
    append_string(new_path, "./", file);
    char **args = get_args(n, new_path);
    if (n->is_plus != '+' && n->is_plus != ';')
    {
        fprintf(stderr, "myfind: -execdir: expected + or ;");
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "myfind: -exec: fork failed\n");
    }
    if (!pid)
    {
        if (chdir(path) < 0)
        {
            fprintf(stderr, "chdir failed\n");
            exit(1);
        }
        if (execvp(n->arg, args) < 0)
        {
            fprintf(stderr, "execvp failed\n");
            exit(1);
        }
    }
    else
    {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        if (!WIFEXITED(wstatus))
            return 0;
        res = !WEXITSTATUS(wstatus);
    }
    free_arg_array(args);
    free(new_path);
    if (n->is_plus == '+')
        return 1;
    return res;
}

int
my_type(struct node *n, int fd)
{
    struct stat buf;
    if (fstat(fd, &buf))
        fail("fstat");
    mode_t mode = buf.st_mode;
    if (!my_strcmp(n->arg, "b"))
        return (mode & S_IFBLK);
    if (!my_strcmp(n->arg, "c"))
        return (mode & S_IFCHR);
    if (!my_strcmp(n->arg, "d"))
        return (mode & S_IFDIR);
    if (!my_strcmp(n->arg, "f"))
        return (mode & S_IFREG);
    if (!my_strcmp(n->arg, "l"))
        return (mode & S_IFLNK);
    if (!my_strcmp(n->arg, "p"))
        return (mode & S_IFIFO);
    if (!my_strcmp(n->arg, "s"))
        return (mode & S_IFSOCK);
    else
        fprintf(stderr, "myfind: -type: invalide type %s\n", n->arg);

    return 0;
}

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "settings.h"
#include "parse_tree.h"
#include "useful.h"

void split(const char *file, char** path, char** name) {
    // Find last slash that has a non-slash after it.
    // Anything after len is a slash.
    int len = get_size(file) - 1;
    while (len > 1 && file[len - 1] == '/')
        --len;

    // Get this annoying case of all slashes out of the way first.
    if (len == 1 && file[0] == '/') { // Eg: /
        *name = my_strdup("/");
        if (file[1] == '/') // Eg: //
            *path = my_strdup(file + 1);
        else
            *path = my_strdup("/");
        return;
    }

    // Eg: a/b////
    //        ^ len
    // Want to return:
    //   path = a
    //   file = b////

    int slash = -1;
    for (int i = len; i; --i) {
        if (file[i-1] == '/') {
            slash = i - 1;
            break;
        }
    }

    if (slash < 0) { // No slash, eg: abc
        *path = my_strdup(".");
        *name = my_strdup(file);
        return;
    }

    if (!slash) { // Eg: /abc
        *path = my_strdup("/");
        *name = my_strdup(file + 1);
        return;
    }

    // Eg: abc/def 
    *path = my_strdup(file);
    (*path)[slash] = 0;
    *name = my_strdup(file + slash + 1);
}

void search(const char *file, struct node* expr, struct Settings settings,
  int top_level) {
    char *path, *name;
    split(file, &path, &name);

    // Decide whether to follow symlinks.
    int follow = 0;
    if (settings.symlinkPolicy == FOLLOW_NONE ||
      (settings.symlinkPolicy == FOLLOW_ARGS && !top_level))
        follow = O_NOFOLLOW;

    // Using O_PATH + O_NOFOLLOW means we get the symbolic link, not what it
    // points to.
    int fd = open(file, follow | O_RDONLY | O_PATH);
    if (fd < 0)
        fail(file);

    evaluate_node(expr, fd, path, name);

    // Is it a directory?  If this a directory, recurse into it.
    // FIXME: Might get into infinite loop when following symlinks.

    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0)
        fail("stat");
    if (!S_ISDIR(statbuf.st_mode))
        goto out;

    // Directory was opened using O_PATH, which means we can't read it.  Reopen
    // without O_PATH.  If someone deletes the directory beneath us and replaces
    // it with something else then we may fail here or below, but that seems OK.
    close(fd);
    fd = open(file, O_RDONLY);
    if (fd < 0)
        fail(file);

    // Loop through the directory contents.
    DIR* dir = fdopendir(fd);
    if (!dir)
        fail("fdopendir");

    do {
        struct dirent *ent;
        errno = 0;
        ent = readdir(dir);
        if (!ent) {
           if (errno)
              fail("readdir");
           break;
        }
        if (!my_strcmp(ent->d_name, "."))
            continue;
        if (!my_strcmp(ent->d_name, ".."))
            continue;

        // Form "file/d_name".
        int file_len = get_size(file) - 1;
        int d_name_len = get_size(ent->d_name) - 1;
        char *newfile = malloc(file_len + d_name_len + 2);
        if (!newfile)
            fail("malloc");
        int target = 0;
        for (int i = 0; i < file_len; ++i)
            newfile[target++] = file[i];
        if (file[file_len - 1] != '/')
            newfile[target++] = '/';
        for (int i = 0; i < d_name_len; ++i)
            newfile[target++] = ent->d_name[i];
        newfile[target++] = 0;
        search(newfile, expr, settings, 0);
        free(newfile);
    } while (1);

    closedir(dir);

out:
    close(fd);
    free(name);
    free(path);
}

int main(int argc, char *argv[])
{
    struct Settings settings = { FOLLOW_NONE };
    int first_non_option, first_expression;

    for (first_non_option = 1; first_non_option < argc; ++first_non_option) {
        const char* opt = argv[first_non_option];

        if (!my_strcmp(opt, "-P"))
            settings.symlinkPolicy = FOLLOW_NONE;
        else if (!my_strcmp(opt, "-H"))
            settings.symlinkPolicy = FOLLOW_ARGS;
        else if (!my_strcmp(opt, "-L"))
            settings.symlinkPolicy = FOLLOW_ALL;
        else if (!my_strcmp(opt, "--")) {
            // Next argument is the first non-option argument.
            ++first_non_option;
            break;
        } else
            // This argument is the first non-option argument.
            break;
    }

    for (first_expression = first_non_option; first_expression < argc;
      ++first_expression) {
        const char* opt = argv[first_expression];

        if (!my_strcmp(opt, "!")) // Expression.
            break;
        if (!my_strcmp(opt, "(")) // Expression.
            break;

        if (opt[0] == '-' && opt[1]) // Eg -print (but not just '-')
            break;
    }

    int end = 0;
    int print = 0;
    struct node* expr = build_tree(argv + first_expression, \
    argc - first_expression, 0, &end, &print);
    if(!print)
    {
        struct node *print_node = init_node();
        if(!print_node)
        {
            fail("malloc");
        }
        print_node->type = PRINT;
        expr = link_nodes(expr, print_node, 0);
    }
    //print2D(expr);

    for (int i = first_non_option; i < first_expression; ++i)
        search(argv[i], expr, settings, 1);
    if (first_non_option == first_expression) // No files?  Use .
        search(".", expr, settings, 1);

    return 0;
}

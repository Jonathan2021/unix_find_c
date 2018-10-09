#include "libraries.h"
#include "cmd.h"
#include "useful.h"

int get_size(char *str)
{
    int i = 0;
    for(; str[i] != '\0'; ++i);
    return i+1;
}

void append_string(char *dest, char *first, char *second)
{
    int i = 0;
    int j = 0;
    for(; first[i] != '\0'; ++i)
        dest[i] = first[i];
    for(; second[j] != '\0'; ++j)
        dest[i+j] = second[j];
    dest[i+j] = '\0';
}

struct cmd_arg *parse_arg(int start, int end, char *argv[])
{
    if(start == end)
        return NULL;
    struct cmd_arg *first_arg= malloc(sizeof(struct cmd_arg));
    if(!first_arg)
        return NULL;
    first_arg->arg = argv[start++];
    first_arg->next = NULL;
    struct cmd_arg *cur = first_arg;
    for(; start<end; ++start)
    {
        struct cmd_arg *add = malloc(sizeof(struct cmd_arg));
        if(!add)
            break;
        add->next = NULL;
        add->arg = argv[start];
        cur->next = add;
        cur = add;
    }
    return first_arg;
}

void fix_path(char *str)
{
    int has_slash = 0;
    int i = 0;
    for(; str[i] != '\0'; ++i)
    {
        if(str[i] == '/' && str[i+1] == '\0')
        {
            has_slash = 1;
        }
    }
    if(!has_slash)
    {
        str[i] = '/';
        str[i+1] = '\0';
    }
}

int my_print(DIR *dir, char *str)
{
    struct dirent *de;
    dir = opendir(str);
    if(!dir)
    {
        fprintf(stderr, "myfind: cannot do opendir(%s): can't open directory",\
        str);
        return 1;
    }
    printf("%s\n", str);
    fix_path(str);
    while( (de = readdir(dir)) != NULL)
    {
        if(my_strcmp(de->d_name, ".") && my_strcmp(de->d_name, ".."))
        {
            if(de->d_type == DT_DIR)
            {
                char *arg = malloc((get_size(str)  + get_size(de->d_name))*\
                sizeof(char) +1);
                append_string(arg, str, de->d_name);
                return 0 + my_print(dir, arg);
            }
            else
                printf("%s%s\n", str, de->d_name);
        }
    }
    free(str);
    closedir(dir);
    return 0;
}

/*
int open_dir(DIR *dir, const char *)
{
    dir = opendir(str);
    if(!dir)
    {
        fprintf(stderr, "myfind: cannot do opendir(%s): can't open directory",\
        str);
        return 1;
    }
    return 0;
}
*/

int print(struct cmd_arg *args)
{
    DIR *dir = NULL;
    int error = 0;
    if(!args)
    {
        args = malloc(sizeof(struct cmd_arg));
        args->next = NULL;
        args->arg = ".";
    }
    for(; args; args = args->next)
    {
        char *arg = malloc(get_size(args->arg) * sizeof(char) + 1);
        append_string(arg, args->arg, "");
        error += my_print(dir, arg);
    }
    return (error > 0);
}

struct cmd *get_commands(void)
{
    struct cmd *commands = malloc(1 * sizeof(struct cmd));
    commands[0].handle= print;
    commands[0].command_name = "print";
    return commands;
}

int main(int argc, char *argv[])
{
    struct cmd *commands = get_commands();
    struct cmd_arg *args= parse_arg(1, argc, argv);
    commands[0].handle(args);
    return 0;
}
/*
int call_command(const char *str, const char **arg)
{
    size_t nb
}
*/

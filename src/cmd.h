#ifndef _CMD_H_
#define _CMD_H_

#include "libraries.h"

struct cmd_arg
{
    char *arg;
    struct cmd_arg *next;
};

typedef int(*handler)(struct cmd_arg *args);

struct cmd
{
    handler handle;
    const char *command_name;
};

#endif /* !_CMD_H_ */

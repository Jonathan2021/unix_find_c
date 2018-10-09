#include "libraries.h"

int my_strcmp(const char *a, const char *b)
{
    size_t i = 0;
    for(; a[i] != '\0' && b[i] != '\0'; ++i)
    {
        if(a[i] != b[i])
            return 1;
    }
    return !(a[i] == b[i]);
}

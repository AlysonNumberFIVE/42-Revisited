
#include "../lib/shell.h"

void    print_bool(bool value) 
{
    value == true ? printf("true") : printf("false");
    printf("\n");
}

bool    is_newline(size_t read_bytes, char buffer[]) 
{
    return (read_bytes == 1 && buffer[0] == '\n');
}
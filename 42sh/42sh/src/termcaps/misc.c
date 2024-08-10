
#include "../../lib/termcaps.h"


char *delete_this_char(char buffer[], int cursor_position)
{
    char *new_buffer;
    int i;

    i = 0;
    new_buffer = NULL;
    while (buffer[i])
    {
        if (i == cursor_position)
        {
            i++;
            continue;
        }
        new_buffer = charpush(new_buffer, buffer[i]);
        i++;
    }
    return new_buffer;
}

char *delete_to_end_of_line(char buffer[], int cursor_position)
{
    char    *new_buffer;
    int     i;

    new_buffer = NULL;
    i = 0;
    while (i < cursor_position)
    {
        new_buffer = charpush(new_buffer, buffer[i]);
        i++;
    }
    return (new_buffer);
}

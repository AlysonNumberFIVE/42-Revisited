
#include "../../lib/termcaps.h"

char       **adjust_history_lookback(t_termcap *terminal, char *new_command)
{
    char **new_history;
    ssize_t i;
    size_t counter;

    counter = 0;
    new_history = NULL;
    if (arraylen(terminal->history) < terminal->history_file_size)
    {
        new_history = arraypush(new_history, new_command);
        return new_history;
    }

    i = arraylen(terminal->history) - 1 - terminal->history_file_size;
    while (counter < terminal->history_file_size)
    {
        new_history = arraypush(new_history, terminal->history[i]);
        i++;
        counter++;
    }

    return new_history;
}
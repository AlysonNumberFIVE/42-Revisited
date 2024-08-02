
#include "../../lib/termcaps.h"
#include <dirent.h>

char **autocomplete_list(char *prefix, char *path)
{
    DIR             *dir;
    struct dirent   *directory; 
    char            **match_list;

    match_list = NULL;
    directory = NULL;

    dir = opendir(path);
    if (dir == NULL)
        return NULL;

    while ((directory = readdir(dir)) != NULL)
    {
        if (strncmp(directory->d_name, prefix, strlen(prefix)) == 0)
            match_list = arraypush(match_list, directory->d_name);
    }
    closedir(dir);

    return match_list;
}



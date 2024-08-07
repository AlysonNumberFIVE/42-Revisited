
#include "../../lib/termcaps.h"
#include <dirent.h>

static char g_local_tools[][10] = {
    "cd", 
    "clear", 
    "env", 
    "setenv",
};


char    **read_local_tools(char *prefix)
{
    int     i;
    char    **files;

    files = NULL;
    i = 0;
    while (i < LOCAL_TOOLS_LENGTH)
    {
        if (strncmp(g_local_tools[i], prefix, strlen(prefix)) == 0)
            files = arraypush(files, g_local_tools[i]);
        i++;
    }

    return files;
}

char    **read_directory(char *folder, char *prefix)
{
    DIR             *directory_ptr;  
    struct dirent   *directory;
    char            **files;

    files = NULL;
    directory = NULL;
    directory_ptr = opendir(folder);
    if (directory_ptr == NULL)
        return NULL;
    
    while ((directory = readdir(directory_ptr)))
    {
        if (strcmp(directory->d_name, ".") == 0 || strcmp(directory->d_name, "..") == 0)
            continue ;

        if (strncmp(directory->d_name, prefix, strlen(prefix)) == 0)
            files = arraypush(files, directory->d_name);
    }   

    closedir(directory_ptr);
    return files;
}


// TODO: Replace the *path_env with the shell's own PATH
char    **get_path_programs(char *path_env, char *prefix)
{
    char            **matching_programs;
    char            **path_locations;
    char            **files;
    int             i;

    matching_programs = NULL;
    path_locations = split(path_env, ':');
    if (path_locations == NULL)
        return NULL;

    i = 0;
    while (path_locations[i])
    {   
        files = read_directory(path_locations[i], prefix);
        if (files != NULL)
        {
            matching_programs = arraymerge(matching_programs, files);
            free2d(files);
            files = NULL;
        }
        i++;
    }

    files = read_local_tools(prefix);
    if (files != NULL)
    {
        matching_programs = arraymerge(matching_programs, files);
        free2d(files);
        files = NULL;
    } 
    
    i = 0;
    while (matching_programs[i])
    {
        printf("> %s\n", matching_programs[i]);
        i++;
    }

    return matching_programs;
}


bool     is_first_word(char buffer[], int cursor_position)
{
    int     i;
    int     word;

    i = 0;
    word = 0;
    while (buffer[i])
    {
        if ((buffer[i] == ' ' || buffer[i] == '\t') && cursor_position >= i)
            return false;
    
        if (cursor_position == i && word > 0)
            return false;
    
        else if (cursor_position == i && word == 0)
            return true;

        i++;
    }

    return true;    
}

void    autocomplete(char buffer[], int cursor_position)
{
    t_autocomplete *autocomplete;
    char **tokens;

    autocomplete = (t_autocomplete *)malloc(sizeof(t_autocomplete));
    if (is_first_word(buffer, cursor_position))
    {

    }

}

int     main(void)
{
    get_path_programs("/Users/alysonngonyama/anaconda3/bin:/Users/alysonngonyama/anaconda3/condabin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", "");



    return 0;
}
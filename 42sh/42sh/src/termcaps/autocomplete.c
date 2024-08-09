
#include "../../lib/termcaps.h"
#include <dirent.h>

static char g_local_tools[][10] = {
    "cd", 
    "clear", 
    "env", 
    "setenv",
};


static char    **token_extraction(char buffer[])
{
    char    **tokens;
    char    *str;
    int     i;

    str = NULL;
    tokens = NULL;
    i = 0;
    while (buffer[i])
    {
        if (buffer[i] == ' ' || buffer[i] == '\t') 
        {
            tokens = arraypush(tokens, str);
            free_and_null(& str);
            while (buffer[i] && (buffer[i] == ' ' || buffer[i] == '\t'))
                i++;
            continue ;
        } 
        str = charpush(str, buffer[i]);
        i++;
    }
    tokens = arraypush(tokens, str);
    free_and_null(&str);
    return tokens;
}

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

bool    are_you_sure(void)
{
    char    tab;
    ssize_t read_bytes;

    read_bytes = read(STDIN_FILENO, &tab, 1);
    if (read_bytes == -1)
        return false;

    if (tab == TAB || tab == 'y' || tab == 'Y')
        return true ;

    return false;
}


// TODO: *path_env will be taken from PATH environment variable
char    **autocomplete(char buffer[], t_termcap *terminal, char *path_env)
{
    char **tokens;
    int i;
    char **possible_commands;

    i = 0;
    tokens = token_extraction(buffer);
    possible_commands = NULL;
    if (is_first_word(buffer, terminal->cursor_position))
    {
        possible_commands = get_path_programs(path_env, tokens[0]);
        if (possible_commands == NULL)
            return NULL;

        // TODO: The LISTMAX value in the environment variable will be used 
        // here to controll this behaviour.
        if (arraylen(possible_commands) >= 100)
        {
            move_cursor(terminal, 0, terminal->cursor_row + 1);
            printf("do you wish to see all %zu possibilities?", arraylen(possible_commands));
            fflush(stdout);
            if (are_you_sure() == true)
                printf("\nprint_everything");
            else   
            {
                clear_screen_downward();
                return NULL;
            }
        }  

        move_cursor(terminal, 0, terminal->cursor_row + 1);
        if (arraylen(possible_commands) > 1)
        {
            while (possible_commands[i])
            {
                printf("%s ", possible_commands[i]);
                fflush(stdout);
                i++;
            }
        }
        else
        {
            refresh_line(terminal);
            return NULL;
        }
    } 
    return possible_commands; 

}

// int     main(void)
// {

//    get_path_programs("/Users/alysonngonyama/anaconda3/bin:/Users/alysonngonyama/anaconda3/condabin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", "");



//     return 0;
// }
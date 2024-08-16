
#include "../../lib/termcaps.h"
#include <dirent.h>
#include <ctype.h>

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
    if (str != NULL)
    {
        tokens = arraypush(tokens, str);
        free_and_null(&str);
    }
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

        if (directory->d_name[0] == '.')
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

    free2d(path_locations);
    return matching_programs;
}

#define WHITESPACE 1
#define PRINTABLE 2

bool     second_or_more(char buffer[], int cursor_position)
{
    int     i;
    int     word;

    i = 0;
    word = 0;
    while (buffer[i])
    {
        if ((buffer[i] == ' ' || buffer[i] == '\t'))
        {
            word++;
            while (buffer[i] && (buffer[i] == ' ' || buffer[i] == '\t'))
            {
                if (cursor_position == i)
                    return 1;
                i++;
            }
            continue ;
        }
        if (cursor_position == i && isprint(buffer[i]))
            return 2;

        i++;
    }

    return 3;    
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

char *read_buffer_directory(char buffer[], int cursor_position, char *command) {
    int buffer_len = strlen(buffer);
    int command_len = strlen(command);

    // Ensure cursor_position is within bounds
    if (cursor_position < 0 || cursor_position > buffer_len) {
        fprintf(stderr, "Error: cursor_position out of bounds\n");
        return NULL;
    }

    int start = cursor_position;
    int end = cursor_position;

    // Find the start of the word
    while (start > 0 && !isspace(buffer[start - 1])) {
        start--;
    }

    // Find the end of the word
    while (end < buffer_len && !isspace(buffer[end])) {
        end++;
    }

    // Calculate the new buffer size
    int new_buffer_len = buffer_len - (end - start) + command_len;
    char *new_buffer = (char *)malloc(new_buffer_len + 1);

    if (new_buffer == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy the part before the word
    strncpy(new_buffer, buffer, start);
    new_buffer[start] = '\0';

    // Append the command
    strcat(new_buffer, command);

    // Append the part after the word
    strcat(new_buffer, buffer + end);

    return new_buffer;
}

char     **this_directory(char buffer[], int cursor_position)
{
    char *newstr; 
    char **second;
    char **files;
    char *word;
    int i;
    word = NULL;
    second = NULL;
    newstr = NULL;
    files = read_directory(".", "");
    if (files == NULL)
        return NULL;

    return files;
    i = 0;
    while (files[i])
    {
    //    newstr = read_buffer_directory(buffer, cursor_position, files[i]);
        second = arraypush(second, newstr);
        i++;
    }
    
    return second;
}   

char    *insert_word(char buffer[], int pos, char *insertion_word) 
{
    int i;
    int save;
    char *first = NULL;
    char *second = NULL;
    char *finish = NULL;
    int c;

    i = 0;
    while (buffer[i])
    {
        if (i == pos)
        {
            save = i;
            while (i > -1 && buffer[i] != ' ')
                i--;
            c = 0;
            while (c < i)
            {
                first = charpush(first, buffer[c]);
                c++;
            }

            i = save;
            while (buffer[i] && buffer[i] != ' ' )
                i++;
            c = i;
            while (buffer[c])
            {
                second = charpush(second, buffer[c]);
                c++;
            }
            finish = join(first, insertion_word);
            second = join(second, finish);
            return second;
        }
        i++;
    }
    return NULL;
}


char    **insert_new_command(char buffer[], int cursor_position, char **possible_commands)
{
    char **new; 
    char *test;
    int i;

    new = NULL;
    i = 0;
    while (possible_commands[i])
    {
        test = read_buffer_directory(buffer, cursor_position, possible_commands[i]);
        new = arraypush(new, test);
        i++;
    }

    return new;
}


// TODO: *path_env will be taken from PATH environment variable
char    **autocomplete(char buffer[], t_termcap *terminal, char *path_env)
{
    char **tokens;
    int i;
    char **possible_commands;
    int cursor_type;

    i = 0;

    tokens = token_extraction(buffer);
    if (tokens == NULL)
        return NULL;
    
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
            if (are_you_sure() == false)
            {           
                free2d(possible_commands);
                free2d(tokens);
                clear_current_line();
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
            free2d(tokens);
            refresh_line(terminal);
            return NULL;
        }
        free2d(tokens);
        return possible_commands; 
    }  

    cursor_type = second_or_more(buffer, terminal->cursor_position);

    if (cursor_type == WHITESPACE)
    {

        possible_commands = this_directory(buffer, terminal->cursor_position);
        if (possible_commands == NULL)
            return NULL;

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
            free2d(tokens);
            refresh_line(terminal);
            return NULL;
        }
    }

    free2d(tokens);
   // return possible_commands;   
   return insert_new_command(buffer, terminal->cursor_position, possible_commands); 

}

// int     main(void)
// {

//    get_path_programs("/Users/alysonngonyama/anaconda3/bin:/Users/alysonngonyama/anaconda3/condabin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin", "");



//     return 0;
// }
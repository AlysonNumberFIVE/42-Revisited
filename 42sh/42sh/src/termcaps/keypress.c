
#include "../../lib/termcaps.h"


char **possible_commands = NULL;
size_t command_list = 0;
char *current = NULL;
t_history *history;


static char directions()
{
    char token_sequence[3];
    size_t read_bytes;

    read_bytes = read(STDIN_FILENO, &token_sequence[0], 1);
    if (read_bytes == 0)
        return 0;

    read_bytes = read(STDIN_FILENO, &token_sequence[1], 1);
    if (read_bytes == 0)
        return 0;
 
    if (token_sequence[0] == '[')
    {
        if (token_sequence[1] == ARROW_RIGHT)
            return 1;
        else if (token_sequence[1] == ARROW_LEFT)
            return -1;
        else if (token_sequence[1] == ARROW_UP)
        {
            // TODO: Make an env variable for HISTFILESIZE to
            // determine upper limit of the history file.
            navigate_up(&history);
            return ARROW_UP;
        }
        else if (token_sequence[1] == ARROW_DOWN)
        {
            navigate_down(&history);
            return ARROW_DOWN;
        }
    }
    return 0;
}


void    handle_newline(t_termcap **terminal_id)
{
    t_termcap *local_terminal_id;

    local_terminal_id = *terminal_id;
    if (local_terminal_id->cursor_position > 0)
    {
        // Read only must be a permanent record of what the buffer was the firs time
        // it was written to preserver its data in memory.
        if (strlen(history->buffer_read_only) == 0)
            memcpy(history->buffer_read_only, history->buffer, strlen(history->buffer));
        
        insert_history(&history, history->buffer, history->buffer_read_only);
    }

    printf("\n");
    local_terminal_id->cursor_position = 0;
    local_terminal_id->cursor_row++;
    move_cursor(local_terminal_id, 0, local_terminal_id->cursor_row);
    printf("%s", local_terminal_id->prompt);
    fflush(stdout);
}


int handle_keypress(t_termcap *terminal_id)
{
    ssize_t read_bytes;
    char c;
    int cursor_adjuster;

    cursor_adjuster = 0;
    c = 0;
    read_bytes = 0;
    while (42)
    {
        read_bytes = read(STDIN_FILENO, &c, 1);
        if (read_bytes == -1) 
            return -1;

        if (c == CTRL_C)
            exit(1);
            
        else if (c == TAB) 
        {   
     
            if (possible_commands != NULL && strcmp(history->buffer, current) == 0)
            {
                if (command_list >= arraylen(possible_commands) - 1)
                    command_list = 0;
                
                memset(history->buffer, 0, strlen(history->buffer));
                memcpy(history->buffer, possible_commands[command_list], strlen(possible_commands[command_list]));
                terminal_id->cursor_position = strlen(history->buffer);
                command_list++;
                current = strdup(history->buffer);
            }
            else
            {
                clear_screen_downward();
                // TODO: Move this string to PATH in the env
                possible_commands = autocomplete(history->buffer, terminal_id, 
                    "/Users/alysonngonyama/anaconda3/bin:/Users/alysonngonyama/anaconda3/condabin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"
                );
                current = strdup(history->buffer);
            }
        }
        else if (c == NEWLINE)
        {
            if (possible_commands != NULL)
            {
                clear_next_line();
                free2d(possible_commands);
                possible_commands = NULL;
            }
            handle_newline(&terminal_id);
            return 1 ;
        }
        else if (c == ESC_SEQ)
        { 
            cursor_adjuster = directions();
            if (cursor_adjuster == 0)
                continue ;
        
            else if (cursor_adjuster == ARROW_DOWN || cursor_adjuster == ARROW_DOWN)
                terminal_id->cursor_position = strlen(history->buffer) - 1;
            
        
            terminal_id->cursor_position += cursor_adjuster;
            if (terminal_id->cursor_position < 0)
                terminal_id->cursor_position = 0;
            else if ((size_t)terminal_id->cursor_position >= strlen(history->buffer))
                terminal_id->cursor_position = strlen(history->buffer);
        }
        else if (c == BACKSPACE)
        {
            if (terminal_id->cursor_position > 0)
            {
                memmove(&history->buffer[terminal_id->cursor_position - 1], &history->buffer[terminal_id->cursor_position], strlen(history->buffer) - terminal_id->cursor_position + 1);
                terminal_id->cursor_position--;
            }
        }
        else 
        {
            if ((size_t)terminal_id->cursor_position <= strlen(history->buffer) && terminal_id->cursor_position > -1) 
            {
                memmove(&history->buffer[terminal_id->cursor_position + 1], &history->buffer[terminal_id->cursor_position], strlen(history->buffer) - terminal_id->cursor_position);
                history->buffer[terminal_id->cursor_position] = c;
                terminal_id->cursor_position++;
            }
        }
        refresh_line(terminal_id);
    }
    return 0;
}



void write_terminal_buffer(char buffer[], t_termcap *terminal)
{
    size_t len;
    size_t i;

    len = strlen(buffer);
    i = 0;

    fflush(stdout);
    while (i < len)
    {
        if (i == (size_t)terminal->cursor_position)
        {
            move_cursor(
                terminal, terminal->cursor_position + terminal->prompt_length,
                terminal->cursor_row
            );
            printf("\033[7m%c\033[m", buffer[terminal->cursor_position]);
            fflush(stdout);
        }

        i++;
    }
}

void refresh_line(t_termcap *terminal)
{
    move_cursor(terminal, 0, terminal->cursor_row);
    clear_line(terminal);
    printf("%s%s", terminal->prompt, history->buffer);

    write_terminal_buffer(history->buffer, terminal);
    fflush(stdout);
}

int main(void)
{
    t_termcap *termcap_manager;
    int new_data;

    enable_raw_mode();
    termcap_manager = init_termcaps();

    // TODO: Store history file in HISTFILESIZE 
    history = init_history(".hidden_history_file_TEST");
    goto_last(&history);
    set_prompt(termcap_manager, "42sh$>");

    new_data = 0;
    clear_screen();
    while (1)
    {
        printf("%s", termcap_manager->prompt);
        fflush(stdout);

        new_data = handle_keypress(termcap_manager);

        move_cursor(termcap_manager, 0, termcap_manager->cursor_row);
        if (new_data == 1)
        {  
            memset(history->buffer, 0, sizeof(history->buffer));
            termcap_manager->cursor_position = 0;
        }
        else 
            termcap_manager->cursor_position = strlen(history->buffer);


    }
    return 0;
}
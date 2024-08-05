
#include "../../lib/termcaps.h"

char buffer[4096];
ssize_t history_depth = 0;

static char directions(bool *is_command_history, size_t history_size)
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
            history_depth++;
            if (history_depth >= history_size - 1)
                history_depth = history_size - 1;
            *is_command_history = true;
        }
        else if (token_sequence[1] == ARROW_DOWN)
        {
            history_depth--;
            if (history_depth < 0)
                history_depth = 0;
            *is_command_history = true;
        }
    }
    return 0;
}


void    handle_newline(t_termcap **terminal_id)
{
    t_termcap *local_terminal_id;

    local_terminal_id = *terminal_id;
    printf("\n");
    local_terminal_id->cursor_position = 0;
    local_terminal_id->cursor_row++;
    move_cursor(local_terminal_id, 0, local_terminal_id->cursor_row);
    printf("%s", local_terminal_id->prompt);
    fflush(stdout);
}

char *history_lookup(t_termcap *terminal_id)
{     
    char *newstr;
    newstr = strdup(terminal_id->history[history_depth]);  
    return newstr;
}

void handle_keypress(t_termcap *terminal_id)
{
    ssize_t read_bytes;
    char c;
    bool is_command_history;
    int cursor_adjuster;
    char *tmp;

    is_command_history = false;
    cursor_adjuster = 0;
    c = 0;
    read_bytes = 0;
    while (42)
    {
        read_bytes = read(STDIN_FILENO, &c, 1);
        if (read_bytes == -1) 
            return ;

        if (c == CTRL_C)
        {
            exit(1);
            return  ;
        }
        else if (c == NEWLINE)
        {
            handle_newline(&terminal_id);
            break ;
        }
        else if (c == ESC_SEQ)
        { 
            cursor_adjuster = directions(&is_command_history, terminal_id->history_file_size);

            if (is_command_history == true)
            {
                tmp = history_lookup(terminal_id);
                memset(buffer, 0, READ_BUFF_SIZE);
                memmove(buffer, tmp, strlen(tmp));
                clear_line(terminal_id);
                printf("%s", buffer);
                fflush(stdout);
                free(tmp);
                is_command_history = false;
                break ;
            }
            if (cursor_adjuster == 0)
                continue ;

            terminal_id->cursor_position += cursor_adjuster;
            if (terminal_id->cursor_position < 0)
                terminal_id->cursor_position = 0;
            else if ((size_t)terminal_id->cursor_position >= strlen(buffer))
                terminal_id->cursor_position = strlen(buffer);
        }
        else if (c == BACKSPACE)
        {
            if (terminal_id->cursor_position > 0)
            {
                memmove(&buffer[terminal_id->cursor_position - 1], &buffer[terminal_id->cursor_position], strlen(buffer) - terminal_id->cursor_position + 1);
                terminal_id->cursor_position--;
            }
        }
        else 
        {
            if ((size_t)terminal_id->cursor_position <= strlen(buffer) && terminal_id->cursor_position > -1) 
            {
                memmove(&buffer[terminal_id->cursor_position + 1], &buffer[terminal_id->cursor_position], strlen(buffer) - terminal_id->cursor_position);
                buffer[terminal_id->cursor_position] = c;
                terminal_id->cursor_position++;
            }
        }
        refresh_line(terminal_id);
    }

}



void write_terminal_buffer(char buffer[], t_termcap *terminal)
{
    size_t len;
    size_t i;

    len = strlen(buffer);
    i = 0;
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
        else
        {
           // write(1,(void*)&buffer[i], 1);
        }
        i++;
    }
}

void refresh_line(t_termcap *terminal)
{
    move_cursor(terminal, 0, terminal->cursor_row);
    clear_line(terminal);
    printf("%s%s", terminal->prompt, buffer);

    write_terminal_buffer(buffer, terminal);
    fflush(stdout);
}

int main(void)
{
    t_termcap *termcap_manager;

    enable_raw_mode();
    termcap_manager = init_termcaps();

    set_prompt(termcap_manager, "42sh$>");

    clear_screen();
    while (1)
    {
        printf("%s", termcap_manager->prompt);
        handle_keypress(termcap_manager);

        move_cursor(termcap_manager, 0, termcap_manager->cursor_row);
        memset(buffer, 0, sizeof(buffer));
        termcap_manager->cursor_position = 0;
    }
    return 0;
}
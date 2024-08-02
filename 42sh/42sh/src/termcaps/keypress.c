
#include "../../lib/termcaps.h"

char buffer[4096];
int history_depth = 0;

static char directions(void)
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
        }
        else if (token_sequence[1] == ARROW_DOWN)
        {
            history_depth--;
            if (history_depth < 0)
                history_depth = 0;
        }
    }
    return 0;
}

void handle_keypress(t_termcap *terminal_id)
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
            return ;

        if (c == CTRL_C)
        {
            exit(1);
            return  ;
        }
        else if (c == NEWLINE)
        {
            printf("\n");
            terminal_id->cursor_position = 0;
            terminal_id->cursor_row++;
            move_cursor(terminal_id, 0, terminal_id->cursor_row);
            printf("%s", terminal_id->prompt);
            fflush(stdout);
            break ;
        }
        else if (c == ESC_SEQ)
        { 
            cursor_adjuster = directions();
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
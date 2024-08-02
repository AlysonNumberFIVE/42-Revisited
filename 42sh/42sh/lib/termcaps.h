#ifndef _42SH_TERMCAPS_H
#define _42SH_TERMCAPS_H

#include "shell.h"
#include <termios.h>
#include <sys/ioctl.h>
#include <termcap.h>

#define PROMPT_LENGTH 128
#define CTRL_C 3
#define ESC_SEQ 27

#define NEWLINE '\r'
#define ARROW_RIGHT 'C'
#define ARROW_LEFT 'D'
#define ARROW_UP 'A'
#define ARROW_DOWN 'B'
#define BACKSPACE 127
#define PATH_DELIM '/'


typedef struct s_termcap_manager 
{
    char *cm;
    char *ce;
    int cursor_position;
    int cursor_row;
    int cursor_column;
    int screen_width;

    char prompt[PROMPT_LENGTH];
    int prompt_length;
}   t_termcap;

// Maybe to avoid global variable management of up/down?
typedef struct s_direction_handler 
{
    int left_or_right;
    int up_or_down;
}   t_direction;


void    refresh_line(t_termcap *terminal);
void    enable_raw_mode(void);
t_termcap *init_termcaps(void);
void    move_cursor(t_termcap *terminal, int column, int row);
void    clear_line(t_termcap *terminal);
void    blink_timer(void);
void    toggle_cursor_blink(int signum);
void    clear_screen(void);
void    set_prompt(t_termcap *terminal, const char *new_prompt);

#endif 
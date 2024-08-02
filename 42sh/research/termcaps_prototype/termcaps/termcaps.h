#ifndef _42SH_TERMCAPS_H
#define _42SH_TERMCAPS_H

#include "shell.h"
#include <termios.h>
#include <sys/ioctl.h>
#include <termcap.h>

typedef struct s_termcap_manager 
{
    char *cm;
    char *ce;
    int cursor_position;
    int cursor_row;
    int cursor_column;
    int screen_width;

    char prompt[128];
    int prompt_length;
}   t_termcap;



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
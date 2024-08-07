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
#define TAB 9
#define PATH_DELIM '/'

#define LOCAL_TOOLS_LENGTH 4


typedef struct  s_termcap_manager 
{
    char    *cm;
    char    *ce;
    int     cursor_position;
    int     cursor_row;
    int     cursor_column;
    int     screen_width;
    int     history_depth;

    int     history_fd;

    char    prompt[PROMPT_LENGTH];
    int     prompt_length;
}                  t_termcap;


// Terminal UI
void    refresh_line(t_termcap *terminal);
void    enable_raw_mode(void);
t_termcap *init_termcaps(void);
void    move_cursor(t_termcap *terminal, int column, int row);
void    clear_line(t_termcap *terminal);
void    blink_timer(void);
void    toggle_cursor_blink(int signum);
void    clear_screen(void);
void    set_prompt(t_termcap *terminal, const char *new_prompt);


typedef struct s_history_manager 
{
    char buffer[READ_BUFF_SIZE]; 

    char buffer_read_only[READ_BUFF_SIZE];
    size_t position;
    int index_DEBUG;

    struct s_history_manager *next;
    struct s_history_manager *prev;

}   t_history;



// Terminal history
void        navigate_up(t_history **history);
void        navigate_down(t_history **history);
void        push_history(t_history **history, char *record);
void        print_history(t_history *history, t_termcap *t);
t_history   *init_history(char *filename);
void        insert_history(t_history **history, char *buffer, char *buffer_read_only);
void        goto_last(t_history **history);



typedef struct  s_autocomplete_manager
{
    char **files;

} t_autocomplete;



#endif 
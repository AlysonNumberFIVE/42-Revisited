
#include "../../lib/termcaps.h"
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>


struct termios orig_termios;


void disable_raw_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void)
{
    struct termios raw;

    raw = orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

t_termcap *init_termcaps(void) 
{
    t_termcap *terminal_id;
    struct winsize w;
    char term_buffer[READ_BUFF_SIZE];

    terminal_id = NULL;

    // TODO: Make  
    char *terminal_type = getenv("TERM");

    if (tgetent(term_buffer, terminal_type) < 0)
        return NULL;
    
    terminal_id = (t_termcap *)malloc(sizeof(t_termcap));

    // TODO: this hidden file must be stored in HISTFILE
    terminal_id->history_fd = open(".hidden_history_file_TEST", O_RDWR);
    if (terminal_id->history_fd  == -1)
    {
        free(terminal_id);
        exit(1);
    }


    terminal_id->cm = tgetstr("cm", NULL);
    terminal_id->ce = tgetstr("ce", NULL);
    terminal_id->cursor_position = 0;

    terminal_id->cursor_row = 0;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminal_id->screen_width = w.ws_col;
    return terminal_id;
}

void move_cursor(t_termcap *terminal, int column, int row)
{
    char *location;
    location = tgoto(terminal->cm, column, row);
    tputs(location, 1, putchar);
}

void clear_line(t_termcap *terminal)
{
    tputs(terminal->ce, 1, putchar);
}

void clear_screen(void) 
{
    printf("\033[2J\033[H");
}


void    set_prompt(t_termcap *terminal, const char *new_prompt)
{
    strncpy(terminal->prompt, new_prompt, sizeof(terminal->prompt) - 1);
    terminal->prompt[sizeof(terminal->prompt) - 1] = '\0';
    terminal->prompt_length = strlen(terminal->prompt);
}

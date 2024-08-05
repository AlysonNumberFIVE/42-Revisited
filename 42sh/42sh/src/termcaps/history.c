
#include "../../lib/termcaps.h"

static t_history  *new_history(char *record)
{
    t_history   *history;
    size_t      record_length;

    history = (t_history *)malloc(sizeof(t_history));
    if (history == NULL)
        return NULL;

    record_length = strnlen(record, READ_BUFF_SIZE - 1);   
    memset(history->buffer, 0, READ_BUFF_SIZE);
    memmove(history->buffer, record, record_length);

    history->position = strlen(record);
    history->next = NULL;
    history->prev = NULL; 
    return history;
}

static void   add_history(t_history **history, char *record)
{
    t_history *traverse; 

    traverse = *history;
    while (traverse->next)
        traverse = traverse->next;

    traverse->next = new_history(record);
    if (traverse->next == NULL)
        return ;

    traverse->next->prev = traverse; 
}

void    push_history(t_history **history, char *record)
{
    if (*history == NULL)
        *history = new_history(record);
    else 
        add_history(history, record);
}

void    print_history(t_history *history)
{
    t_history *traverse;
    int i;

    i = 0;
    traverse = history;
    while (traverse)
    {
        printf("==================\n");
        printf("history number: %d\n", i);
        printf("data: %s\n", traverse->buffer);
        printf("positoin %zu\n", traverse->position);
        traverse = traverse->next;
    }
}

void        goto_last(t_history **history)
{
    while ((*history)->next)
        *history = (*history)->next;
}

void        navigate_up(t_history **history)
{
    if ((*history)->prev != NULL)
        *history = (*history)->prev;
}

void        navigate_down(t_history **history)
{
    if ((*history)->next != NULL)
        *history = (*history)->next;
}

t_history   *init_history(char *filename)
{
    char    *content;
    size_t  i;  
    ssize_t read_bytes;
    t_history *records;
    char **history;

    records = NULL;
    content = read_file(filename, O_RDWR, &read_bytes);
    if (read_bytes == -1)
        return NULL;

    if (read_bytes == 0 || content == NULL)
    {
        push_history(&records, "");
        return records;
    }

    history = split(content, '\n');
    i = 0;
    while (history[i])
    {
        push_history(&records, history[i]);
        i++;
    }
    
    push_history(&records, "");
    goto_last(records);
    return records;
}


// int main(void)
// {
//     t_history *history;

//     history = init_history(".hidden_history_file_TEST");
//     print_history(history);
//     printf("============");
//     printf("history node %s\n", history->buffer);
//     goto_last(&history);
//     printf("============");
//     printf("history node %s\n", history->buffer);
//     return (0);
// }
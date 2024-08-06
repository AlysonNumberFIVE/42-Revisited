
#include "../../lib/termcaps.h"

static t_history  *new_history(char *record, int index)
{
    t_history   *history;
    size_t      record_length;

    history = (t_history *)malloc(sizeof(t_history));
    if (history == NULL)
        return NULL;

    record_length = strnlen(record, READ_BUFF_SIZE - 1);   
    memset(history->buffer, 0, READ_BUFF_SIZE);
    memset(history->buffer_read_only, 0,READ_BUFF_SIZE);

    memcpy(history->buffer, record, record_length);

    memcpy(history->buffer_read_only, history->buffer, record_length);

    history->index_DEBUG = index;
    history->position = strlen(record);
    history->next = NULL;
    history->prev = NULL; 
    return history;
}

static void   add_history(t_history **history, char *record)
{
    t_history *traverse; 
    int index;

    index = 0;
    traverse = *history;
    while (traverse->next)
    {
        traverse->index_DEBUG = index;
        traverse = traverse->next;
        index++;
    }
    index ++ ;
    traverse->next = new_history(record, index);
    if (traverse->next == NULL)
        return ;

    traverse->next->prev = traverse; 
}

void    push_history(t_history **history, char *record)
{
    if (*history == NULL)
        *history = new_history(record, 0);
    else 
        add_history(history, record);
}

void    insert_history(t_history **history, char *buffer, char *buffer_read_only)
{
    t_history *traverse;
    t_history *new_data;
    
    traverse = *history;
    if ((*history)->next != NULL)
    {
        while (traverse->next)
            traverse = traverse->next;

        memcpy(traverse->buffer, buffer, strlen(buffer));
        memset((*history)->buffer, 0, READ_BUFF_SIZE);
        memcpy((*history)->buffer, buffer_read_only, strlen(buffer_read_only));
    }

    new_data = new_history("", 0);
    traverse->next = new_data;
    traverse->next->prev = traverse;
    *history = new_data;
    
}   

void    print_history(t_history *history, t_termcap *t)
{
    t_history *traverse;
    int i;

    i = 0;
    traverse = history;
    while (traverse)
    {
        printf("==================\n");
        clear_line(t);
        printf("history number: %d\n", i);
        clear_line(t);
        printf("data: %s\n", traverse->buffer);
        clear_line(t);

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
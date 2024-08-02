#include "../lib/shell.h"


t_prompt_state *g_st = NULL;


t_input_status      *invalidate_stack()
{
    t_input_status *status;

    status = (t_input_status *)malloc(sizeof(t_input_status));
    status->status_code = false;
    free_stack(&g_st->input_stack);
    return status;
}


void        handle_backtick(char *type)
{
    t_stack *item;

    if (g_st->backtick == true)
    {
        push(&g_st->input_stack, (void *)type, strlen(type));
        g_st->backtick = false;
    }
    else
    { 
        item = pop(&g_st->input_stack);
        if (item != NULL && strcmp((char *)item->data, "BACKTICK") != 0)
        {
            free_stack_item(item);
            while (item->prev != NULL && strcmp((char *)item->data, "BACKTICK") != 0)
            {
                item = pop(&g_st->input_stack);
                free_stack_item(item);
            }
        }
        g_st->backtick = true;
    } 
}

void        handle_quote(char *type)
{
    t_stack *item;

    if (g_st->quote == true)
    {
        push(&g_st->input_stack, (void *)type, strlen(type));
        g_st->quote  = false;
    }
    else
    { 
        item = pop(&g_st->input_stack);
        if (item != NULL && strcmp((char *)item->data, "QUOTE") != 0)
        {
            free_stack_item(item);
            while (item->prev != NULL && strcmp((char *)item->data, "QUOTE") != 0)
            {
                item = pop(&g_st->input_stack);
                free_stack_item(item);
            }
        }
        g_st->quote  = true;
    }
}

void        handle_doublequote(char *type)
{
    t_stack *item;

    if (g_st->doublequote == true)
    {
        push(&g_st->input_stack, (void *)type, strlen(type));
        g_st->doublequote  = false;
    }
    else
    { 
        item = pop(&g_st->input_stack);
        if (item != NULL && strcmp((char *)item->data, "DOUBLEQUOTE") != 0)
        {
            free_stack_item(item);
            while (item->prev != NULL && strcmp((char *)item->data, "DOUBLEQUOTE") != 0)
            {
                item = pop(&g_st->input_stack);
                free_stack_item(item);
            }
        }
        g_st->doublequote  = true;
    }
}

void free_status(t_input_status *status)
{
    if (status->subshell_prompt != NULL)
        free(status->subshell_prompt);
    free(status);
}

void    handle_shell_input(t_token *tokens) 
{
    t_token *trav;
    t_stack *item;
    char *type;

    trav = tokens;
    g_st->is_balanced = true;
    while (trav)
    {
        type = trav->type;
        if (strcmp(type, "BACKTICK") == 0 && g_st->quote == true && g_st->doublequote == true)
             handle_backtick(type);
        if (g_st->backtick == false)
        {
            trav = trav->next;
            continue ;
        }

        if (strcmp(type, "QUOTE") == 0 && g_st->backtick == true && g_st->doublequote == true) 
            handle_quote(type);
        if (g_st->quote == false)
        {
            trav = trav->next;
            continue ;
        }

        if (strcmp(type, "DOUBLEQUOTE") == 0 && g_st->backtick == true && g_st->quote == true) 
            handle_doublequote(type);
        if (g_st->doublequote == false)
        {
            trav = trav->next;
            continue ;
        }

        if (strcmp(type, "OPENBRACKET") == 0 || strcmp(type, "OPENCURLY") == 0 ||
                strcmp(type, "PROCESS_SUB_IN") == 0 || strcmp(type, "PROCESS_SUB_OUT") == 0 )
            push(&g_st->input_stack, (void *)type, strlen(type));
        

        else if (g_st->input_stack != NULL) 
        {   
            if (strcmp(type, "CLOSEBRACKET") == 0)
            {
                item = pop(&g_st->input_stack);
                if (strcmp((char*)item->data, "OPENBRACKET") != 0 &&
                        strcmp((char*)item->data, "PROCESS_SUB_IN") != 0)
                {
                    free_stack_item(item);
                    g_st->is_balanced = false;
                    break ;
                }
            }
            else if (strcmp(type, "CLOSECURLY") == 0)
            {
                item = pop(&g_st->input_stack);
                if (strcmp((char*)item->data, "OPENCURLY") != 0)
                {
                    free_stack_item(item);
                    g_st->is_balanced = false;
                    break ;
                }
            }
        }
        trav = trav->next;
    } 
}
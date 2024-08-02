#include "../lib/shell.h"


static t_token  *new_token(char *name, char *type) 
{
    t_token *token;

    token = (t_token *)malloc(sizeof(t_token));
    token->name = strdup(name);
    token->type = strdup(type);
    token->next = NULL;
    return (token);
}


static void     add_token(t_token **token, char *name, char *type)
{
    t_token *traverse;

    traverse = *token;
    while (traverse->next)
        traverse = traverse->next;
    
    traverse->next = new_token(name,type);
}


void            push_token(t_token **token, char *name, char *type)
{  
    if (*token == NULL)
        *token = new_token(name, type);
    else
        add_token(token, name, type);
}


void         join_tokens(t_token **first, t_token *second)
{
    t_token *trav;

    trav = second;
    while (trav)
    {        
        push_token(first, trav->name, trav->type);
        trav = trav->next;
    }

}
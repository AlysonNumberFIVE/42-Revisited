
#include "../lib/shell.h"


t_stack *new_data(void *data, size_t size) 
{
    t_stack *stack = (t_stack *)malloc(sizeof(t_stack));
    if (!stack) return NULL;

    stack->data = malloc(size);
    if (!stack->data) {
        free(stack);
        return NULL;
    }

    memcpy(stack->data, data, size);
    stack->data_size = size;
    stack->next = NULL;
    stack->prev = NULL;

    return stack;
}

t_stack *pop(t_stack **stack) 
{
    t_stack *traverse; 

    traverse = NULL;

    if (*stack == NULL)
        return NULL;

    traverse = *stack;
    while (traverse->next) 
        traverse = traverse->next;

    if (traverse == *stack) 
        *stack = NULL;
    else
        traverse->prev->next = NULL;

    return traverse;
}

void push(t_stack **stack, void *data, size_t size) 
{
    t_stack *new_node;
    t_stack *traverse;
    
    new_node = new_data(data, size);
    if (!new_node) 
        return;

    if (!*stack) 
        *stack = new_node;
    else 
    {
        traverse = *stack;
        while (traverse->next) 
            traverse = traverse->next;

        traverse->next = new_node;
        new_node->prev = traverse;
    }
}

bool is_empty(t_stack *tokens)
{
    if (tokens == NULL || (tokens->data == NULL && tokens->next == NULL && tokens->prev == NULL))
        return true;
    
    return false;
}

void print_stack(t_stack *tokens) 
{
    t_stack *trav;

    trav = tokens;
    while (trav) 
    {
        printf("%s ", (char *)trav->data);
        trav = trav->next;
    }
    printf("\n");
}

void push_process(t_stack **stack, t_process *proc) 
{
    if (*stack == NULL)
        *stack = new_data(proc, sizeof(t_process));
    else 
        push(stack, proc, sizeof(t_process));
}

void free_stack(t_stack **stack)
{
    t_stack *trav;
    t_stack *tmp;

    trav = *stack;
    while (trav)
    {
        tmp = trav;
        trav = trav->next;
        free(tmp->data);
        free(tmp);
    }
    *stack = NULL;
}


t_stack *pop_process(t_stack **stack) 
{
    return pop(stack);
}

void    free_stack_item(t_stack *item)
{
    free(item->data);
    free(item);
}

t_prompt_state     *create_stack_manager(void) 
{
    t_prompt_state *stack; 

    stack = (t_prompt_state *)malloc(sizeof(t_prompt_state));
    stack->backtick = true;
    stack->quote = true;
    stack->doublequote = true;
	stack->is_balanced = false;
    stack->input_stack = NULL;
    stack->prompt_buffer = NULL;
    return stack;
}
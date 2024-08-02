
#include "../lib/shell.h"


extern t_prompt_state *g_st;

t_map prompts[] = {
    {"OPENBRACKET", "subsh>"},
    {"OPENCURLY", "cursh>"},
    {"PROCESS_SUB_IN", "cmdsubst>"},
    {"PROCESS_SUB_OUT", "cmdsubst>"},
    {"ARITHMATIC_OPENBRACE", "math>"},
    {"BACKTICK", "bquote>"},
    {"QUOTE", "quote>"},
    {"DOUBLEQUOTE", "dquote>"},
    {"EOF", "EOF"}
};

char    *new_prompt(char *token_type)
{
    int i;

    i = 0;
    while (strcmp(prompts[i].key, "EOF") != 0)
    {
        if (strcmp(prompts[i].key, token_type) == 0) 
            return strdup(prompts[i].value);
        i++;
    }

    return NULL;
}

static t_input_status *new_status(bool is_complete, char *prompt)
{
    t_input_status *status;

    status = (t_input_status *)malloc(sizeof(t_input_status));
    status->status_code  = is_complete;
    status->subshell_prompt = strdup(prompt);
    return status;
}

t_input_status *change_prompt(char *current_prompt, char *default_prompt) 
{
    free(current_prompt);
    t_stack *traverse;
    char *prompt;
    char *temp;
    t_input_status *status;
    char *shell_prompt_stack;

    shell_prompt_stack = NULL;
    if (g_st->input_stack != NULL) 
    {
        shell_prompt_stack = (char *)malloc(sizeof(char));
        traverse = g_st->input_stack;
        while (traverse)
        {
            prompt = new_prompt((char *)traverse->data);
            temp = join(shell_prompt_stack, prompt);
            free(shell_prompt_stack);
            shell_prompt_stack = charpush(temp, ' ');
            traverse = traverse->next;
        }
        return new_status(false, shell_prompt_stack);
    }

    current_prompt = strdup(default_prompt);
    free(default_prompt);
    return new_status(true, current_prompt);
}


char    *shell()
{  
    char buffer[READ_BUFF_SIZE];
    t_token *tokens;
    ssize_t read_bytes;
    char *prompt;
    t_input_status *status;

    g_st = create_stack_manager();

    // TODO: Make this in the init_shell() where the prompt will be set.
    prompt = strdup("$>");
    tokens = NULL;
    read_bytes = 0;
    while (_42SH)
    {
        bzero(buffer, READ_BUFF_SIZE);
        write(STDIN_FILENO, prompt, strlen(prompt));
        
        read_bytes = read(FD_STDIN, buffer, READ_BUFF_SIZE);
        if (is_newline(read_bytes, buffer)) 
            continue ;

        tokens = lexer(buffer);
        handle_shell_input(tokens);
        join_tokens(&g_st->prompt_buffer, tokens);
        clear_tokens(tokens);
        status = change_prompt(prompt, strdup("$>"));
        prompt = strdup(status->subshell_prompt);
        if (status->status_code == IS_COMPLETE)
        {
            // exeute code here.
            print_tokens(g_st->prompt_buffer);
            clear_tokens(g_st->prompt_buffer);
            g_st->prompt_buffer = NULL;
        } 


    }
    

}

int main() 
{
    shell();
    return 0;
}
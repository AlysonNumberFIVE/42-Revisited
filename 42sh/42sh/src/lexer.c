
#include "../lib/shell.h"
#include <stdbool.h>
#include <ctype.h>
 
 t_token g_token_list[] = {
	{"<<\0", "HERE_DOCUMENT", NULL},
	{">>\0", "OUTPUT_APPEND", NULL},
	{"<&\0", "INPUT_FD_DUPLICATE", NULL},
	{">&\0", "OUTPUT_FD_DUPLICATE", NULL},
    {"<=\0", "LESS_EQU", NULL},
    {">=\0", "GREATER_EQU", NULL},
    {"<(\0", "PROCESS_SUB_IN", NULL},
    {">(\0", "PROCESS_SUB_OUT", NULL},
    {",(\0", "UNKNOWN", NULL},
	{"||\0", "CONDITIONA_OR", NULL},
    {"&&\0", "CONDITIONA_AND", NULL},
    {"((\0", "ARITHMATIC_OPENBRACE", NULL},
    {"))\0", "ARITHMATIC_CLOSEBRACE", NULL},
    {"++\0", "INCREMENT", NULL},
    {"--\0", "DECREMENT", NULL},
    {"==\0", "DECREMENT", NULL},
    {"=!\0", "NOT_EQUAL", NULL},

	{"eof\0", "EOF", NULL}

};

static void double_token(char *buffer, size_t *counter, t_token **tokens, char *default_val, char *default_type) 
{
    char *new_token;
    int i;

    new_token = subindex(&buffer[*counter], 2);
    i = 0;
    while (strcmp(g_token_list[i].type, "EOF") != 0)
    {
        if (strcmp(new_token, g_token_list[i].name) == 0)
        {
            *counter += 1;
            return push_token(tokens, new_token, g_token_list[i].type);
        }
        i++;
    }
    return push_token(tokens, default_val, default_type);
}



static void number_token(char *buffer, size_t *counter, t_token **tokens)
{
    char *number;
    size_t i;
    bool is_float;

    i = *counter;
    is_float = false;
    number = NULL;
    while (buffer[i] && isdigit(buffer[i]))
    {
        if (buffer[i] == '.') 
        {
            is_float = true;
            number = charpush(number, buffer[i]);
            break;
        }
        number = charpush(number, buffer[i]);
        i++;
    }
    if (is_float == true) 
    {
        i++;
        while (buffer[i] && isdigit(buffer[i]))
        {
            number = charpush(number, buffer[i]);
            i++;
        }
        return push_token(tokens, number, "FLOAT");
    }
    *counter = i;
    return push_token(tokens, number, "NUMBER");
}

static void id_token(char *buffer, size_t *counter, t_token **tokens)
{
    char *token_name;
    size_t i;
    
    i = *counter;
    token_name = NULL;
    while (buffer[i] && (isalnum(buffer[i]) || buffer[i] == '_'))
    {
        token_name = charpush(token_name, buffer[i]);
        i++;
    }
    *counter = i;
    return push_token(tokens, token_name, "ID");
}


static size_t lex(char *buffer, size_t counter, t_token **tokens) 
{
    size_t index;
    size_t buffer_length;

    buffer_length = strlen(buffer);
    index = counter;
    while (index < buffer_length)
    {   
        if (buffer[index] == ' ' || buffer[index] == '\t')
        {
            index++;
            continue ;
        }
        break ;
    }

    if (buffer[index] == '&') double_token(buffer, &index, tokens, "&", "AND");
    else if (buffer[index] == '|') double_token(buffer, &index, tokens, "|", "PIPE");
    else if (buffer[index] == '<') double_token(buffer, &index, tokens, "<", "INPUT_REDIRECT");
    else if (buffer[index] == '>') double_token(buffer, &index, tokens, ">", "OUTPUT_REDIRECT");
    else if (buffer[index] == '(') double_token(buffer, &index, tokens, "(", "OPENBRACKET");
    else if (buffer[index] == ')') double_token(buffer, &index, tokens, ")", "CLOSEBRACKET");
    else if (buffer[index] == '+') double_token(buffer, &index, tokens, "+", "PLUS");
    else if (buffer[index] == ',') double_token(buffer, &index, tokens, ",", "COMMA");
    
    else if (buffer[index] == '[') push_token(tokens, "[", "OPENSQUARE");
    else if (buffer[index] == ']') push_token(tokens, "]", "CLOSESQUARE");
    else if (buffer[index] == '*') push_token(tokens, "*", "STAR");
    else if (buffer[index] == '?') push_token(tokens, "?", "QMARK");
    else if (buffer[index] == '\\') push_token(tokens, "\\", "BACKSLASH");
    else if (buffer[index] == ';') push_token(tokens, ";", "SEMICOLON");  
    else if (buffer[index] == '{') push_token(tokens, "{", "OPENCURLY");
    else if (buffer[index] == '}') push_token(tokens, "}", "CLOSECURLY");
    else if (buffer[index] == '$') push_token(tokens, "$", "DOLLARSIGN");
    else if (buffer[index] == '%') push_token(tokens, "%%", "PERCENT");
    else if (buffer[index] == '#') push_token(tokens, "#", "HASH");
    else if (buffer[index] == '-') push_token(tokens, "-", "DASH");
    else if (buffer[index] == '=') push_token(tokens, "=", "EQUAL");

    else if (buffer[index] == '\'') push_token(tokens, "\'", "QUOTE");
    else if (buffer[index] == '"') push_token(tokens, "\"", "DOUBLEQUOTE");
    else if (buffer[index] == '`') push_token(tokens, "`", "BACKTICK");

    else if (isdigit(buffer[index])) number_token(buffer, &index, tokens);
    else if (isalpha(buffer[index]) || buffer[index] == '_') id_token(buffer, &index, tokens);

    index++;
    return index;
}

void print_tokens(t_token *tokens)
{
    t_token *trav;

    trav = tokens;
    while (trav)
    {
        write(1, (void*)trav->name, strlen(trav->name));
        write(1, (void*)"-> ", 4);
        trav = trav->next;
    }
    write(1, (void*)"\n", 1);
}

void        clear_tokens(t_token *token)
{
    t_token *trav;
    t_token *temp;

    trav = token;
    while (trav)
    {
        temp = trav;
        trav = trav->next;

        free(temp->name);
        free(temp->type);
        free(temp);
    }
}

t_token *lexer(char *input_buffer) 
{
    size_t index;
    size_t input_buffer_length;
    t_token *tokens;

    index = 0;
    tokens = NULL;
    input_buffer_length = strlen(input_buffer);
    while (index < input_buffer_length)
        index = lex(input_buffer, index, &tokens);
    
  //  print_tokens(tokens);
    return tokens;
}   

// int main(void) {
//     lexer(">> << < > & <& <( )) )) ( ) | | !! ! Hello World 12412.124 1412 , hello1241 _4124iifnd ");
// }







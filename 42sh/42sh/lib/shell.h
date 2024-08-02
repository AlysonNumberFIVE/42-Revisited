#ifndef SHELL_H 
#define SHELL_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
# include "../../alylibc/inc/lib.h"


typedef struct s_env 
{
	char 		*key;
	char 		*value;
	struct s_env 	*next;
}	t_env;

typedef struct s_user 
{
	char *name;
	t_env *environ;
}	t_user;

#define ENV_KEY 0
#define ENV_VALUE 1
#define CHDIR_SUCCESS 0
#define FD_STDIN 0
#define READ_BUFF_SIZE 4096
#define _42SH 42
#define FORTY_TWO_SH 42
#define IS_COMPLETE true



// ENV support
void push_env(t_env **env, char *key, char *value);
void delete_env(t_env **env, char *key);
char *get_env(t_env *env, char *find);
void free_env(t_env *env);
void update_env(t_env **env, char *key, char *new_value);
void env_cmd(t_env *env);


// PARSER
typedef struct s_token 
{
	char 			*name;
	char 			*type;
	struct s_token 	*next;
}	t_token;



void   		push_token(t_token **token, char *name, char *type);
void 		print_tokens(t_token *tokens);
t_token 	*lexer(char *input_buffer);
void   		clear_tokens(t_token *token);
void         join_tokens(t_token **first, t_token *second);


// PROGRAM 
typedef struct s_process
{
	char 			*name;
	int 			(*process_handler)(struct s_token *);
	char 			*output;
	struct s_token 	*args;
	int 			result;
}	t_process;


typedef struct s_execution_stack 
{
	t_process 					*process;

	struct s_execution_stack 	*next;
	struct s_execution_stack 	*prev;
}	t_exec_stack;

typedef struct s_stack 
{
	void *data;
	size_t data_size;
	
	struct s_stack *next;
	struct s_stack *prev;
}	t_stack;






void 		push(t_stack **stack, void *data, size_t data_size);
t_stack 	*pop(t_stack **stack);
void 		free_stack(t_stack **stack);
bool 		is_empty(t_stack *tokens);
void 		print_stack(t_stack *tokens);

void		push_process(t_stack **stack, t_process *proc);
t_stack 	*pop_process(t_stack **stack);
void    	free_stack_item(t_stack *item);


typedef struct s_input_status 
{
	bool status_code;
	char *subshell_prompt;
}	t_input_status;

typedef struct s_map
{
	char *key;
	char *value;
}	t_map;



typedef struct s_prompt_state
{
	t_stack 	*input_stack;
	t_token 	*prompt_buffer;

	bool        backtick;
	bool        quote;
	bool 		doublequote;
	bool 		is_balanced;
}	t_prompt_state;

t_prompt_state     *create_stack_manager(void);
void    			handle_shell_input(t_token *tokens);
char    			*new_prompt(char *token_type);
void 				free_status(t_input_status *status);


// helpers
void    			print_bool(bool value);
bool    			is_newline(size_t read_bytes, char buffer[]);

#endif

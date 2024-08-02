

#include "../lib/shell.h"

static t_env *new_env_node(char *key, char *value) 
{
	t_env 	*env;

	env = (t_env *)malloc(sizeof(t_env));
	env->key = strdup(key);
	env->value = strdup(value);
	env->next = NULL;
	return env;
}

static void append_env_node(t_env **env, char *key, char *value) 
{
	t_env 	*traverse;

	traverse = *env;
	while (traverse->next)
		traverse = traverse->next;

	traverse->next = new_env_node(key, value);
}

void push_env(t_env **env, char *key, char *value) 
{
	if (*env == NULL)
		*env = new_env_node(key, value);
	else 
		append_env_node(env, key, value);
}

void env_cmd(t_env *env) 
{
	t_env *traverse;

	traverse = env;
	while (traverse) 
	{
		printf("%s=%s\n", traverse->key, traverse->value);
		traverse = traverse->next;
	}
}

void update_env(t_env **env, char *key, char *new_value)
{
	t_env *traverse;

	traverse = *env;
	while (traverse) 
	{
		if (strcmp(traverse->key, key) == 0)
		{
			free(traverse->value);
			traverse->value = strdup(new_value);
			break;
		}
		traverse = traverse->next;
	}
}

void free_env(t_env *env)
{		
	free(env->key);
	free(env->value);
	free(env);
}

char *get_env(t_env *env, char *find)
{
	t_env *traverse;

	traverse = env;
	while (traverse)
	{
		if (strcmp(traverse->key, find) == 0)
			return strdup(traverse->value);
		traverse = traverse->next;
	}

	return NULL;
}

void delete_env(t_env **env, char *key)
{
	t_env *traverse;
	t_env *tmp;

	traverse = *env;
	if (traverse == NULL) 
		return ;
	if (strcmp(traverse->key, key) == 0)
	{
		tmp = traverse;
		traverse = traverse->next;
		*env = traverse;
		free_env(tmp);
	}
	else 
	{
		while (traverse->next)
		{
			if (strcmp(traverse->next->key, key) == 0)
			{
				tmp = traverse->next;
				traverse->next = traverse->next->next;
				free_env(tmp);
				break;
			}
			traverse = traverse->next;
		}
	}
}


// int main(void) 
// {
// 	t_env *env;

// 	env = NULL;
//  	push_env(&env, "HELLO", "WORLD");
// 	push_env(&env, "MY", "NAME");
// 	update_env(&env, "MY", "SURNAME");
// 	push_env(&env, "IS", "ALYSON");

// //	env_cmd(env);
// 	delete_env(&env, "42");
// 	env_cmd(env);
// 	printf("$>%s\n", get_env(env, "asdg"));
// 	t_env *trav;
// 	// trav = env;
// 	// while (trav) {
// 	// 	printf("trav is %s %s\n", trav->key, trav->value);
// 	// 	trav = trav->next;
// 	// }
// }








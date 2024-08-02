

#include "../lib/shell.h"

t_env 	*init_environ(void) 
{	
	extern char **environ;
	char **env_key_value;
	t_env *local_env;
	size_t index;
	size_t shell_level;

	local_env = NULL;
	index = 0;
	while (environ[index]) 
	{
		printf("env : %s\n", environ[index]);
		env_key_value = split(environ[index], '=');
		if (strcmp(env_key_value[ENV_KEY], "SHLVL") == 0)
		{
			shell_level = (size_t)atoi(env_key_value[ENV_VALUE]);
			shell_level++;
			free(env_key_value[ENV_VALUE]);
			env_key_value[ENV_VALUE] = itoa(shell_level);
		}
		push_env(&local_env, env_key_value[ENV_KEY], env_key_value[ENV_VALUE]);
		free2d((void*)env_key_value);
		index++;
	}

	return local_env;
}

// int main(void) {
// 	t_env *env = init_environ();
// 	env_cmd(env);
// }

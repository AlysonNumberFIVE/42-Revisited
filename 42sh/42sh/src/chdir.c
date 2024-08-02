
#include "../lib/shell.h"
#include <limits.h>

void  switch_pwd_env(t_env **env, char *new_pwd) 
{
    char *current_pwd;

    current_pwd = get_env(*env, "PWD");
    update_env(env, "OLDPWD", current_pwd);
    update_env(env, "PWD", new_pwd);
}

int change_directory(t_env **env, char *path) 
{
    int status;
    char current_dir[PATH_MAX];

    bzero(current_dir, PATH_MAX);
    status = chdir(path);
    if (status == CHDIR_SUCCESS)
    {
        getcwd(current_dir, PATH_MAX);
        switch_pwd_env(env, current_dir);
    }
    return status;
}


// int main(void)
// {
//     t_env *env = NULL;
//     push_env(&env, "PWD", "/Users/alysonngonyama/42/42sh/src");
//     push_env(&env, "OLDPWD", "");

//     system("ls");
//     env_cmd(env);
//     change_directory(&env, "/Users/alysonngonyama");
//     printf("\n\n");
//     system("ls");
//     env_cmd(env);

//     change_directory(&env, "Desktop");
//     printf("\n\n");
//     system("ls");
//     env_cmd(env);
// }


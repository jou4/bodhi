#include "env.h"

Env *env_new()
{
    return map_new();
}

void env_destroy(Env *env)
{
    map_destroy(env);
}

Env *env_local_new(Env *env)
{
    return map_clone(env);
}

void env_local_destroy(Env *env)
{
    map_destroy(env);
}

void env_set(Env *env, const char *key, void *val)
{
    map_set(env, key ,val);
}

void *env_get(Env *env, const char *key)
{
    return map_get(env, key);
}

Vector *env_keys(Env *env)
{
    return map_keys(env);
}

Vector *env_values(Env *env)
{
    return map_values(env);
}

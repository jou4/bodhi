#ifndef _env_h_
#define _env_h_

#include "util.h"

typedef Map Env;

Env *env_new();
void env_destroy(Env *env);
Env *env_local_new(Env *env);
void env_local_destroy(Env *env);
void env_set(Env *env, const char *key, void *val);
void *env_get(Env *env, const char *key);
Vector *env_keys(Env *env);
Vector *env_values(Env *env);


#endif

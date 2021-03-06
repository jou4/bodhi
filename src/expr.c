#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "expr.h"


BDExprIdent *bd_expr_ident(const char *name, BDType *type)
{
    BDExprIdent *ident = malloc(sizeof(BDExprIdent));
    ident->name = mem_strdup(name);
    ident->type = type;
    return ident;
}

BDExprIdent *bd_expr_ident_clone(BDExprIdent *ident)
{
    return bd_expr_ident(ident->name, bd_type_clone(ident->type));
}

Vector *bd_expr_idents_clone(Vector *idents)
{
    if(idents == NULL){ return NULL; }

    Vector *new_idents = vector_new();
    BDExprIdent *ident;
    int i;
    for(i = 0; i < idents->length; i++){
        ident = vector_get(idents, i);
        vector_add(new_idents, bd_expr_ident_clone(ident));
    }
    return new_idents;
}

void bd_set_env_expr_idents(Env *env, Vector *idents)
{
    BDExprIdent *ident;
    int i;
    for(i = 0; i < idents->length; i++){
        ident = vector_get(idents, i);
        env_set(env, ident->name, ident->type);
    }
}

BDExprIdent *bd_expr_ident_typevar(const char *name)
{
    return bd_expr_ident(name, bd_type_var(NULL));
}

void bd_expr_ident_destroy(BDExprIdent *ident)
{
    free(ident->name);
    bd_type_destroy(ident->type);
    free(ident);
}

char *bd_expr_ident_show(BDExprIdent *ident)
{
    char str[100];
    sprintf(str, "%s::%s", ident->name, bd_type_show(ident->type));
    return mem_strdup(str);
}

BDExprClosure *bd_expr_closure(const char *entry, Vector *freevars)
{
    BDExprClosure *e = malloc(sizeof(BDExprClosure));
    e->entry = mem_strdup(entry);
    e->freevars = freevars;
    return e;
}

void bd_expr_closure_destroy(BDExprClosure *cls)
{
    free(cls->entry);
    vector_each(cls->freevars, free);
    free(cls);
}

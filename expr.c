#include <stdlib.h>
#include <stdio.h>
#include "util/mem.h"
#include "expr.h"


BDExprIdent *bd_expr_ident(const char *name, BDType *type)
{
    BDExprIdent *ident = malloc(sizeof(BDExprIdent));
    ident->name = mem_strdup(name);
    ident->type = type;
    return ident;
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

void bd_expr_ident_show(BDExprIdent *ident)
{
    printf("%s::%s", ident->name, bd_type_show(ident->type));
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

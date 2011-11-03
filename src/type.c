#include <stdio.h>
#include "util.h"
#include "show.h"
#include "type.h"

BDType *bd_type(BDTypeKind kind)
{
    BDType *t = malloc(sizeof(BDType));
    t->kind = kind;
    return t;
}

void bd_type_destroy(BDType *t)
{
    if(t == NULL){
        return;
    }

    switch(t->kind){
        case T_UNIT:
        case T_BOOL:
        case T_CHAR:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            break;
        case T_FUN:
            bd_type_destroy(t->u.u_fun.ret);
            vector_each(t->u.u_fun.formals, bd_type_destroy);
            break;
        case T_TUPLE:
            vector_each(t->u.u_tuple.elems, bd_type_destroy);
            break;
        case T_LIST:
            bd_type_destroy(t->u.u_list.elem);
            break;
        case T_ARRAY:
            bd_type_destroy(t->u.u_array.elem);
            break;
        case T_VAR:
            bd_type_destroy(t->u.u_var.content);
            break;
    }
    free(t);
}

BDType *bd_type_clone(BDType *t)
{
    if(t == NULL){ return NULL; }

    switch(t->kind){
        case T_UNIT:
            return bd_type_unit();
        case T_BOOL:
            return bd_type_bool();
        case T_CHAR:
            return bd_type_char();
        case T_INT:
            return bd_type_int();
        case T_FLOAT:
            return bd_type_float();
        case T_STRING:
            return bd_type_string();
        case T_FUN:
            {
                Vector *old_formals = t->u.u_fun.formals;
                Vector *new_formals = vector_new();
                int i;

                for(i = 0; i < old_formals->length; i++){
                    vector_add(new_formals, bd_type_clone(vector_get(old_formals, i)));
                }

                return bd_type_fun(new_formals, bd_type_clone(t->u.u_fun.ret));
            }
            break;
        case T_TUPLE:
            {
                Vector *old_elems = t->u.u_tuple.elems;
                Vector *new_elems = vector_new();
                int i;

                for(i = 0; i < old_elems->length; i++){
                    vector_add(new_elems, bd_type_clone(vector_get(old_elems, i)));
                }

                return bd_type_tuple(new_elems);
            }
            break;
        case T_LIST:
            return bd_type_list(bd_type_clone(t->u.u_list.elem));
        case T_ARRAY:
            return bd_type_array(bd_type_clone(t->u.u_array.elem));
        case T_VAR:
            return bd_type_var(bd_type_clone(t->u.u_var.content));
    }
}

BDType *bd_type_unit()
{
    return bd_type(T_UNIT);
}

BDType *bd_type_bool()
{
    return bd_type(T_BOOL);
}

BDType *bd_type_char()
{
    return bd_type(T_CHAR);
}

BDType *bd_type_int()
{
    return bd_type(T_INT);
}

BDType *bd_type_float()
{
    return bd_type(T_FLOAT);
}

BDType *bd_type_string()
{
    return bd_type(T_STRING);
}

BDType *bd_type_fun(Vector *formals, BDType *ret)
{
    BDType *t = bd_type(T_FUN);
    t->u.u_fun.ret = ret;
    if(formals == NULL){
        formals = vector_new();
    }
    t->u.u_fun.formals = formals;
    return t;
}

BDType *bd_type_tuple(Vector *elems)
{
    BDType *t = bd_type(T_TUPLE);
    if(elems == NULL){
        elems = vector_new();
    }
    t->u.u_tuple.elems = elems;
    return t;
}

BDType *bd_type_list(BDType *elem)
{
    BDType *t = bd_type(T_LIST);
    t->u.u_list.elem = elem;
    return t;
}

BDType *bd_type_array(BDType *elem)
{
    BDType *t = bd_type(T_ARRAY);
    t->u.u_array.elem = elem;
    return t;
}

BDType *bd_type_var(BDType *content)
{
    BDType *t = bd_type(T_VAR);
    t->u.u_var.content = content;
    return t;
}

BDType *bd_type_schema(Vector *vars, BDType *body)
{
    if(vars == NULL){
        vars = vector_new();
    }

    BDType *t = bd_type(T_SCHEMA);
    t->u.u_schema.vars = vars;
    t->u.u_schema.body = body;
    return t;
}


char *type_strings[] = {
    "'a", "'b", "'c", "'d", "'e", "'f", "'g", "'h", "'i",
    "'j", "'k", "'l", "'m", "'n", "'o", "'p", "'q", "'r",
    "'s", "'t", "'u", "'v", "'w", "'x", "'y", "'z"
};

void bd_type_to_string(BDType *t, StringBuffer *sb, Vector *vars)
{
    switch(t->kind){
        case T_UNIT:
            sb_append(sb, "()");
            return;
        case T_BOOL:
            sb_append(sb, "Bool");
            return;
        case T_CHAR:
            sb_append(sb, "Char");
            return;
        case T_INT:
            sb_append(sb, "Int");
            return;
        case T_FLOAT:
            sb_append(sb, "Float");
            return;
        case T_STRING:
            sb_append(sb, "String");
            return;
        case T_FUN:
            {
                Vector *formals = t->u.u_fun.formals;
                BDType *ret = t->u.u_fun.ret;

                sb_append(sb, "(");
                int i;
                for(i = 0; i < formals->length; i++){
                    bd_type_to_string(vector_get(formals, i), sb, vars);
                    sb_append(sb, " -> ");
                }

                bd_type_to_string(ret, sb, vars);
                sb_append(sb, ")");

                return;
            }
        case T_TUPLE:
            {
                sb_append(sb, "(");

                Vector *elems = t->u.u_tuple.elems;
                int i;
                for(i = 0; i < elems->length; i++){
                    if(i > 0){
                        sb_append(sb, ", ");
                    }
                    bd_type_to_string(vector_get(elems, i), sb, vars);
                }

                sb_append(sb, ")");
                return;
            }
        case T_LIST:
            {
                sb_append(sb, "[");
                bd_type_to_string(t->u.u_list.elem, sb, vars);
                sb_append(sb, "]");
                return;
            }
        case T_ARRAY:
            {
                sb_append(sb, "[|");
                bd_type_to_string(t->u.u_array.elem, sb, vars);
                sb_append(sb, "|]");
                return;
            }
        case T_VAR:
            {
                if(t->u.u_var.content == NULL){
                    int index = vector_index_of(vars, t);
                    if(index < 0){
                        vector_add(vars, t);
                        sb_append(sb, type_strings[vars->length - 1]);
                        return;
                    }
                    else{
                        sb_append(sb, type_strings[index]);
                        return;
                    }
                }
                bd_type_to_string(t->u.u_var.content, sb, vars);
                return;
            }
        case T_SCHEMA:
            {
                Vector *vec = t->u.u_schema.vars;
                int i;
                for(i = 0; i < vec->length; i++){
                    bd_type_to_string(vector_get(vec, i), sb, vars);
                    if(i < vec->length - 1){
                        sb_append(sb, ", ");
                    }
                    else{
                        sb_append(sb, " => ");
                    }
                }

                bd_type_to_string(t->u.u_schema.body, sb, vars);
                return;
            }
    }
}

char *bd_type_show(BDType *t)
{
    StringBuffer *sb = sb_new();
    Vector *vars = vector_new();

    bd_type_to_string(t, sb, vars);

    char *result = sb_to_string(sb);

    sb_destroy(sb);
    vector_destroy(vars);

    return result;
}


/*
void bd_type_show_test(BDType *t)
{
    printf("%s\n", bd_type_show(t));
    return;
}

int main()
{
    BDType *t1, *t2, *t3;

    t1 = bd_type_var(NULL);
    t2 = bd_type_var(bd_type(T_BOOL));
    bd_type_show_test(t1);
    bd_type_show_test(t2);
    bd_type_destroy(t1);
    bd_type_destroy(t2);

    t1 = bd_type_array(bd_type(T_UNIT));
    t2 = bd_type_array(bd_type(T_BOOL));
    bd_type_show_test(t1);
    bd_type_show_test(t2);
    bd_type_destroy(t1);
    bd_type_destroy(t2);

    t1 = bd_type_tuple(NULL);
    vector_add(t1->u.u_tuple.elems, bd_type(T_UNIT));
    vector_add(t1->u.u_tuple.elems, bd_type(T_BOOL));
    vector_add(t1->u.u_tuple.elems, bd_type(T_INT));
    t2 = bd_type_tuple(NULL);
    vector_add(t2->u.u_tuple.elems, bd_type(T_INT));
    vector_add(t2->u.u_tuple.elems, bd_type(T_FLOAT));
    bd_type_show_test(t1);
    bd_type_show_test(t2);
    bd_type_destroy(t1);
    bd_type_destroy(t2);

    t1 = bd_type_fun(NULL, bd_type(T_BOOL));
    vector_add(t1->u.u_fun.formals, bd_type(T_UNIT));
    t2 = bd_type_fun(NULL, bd_type(T_UNIT));
    vector_add(t2->u.u_fun.formals, bd_type(T_INT));
    vector_add(t2->u.u_fun.formals, bd_type(T_BOOL));
    vector_add(t2->u.u_fun.formals, bd_type(T_FLOAT));
    bd_type_show_test(t1);
    bd_type_show_test(t2);
    bd_type_destroy(t1);
    bd_type_destroy(t2);

    return 0;
}
*/

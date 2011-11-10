#include <stdarg.h>
#include "util.h"
#include "id.h"
#include "type.h"
#include "primitives.h"

#define Sig prim_sig

#define PTUnit pt_type_new(PT_UNIT)
#define PTBool pt_type_new(PT_BOOL)
#define PTInt pt_type_new(PT_INT)
#define PTFloat pt_type_new(PT_FLOAT)
#define PTChar pt_type_new(PT_CHAR)
#define PTString pt_type_new(PT_STRING)
#define PTVar(id) pt_type_var(id)

#define PTFun pt_type_fun
#define PTTuple pt_type_tuple
#define PTList(t) pt_type_list(PT_LIST, t)
#define PTArray(t) pt_type_list(PT_ARRAY, t)

PrimType *pt_type_new(PrimTypeKind kind)
{
    PrimType *t = malloc(sizeof(PrimType));
    t->kind = kind;
    t->elems = vector_new();
    return t;
}

void pt_type_destroy(PrimType *t)
{
    int i;
    for(i = 0; i < t->elems->length; i++){
        pt_type_destroy(vector_get(t->elems, i));
    }
    free(t);
}

PrimType *pt_type_var(int id)
{
    PrimType *t = pt_type_new(PT_VAR);
    t->id = id;
    return t;
}

PrimType *pt_type_list(PrimTypeKind kind, PrimType *elem)
{
    PrimType *t = pt_type_new(kind);
    vector_add(t->elems, elem);
    return t;
}

PrimType *pt_type_fun(int num, ...)
{
    va_list ap;
    int i;

    va_start(ap, num);

    PrimType *t = pt_type_new(PT_FUN);

    for( i=0; i<num; i++ ){
        vector_add(t->elems, va_arg(ap, PrimType*));
    }

    va_end(ap);

    return t;
}

PrimType *pt_type_tuple(int num, ...)
{
    va_list ap;
    int i;

    va_start(ap, num);

    PrimType *t = pt_type_new(PT_TUPLE);

    for( i=0; i<num; i++ ){
        vector_add(t->elems, va_arg(ap, PrimType*));
    }

    va_end(ap);

    return t;
}

BDType *convert_prim_type(Vector *mem, PrimType *t)
{
    switch(t->kind){
        case PT_UNIT:
            return bd_type_unit();
        case PT_BOOL:
            return bd_type_bool();
        case PT_INT:
            return bd_type_int();
        case PT_FLOAT:
            return bd_type_float();
        case PT_CHAR:
            return bd_type_char();
        case PT_STRING:
            return bd_type_string();
        case PT_FUN:
            {
                Vector *formals = vector_new();
                BDType *formal, *ret;
                int i;
                for(i = 0; i < t->elems->length - 1; i++){
                    formal = convert_prim_type(mem, vector_get(t->elems, i));
                    vector_add(formals, formal);
                }
                ret = convert_prim_type(mem, vector_get(t->elems, i));
                return bd_type_fun(formals, ret);
            }
        case PT_TUPLE:
            {
                Vector *elems = vector_new();
                BDType *elem;
                int i;
                for(i = 0; i < t->elems->length; i++){
                    elem = convert_prim_type(mem, vector_get(t->elems, i));
                    vector_add(elems, elem);
                }
                return bd_type_tuple(elems);
            }
        case PT_LIST:
            {
                BDType *elem = convert_prim_type(mem, vector_get(t->elems, 0));
                return bd_type_list(elem);
            }
        case PT_ARRAY:
            {
                BDType *elem = convert_prim_type(mem, vector_get(t->elems, 0));
                return bd_type_array(elem);
            }
        case PT_VAR:
            {
                BDType *var = vector_get(mem, t->id);
                if( ! var){
                    var = bd_type_var(NULL);
                    vector_set(mem, t->id, var);
                }
                return var;
            }
    }
}

PrimSig *prim_sig(char *name, int num, ...)
{
    va_list ap;
    int i;

    PrimType *fun = pt_type_new(PT_FUN);

    va_start(ap, num);
    for(i=0; i < num; i++){
        vector_add(fun->elems, va_arg(ap, PrimType*));
    }
    va_end(ap);

    Vector *mem = vector_new();

    PrimSig *sig = malloc(sizeof(PrimSig));
    sig->name = name;
    sig->lbl = bd_generate_toplevel_lbl(name);
    sig->type = convert_prim_type(mem, fun);

    vector_destroy(mem);
    pt_type_destroy(fun);

    return sig;
}

#define defprim(sig) vector_add(prims, sig)

Vector *primitives()
{
    Vector *prims = vector_new();

    defprim(Sig("i2f", 2, PTInt, PTFloat));
    defprim(Sig("f2i", 2, PTFloat, PTInt));
    defprim(Sig("i2c", 2, PTInt, PTChar));
    defprim(Sig("c2i", 2, PTChar, PTInt));

    defprim(Sig("head", 2, PTList(PTVar(1)), PTVar(1)));
    defprim(Sig("tail", 2, PTList(PTVar(1)), PTList(PTVar(1))));
    defprim(Sig("empty", 2, PTList(PTVar(1)), PTBool));

    defprim(Sig("array_make", 3, PTInt, PTVar(1), PTArray(PTVar(1))));
    defprim(Sig("array_set", 4, PTArray(PTVar(1)), PTInt, PTVar(1), PTUnit));
    defprim(Sig("array_get", 3, PTArray(PTVar(1)), PTInt, PTVar(1)));
    defprim(Sig("array_length", 2, PTArray(PTVar(1)), PTInt));
    defprim(Sig("array_of_list", 2, PTList(PTVar(1)), PTArray(PTVar(1))));
    defprim(Sig("array_to_list", 2, PTArray(PTVar(1)), PTList(PTVar(1))));

    defprim(Sig("string_make", 2, PTInt, PTString));
    defprim(Sig("string_set", 4, PTString, PTInt, PTChar, PTUnit));
    defprim(Sig("string_get", 3, PTString, PTInt, PTChar));
    defprim(Sig("string_length", 2, PTString, PTInt));
    defprim(Sig("string_append", 3, PTString, PTString, PTString));
    defprim(Sig("string_concat", 3, PTString, PTList(PTString), PTString));
    defprim(Sig("string_compare", 3, PTString, PTString, PTBool));
    defprim(Sig("string_of_cstr", 1, PTString));
    defprim(Sig("string_to_cstr", 2, PTString, PTVar(1)));

    defprim(Sig("ref", 2, PTVar(1), PTArray(PTVar(1))));
    defprim(Sig("deref", 2, PTArray(PTVar(1)), PTVar(1)));
    defprim(Sig("setref", 3, PTArray(PTVar(1)), PTVar(1), PTUnit));

    defprim(Sig("print_bool", 2, PTBool, PTUnit));
    defprim(Sig("print_int", 2, PTInt, PTUnit));
    defprim(Sig("print_float", 2, PTFloat, PTUnit));
    defprim(Sig("print_char", 2, PTChar, PTUnit));
    defprim(Sig("print_string", 2, PTString, PTUnit));
    defprim(Sig("print", 2, PTVar(1), PTUnit));

    defprim(Sig("raise", 2, PTVar(1), PTUnit));
    defprim(Sig("get_exception", 2, PTUnit, PTVar(1)));

    return prims;
}

/*
int main()
{
    Vector *prims = primitives();
    PrimSig *sig;
    int i;

    for(i = 0; i < prims->length; i++){
        sig = vector_get(prims, i);
        printf("%s :: %s\n", sig->name, bd_type_show(sig->type));
    }

    return 0;
}
*/

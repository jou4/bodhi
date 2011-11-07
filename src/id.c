#include <stdlib.h>
#include <stdio.h>
#include "id.h"

char *bd_generate_id(BDType *type)
{
    static int counter;
    counter++;

    char *typeid;
    switch(type->kind){
        case T_UNIT:    typeid = "u"; break;
        case T_BOOL:    typeid = "b"; break;
        case T_INT:     typeid = "i"; break;
        case T_FLOAT:   typeid = "d"; break;
        case T_CHAR:    typeid = "c"; break;
        case T_STRING:  typeid = "s"; break;
        case T_FUN:     typeid = "f"; break;
        case T_TUPLE:   typeid = "t"; break;
        case T_LIST:    typeid = "l"; break;
        case T_ARRAY:   typeid = "a"; break;
        case T_VAR:     typeid = "v"; break;
        default:        typeid = "-"; break;
    }

    char *str = (char *)malloc(sizeof(char) * 20);
    sprintf(str, "T%s%d", typeid, counter);
    return str;
}

char *add_prefix(const char *prefix, const char *name)
{
    StringBuffer *sb = sb_new();
    sb_append(sb, prefix);
    sb_append(sb, name);
    char *alt = sb_to_string(sb);
    sb_destroy(sb);
    return alt;
}

char *bd_generate_toplevel_lbl(const char *name)
{
    return add_prefix("_bodhi_", name);
}

char *bd_generate_cfunc_lbl(const char *name)
{
    return add_prefix("_", name);
}


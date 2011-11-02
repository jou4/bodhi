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
        case T_FUN:     typeid = "f"; break;
        case T_TUPLE:   typeid = "t"; break;
        case T_ARRAY:   typeid = "a"; break;
        case T_VAR:     typeid = "v"; break;
    }

    char *str = (char *)malloc(sizeof(char) * 20);
    sprintf(str, "T%s%d", typeid, counter);
    return str;
}
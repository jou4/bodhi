#include <stdlib.h>
#include "util/mem.h"
#include "bytecode.h"


BDByteCodeValue *bd_byte_code_value_lbl(char *lbl)
{
    BDByteCodeValue *v = malloc(sizeof(BDByteCodeValue));
    v->type = BCV_LBL;
    v->u.name = mem_strdup(lbl);
    return v;
}

BDByteCodeValue *bd_byte_code_value_reg(char *reg)
{
    BDByteCodeValue *v = malloc(sizeof(BDByteCodeValue));
    v->type = BCV_REG;
    v->u.name = mem_strdup(reg);
    return v;
}

BDByteCodeValue *bd_byte_code_value_int(int val)
{
    BDByteCodeValue *v = malloc(sizeof(BDByteCodeValue));
    v->type = BCV_INT;
    v->u.intval = val;
    return v;
}

BDByteCode *bd_byte_code_let(char *reg, BDType *type, BDByteCode *inner)
{
    BDByteCode *code = bd_byte_code(BCO_LET, NULL, NULL);
    code->reg = mem_strdup(reg);
    code->type = bd_type_clone(type);
    code->code = inner;
    return code;
}

BDByteCode *bd_byte_code_ret(BDByteCode *inner)
{
    BDByteCode *code = bd_byte_code(BCO_RET, NULL, NULL);
    code->code = inner;
    return code;
}

BDByteCode *bd_byte_code(BDByteCodeOp op, BDByteCodeValue *arg1, BDByteCodeValue *arg2)
{
    BDByteCode *code = malloc(sizeof(BDByteCode));
    code->op = op;
    code->arg1 = arg1;
    code->arg2 = arg2;
    return code;
}

char *bd_byte_code_op_text(BDByteCodeOp op)
{
    char *text = malloc(sizeof(char) * 10);
    char *name;

    switch(op){
        case BCO_NOP:
            name = "NOP";
            break;
        case BCO_LET:
            name = "LET";
            break;
        case BCO_CALL:
            name = "CALL";
            break;
        case BCO_RET:
            name = "RET";
            break;
        case BCO_CMP:
            name = "CMP";
            break;
        case BCO_JMP:
            name = "JMP";
            break;
        case BCO_JE:
            name = "JE";
            break;
        case BCO_JLE:
            name = "JLE";
            break;
        case BCO_ADD:
            name = "ADD";
            break;
        case BCO_SUB:
            name = "SUB";
            break;
        case BCO_MUL:
            name = "MUL";
            break;
        case BCO_DIV:
            name = "DIV";
            break;
        case BCO_SET_ARG:
            name = "SET_ARG";
            break;
        case BCO_GET_ARG:
            name = "GET_ARG";
            break;
        case BCO_LBL:
            name = "LBL";
            break;
    }

    sprintf(text, "%s", name);

    return text;
}

char *bd_byte_code_value_text(BDByteCodeValue *value)
{
    switch(value->type){
        case BCV_LBL:
        case BCV_REG:
            return value->u.name;
        case BCV_INT:
            {
                char *text = malloc(sizeof(char) * 10);
                sprintf(text, "%d", value->u.intval);
                return text;
            }
    }
}

char *bd_byte_code_text(BDByteCode *code)
{
    char *text = malloc(sizeof(char) * 100);

    char *op = bd_byte_code_op_text(code->op);

    switch(code->op){
        case BCO_NOP:
            sprintf(text, "%s", op);
            break;
        case BCO_LET:
            sprintf(text, "%s\t%s::%s = %s", op, code->reg, bd_type_show(code->type),
                    bd_byte_code_text(code->code));
            break;
        case BCO_RET:
            sprintf(text, "%s\t%s", op, bd_byte_code_text(code->code));
            break;
        case BCO_CALL:
        case BCO_JMP:
        case BCO_JE:
        case BCO_JLE:
        case BCO_GET_ARG:
            sprintf(text, "%s\t%s", op, bd_byte_code_value_text(code->arg1));
            break;
        case BCO_CMP:
        case BCO_ADD:
        case BCO_SUB:
        case BCO_MUL:
        case BCO_DIV:
        case BCO_SET_ARG:
            sprintf(text, "%s\t%s, %s", op, bd_byte_code_value_text(code->arg1),
                    bd_byte_code_value_text(code->arg2));
            break;
        case BCO_LBL:
            sprintf(text, "%s\t%s", op, bd_byte_code_value_text(code->arg1));
            break;
    }

    return text;
}

void bd_byte_code_show(BDByteCode *code)
{
    if(code->op == BCO_LBL){
        printf("%s\n", bd_byte_code_text(code));
    }
    else{
        printf("\t%s\n", bd_byte_code_text(code));
    }
}



/*
int main(){
    bd_byte_code_show(bd_byte_code( BCO_LBL, bd_byte_code_value_lbl("main"), NULL ));

    return 0;
}
*/

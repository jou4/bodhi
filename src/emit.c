#include "compile.h"
#include "string.h"

typedef struct {
    FILE *ch;
    int tailpoint;
    int main;
} EmitState;

#define OC st->ch


int strne(char *s1, char *s2)
{
    if((s1 == NULL) || (s2 == NULL)){
        return 0;
    }

    if(strcmp(s1, s2) == 0){
        return 0;
    }
    else{
        return 1;
    }
}

void emit_inst(EmitState *st, BDAInst *inst, char *dst)
{
    switch(inst->kind)
    {
        case AI_NOP:
            return;

        case AI_SET_C:
            fprintf(OC, "\tmovq '%c', %s\n", inst->u.u_char, dst);
            break;
        case AI_SET_I:
            fprintf(OC, "\tmovq $%d, %s\n", inst->u.u_int, dst);
            break;

        case AI_SETGLOBAL_C:
            fprintf(OC, "\tmovb %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, "%rip");
            break;
        case AI_SETGLOBAL_I:
            fprintf(OC, "\tmovq %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, "%rip");
            break;
        case AI_SETGLOBAL_F:
            // TODO
            break;
        case AI_SETGLOBAL_L:
            fprintf(OC, "\tmovq %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, "%rip");
            break;

        case AI_MOV:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s, %s\n", inst->lbl, dst);
            }
            break;
        case AI_MOVGLOBAL:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s(%s), %s\n", inst->lbl, "%rip", dst);
            }
            break;
        case AI_MOVGLOBAL_L:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tleaq %s(%s), %s\n", inst->lbl, "%rip", dst);
            }
            break;

        case AI_NEG:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s, %s\n", inst->lbl, dst);
            }
            fprintf(OC, "\tnegq %s\n", dst);
            break;

        case AI_ADD:
        case AI_SUB:
        case AI_MUL:
        case AI_DIV:
            {
                char *op;
                switch(inst->kind){
                    case AI_ADD:
                        op = "addq"; break;
                    case AI_SUB:
                        op = "subq"; break;
                    case AI_MUL:
                        op = "mulq"; break;
                    case AI_DIV:
                        op = "divq"; break;
                }

                if(strne(inst->u.u_bin.l, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", inst->u.u_bin.l, dst);
                }
                fprintf(OC, "\t%s %s, %s\n", op, inst->u.u_bin.r, inst->u.u_bin.l);
            }
            break;

        case AI_FNEG:
        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:

        case AI_IFEQ:
        case AI_IFLE:
            break;

        case AI_CALL:
            if(st->tailpoint){
                fprintf(OC, "\tpopq %s\n", "%rbp");
                fprintf(OC, "\tjmp %s\n", inst->lbl);
            }
            else{
                fprintf(OC, "\tcall %s\n", inst->lbl);
            }
            break;
        case AI_CALLPTR:
            {
                if(inst->lbl[0] == '%'){
                    if(st->tailpoint){
                        fprintf(OC, "\tpopq %s\n", "%rbp");
                        fprintf(OC, "\tjmp *%s\n", inst->lbl);
                    }
                    else{
                        fprintf(OC, "\tcall *%s\n", inst->lbl);
                    }
                }
                else{
                    if(st->tailpoint){
                        fprintf(OC, "\tpopq %s\n", "%rbp");
                        fprintf(OC, "\tjmp *%s(%s)\n", inst->lbl, "%rip");
                    }
                    else{
                        fprintf(OC, "\tcall *%s(%s)\n", inst->lbl, "%rip");
                    }
                }
            }
            break;

        case AI_TAILCALLPOINT:
            if( ! st->main){
                fprintf(OC, "\tmovq %s, %s\n", "%rbp", "%rsp");
                st->tailpoint = 1;
            }
            return;

        case AI_JMP:
        case AI_JEQ:
        case AI_JLE:
            break;

        case AI_PUSHLCL_C:
            fprintf(OC, "\tmov %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_CHAR, "%rbp");
            break;
        case AI_PUSHLCL_I:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_INT, "%rbp");
            break;
        case AI_PUSHLCL_F:
            fprintf(OC, "\tmovd %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_FLOAT, "%rbp");
            break;
        case AI_PUSHLCL_L:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_LBL, "%rbp");
            break;

        case AI_GETLCL_C:
            fprintf(OC, "\tmov -%d(%s), %s\n", inst->u.u_int + SIZE_CHAR, "%rbp", dst);
            break;
        case AI_GETLCL_I:
            fprintf(OC, "\tmovq -%d(%s), %s\n", inst->u.u_int + SIZE_INT, "%rbp", dst);
            break;
        case AI_GETLCL_F:
            fprintf(OC, "\tmovd -%d(%s), %s\n", inst->u.u_int + SIZE_FLOAT, "%rbp", dst);
            break;
        case AI_GETLCL_L:
            fprintf(OC, "\tmovq -%d(%s), %s\n", inst->u.u_int + SIZE_LBL, "%rbp", dst);
            break;

        case AI_PUSHARG_C:
        case AI_PUSHARG_I:
        case AI_PUSHARG_F:
        case AI_PUSHARG_L:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;

                BDReg reg = argreg(offset);
                if(reg != RACC){
                    char *regname = reg_name(reg);
                    if(strne(regname, lbl) > 0){
                        fprintf(OC, "\tmovq %s, %s\n", lbl, regname);
                    }
                    return;
                }

                if(st->tailpoint){
                    offset = offset * 8 + 16;
                }
                else{
                    offset = offset * 8;
                }
                fprintf(OC, "\tmovq %s, -%d(%s)\n", lbl, offset, "%rsp");
            }
            break;

        case AI_GETARG_C:
        case AI_GETARG_I:
        case AI_GETARG_F:
        case AI_GETARG_L:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;

                BDReg reg = argreg(offset);
                if(reg != RACC){
                    char *regname = reg_name(reg);
                    if(strne(regname, dst)){
                        fprintf(OC, "\tmovq %s, %s\n", regname, dst);
                    }
                    return;
                }

                offset = offset * 8;
                fprintf(OC, "\tmovq %d(%s), %s\n", offset, "%rbp", dst);
            }
            break;

        case AI_MAKECLS:
        case AI_LOADFVS:
        case AI_GETCLS_ENTRY:

        case AI_PUSHFV_C:
        case AI_PUSHFV_I:
        case AI_PUSHFV_F:
        case AI_PUSHFV_L:

        case AI_GETFV_C:
        case AI_GETFV_I:
        case AI_GETFV_F:
        case AI_GETFV_L:

        case AI_MAKETUPLE:
        case AI_LOADELMS:
        case AI_PUSHELM:
        case AI_GETELM:
            break;

    }
}

void emit(EmitState *st, BDAExpr *e)
{
    switch(e->kind){
        case AE_ANS:
            emit_inst(st, e->u.u_ans.val, "%rax");
            break;
        case AE_LET:
            emit_inst(st, e->u.u_let.val, e->u.u_let.ident->name);
            emit(st, e->u.u_let.body);
            break;
    }
}

void emit_fundef(EmitState *st, BDAExprDef *def)
{
    char *textlbl;
    if(st->main){
        textlbl = def->ident->name;
    }
    else{
        textlbl = bd_generate_id(def->ident->type);

        // .data(ptr to functoin)
        fprintf(OC, "\t.data\n");
        fprintf(OC, "%s:\n", def->ident->name);
        fprintf(OC, "\t.quad %s\n", textlbl);

    }

    // .text
    fprintf(OC, "\t.text\n");
    fprintf(OC, "%s:\n", textlbl);
    fprintf(OC, "\tpushq %s\n", "%rbp");
    fprintf(OC, "\tmovq %s, %s\n", "%rsp", "%rbp");
    // TODO
    fprintf(OC, "\tsubq $%d, %s\n", 8 * 30, "%rsp");
    emit(st, def->body);
    fprintf(OC, "\tleave\n");
    fprintf(OC, "\tret\n");
}

void bd_emit(FILE *ch, BDAProgram *prog)
{
    EmitState *st = malloc(sizeof(EmitState));
    st->ch = ch;

    BDAExprConst *c;
    BDExprIdent *ident;
    BDAExprDef *def;

    Vector *vec;
    int i;

    // consts
    vec = prog->consts;
    for(i = 0; i < vec->length; i++){
        c = vector_get(vec, i);
        switch(c->kind){
            case AEC_CHAR:
                fprintf(OC, "\t.data\n");
                fprintf(OC, "%s:\n", c->lbl);
                fprintf(OC, "\t.byte %d\n", (int)c->u.u_char);
                break;
            case AEC_INT:
                fprintf(OC, "\t.data\n");
                fprintf(OC, "%s:\n", c->lbl);
                fprintf(OC, "\t.long %d\n", c->u.u_int);
                break;
            case AEC_FLOAT:
                // TODO
                break;
            case AEC_STR:
                {
                    char *tmpname = bd_generate_id(bd_type_string());
                    fprintf(OC, "\t.cstring\n");
                    fprintf(OC, "%s:\n", tmpname);
                    fprintf(OC, "\t.ascii \"%s\\0\"\n", c->u.u_str);
                    fprintf(OC, "\t.data\n");
                    fprintf(OC, "%s:\n", c->lbl);
                    fprintf(OC, "\t.quad %s\n", tmpname);
                }
                break;
        }
    }

    // globals
    vec = prog->globals;
    for(i = 0; i < vec->length; i++){
        ident = vector_get(vec, i);
        fprintf(OC, "\t.data\n");
        fprintf(OC, "%s:\n", ident->name);
        switch(ident->type->kind){
            case AEC_CHAR:
                fprintf(OC, "\t.byte\n");
                break;
            case AEC_INT:
                fprintf(OC, "\t.long\n");
                break;
            case AEC_FLOAT:
                // TODO
                break;
            default:
                fprintf(OC, "\t.quad\n");
                break;
        }
    }


    // functions
    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        st->tailpoint = 0;
        st->main = 0;
        emit_fundef(st, def);
    }

    // main function
    fprintf(OC, "\t.text\n");
    fprintf(OC, ".globl _main\n");
    def = prog->maindef;
    st->tailpoint = 0;
    st->main = 1;
    emit_fundef(st, def);
}

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

        case AI_SETGLOBAL:
            break;

        case AI_MOV:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s, %s\n", inst->lbl, dst);
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
                fprintf(OC, "\tjmp %s\n", inst->lbl);
            }
            else{
                fprintf(OC, "\tcall %s\n", inst->lbl);
            }
            break;

        case AI_TAILCALLPOINT:
            if( ! st->main){
                fprintf(OC, "\tleave\n");
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
    fprintf(OC, "%s:\n", def->ident->name);
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

    BDAExprDef *def;
    Vector *vec;
    int i;

    // TODO data section

    // text section
    fprintf(OC, ".text\n");
    fprintf(OC, ".globl _main\n");

    vec = prog->fundefs;
    for(i = 0; i < vec->length; i++){
        def = vector_get(vec, i);
        st->tailpoint = 0;
        st->main = 0;
        emit_fundef(st, def);
    }

    def = prog->maindef;
    st->tailpoint = 0;
    st->main = 1;
    emit_fundef(st, def);
}

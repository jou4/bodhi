#include "compile.h"
#include "string.h"

typedef struct {
    FILE *ch;
    int tailpoint;
    int main;
} EmitState;

#define OC st->ch
#define TAILCALL st->tailpoint


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

void emit(EmitState *st, BDAExpr *e);

void emit_inst(EmitState *st, BDAInst *inst, char *dst)
{
    switch(inst->kind)
    {
        case AI_NOP:
            return;

        case AI_SET_C:
            fprintf(OC, "\tmovq $%d, %s\n", (int)inst->u.u_char, dst);
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
                        op = "imulq"; break;
                    case AI_DIV:
                        op = "idivq";
                        fprintf(OC, "\tmovq $0, %s\n", "%rdx");
                        break;
                }

                fprintf(OC, "\t%s %s, %s\n", op, inst->u.u_bin.r, inst->u.u_bin.l);
                if(strne(inst->u.u_bin.l, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", inst->u.u_bin.l, dst);
                }
            }
            break;

        case AI_FNEG:
        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            // TODO
            break;

        case AI_IFEQ:
        case AI_IFLE:
            {
                char *l = inst->u.u_if.l;
                char *r = inst->u.u_if.r;
                BDAExpr *t = inst->u.u_if.t;
                BDAExpr *f = inst->u.u_if.f;

                char *t_lbl = bd_generate_id(NULL);
                char *f_lbl = bd_generate_id(NULL);
                char *end_lbl = bd_generate_id(NULL);

                fprintf(OC, "\tcmp %s, %s\n", r, l);
                switch(inst->kind){
                    case AI_IFEQ:
                        fprintf(OC, "\tje %s\n", t_lbl);
                        break;
                    case AI_IFLE:
                        fprintf(OC, "\tjle %s\n", t_lbl);
                        break;
                }

                fprintf(OC, "%s:\n", f_lbl);
                emit(st, f);
                fprintf(OC, "\tjmp %s\n", end_lbl);

                fprintf(OC, "%s:\n", t_lbl);
                emit(st, t);

                fprintf(OC, "%s:\n", end_lbl);
            }
            break;

        case AI_CALL:
            if(TAILCALL){
                fprintf(OC, "\tpopq %s\n", "%rbp");
                fprintf(OC, "\tjmp %s\n", inst->lbl);
            }
            else{
                fprintf(OC, "\tcall %s\n", inst->lbl);
                if(strne(reg_name(RACC), dst)){
                    fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                }
            }
            break;
        case AI_CALLPTR:
            {
                if(inst->lbl[0] == '%'){
                    if(TAILCALL){
                        fprintf(OC, "\tpopq %s\n", "%rbp");
                        fprintf(OC, "\tjmp *%s\n", inst->lbl);
                    }
                    else{
                        fprintf(OC, "\tcall *%s\n", inst->lbl);
                        if(strne(reg_name(RACC), dst)){
                            fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                        }
                    }
                }
                else{
                    if(TAILCALL){
                        fprintf(OC, "\tpopq %s\n", "%rbp");
                        fprintf(OC, "\tjmp *%s(%s)\n", inst->lbl, "%rip");
                    }
                    else{
                        fprintf(OC, "\tcall *%s(%s)\n", inst->lbl, "%rip");
                        if(strne(reg_name(RACC), dst)){
                            fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                        }
                    }
                }
            }
            break;

        case AI_TAILCALLPOINT:
            if( ! st->main){
                fprintf(OC, "\tmovq %s, %s\n", "%rbp", "%rsp");
                TAILCALL = 1;
            }
            return;

        case AI_PUSHLCL_C:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_ALIGN, "%rbp");
            break;
        case AI_PUSHLCL_I:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_ALIGN, "%rbp");
            break;
        case AI_PUSHLCL_F:
            fprintf(OC, "\tmovd %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_ALIGN, "%rbp");
            break;
        case AI_PUSHLCL_L:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, inst->u.u_int + SIZE_ALIGN, "%rbp");
            break;

        case AI_GETLCL_C:
            fprintf(OC, "\tmovq -%d(%s), %s\n", inst->u.u_int + SIZE_ALIGN, "%rbp", dst);
            break;
        case AI_GETLCL_I:
            fprintf(OC, "\tmovq -%d(%s), %s\n", inst->u.u_int + SIZE_ALIGN, "%rbp", dst);
            break;
        case AI_GETLCL_F:
            fprintf(OC, "\tmovd -%d(%s), %s\n", inst->u.u_int + SIZE_ALIGN, "%rbp", dst);
            break;
        case AI_GETLCL_L:
            fprintf(OC, "\tmovq -%d(%s), %s\n", inst->u.u_int + SIZE_ALIGN, "%rbp", dst);
            break;

        case AI_PUSHARG_C:
        case AI_PUSHARG_I:
        case AI_PUSHARG_F:
        case AI_PUSHARG_L:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;

                BDReg reg = argreg(offset);
                if(reg >= 0){
                    char *regname = reg_name(reg);
                    if(strne(regname, lbl) > 0){
                        fprintf(OC, "\tmovq %s, %s\n", lbl, regname);
                    }
                    return;
                }

                if(TAILCALL){
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
                if(reg >= 0){
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
            {
                char *lbl = inst->lbl;
                int fvs_size = inst->u.u_int;

                char *lbl_dst = reg_name(argreg(0));
                char *fvs_size_dst = reg_name(argreg(1));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tmovq $%d, %s\n", fvs_size, fvs_size_dst);
                fprintf(OC, "\tcall %s\n", "_bodhi_core_make_closure");

                if(strne("%rax", dst)){
                    fprintf(OC, "\tmovq %s, %s\n", "%rax", dst);
                }
            }
            break;
        case AI_LOADFVS:
            {
                char *lbl = inst->lbl;
                char *lbl_dst = reg_name(argreg(0));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tcall %s\n", "_bodhi_core_closure_freevars");
                fprintf(OC, "\tmovq %s, %s\n", "%rax", reg_hp);
            }
            break;
        case AI_GETCLS_ENTRY:
            {
                char *lbl = inst->lbl;
                char *lbl_dst = reg_name(argreg(0));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tcall %s\n", "_bodhi_core_closure_entry");

                if(strne("%rax", dst)){
                    fprintf(OC, "\tmovq %s, %s\n", "%rax", dst);
                }
            }
            break;

        case AI_PUSHFV_C:
        case AI_PUSHFV_I:
        case AI_PUSHFV_F:
        case AI_PUSHFV_L:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;
                fprintf(OC, "\tmovq %s, %d(%s)\n", lbl, offset, reg_hp);
            }
            break;

        case AI_GETFV_C:
        case AI_GETFV_I:
        case AI_GETFV_F:
        case AI_GETFV_L:
            {
                int offset = inst->u.u_int;
                fprintf(OC, "\tmovq %d(%s), %s\n", offset, reg_hp, dst);
            }
            break;

        case AI_MAKETUPLE:
            {
                int size = inst->u.u_int;
                char *size_dst = reg_name(argreg(0));

                fprintf(OC, "\tmovq $%d, %s\n", size, size_dst);
                fprintf(OC, "\tcall %s\n", "_bodhi_core_make_tuple");

                if(strne("%rax", dst)){
                    fprintf(OC, "\tmovq %s, %s\n", "%rax", dst);
                }
            }
            break;
        case AI_LOADELMS:
            {
                char *lbl = inst->lbl;
                char *lbl_dst = reg_name(argreg(0));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tcall %s\n", "_bodhi_core_tuple_elems");
                fprintf(OC, "\tmovq %s, %s\n", "%rax", reg_hp);
            }
            break;
        case AI_PUSHELM:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;
                fprintf(OC, "\tmovq %s, %d(%s)\n", lbl, offset * SIZE_ALIGN, reg_hp);
            }
            break;
        case AI_GETELM:
            {
                char *lbl = inst->lbl;
                int offset = inst->u.u_int;
                fprintf(OC, "\tmovq %d(%s), %s\n", offset * SIZE_ALIGN, reg_hp, dst);
            }
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
            {
                BDExprIdent *ident = e->u.u_let.ident;
                char *dst = NULL;
                if(ident->type->kind != T_UNIT){
                    dst = ident->name;
                }
                emit_inst(st, e->u.u_let.val, dst);
                emit(st, e->u.u_let.body);
            }
            break;
    }
}

typedef struct {
    int locals;
    int args;
    int accum_args;
} FrameSizeCounter;

FrameSizeCounter *frame_size_counter()
{
    FrameSizeCounter *counter = malloc(sizeof(FrameSizeCounter));
    counter->locals = 0;
    counter->args = 0;
    counter->accum_args = 0;
    return counter;
}

void frame_size_counter_destroy(FrameSizeCounter *counter)
{
    free(counter);
}

int max(int a, int b)
{
    if(a > b){
        return a;
    }
    return b;
}

void frame_size_expr(BDAExpr *e, FrameSizeCounter *counter);

void frame_size_inst(BDAInst *inst, FrameSizeCounter *counter)
{
    switch(inst->kind){
        case AI_IFEQ:
        case AI_IFLE:
            {
                FrameSizeCounter *c1 = frame_size_counter();
                FrameSizeCounter *c2 = frame_size_counter();

                frame_size_expr(inst->u.u_if.t, c1);
                frame_size_expr(inst->u.u_if.f, c2);

                counter->locals += max(c1->locals, c2->locals);
                counter->args += max(c1->locals, c2->locals);

                frame_size_counter_destroy(c1);
                frame_size_counter_destroy(c2);
            }
            break;

        case AI_PUSHLCL_C:
        case AI_PUSHLCL_I:
        case AI_PUSHLCL_F:
        case AI_PUSHLCL_L:
            counter->locals += SIZE_ALIGN;
            break;

        case AI_PUSHARG_C:
        case AI_PUSHARG_I:
        case AI_PUSHARG_L:
            {
                int index = inst->u.u_int;
                if(index >= ARG_REG_NUM){
                    counter->accum_args += SIZE_ALIGN;
                }
            }
            break;

        case AI_PUSHARG_F:
            {
                int index = inst->u.u_int;
                if(index >= FARG_REG_NUM){
                    counter->accum_args += SIZE_ALIGN;
                }
            }
            break;

        case AI_CALLPTR:
        case AI_CALL:
            counter->args = max(counter->args, counter->accum_args);
            counter->accum_args = 0;
            break;
    }
}

void frame_size_expr(BDAExpr *e, FrameSizeCounter *counter)
{
    switch(e->kind){
        case AE_ANS:
            frame_size_inst(e->u.u_ans.val, counter);
            break;
        case AE_LET:
            frame_size_inst(e->u.u_let.val, counter);
            frame_size_expr(e->u.u_let.body, counter);
            break;
    }
}

int frame_size(BDAExpr *e)
{
    FrameSizeCounter *counter = frame_size_counter();
    frame_size_expr(e, counter);

    int size = counter->locals + counter->args;

    frame_size_counter_destroy(counter);

    return size;
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
    // frame size (local vars + push args + buffer for tail-call)
    fprintf(OC, "\tsubq $%d, %s\n", frame_size(def->body) + SIZE_ALIGN * TAIL_JMP_THREASHOLD, "%rsp");
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
        st->main = 0;
        st->tailpoint = 0;
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

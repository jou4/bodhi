#include "compile.h"
#include "string.h"

typedef struct {
    FILE *ch;
    int tailpoint;
    int main;
    int hasfneg;
    char *fneglbl;
} EmitState;

#define OC st->ch
#define TAILCALL st->tailpoint
#define HAS_FNEG st->hasfneg
#define FNEG_LBL st->fneglbl

extern size_t heap_size;

#define GC_INIT \
	fprintf(OC, "\tmovq $%lu, %s\n", heap_size, reg_name(RARG1)); \
	fprintf(OC, "\tmovq $%lu, %s\n", heap_size, reg_name(RARG2)); \
	fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_gc_init"))
#define GC_FINISH fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_gc_finish"))
#define GC_SET_BASEPTR fprintf(OC, "\tmovq %s, %s(%s)\n", reg_bp, bd_generate_lbl("BASE_PTR"), reg_ip)
#define GC_SET_STACKPTR fprintf(OC, "\tmovq %s, %s(%s)\n", reg_sp, bd_generate_lbl("STACK_PTR"), reg_ip)


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

void emit(EmitState *st, BDAExpr *e, BDType *ret_type);

void emit_inst(EmitState *st, BDAInst *inst, char *dst, BDType *ret_type)
{
    switch(inst->kind)
    {
        case AI_NOP:
            return;

        case AI_SET_C:
            fprintf(OC, "\tmovq $%d, %s\n", (int)inst->u.u_char, dst);
            break;
        case AI_SET_I:
            fprintf(OC, "\tmovq $%ld, %s\n", inst->u.u_int, dst);
            break;

        case AI_SETGLOBAL_C:
            fprintf(OC, "\tmovb %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, reg_ip);
            break;
        case AI_SETGLOBAL_I:
            fprintf(OC, "\tmovq %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, reg_ip);
            break;
        case AI_SETGLOBAL_F:
            // TODO
            break;
        case AI_SETGLOBAL_L:
            fprintf(OC, "\tmovq %s, %s(%s)\n", inst->u.u_bin.r, inst->u.u_bin.l, reg_ip);
			// For GC.
            fprintf(OC, "\tleaq %s(%s), %s\n", inst->u.u_bin.l, reg_ip, reg_name(RARG1));
            fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_push_global_ptr"));
            break;

        case AI_MOV:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s, %s\n", inst->lbl, dst);
            }
            break;
        case AI_MOV_F:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovsd %s, %s\n", inst->lbl, dst);
            }
            break;
        case AI_MOVGLOBAL:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s(%s), %s\n", inst->lbl, reg_ip, dst);
            }
            break;
        case AI_MOVGLOBAL_F:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovsd %s(%s), %s\n", inst->lbl, reg_ip, dst);
            }
            break;
        case AI_MOVGLOBAL_L:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tleaq %s(%s), %s\n", inst->lbl, reg_ip, dst);
            }
            break;

        case AI_NEG:
            if(strne(inst->lbl, dst)){
                fprintf(OC, "\tmovq %s, %s\n", inst->lbl, dst);
            }
            fprintf(OC, "\tnegq %s\n", dst);
            break;
        case AI_FNEG:
            if( ! HAS_FNEG){
                HAS_FNEG = 1;
                FNEG_LBL = bd_generate_id(NULL);
            }
            if( ! strne(inst->lbl, dst)){
                fprintf(OC, "\tmovsd %s, %s\n", inst->lbl, reg_name(REXT5));
                inst->lbl = reg_name(REXT5);
            }
            fprintf(OC, "\tmovsd %s(%s), %s\n", FNEG_LBL, reg_ip, dst);
            fprintf(OC, "\txorpd %s, %s\n", inst->lbl, dst);
            break;

        case AI_ADD:
        case AI_SUB:
        case AI_MUL:
        case AI_DIV:
        case AI_MOD:
            {
                char *op = NULL;
                switch(inst->kind){
                    case AI_ADD:
                        op = "addq"; break;
                    case AI_SUB:
                        op = "subq"; break;
                    case AI_MUL:
                        op = "imulq"; break;
                    case AI_DIV:
                    case AI_MOD:
                        op = "idivq";
                        fprintf(OC, "\tmovq $0, %s\n", "%rdx");
                        break;
					default:
						break;
                }

                fprintf(OC, "\t%s %s, %s\n", op, inst->u.u_bin.r, inst->u.u_bin.l);
                if(inst->kind != AI_MOD){
                    if(strne(inst->u.u_bin.l, dst)){
                        fprintf(OC, "\tmovq %s, %s\n", inst->u.u_bin.l, dst);
                    }
                }
                else{
                    if(strne("%rdx", dst)){
                        fprintf(OC, "\tmovq %s, %s\n", "%rdx", dst);
                    }
                }
            }
            break;

        case AI_FADD:
        case AI_FSUB:
        case AI_FMUL:
        case AI_FDIV:
            {
                char *op = NULL;
                switch(inst->kind){
                    case AI_FADD:
                        op = "addsd"; break;
                    case AI_FSUB:
                        op = "subsd"; break;
                    case AI_FMUL:
                        op = "mulsd"; break;
                    case AI_FDIV:
                        op = "divsd";
                        break;
					default:
						break;
                }

                fprintf(OC, "\t%s %s, %s\n", op, inst->u.u_bin.r, inst->u.u_bin.l);
                if(strne(inst->u.u_bin.l, dst)){
                    fprintf(OC, "\tmovsd %s, %s\n", inst->u.u_bin.l, dst);
                }
            }
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
					default:
						break;
                }

                EmitState clone_st;

                clone_st = *st;
                fprintf(OC, "%s:\n", f_lbl);
                emit(&clone_st, f, ret_type);
                fprintf(OC, "\tjmp %s\n", end_lbl);

                clone_st = *st;
                fprintf(OC, "%s:\n", t_lbl);
                emit(&clone_st, t, ret_type);

                fprintf(OC, "%s:\n", end_lbl);
            }
            break;

        case AI_CALL:
            if(TAILCALL){
                fprintf(OC, "\tpopq %s\n", reg_bp);
                fprintf(OC, "\tjmp %s\n", inst->lbl);
            }
            else{
                fprintf(OC, "\tcall %s\n", inst->lbl);
                if(ret_type != NULL && ret_type->kind != T_FLOAT){
                    if(strne(reg_name(RACC), dst)){
                        fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                    }
                }
                else{
                    if(strne(reg_name(RFACC), dst)){
                        fprintf(OC, "\tmovsd %s, %s\n", reg_name(RFACC), dst);
                    }
                }
            }
            break;
        case AI_CALLPTR:
            {
                if(inst->lbl[0] == '%'){
                    if(TAILCALL){
                        fprintf(OC, "\tpopq %s\n", reg_bp);
                        fprintf(OC, "\tjmp *%s\n", inst->lbl);
                    }
                    else{
                        fprintf(OC, "\tcall *%s\n", inst->lbl);
                        if(ret_type != NULL && ret_type->kind != T_FLOAT){
                            if(strne(reg_name(RACC), dst)){
                                fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                            }
                        }
                        else{
                            if(strne(reg_name(RFACC), dst)){
                                fprintf(OC, "\tmovsd %s, %s\n", reg_name(RFACC), dst);
                            }
                        }
                    }
                }
                else{
                    if(TAILCALL){
                        fprintf(OC, "\tpopq %s\n", reg_bp);
                        fprintf(OC, "\tjmp *%s(%s)\n", inst->lbl, reg_ip);
                    }
                    else{
                        fprintf(OC, "\tcall *%s(%s)\n", inst->lbl, reg_ip);
                        if(ret_type != NULL && ret_type->kind != T_FLOAT){
                            if(strne(reg_name(RACC), dst)){
                                fprintf(OC, "\tmovq %s, %s\n", reg_name(RACC), dst);
                            }
                        }
                        else{
                            if(strne(reg_name(RFACC), dst)){
                                fprintf(OC, "\tmovsd %s, %s\n", reg_name(RFACC), dst);
                            }
                        }
                    }
                }
            }
            break;

        case AI_TAILCALLPOINT:
            if( ! st->main){
                fprintf(OC, "\tmovq %s, %s\n", reg_bp, reg_sp);
				GC_SET_STACKPTR;	// For GC.
                TAILCALL = 1;
            }
            return;

        case AI_PUSHLCL_C:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, (int)inst->u.u_int + SIZE_ALIGN, reg_bp);
            break;
        case AI_PUSHLCL_I:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, (int)inst->u.u_int + SIZE_ALIGN, reg_bp);
            break;
        case AI_PUSHLCL_F:
            fprintf(OC, "\tmovsd %s, -%d(%s)\n", inst->lbl, (int)inst->u.u_int + SIZE_ALIGN, reg_bp);
            break;
        case AI_PUSHLCL_L:
            fprintf(OC, "\tmovq %s, -%d(%s)\n", inst->lbl, (int)inst->u.u_int + SIZE_ALIGN, reg_bp);
            break;

        case AI_GETLCL_C:
            fprintf(OC, "\tmovq -%d(%s), %s\n", (int)inst->u.u_int + SIZE_ALIGN, reg_bp, dst);
            break;
        case AI_GETLCL_I:
            fprintf(OC, "\tmovq -%d(%s), %s\n", (int)inst->u.u_int + SIZE_ALIGN, reg_bp, dst);
            break;
        case AI_GETLCL_F:
            fprintf(OC, "\tmovsd -%d(%s), %s\n", (int)inst->u.u_int + SIZE_ALIGN, reg_bp, dst);
            break;
        case AI_GETLCL_L:
            fprintf(OC, "\tmovq -%d(%s), %s\n", (int)inst->u.u_int + SIZE_ALIGN, reg_bp, dst);
            break;

        case AI_PUSHARG_C:
        case AI_PUSHARG_I:
        case AI_PUSHARG_L:
            {
                char *lbl = inst->lbl;
                int offset = (int)inst->u.u_int;

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
                fprintf(OC, "\tmovq %s, -%d(%s)\n", lbl, offset, reg_sp);
            }
            break;
        case AI_PUSHARG_F:
            {
                char *lbl = inst->lbl;
                int offset = (int)inst->u.u_int;

                BDReg reg = fargreg(offset);
                if(reg >= 0){
                    char *regname = reg_name(reg);
                    if(strne(regname, lbl) > 0){
                        fprintf(OC, "\tmovsd %s, %s\n", lbl, regname);
                    }
                    return;
                }

                if(TAILCALL){
                    offset = offset * 8 + 16;
                }
                else{
                    offset = offset * 8;
                }
                fprintf(OC, "\tmovsd %s, -%d(%s)\n", lbl, offset, reg_sp);
            }
            break;

        case AI_GETARG_C:
        case AI_GETARG_I:
        case AI_GETARG_L:
            {
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
                fprintf(OC, "\tmovq %d(%s), %s\n", offset, reg_bp, dst);
            }
            break;
        case AI_GETARG_F:
            {
                int offset = inst->u.u_int;

                BDReg reg = fargreg(offset);
                if(reg >= 0){
                    char *regname = reg_name(reg);
                    if(strne(regname, dst)){
                        fprintf(OC, "\tmovsd %s, %s\n", regname, dst);
                    }
                    return;
                }

                offset = offset * 8;
                fprintf(OC, "\tmovsd %d(%s), %s\n", offset, reg_bp, dst);
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
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_make_closure"));

                if(strne(reg_acc, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", reg_acc, dst);
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
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_closure_vars"));
                fprintf(OC, "\tmovq %s, %s\n", reg_acc, reg_hp);
            }
            break;
        case AI_LOADFVS_SELF:
            {
                fprintf(OC, "\tmovq -%d(%s), %s\n", SIZE_ALIGN, reg_bp, reg_hp);
            }
            break;
        case AI_GETCLS_ENTRY:
            {
                char *lbl = inst->lbl;
                char *lbl_dst = reg_name(argreg(0));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_closure_entry"));

                if(strne(reg_acc, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", reg_acc, dst);
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
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_make_tuple"));

                if(strne(reg_acc, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", reg_acc, dst);
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
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_tuple_elems"));
                fprintf(OC, "\tmovq %s, %s\n", reg_acc, reg_hp);
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
                int offset = inst->u.u_int;
                fprintf(OC, "\tmovq %d(%s), %s\n", offset * SIZE_ALIGN, reg_hp, dst);
            }
            break;

        case AI_MAKESTRING:
            {
                char *lbl = inst->lbl;
                char *lbl_dst = reg_name(argreg(0));

                if(strne(lbl, lbl_dst)){
                    fprintf(OC, "\tmovq %s, %s\n", lbl, lbl_dst);
                }
                fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_make_string"));

                if(strne(reg_acc, dst)){
                    fprintf(OC, "\tmovq %s, %s\n", reg_acc, dst);
                }
            }
            break;
		default:
			break;

    }
}

void emit(EmitState *st, BDAExpr *e, BDType *ret_type)
{
    switch(e->kind){
        case AE_ANS:
            if(ret_type != NULL && ret_type->kind != T_FLOAT){
                emit_inst(st, e->u.u_ans.val, reg_name(RACC), ret_type);
            }
            else{
                emit_inst(st, e->u.u_ans.val, reg_name(RFACC), ret_type);
            }
            break;
        case AE_LET:
            {
                BDExprIdent *ident = e->u.u_let.ident;
                char *dst = NULL;
                if(ident->type != NULL){
                    dst = ident->name;
                }
                emit_inst(st, e->u.u_let.val, dst, ident->type);
                emit(st, e->u.u_let.body, ret_type);
            }
            break;
		default:
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

		default:
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
		default:
			break;
    }
}

int frame_size(BDAExpr *e)
{
    FrameSizeCounter *counter = frame_size_counter();
    frame_size_expr(e, counter);

    int size = counter->locals + counter->args;

    frame_size_counter_destroy(counter);

    size += SIZE_ALIGN * TAIL_JMP_THREASHOLD;

    if(size % STACK_ALIGN != 0){
        size += SIZE_ALIGN;
    }

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

        // .data(ptr to function)
        fprintf(OC, "\t.data\n");
        fprintf(OC, "%s:\n", def->ident->name);
        fprintf(OC, "\t.quad %s\n", textlbl);
    }

    // .text
    fprintf(OC, "\t.text\n");
    fprintf(OC, "%s:\n", textlbl);
    fprintf(OC, "\tpushq %s\n", reg_bp);
    fprintf(OC, "\tmovq %s, %s\n", reg_sp, reg_bp);

    // frame size (local vars + push args + buffer for tail-call)
    fprintf(OC, "\tsubq $%d, %s\n", frame_size(def->body), reg_sp);
	GC_SET_STACKPTR;	// For GC.

    emit(st, def->body, bd_return_type(def->ident->type));

    fprintf(OC, "\tleave\n");
	GC_SET_STACKPTR;	// For GC.
    fprintf(OC, "\tret\n");
}

void emit_entry(EmitState *st)
{
    char *lbl = bd_generate_lbl("main");
    char *mainlbl = bd_generate_lbl("bodhi_main");
    char *beginlbl = bd_generate_lbl("HandleArgsBegin");
    char *endlbl = bd_generate_lbl("HandleArgsEnd");
    int offset_arg1 = SIZE_ALIGN * 1;
    int offset_arg2 = SIZE_ALIGN * 2;
    int offset_arg3 = SIZE_ALIGN * 3;

    fprintf(OC, "\t.text\n");
    fprintf(OC, ".globl %s\n", lbl);
    fprintf(OC, "%s:\n", lbl);
    fprintf(OC, "\tpushq %s\n", reg_bp);
    fprintf(OC, "\tmovq %s, %s\n", reg_sp, reg_bp);
    fprintf(OC, "\tsubq $%d, %s\n", STACK_ALIGN * 2, reg_sp);

    // Save bp & sp.
    GC_SET_BASEPTR;
	GC_SET_STACKPTR;

    // Save argc.
    fprintf(OC, "\tmovq %s, -%d(%s)\n", reg_name(RARG1), offset_arg1, reg_bp);
    // Save argv.
    fprintf(OC, "\tmovq %s, -%d(%s)\n", reg_name(RARG2), offset_arg2, reg_bp);
    // Save [].
    fprintf(OC, "\tmovq %s(%s), %s\n", bd_generate_lbl("NIL_PTR"), reg_ip, reg_acc);
    fprintf(OC, "\tmovq %s, -%d(%s)\n", reg_acc, offset_arg3, reg_bp);

    // Init gc.
    GC_INIT;

    // Get args from command line and generate list.
    fprintf(OC, "%s:\n", beginlbl);
    fprintf(OC, "\tmovq -%d(%s), %s\n", offset_arg1, reg_bp, reg_acc);
    fprintf(OC, "\tcmp $1, %s\n", reg_acc);
    fprintf(OC, "\tjle %s\n", endlbl);
    fprintf(OC, "\tsubq $1, %s\n", reg_acc);
    fprintf(OC, "\tmovq %s, -%d(%s)\n", reg_acc, offset_arg1, reg_bp);
    fprintf(OC, "\tmovq -%d(%s), %s\n", offset_arg2, reg_bp, reg_name(REXT1));
    fprintf(OC, "\tmovq (%s, %s, %d), %s\n", reg_name(REXT1), reg_acc, SIZE_ALIGN, reg_name(RARG1));
    fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_make_string"));
    fprintf(OC, "\tmovq %s, %s\n", reg_acc, reg_name(RARG1));
    fprintf(OC, "\tmovq -%d(%s), %s\n", offset_arg3, reg_bp, reg_name(RARG2));
    fprintf(OC, "\tcall %s\n", bd_generate_toplevel_lbl("core_list_cons"));
    fprintf(OC, "\tmovq %s, -%d(%s)\n", reg_acc, offset_arg3, reg_bp);
    fprintf(OC, "\tjmp %s\n", beginlbl);
    fprintf(OC, "%s:\n", endlbl);

    // Call main function.
    fprintf(OC, "\tmovq -%d(%s), %s\n", offset_arg3, reg_bp, reg_name(RARG1));
    fprintf(OC, "\tcall %s\n", mainlbl);

    // Destroy gc.
    GC_FINISH;

    // return 0;
    fprintf(OC, "\tmovq $0, %s\n", reg_acc);
    fprintf(OC, "\tleave\n");
    fprintf(OC, "\tret\n");
}

void bd_emit(FILE *ch, BDAProgram *prog)
{
    EmitState *st = malloc(sizeof(EmitState));
    st->ch = ch;
    st->main = 0;
    st->tailpoint = 0;
    st->hasfneg = 0;
    st->fneglbl = NULL;

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
                fprintf(OC, "\t.quad %d\n", (int)c->u.u_char);
                break;
            case AEC_INT:
                fprintf(OC, "\t.data\n");
                fprintf(OC, "%s:\n", c->lbl);
                fprintf(OC, "\t.quad %ld\n", c->u.u_int);
                break;
            case AEC_FLOAT:
                fprintf(OC, "\t.data\n");
                fprintf(OC, "%s:\n", c->lbl);
                fprintf(OC, "\t.long %d\n", gethi(c->u.u_double));
                fprintf(OC, "\t.long %d\n", getlo(c->u.u_double));
                break;
            case AEC_STR:
                {
                    fprintf(OC, "\t.data\n");
                    //fprintf(OC, "\t.cstring\n");
                    fprintf(OC, "%s:\n", c->lbl);
                    fprintf(OC, "\t.ascii \"%s\\0\"\n", c->u.u_str);
                }
                break;
			default:
				break;
        }
    }

    // globals
    vec = prog->globals;
    for(i = 0; i < vec->length; i++){
        ident = vector_get(vec, i);
        fprintf(OC, "\t.data\n");
        fprintf(OC, "%s:\n", ident->name);
		fprintf(OC, "\t.quad 0\n");
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
    def = prog->maindef;
    st->tailpoint = 0;
    st->main = 1;
    emit_fundef(st, def);

    // entry point
    st->tailpoint = 0;
    st->main = 0;
    emit_entry(st);

    // for fneg
    if(HAS_FNEG){
        fprintf(OC, "\t.data\n");
        fprintf(OC, "%s:\n", FNEG_LBL);
		fprintf(OC, "\t.long 0\n");
		fprintf(OC, "\t.long -2147483648\n");
    }
}

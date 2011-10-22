#include "compile.h"

FILE *output;

int is_diff_reg(BDAsmVal *x, BDAsmVal *y)
{
    if(x->kind == AV_REG && y->kind == AV_REG){
        if(x->u.u_reg.index == y->u.u_reg.index){
            return 0;
        }
    }
    return 1;
}

int find_stack(Vector *stack, char *name)
{
    int i;
    char *target;
    for(i = 0; i < stack->length; i++){
        target = vector_get(stack, i);
        if(strcmp(target, name) == 0){
            return i;
        }
    }
    return -1;
}

void emit_expr(Vector *stack, BDAsmExpr *e, BDAsmVal *target)
{
    switch(e->kind){
        case AE_NOP:
            break;
        case AE_SET:
        case AE_MOV:
            if(is_diff_reg(target, e->u.u_uniop.val)){
                fprintf(output, "\tmov\t%s, %s\n", bd_asmval_emit(target), bd_asmval_emit(e->u.u_uniop.val));
            }
            break;
        case AE_UNIOP:
        case AE_BINOP:
            {
                char *op;
                switch(e->u.u_binop.kind){
                    case OP_ADD:
                        op = "add";
                        break;
                    case OP_SUB:
                        op = "sub";
                        break;
                    case OP_MUL:
                        op = "mul";
                        break;
                    case OP_DIV:
                        op = "div";
                        break;
                }
                if(target == NULL){
                    fprintf(output, "\t%s\t%s, %s\n",
                            op, bd_asmval_emit(e->u.u_binop.l), bd_asmval_emit(e->u.u_binop.r));
                    fprintf(output, "\tmov\t[rsp], %s\n", bd_asmval_emit(e->u.u_binop.l));
                }
                else{
                    if(is_diff_reg(target, e->u.u_binop.l)){
                        fprintf(output, "\tmov\t%s, %s\n", bd_asmval_emit(target), bd_asmval_emit(e->u.u_binop.l));
                    }
                    fprintf(output, "\t%s\t%s, %s\n", op, bd_asmval_emit(target), bd_asmval_emit(e->u.u_binop.r));
                }
            }
            break;
        case AE_IF:
        case AE_CALLCLS:
        case AE_CALLDIR:
            break;
        case AE_STORE:
            {
                int stack_len = stack->length;
                vector_add(stack, e->u.u_store.name);
                fprintf(output, "\tmov\tqword [rbp - %d], %s\n", stack_len * 8, bd_asmval_emit(e->u.u_store.lbl));
            }
            break;
        case AE_LOAD:
            {
                int stack_i = find_stack(stack, bd_asmval_emit(e->u.u_store.lbl));
                fprintf(output, "\tmov\t%s, [rbp - %d]\n", bd_asmval_emit(target), stack_i * 8);
            }
            break;
    }
}

void emit_ins(Vector *stack, BDAsmIns *ins)
{
    switch(ins->kind){
        case AI_ANS:
            emit_expr(stack, ins->u.u_ans.expr, NULL);
            break;
        case AI_LET:
            emit_expr(stack, ins->u.u_let.val, ins->u.u_let.lbl);
            emit_ins(stack, ins->u.u_let.body);
            break;
    }
}

void bd_emit(BDAsmProg *prog)
{
    output = fopen("test/test.s", "w");

    fprintf(output, "extern print_number\n");
    fprintf(output, "extern exit\n");
    fprintf(output, "section .text\n");
    fprintf(output, "global _main\n");
    fprintf(output, "_main:\n");
    fprintf(output, "\tpush rbp\n");
    fprintf(output, "\tmov rbp, rsp\n");
    fprintf(output, "\tsub rsp, 64\n");

    Vector *stack = vector_new();
    emit_ins(stack, prog->main);

    fprintf(output, "\tcall print_number\n");
    fprintf(output, "\tcall exit\n");
    fclose(output);
}

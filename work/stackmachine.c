#include <stdio.h>
#include <stdlib.h>

typedef long intnat;
typedef int int32;
typedef long int64;

typedef int32 opcode_t;
typedef opcode_t *code_t;
typedef intnat value;

#define Val_long(x)     (((intnat)(x) << 1) + 1)
#define Long_val(x)     ((x) >> 1)
#define Max_long (((intnat)1 << (8 * sizeof(value) - 2)) - 1)
#define Min_long (-((intnat)1 << (8 * sizeof(value) - 2)))
#define Val_int(x) Val_long(x)
#define Int_val(x) ((int) Long_val(x))
#define Unsigned_long_val(x) ((uintnat)(x) >> 1)
#define Unsigned_int_val(x)  ((int) Unsigned_long_val(x))

#define Val_bool(x) Val_int((x) != 0)
#define Bool_val(x) Int_val(x)
#define Val_false Val_int(0)
#define Val_true Val_int(1)
#define Val_not(x) (Val_false + Val_true - (x))
#define Val_unit Val_int(0)

value new_string(value length){
    value newstr = (value)malloc(sizeof(char) * length);
    printf("%p\n", (void *)newstr);
    return newstr;
}

extern value new_string();

typedef value(*primitive_func)();
primitive_func primitives[] = {
    new_string,
    NULL
};

#define Primitive(n) (primitives[n])

enum Instructions {
    PUSHACC,
    ACC,

    ADDINT,
    SUBINT,
    MULINT,
    DIVINT,
    MODINT,

    ANDINT,
    ORINT,
    XORINT,

    STRINGFILL,
    STRINGSET,
    STRINGGET,

    CALL1,
    CALL2,
    CALLN,

    CCALL1,
    CCALL2,
    CCALLN,

    CONSTINT,
    PUSHCONSTINT,

    STOP
};

value run(code_t prog)
{
    register code_t pc;
    register value * sp;
    register value accu;

    int stack_n = 100;
    value *stack_low = malloc(sizeof(value) * stack_n);
    value *stack_high = stack_low + stack_n;

    pc = prog;
    sp = stack_high;
    accu = Val_int(0);
    opcode_t curr_instr;

    while(1){
        curr_instr = *pc++;
        switch(curr_instr){
            case PUSHACC:
                *--sp = accu;
            case ACC:
                accu = sp[*pc++];
                break;

            case ADDINT:
                accu = (value)((intnat)accu + (intnat)*sp++ - 1);
                break;
            case SUBINT:
                accu = (value)((intnat)accu - (intnat)*sp++ + 1);
                break;
            case MULINT:
                accu = Val_long(Long_val(accu) * Long_val(*sp++));
                break;
            case DIVINT:
                accu = Val_long(Long_val(accu) / Long_val(*sp++));
                break;
            case MODINT:
                accu = Val_long(Long_val(accu) % Long_val(*sp++));
                break;

            case ANDINT:
                accu = (value)((intnat)accu & (intnat)*sp++);
                break;
            case ORINT:
                accu = (value)((intnat)accu | (intnat)*sp++);
                break;
            case XORINT:
                accu = (value)((intnat)accu ^ (intnat)*sp++ | 1);
                break;

            case STRINGCOPY:
            case STRINGSET:
            case STRINGGET:

            case CALL1:
            case CALL2:
            case CALLN:
                break;

            case CCALL1:
                accu = Primitive(*pc++)(accu);
                break;
            case CCALL2:
            case CCALLN:
                break;

            case PUSHCONSTINT:
                *--sp = accu;
            case CONSTINT:
                accu = Val_int(*pc);
                pc++;
                break;

            case STOP:
                return accu;
        }

    }
}

int main()
{
    code_t prog = malloc(sizeof(opcode_t) * 1000);
    prog[0] = PUSHCONSTINT;
    prog[1] = 10;
    prog[2] = PUSHCONSTINT;
    prog[3] = 20;
    prog[4] = PUSHCONSTINT;
    prog[5] = 30;
    prog[6] = MULINT;
    prog[7] = ADDINT;
    prog[8] = PUSHCONSTINT;
    prog[9] = 10;
    prog[10] = CCALL1;
    prog[11] = 0;
    prog[12] = STOP;

    value ret = run(prog);
    //printf("%d\n", Int_val(ret));
    printf("%p\n", (void *)ret);


    return 0;
}

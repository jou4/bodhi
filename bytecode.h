#ifndef _bytecode_h_
#define _bytecode_h_

typedef enum {
    NOP,
    CALL,
    RET,
    CMP,
    JMP,
    JMP_EQ,
    JMP_LE,
    ADD,
    SUB,
    MUL,
    DIV,
    CREATE_FRAME,
    REMOVE_FRAME,
    SET_LOCALVAR,
    GET_LOCALVAR,
    SET_ARG,
    GET_ARG,
    SET_ARG_FASTCALL,
    GET_ARG_FASTCALL,
    LET,
} BDByteCodeOp;

typedef struct BDByteCode BDByteCode;

typedef struct {
    int reg;
    BDType type;
} BDByteCodeLet;

struct BDByteCode {
    BDByteCodeOp op;
    union {
    } u;
    char *callee;
    int i_const;
    int localvar1;
    int localvar2;
    int register1;
    int register2;
}

#endif

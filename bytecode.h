#ifndef _bytecode_h_
#define _bytecode_h_

typedef enum {
    BOP_NOP,
    BOP_CALL,
    BOP_RET,
    BOP_JMP,
    BOP_CREATE_FRAME,
    BOP_REMOVE_FRAME,
    BOP_SET_LOCALVAR,
    BOP_GET_LOCALVAR,
    BOP_SET_ARG,
    BOP_GET_ARG,
    BOP_SET_ARG_FASTCALL,
    BOP_GET_ARG_FASTCALL,
} BDByteCodeOp;

typedef struct {
    BDByteCodeOp op;
    char *callee;
    int i_const;
    int localvar1;
    int localvar2;
    int register1;
    int register2;
} BDByteCode;

#endif

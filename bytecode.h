#ifndef _bytecode_h_
#define _bytecode_h_

typedef enum {
    BOP_NOP,
    BOP_CALL,
    BOP_RET,
    BOP_CREATE_FRAME,
    BOP_REMOVE_FRAME,
    BOP_SET_LOCALVAR,
    BOP_GET_LOCALVAR,
} BDByteCodeOp;

#endif

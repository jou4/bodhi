#ifndef _bytecode_h_
#define _bytecode_h_

#include "show.h"
#include "type.h"

/*
 * [ex01]
 * let a = 10 in
 * let b = 20 in
 * let c = a + b in
 * c
 *
 * LET R1 = 10
 * LET R2 = 20
 * RET R1 + R2
 *
 *
 * [ex02]
 * let rec f x y = x * y in f 10
 *
 * LBL f
 * LET R1 = GET_ARG 1
 * LET R2 = GET_ARG 2
 * RET MUL R1 R2
 *
 * LET R3 = 10
 * SET_ARG 1 R3
 * RET CALL f
 *
 *
 * [ex03]
 * let rec f x = let rec g y = x + y in g in (f 3) 7
 *
 * LBL g
 * LET R1 = GET_ARG 1
 * LET R2 = GET_ARG 2
 * RET R1 + R2
 *
 * LBL f
 * LET R4 = GET_ARG 1
 * SET_ARG 1 g
 * SET_ARG 2 R4
 * RET CALL make_cls
 *
 * LET R6 = 3
 * SET_ARG R6
 * LET R7 = CALL f
 * LET R8 = 7
 * SET_ARG 1 R7
 * LET R9 = GET_FV 1
 * LET R10 = GET_FN
 * SET_ARG 1 R9
 * SET_ARG 2 R8
 * RET CALL R10
 *
 *
 * [ex04]
 * let rec f x y = if x > 0 then f (x - 1) (y + 1) else y in f 10 0
 *
 * LBL f
 * LET R1 = GET_ARG 1
 * LET R2 = GET_ARG 2
 * CMP R1 0
 * JLE f1
 * JMP f2
 * RET R2
 * LBL f1
 * LET R3 = SUB R1 1
 * LET R4 = ADD R2 1
 * SET_ARG 1 R3
 * SET_ARG 2 R4
 * JMP f_c
 *
 */

typedef enum {
    BCO_NOP,
    BCO_LET,
    BCO_CALL,
    BCO_RET,
    BCO_CMP,
    BCO_JMP,
    BCO_JE,
    BCO_JLE,
    BCO_ADD,
    BCO_SUB,
    BCO_MUL,
    BCO_DIV,
    BCO_SET_ARG,
    BCO_GET_ARG,
    BCO_LBL
} BDByteCodeOp;

typedef struct BDByteCode BDByteCode;
typedef struct BDByteCodeExpr BDByteCodeExpr;

typedef struct {
    BDByteCode *expr;
} BDByteCodeAns;

typedef struct {
    char *reg;
    BDType type;
    BDByteCode *expr;
} BDByteCodeLet;

typedef struct {
    BDByteCode *code;
} BDByteCodeRet;

typedef enum {
    BCV_LBL,
    BCV_REG,
    BCV_INT
} BDByteCodeValueType;

typedef struct {
    BDByteCodeValueType type;
    union {
        char *name;
        int intval;
    } u;
} BDByteCodeValue;

typedef enum {
    BCT_ANS,
    BCT_LET
} BDByteCodeType;

struct BDByteCode {
    BDByteCodeType type;
    union {
        BDByteCodeAns u_ans;
        BDByteCodeLet u_let;
    } u;
};

BDByteCodeValue *bd_byte_code_value_lbl(char *lbl);
BDByteCodeValue *bd_byte_code_value_reg(char *reg);
BDByteCodeValue *bd_byte_code_value_int(int val);

BDByteCode *bd_byte_code_let(char *reg, BDType *type, BDByteCode *code);
BDByteCode *bd_byte_code_ret(BDByteCode *code);
BDByteCode *bd_byte_code(BDByteCodeOp op, BDByteCodeValue *arg1, BDByteCodeValue *arg2);

void bd_byte_code_show(BDByteCode *code);

#endif

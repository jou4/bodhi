#ifndef _value_h_
#define _value_h_

typedef union GCObject GCObject;

typedef enum {
    TBool,
    TInt,
    TFloat,
    TString,
    TTuple,
    TArray,
} ValueType;

typedef struct {
    int length;
} GCString;

typedef struct {
    int length;
    Value *values;
} GCArray;

typedef struct {
    ValueType type;
    union {
        int n;
        double d;
        GCString string;
        GCArray array;
    } u;
    byte marked;
    byte copied;
    Value *forward;
    byte age;
} Value;

#endif

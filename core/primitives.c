#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"


void *bodhi_head(BDValue *v)
{
    if(is_nil(v)){
        printf("*** Exception: head: Empty list was passed.\n");
        exit(EXIT_FAILURE);
    }

    assert_expected_value(v, T_LIST);
    return v_list_head(v);
}

void *bodhi_tail(BDValue *v)
{
    if(is_nil(v)){
        printf("*** Exception: tail: Empty list was passed.\n");
        exit(EXIT_FAILURE);
    }

    assert_expected_value(v, T_LIST);
    return v_list_tail(v);
}

int bodhi_empty(BDValue *v)
{
    if(is_nil(v)){
        return 1;
    }
    return 0;
}


void bodhi_print_bool(int v)
{
    if(v == 0){
        printf("False\n");
    }
    else{
        printf("True\n");
    }
}

void bodhi_print_int(long v)
{
    printf("%ld\n", v);
}

void bodhi_print_float(double v)
{
    printf("%.14g\n", v);
}

void bodhi_print_char(char v)
{
    printf("%c\n", v);
}

void bodhi_print_string(BDValue *v)
{
    printf("%s\n", v_string_val(v));
}

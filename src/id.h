#ifndef _id_h_
#define _id_h_

#include "type.h"

char *bd_generate_id(BDType *type);
char *bd_generate_toplevel_lbl(const char *name);
char *bd_generate_lbl(const char *name);
char *bd_generate_cfunc_lbl(const char *name);
int is_trash_name(const char *name);

#endif

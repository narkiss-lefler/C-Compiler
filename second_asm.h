
#ifndef _SECOND_ASM_H_
#include <stdio.h>
#include <stdio.h>
#include "first_asm.h"


char* check_entry(line my_line,int index,int line_count);
FILE *create_entry_extern_file(const char *file_name,const char *str,int type);

FILE *create_object_file(const char *file_name);
void free_memory(ic *ic_arr,const int *size_ic,dc *dc_arr,const int *size_dc,
                 label * label_arr,const int *label_size,macro * macro_arr,const int *size);

#endif



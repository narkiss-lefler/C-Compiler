
#ifndef _FIRST_H_
#include <stdio.h>
#define MAX_LINE_LEN 80
#define LINE_CAP 82
#define LEN_EXTENTION 3

/*Each line we read from the current file*/
typedef struct line {
    char *value; /*The text itself of the current line*/
    int val_length; /*The length of the line*/
} line;

/*each instance of a new macro*/
typedef struct macro {
    char name[MAX_LINE_LEN]; /*The name of the macro*/
    char value[MAX_LINE_LEN]; /*The content that the macro contains*/
} macro;

typedef struct file{
    char * name; /*The name of the current file*/
    FILE *my_file; /*The file object*/
}file1;


FILE * create_file(const char* file_name);
int check_my_line(line my_line);
void check_macro(line my_line, macro* my_macro);
int check_endmcro(line cur_line);
char *add_mcro_val( line cur_line,char *arr,int flag);
void insert_macro(macro **macro_arr, int* size, int* capacity,macro *new_m_1);
file1 pre_assambler_file( const char *file_name);
void pre_assambler(macro * macro_arr, line my_line, FILE*sec_file,int size);
char *my_strdup(const char* str);
#endif



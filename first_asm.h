
#ifndef _FIRST_ASM_H_
#include <stdio.h>
#include "pre_asm.h"
#include <string.h>
#define MAX_LBL_LEN 32
#define MAX_ACT_LEN 5
#define RTS_NUM 448
#define LINE_CAP 82

#define DATA 1
#define ERROR 1
#define ENTRY 1
#define OK 0
#define NONE 0
#define WITH_LBL 0
#define EXTERN 2
#define TYP_EXTERN 1
#define STRING 2
#define ACTION 3
#define NUMBER_OF_ACTIONS 16
#define BASE_TEN 10
#define STOP_NUM 480
#define COUNT_PLUS_TWO 2

#define REG_0 0
#define REG_1 1
#define REG_2 2
#define REG_3 3
#define REG_4 4
#define REG_5 5
#define REG_6 6
#define REG_7 7
#define REG_ERR 8

#define TYP_1 1
#define TYP_2 2
#define TYP_3 3
#define TYP_4 4
#define TYP_5 5
#define TYP_6 6
#define TYP_7 7
#define TYP_8 8

#define MOV_1_BIT 1
#define MOV_2_BIT 2
#define MOV_3_BIT 3
#define MOV_4_BIT 4
#define MOV_5_BIT 5
#define MOV_6_BIT 6
#define MOV_9_BIT 9

#define MOV 1
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13

#define BIN_NUM 1
#define BIN_LBL 3
#define BIN_REG 5
#define MAX_ADDRESS_LEN 4
#define MOV_SIX_SPOTS 63

/*An instance of a label*/
typedef struct label {
    char name[MAX_LBL_LEN]; /*name of the label*/
    int address;  /*address of the label*/
    int symbol;   /* type of the label: 1 entry, 2 extern, 0 none*/
    int type;   /* witch type of line the label belong: 1 data, 2 string, 3 action*/
    int err;  /* is the label legal? 1 err, 0 ok*/
    int index; /*update initial index of the line after the label*/
    char action_name[MAX_ACT_LEN]; /*type of action/instruction after the label */
} label;

/*the first word in the line*/
typedef struct word{
    int index; /*update initial index of the line after the first word*/
    int length; /*the length of the first word*/
    char word[MAX_LBL_LEN]; /*the first word in the line*/
} word;

/*Action from the known array of actions*/
typedef struct action {
    char name[MAX_ACT_LEN]; /*name of the action*/
} action;

/*A word belonging to the IC array */
typedef struct IC{
    char name[MAX_LBL_LEN]; /*If the current IC word contains a label its name
                              * will appear here, otherwise the value is meaningless*/
    int address; /*the address of the current IC word*/
    long int bin_code; /*The binary encoding of the current IC word*/
}ic;

/*A word belonging to the DC array */
typedef struct DC{
    int num; /*The binary encoding of the current number, as a DC word*/
    int letter; /*The binary encoding of the current value char, as a DC word*/
    int type; /*type of the DC word: 1 data(num), 2 string(char)*/
    long int address; /*the address of the current DC word*/
}dc;

word check_line(line my_line);
void check_label(line my_line,int length,int line_count, int IC,int DC, char *res,action action_arr[],label*my_label);
void insert_label(label ** label_arr, int* label_size, int* label_cap,label *my_label,int count_line);
int insert_extern_label(label ** label_arr, int* label_size, int* label_cap, char *str,int line_count);

char * check_data_lbl(int line_count,line my_line, int length,int type);
char * is_integers_in_data( char*str, int line_count);
char * check_string_lbl(int line_count,line my_line, int length);
int check_line_action(int line_count,line my_line, int length,char *action_name,int IC,ic **ic_arr,int *size_ic, int*cap_ic,int type);

int check_one_op_action(int line_count,line line_1,int index,char *name, int IC,ic **ic_arr,int *size_ic, int*cap_ic,int action_num);
int check_two_op_action(int line_count,line line_1,int index,char *name, int IC,ic **ic_arr,int *size_ic, int*cap_ic,int action_num);
void insert_action_to_ic(ic **ic_arr,int *size_ic, int*cap_ic, ic *my_ic);
void insert_to_dc(dc **dc_arr,int *size_dc, int*cap_dc,dc *my_dc);

int check_register(char *res,int line_count);
int int_to_bin(int num,int type,int p_1,int p_2);
int * is_string_is_int(char *my_string,int len);
char * is_lbl_in_line( char*str, int line_count);

#endif



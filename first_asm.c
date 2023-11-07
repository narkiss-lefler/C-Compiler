
#include <stdio.h>
#include <string.h>
#include <printf.h>
#include "first_asm.h"
#include <stdlib.h>
#include <ctype.h>



/*the file first_asm program read the commands in the file line by line,
 * creates and puts the "words" in the dynamic arrays of the labels, ic array and dc array*/


/*the function checks if there is a definition of a label at the beginning the of the line*/
void check_label (line my_line, int i,int line_count,int IC,int DC,char *result,action action_arr[],label *my_label)
{
  int l=0;
  int k=0;
  int a=1;
  char name[LINE_CAP];

  /*checking if the first char in the label is indeed char and not a number*/
  if(isalpha(result[0]) != 0){
    for(; a< strlen (result); a++){
      /*checking if the rest of the chars in the label are legal, only num or char*/
      if(isalpha(result[a]) != 0 || isdigit(result[a]) != 0){
          continue;
      }
      printf ("illegal label. err line: %d\n",line_count);
      my_label->err = ERROR;
      return;
    }
  }
  else{
        printf ("illegal label. err line: %d\n",line_count);
        my_label->err = ERROR;
        return;
  }
  /*skips spaces*/
  while (i < my_line.val_length && isspace(my_line.value[i]) != 0) {
      i++;
    }
  /*checking the second word in the line, after the label */
  while (i < my_line.val_length && isspace(my_line.value[i]) == 0 && k < LINE_CAP-1) {
      name[k] = my_line.value[i];
      i++;
      k++;
    }
  if(k>0){
      name[k] = '\0'; } /*the second word itself*/

  if(strcmp(name,".entry") == 0 || strcmp(name,".extern") == 0){
      printf ("Cant put label before entry/extern err line: %d\n",line_count);
      my_label->err = ERROR;
      return;
    }
  /*we create the new label*/
  if(strcmp(name,".string") == 0 || strcmp(name,".data") == 0){
      strcpy(my_label->name,result);
      my_label->address = DC+1;
      my_label->symbol = NONE;
      my_label->err = OK;
      my_label->index = i;
      if(strcmp(name,".string") == 0){
        my_label->type = STRING;
        return;
      }
      my_label->type = DATA;
      return;
    }

    for(; l<NUMBER_OF_ACTIONS; l++){
      if (strcmp (name,action_arr[l].name) != 0){
          continue;
      }
      
      /*we create the new label*/
      strcpy(my_label->name,result);
      my_label->address = IC+1;
      my_label->symbol = NONE;
      my_label->type = ACTION;
      my_label->err = OK;
      my_label->index = i;
      strcpy(my_label->action_name,action_arr[l].name);
      return;
      }
    my_label->err = ERROR;
    printf ("Defining a label without an instruction or operation sentence. err line: %d\n",line_count);
}

/*the function check the first word in the line to examine whether it's a label or command*/
word check_line (line my_line)
{
  word my_word;
  int i =0;
  int j =0;
  char result[LINE_CAP];
  /*skip spaces*/
  while (i < my_line.val_length && isspace(my_line.value[i]) != 0) {
      i++;
    }
  /*collect the first word in the sentence*/
  while (i < my_line.val_length && isspace (my_line.value[i]) == 0 && j < LINE_CAP -1) {
      result[j] = my_line.value[i];
      i++;
      j++;
    }
  result[j] = '\0';

  strcpy(my_word.word,result);
  my_word.index =i;
  my_word.length = (int)strlen (result);
  return my_word;
}


/*the functions insert a new label to the label array*/
void insert_label (label **label_arr, int *label_size, int *label_cap, label *my_label,int line_count)
{
  label * new_label =NULL;
  /*if the label array is empty*/
  if(*label_size ==0){
    *((label_arr)[*label_size]) = *my_label;
    (*label_size)++;
    return ;
    }
  /*we resize the label array*/
  new_label = (label *)realloc(*label_arr, (1+(*label_cap)) * sizeof(label));
  if (new_label == NULL){
      printf("Memory reallocation failed.\n");
    }
  /*add new label into the label array*/
  (new_label)[*label_size] = *my_label;
  *label_arr = new_label;
  (*label_size)++;
  (*label_cap)++;

}

/*the function checks if the data inside the data line command is indeed a legal numbers separated by commas*/
char *is_integers_in_data ( char *str, int line_count)
{
  char* token;
  char* endptr;
  char* string_copy = NULL;
  string_copy = my_strdup(str);

  token = strtok(str, ",");

  /*we check each string between the commas*/
  while (token != NULL) {
      if (token[0] == '+' || token[0] == '-') {
          strtol(token + 1, &endptr, BASE_TEN);
        } else {
          strtol(token, &endptr, BASE_TEN);
        }
      if (*endptr != '\0') {
          printf ("illegal data, not a valid integer. err in line: %d\n",line_count);
          free(string_copy);
          return NULL;
        }
      token = strtok(NULL, ",");
    }
  /*return the legal string*/
  return string_copy;
}

/*The function checks if the content of the string line is correct that we
 * get a single string with quotation marks at the beginning and at the end
 * without internal quotation marks characters*/

char *check_string_lbl (int line_count, line my_line, int length)
{
  char * string_copy =NULL;
  char str[LINE_CAP];
  int j =0;
  /*skip spaces*/
  while (length < my_line.val_length && isspace(my_line.value[length]) != 0) {
      length++;
    }
   /*we collect the string*/
  if(length < my_line.val_length && my_line.value[length] == '"'){
    length++;
    /*we collect all the chars after "*/
    while (length < my_line.val_length && j < (LINE_CAP-1) && my_line.value[length] != '"') {
      str[j] = my_line.value[length];
      length++;
      j++;
    }
    /*we check if the string ends with " */
    if(length < my_line.val_length && my_line.value[length] == '"'){
    str[j]='\0';
    length++;
    /*we check if there are chars after the end of the string*/
    while (length < my_line.val_length){
      if(isspace(my_line.value[length]) == 0){
          printf ("String doesnt written properly. err line: %d\n",line_count);
          return NULL;
      }
      length++;
    }
    string_copy = my_strdup(str);
    return string_copy;
    }
      printf ("String doesnt written properly. err line: %d\n",line_count);
      return NULL;
  }
  printf ("String doesnt written properly. err line: %d\n",line_count);
  return NULL;
}


/*Check if the data line/extern line is legal in terms of commas*/
char * check_data_lbl (int line_count, line my_line, int length, int type)
{
  char str[LINE_CAP];
  int non_space = 0;
  int j=0;
  int a=0;

  /*Checking the amount of non-space characters and reducing the line accordingly without spaces*/
  for(; length<my_line.val_length; length++){
    if (isspace (my_line.value[length]) == 0){
        str[j] = my_line.value[length];
        j++;
        non_space++;
      }
  }
  str[j] = '\0';

  /*Checking if the line consists only of spaces*/
  if (non_space == 0){
      printf ("Empty data without numbers. err line: %d\n",line_count);
      return NULL;
    }

  /*Reducing spam characters at the end of the line*/
  if (j == my_line.val_length){
      str[non_space] = '\0';
    }

  /*Checking the integrity of commas in the reduced line*/
  if(str[non_space-1] == ',' || str[0] == ','){
      printf ("forbidden comma. err line: %d\n",line_count);
      return NULL;
    }
  /*Check for double commas*/
  while (str[a] != '\0') {
      if (str[a] == ',' && str[a + 1] == ',') {
          printf ("Two adjacent commas. err line: %d\n",line_count);
          return NULL;}
      a++;
    }

  /*we check if the extern line contains legal labels*/
  if (type == TYP_EXTERN){
    char * res = is_lbl_in_line(str,line_count);
    if (res != NULL){
        return res;
      }
    return NULL;
    }

  /*we check if the data line contains legal numbers*/
  else{
    char * res = is_integers_in_data (str,line_count);
    if (res != NULL){
        return res;
      }
  }
  return NULL;
}


int check_line_action (int line_count, line my_line, int length, char *action_name,int IC,ic **ic_arr,int *size_ic, int*cap_ic,int type)
{
  ic *new1_ic = NULL;
  int count = IC;

  /*skip spaces*/
  while (length < my_line.val_length && isspace(my_line.value[length]) != 0) {
      length++;
    }
  /*create another IC word*/
  new1_ic = (ic*) malloc (sizeof (ic));
  if(new1_ic == NULL){
    printf("Memory reallocation failed.\n");
    exit (1);
  }

  /*We make sure that no characters are appear after the operations rts/stop
   * that require 0 operators and then fill the data in the new ic word and
   * insert it to the ic array*/

  if (length == my_line.val_length){
    if(strcmp(action_name,"rts") == 0){
      new1_ic->name[0] = '\0';
      new1_ic->bin_code = int_to_bin(RTS_NUM,TYP_6,0,0);
      if(type == WITH_LBL){
        new1_ic->address = count;
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
        free (new1_ic);
        return count;
      }
      else{
        new1_ic->address = count+1;
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
        free (new1_ic);
        return count+1;
        }
    }

    else{
      new1_ic->name[0] = '\0';
      new1_ic->bin_code = int_to_bin(STOP_NUM,TYP_6,0,0);
      if(type == WITH_LBL){
          new1_ic->address = count;
          insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
          free (new1_ic);
          return count;
        }

      else{
          new1_ic->address = count+1;
          insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
          free (new1_ic);
          return count+1;
        }
    }
  }
  else{
      free (new1_ic);
      printf ("illegal char/operator after the action. err line: %d\n",line_count);
      return 0;
  }
}

/*Check if the register is written correctly*/
int check_register (char *res,int line_count)
{
  if(strcmp(res,"@r0")==0){
      return REG_0;
    }
  if(strcmp(res,"@r1")==0){
      return REG_1;
    }
  if(strcmp(res,"@r2")==0){
      return REG_2;
    }
  if(strcmp(res,"@r3")==0){
      return REG_3;
    }
  if(strcmp(res,"@r4")==0){
      return REG_4;
    }
  if(strcmp(res,"@r5")==0){
      return REG_5;
    }
  if(strcmp(res,"@r6")==0){
      return REG_6;
    }
  if(strcmp(res,"@r7")==0){
      return REG_7;
    }
  else{
      printf ("illegal register after the @. err line: %d\n",line_count);
      return REG_ERR;
    }
}

/*We code the integer number into a binary number according to the operation
 * and the operators that are inserted in the call to the function*/
int int_to_bin(int num,int type,int p_1,int p_2)

{ /*The first word, consisting of the type of operation and the type of
 * operators*/
  int i =0;
  if(type == TYP_1){
    i = (((((p_1<<MOV_4_BIT)|num)<<MOV_3_BIT)|p_2)<<MOV_2_BIT);
    return i;
  }

  /*Additional register information word*/
  if(type == TYP_2){
    i = (((num<<MOV_5_BIT)|p_1)<<MOV_2_BIT);
    return i;
    }

  /* Another information word of a number (action)*/
  if(type == TYP_3){
    if(num<0){
      num = num * (-1);
      i = (((~num)+MOV_1_BIT)<<MOV_2_BIT);
      return i;
    }
    i = (num<<MOV_2_BIT);
    return i;
  }

  /*Another information word of a label (action)*/
  if(type == TYP_4){
    i = ((num<<MOV_2_BIT)|p_1);
    return i;
  }

  /* Another information word of a number (data)*/
  if(type == TYP_5){
    if(num<0){
        num = num * (-1);
        i = (((~num)+MOV_1_BIT));
      return i;
    }
    return num;
  }

  /*rst or stop actions*/
  if(type == TYP_6){
      i = ((p_1<<MOV_9_BIT)|num);
      return i;
    }

  /*label of entry/regular type*/
  if(type == TYP_7){
      i = (((num<<MOV_1_BIT)|p_1)<<MOV_1_BIT);
      return i;
    }

  /*label of extern type*/
  if(type == TYP_8){
    i = (num|p_1);
      return i;
    }
    /*Another information word of a label (string)*/
  else{
      return num;
  }
}

/*We check if the operator is a valid integer number*/
int * is_string_is_int (char *my_string, int len)
{
  int i=0;
  int num =0;
  int *p =NULL;

  if (my_string[0] == '+' || my_string[0] == '-'){
      i++;
    }
  for (; i < strlen (my_string); i++) {
      if (!isdigit(my_string[i])) {
          return NULL;
        }
    }
  num = atoi(my_string);
  p = &num;
  return p;
}


/*We check the legality of the line which receives a command which should
 * contain only one operator. If the line is correct, we create the IC words
 * and insert them into the array*/

int check_one_op_action (int line_count, line line_1, int i ,char *name, int IC,ic **ic_arr,int *size_ic, int*cap_ic,int action_num)
{
  int count =IC;
  int k = 0;
  int j =1;
  char op[LINE_CAP];
  /*skip spaces*/
  while (i < line_1.val_length && isspace(line_1.value[i]) != 0){
      i++;
    }
  /*collect the operator*/
  while (i < line_1.val_length && isspace (line_1.value[i]) == 0 && k < LINE_CAP-1){
      op[k] = line_1.value[i];
      i++;
      k++;
    }
  op[k] ='\0';

  if(op[0] == '\0'){
      printf ("operator is missing. err line: %d\n",line_count);
      return 0;
  }

  /*Make sure there are no more characters after the single operator*/
  while (i < line_1.val_length && isspace(line_1.value[i]) != 0){
      i++;
    }

  if (i == line_1.val_length){
      /*we create 2 new IC words*/
      ic *new1_ic = (ic*) malloc (sizeof (ic));
      ic *new2_ic = (ic*) malloc (sizeof (ic));
      if(new1_ic == NULL || new2_ic == NULL){
        printf("Memory reallocation failed.\n");
        exit (1);
      }

      if (op[0] == '@'){
          int reg_num = check_register (op, line_count);

          if (reg_num != REG_ERR){
            /*The operator is a register*/

            /*Coding of the instruction word*/
            new1_ic->name[0] = '\0';
            new1_ic->address = count;
            new1_ic->bin_code = int_to_bin(action_num,TYP_1,0,BIN_REG);

            /*Coding of the register word*/
            new2_ic->name[0] = '\0';
            new2_ic->address = count+1;
            new2_ic->bin_code = int_to_bin(0,TYP_2,reg_num,0);

            /*insert the 2 words*/
            insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
            insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
            free (new1_ic);
            free (new2_ic);
            return count+1;
            }
          free (new1_ic);
          free (new2_ic);
          return 0;
        }

      if(action_num == PRN){
          int *my_num= is_string_is_int(op,k);
          if (my_num!= NULL){
              /*The operator is integer*/

              /*Coding of the instruction word*/
              new1_ic->name[0] = '\0';
              new1_ic->address = count;
              new1_ic->bin_code = int_to_bin(PRN,TYP_1,0,BIN_NUM);

              /*Coding of the integer word*/
              new2_ic->name[0] = '\0';
              new2_ic->address = count+1;
              new2_ic->bin_code = int_to_bin (*my_num,TYP_3,0,0);

              /*insert the 2 words*/
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
              free (new1_ic);
              free (new2_ic);
              return count+1;
          }
        }

      /*Checking whether the label is valid*/
      if(isalpha(op[0]) == 0){
          printf ("Not a legal label,register. err line: %d\n",line_count);
          free (new1_ic);
          free (new2_ic);
          return 0;

      }

      if(isalpha(op[0]) != 0){
          for(; j< strlen (op); j++){
              if(isalpha (op[j]) == 0 && isdigit (op[j]) == 0){
                  printf ("Not a legal label,register. err line: %d\n",line_count);
                  free (new1_ic);
                  free (new2_ic);
                  return 0;
              }
            }
      }

      /*insert the 2 words*/
      new1_ic->name[0] = '\0';
      new1_ic->address = count;
      new1_ic->bin_code = int_to_bin (action_num,TYP_1,0,BIN_LBL);
      insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);

      strcpy(new2_ic->name,op);
      new2_ic->name[k]= '\0';
      new2_ic->address = count+1;
      insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);

      free (new1_ic);
      free (new2_ic);
      return count+1;
    }
  printf ("Action can only get one operator. err line: %d\n",line_count);
  return 0;
}

/*We check the legality of the line which receives a command which should
 * contain only two operators. If the line is correct, we create the IC words
 * and insert them into the array*/
int check_two_op_action (int line_count, line line_1, int i, char *name, int IC, ic **ic_arr, int *size_ic, int *cap_ic, int action_num)
{
  int count =IC;
  int *num_2 = NULL;
  int *num_3 = NULL;
  int *num_4 = NULL;
  int reg_num =0;
  int k = 0;
  int j =0;
  int g =1;
  int h =1;
  char op1[LINE_CAP];
  char op2[LINE_CAP];

  /*skip spaces*/
  while (i < line_1.val_length && isspace (line_1.value[i]) != 0){
      i++;
    }
  if (line_1.value[i] == ','){
      printf ("cant get comma before operator. err line: %d\n",line_count);
      return 0;
  }
  /*collect the first operator*/
  while (i < line_1.val_length && isspace(line_1.value[i]) == 0 && k < (LINE_CAP-1) && line_1.value[i] != ','){
      op1[k] = line_1.value[i];
      i++;
      k++;
    }
  op1[k]='\0';
  /*skip spaces*/
  while (i < line_1.val_length && isspace(line_1.value[i]) != 0){
      i++;
    }
  if(line_1.value[i] == ','){
      i++;
      /*skip spaces*/
      while (i < line_1.val_length && isspace(line_1.value[i]) != 0){
          i++;
        }
      if (line_1.value[i] == ','){
        printf ("cant get comma before operator. err line: %d\n",line_count);
        return 0;
        }
      /*collect the second operator*/
      while (i < line_1.val_length && isspace(line_1.value[i]) == 0 && j < (LINE_CAP-1) && line_1.value[i] != ','){
          op2[j] = line_1.value[i];
          i++;
          j++;
        }
      op2[j] ='\0';

      if(op1[0] == '\0' || op2[0] == '\0'){
          printf ("operator is missing. err line: %d\n",line_count);
          return 0;
        }
      /*check there are no illegal chars after*/
      while (i < line_1.val_length && isspace(line_1.value[i]) != 0){
          i++;
        }

      /*we create 3 new IC words*/
      if (i == line_1.val_length){
      ic *new1_ic = (ic*) malloc (sizeof (ic));
      ic *new2_ic = (ic*) malloc (sizeof (ic));
      ic *new3_ic = (ic*) malloc (sizeof (ic));

      if(new1_ic == NULL || new2_ic == NULL || new3_ic == NULL){
          printf("Memory reallocation failed.\n");
          exit (1);
        }

      if(isalpha (op1[0]) == 0){
      if( action_num == LEA){
          free(new1_ic);
          free (new2_ic);
          free (new3_ic);
          printf ("illegal operator, lea gets only label. err line: %d\n",line_count);
          return 0;
        }

      if(op1[0] != '@'){
        int *my_num = is_string_is_int(op1,k);
        if(my_num != NULL){

          /*The first operator is integer*/
          if(op2[0] == '@'){
              reg_num = check_register (op2, line_count);
              if(reg_num != REG_ERR){
                  /*The first operator is a number and the second operator is a
                   * register*/

                  /*Coding of the instruction word*/
                  new1_ic->name[0] = '\0';
                  new1_ic->address = count;
                  new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_NUM,BIN_REG);

                  /*Coding of the integer word*/
                  new2_ic->name[0] = '\0';
                  new2_ic->address = count+1;
                  new2_ic->bin_code = int_to_bin (*my_num,TYP_3,0,0);

                  /*Coding of the register word*/
                  new3_ic->name[0] = '\0';
                  new3_ic->address = count+COUNT_PLUS_TWO;
                  new3_ic->bin_code = int_to_bin(0,TYP_2,reg_num,0);

                  /*insert the 3 words*/
                  insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
                  insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
                  insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

                  free (new1_ic);
                  free (new2_ic);
                  free (new3_ic);
                  return count+COUNT_PLUS_TWO;

                }
              free(new1_ic);
              free (new2_ic);
              free (new3_ic);
              printf ("Illegal second register operator. err line: %d\n",line_count);
              return 0;
            }
          if(isalpha (op2[0]) !=0){
              int a=1;
              /*The first operator is a number and the second operator is a
               * label*/

              /*Coding of the instruction word*/
              new1_ic->name[0] = '\0';
              new1_ic->address = count;
              new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_NUM,BIN_LBL);

              /*Coding of the integer word*/
              new2_ic->name[0] = '\0';
              new2_ic->address = count+1;
              new2_ic->bin_code = int_to_bin (*my_num,TYP_3,0,0);

              /*Checking whether the label is valid*/
              if(isalpha(op2[0]) == 0){
                  if(isalpha (op2[a]) == 0 && isdigit (op2[a]) == 0){
                      printf ("Not a legal label or register. err line: %d\n",line_count);
                      free (new1_ic);
                      free (new2_ic);
                      free (new3_ic);
                      return 0;
                    }
                }

              if(isalpha(op2[0]) != 0){
                  for(; a< strlen (op2); a++){
                      if(isalpha (op2[a]) == 0 && isdigit (op2[a]) == 0){
                          printf ("Not a legal label or register. err line: %d\n",line_count);
                          free (new1_ic);
                          free (new2_ic);
                          free (new3_ic);
                          return 0;
                        }
                    }
                }

              /*save the label word*/
              strcpy(new3_ic->name,op2);
              new3_ic->name[j]= '\0';
              new3_ic->address = count+COUNT_PLUS_TWO;

              /*insert the 3 words*/
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

              free (new1_ic);
              free (new2_ic);
              free (new3_ic);
              return count+COUNT_PLUS_TWO;

            }
          num_2 = is_string_is_int (op2,j);
          if(action_num == CMP && num_2 != NULL){
              /*The first operator is a number and the second operator is a
               * number*/

              /*Coding of the instruction word*/
              new1_ic->name[0] = '\0';
              new1_ic->address = count;
              new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_NUM,BIN_NUM);

              /*Coding of the integer word*/
              new2_ic->name[0] = '\0';
              new2_ic->address = count+1;
              new2_ic->bin_code = int_to_bin (atoi(op1),TYP_3,0,0);

              /*Coding of the integer word*/
              new3_ic->name[0] = '\0';
              new3_ic->address = count+COUNT_PLUS_TWO;
              new3_ic->bin_code = int_to_bin (atoi(op2),TYP_3,0,0);

              /*insert the 3 words*/
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

              free (new1_ic);
              free (new2_ic);
              free (new3_ic);
              return count+COUNT_PLUS_TWO;
          }
        free(new1_ic);
        free (new2_ic);
        free (new3_ic);
        printf ("illegal second operator. err line: %d\n",line_count);
        return 0;
        }
        free(new1_ic);
        free (new2_ic);
        free (new3_ic);
        printf ("illegal first operator. err line: %d\n",line_count);
        return 0;
        }

        reg_num = check_register (op1, line_count);
        if(reg_num != REG_ERR){
            /*The first operator is a register*/
            if(isalpha (op2[0]) != 0){
              int a=1;
              /*The first operator is a register the second operator is a label*/

              /*Coding of the instruction word*/
              new1_ic->name[0] = '\0';
              new1_ic->address = count;
              new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_REG,BIN_LBL);

              /*Coding of the register word*/
              new2_ic->name[0] = '\0';
              new2_ic->address = count+1;
              new2_ic->bin_code = int_to_bin(reg_num,TYP_2,0,0);

              /*Checking whether the label is valid*/
              if(isalpha(op2[0]) == 0){
                  if(isalpha (op2[a]) == 0 && isdigit (op2[a]) == 0){
                      printf ("Not a legal label,register. err line: %d\n",line_count);
                      free (new1_ic);
                      free (new2_ic);
                      free (new3_ic);
                      return 0;
                    }
                }

              if(isalpha(op2[0]) != 0){
                  for(; a< strlen (op2); a++){
                      if(isalpha (op2[a]) == 0 && isdigit (op2[a]) == 0){
                          printf ("Not a legal label,register. err line: %d\n",line_count);
                          free (new1_ic);
                          free (new2_ic);
                          free (new3_ic);
                          return 0;
                        }
                    }
                }
              /*save the label word*/
              strcpy(new3_ic->name,op2);
              new3_ic->name[j]= '\0';
              new3_ic->address = count+COUNT_PLUS_TWO;

              /*insert the 3 words*/
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

              free (new2_ic);
              free (new1_ic);
              free (new3_ic);
              return count+COUNT_PLUS_TWO;

            }
            if(op2[0] == '@'){
              int my_reg_num = check_register (op2, line_count);
              if(my_reg_num != REG_ERR){
                /*The first & second operator is a register*/

                /*Coding of the instruction word*/
                new1_ic->name[0] = '\0';
                new1_ic->address = count;
                new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_REG,BIN_REG);

                /*Coding of the register word*/
                new2_ic->name[0] = '\0';
                new2_ic->address = count+1;
                new2_ic->bin_code = int_to_bin(reg_num,TYP_2,my_reg_num,0);

                /*insert the 2 words*/
                insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
                insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
                free (new2_ic);
                free (new1_ic);
                free (new3_ic);

                return count+1;
                }
              free(new1_ic);
              free (new2_ic);
              free (new3_ic);
              printf ("Illegal second register operator. err line: %d\n",line_count);
              return 0;
              }

            num_3 = is_string_is_int (op2,j);
            if(action_num == MOV && num_3 != NULL){
              /*The first operator is register & second operator is a number*/

              /*Coding of the instruction word*/
              new1_ic->name[0] = '\0';
              new1_ic->address = count;
              new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_REG,BIN_NUM);

              /*Coding of the register word*/
              new2_ic->name[0] = '\0';
              new2_ic->address = count+1;
              new2_ic->bin_code = int_to_bin(reg_num,TYP_2,0,0);

              /*Coding of the integer word*/
              new3_ic->name[0] = '\0';
              new3_ic->address = count+2;
              new3_ic->bin_code = int_to_bin (*num_3,TYP_3,0,0);

              /*insert the 3 words*/
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
              insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

              free (new2_ic);
              free (new1_ic);
              free (new3_ic);
              return count+COUNT_PLUS_TWO;
            }
            free(new1_ic);
            free (new2_ic);
            free (new3_ic);
            printf ("Illegal second operator. err line: %d\n",line_count);
            return 0;
          }
        /*The first operator is an invalid register*/
        free(new1_ic);
        free (new2_ic);
        free (new3_ic);
        printf ("illegal first register operator. err line: %d\n",line_count);
        return 0;

      }

      /*We assume that the first operator is a label*/

      /*Checking whether the first label is valid*/
      if(isalpha(op1[0]) == 0){
          if(isalpha (op1[g]) == 0 && isdigit (op1[g]) == 0){
              printf ("Not a legal label,register. err line: %d\n",line_count);
              free (new1_ic);
              free (new2_ic);
              free (new3_ic);
              return 0;
            }
        }

      if(isalpha(op1[0]) != 0){
          for(; g< strlen (op1); g++){
              if(isalpha (op1[g]) == 0 && isdigit (op1[g]) == 0){
                  printf ("Not a legal label,register. err line: %d\n",line_count);
                  free (new1_ic);
                  free (new2_ic);
                  free (new3_ic);
                  return 0;
                }
            }
        }

      if(action_num != MOV && is_string_is_int (op2,j) != NULL){
      printf ("This action cant get num as second operator. err line: %d\n",line_count);
      free(new1_ic);
      free (new2_ic);
      free (new3_ic);
      return 0;
        }

      if(isalpha (op2[0]) !=0){
        /*We assume that the first and second operators are labels*/

        for(; h< strlen (op2); h++){
            if(isalpha (op2[h]) == 0 && isdigit (op2[h]) == 0){
                printf ("Not a legal label,register. err line: %d\n",line_count);
                free (new1_ic);
                free (new2_ic);
                free (new3_ic);
                return 0;
              }
          }
        /*Coding of the instruction word*/
        new1_ic->name[0] = '\0';
        new1_ic->address = count;
        new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_LBL,BIN_LBL);

        /*save the label word*/
        strcpy(new2_ic->name,op1);
        new2_ic->name[k]= '\0';
        new2_ic->address = count+1;

        /*save the label word*/
        strcpy(new3_ic->name,op2);
        new3_ic->name[j]= '\0';
        new3_ic->address = count+COUNT_PLUS_TWO;

        /*insert the 3 words*/
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

        free (new2_ic);
        free (new1_ic);
        free (new3_ic);
        return count+COUNT_PLUS_TWO;
        }

      if(op2[0] == '@'){
        reg_num = check_register (op2, line_count);
        if(reg_num != REG_ERR){
          /*We assume that the first operator is a label and the second is a
           * register*/

          /*Coding of the instruction word*/
          new1_ic->name[0] = '\0';
          new1_ic->address = count;
          new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_LBL,BIN_REG);

          /*save the label word*/
          strcpy(new2_ic->name,op1);
          new2_ic->name[k]= '\0';
          new2_ic->address = count+1;

          /*Coding of the register word*/
          new3_ic->name[0] = '\0';
          new3_ic->address = count+COUNT_PLUS_TWO;
          new3_ic->bin_code = int_to_bin(0,TYP_2,reg_num,0);

          /*insert the 3 words*/
          insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
          insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
          insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

          free (new2_ic);
          free (new1_ic);
          free (new3_ic);
          return count+COUNT_PLUS_TWO;

        }
        printf ("Illegal second register operator. err line: %d\n",line_count);
        free(new1_ic);
        free (new2_ic);
        free (new3_ic);
        return 0;
      }

      num_4 = is_string_is_int (op2,j);
      if(action_num == MOV && num_4 != NULL){
        /*We assume that the first operator is a label and the second is a
         * number*/

        /*Coding of the instruction word*/
        new1_ic->name[0] = '\0';
        new1_ic->address = count;
        new1_ic->bin_code = int_to_bin(action_num,TYP_1,BIN_LBL,BIN_NUM);

        /*save the label word*/
        strcpy(new2_ic->name,op1);
        new2_ic->name[k]= '\0';
        new2_ic->address = count+1;

        /*save the integer word*/
        new3_ic->name[0] = '\0';
        new3_ic->address = count+COUNT_PLUS_TWO;
        new3_ic->bin_code = int_to_bin (*num_4,TYP_3,0,0);

        /*insert the 3 words*/
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new1_ic);
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new2_ic);
        insert_action_to_ic ( ic_arr, size_ic,cap_ic, new3_ic);

        free (new2_ic);
        free (new1_ic);
        free (new3_ic);
        return count+COUNT_PLUS_TWO;
        }
      free(new1_ic);
      free (new2_ic);
      free (new3_ic);
      printf ("Illegal second operator. err line: %d\n",line_count);
      return 0;
        }
      printf ("another chars after second operator. err line: %d\n",line_count);
      return 0;
  }
  printf ("no comma between two operators. err line: %d\n",line_count);
  return 0;
}

/*We insert a new word into the array of IC words*/
void insert_action_to_ic (ic **ic_arr, int *size_ic, int *cap_ic, ic *cur_ic)
{
  ic *new_ic = NULL;
  /*if the IC array is empty*/
  if(*size_ic ==0){
      *((ic_arr)[*size_ic]) = *cur_ic;
      (*size_ic)++;
      return;
    }
  /*we resize the IC array*/
  new_ic = (ic *) realloc (*ic_arr, (1 + (*cap_ic)) * sizeof (ic));
  if (new_ic == NULL){
      printf ("Memory reallocation failed.\n");
      exit (1);
    }

  /*add new IC word into the IC array*/
  (new_ic)[*size_ic] = *cur_ic;
  *ic_arr = new_ic;
  (*size_ic)++;
  (*cap_ic)++;
  }

/*we check if the extern line contains legal labels*/
char *is_lbl_in_line (char *str, int line_count)
{
  int l = 0;
  char* token;
  char* string_copy = my_strdup(str);
  token = strtok(str, ",");

  while (token != NULL) {
      int i = 1;
      l=(int)strlen (token);
      if(isalpha (token[0]) != 0 ){
        for(; i<l; i++){
          if((isalpha (token[i]) != 0 ) || isdigit (token[i]) !=0){
              continue;
          }
          printf ("illegal label in the line. err in line: %d\n",line_count);
          free(string_copy);
          return NULL;
        }
        token = strtok(NULL, ",");
        continue;
      }
      printf ("illegal label in the line. err in line: %d\n",line_count);
      free(string_copy);
      return NULL;
    }
  return string_copy;
}

/*We insert a new label into the label array coming from extern line*/
int insert_extern_label (label **label_arr, int *label_size, int *label_cap, char *str,int line_count)
{
  char* token = NULL;
  token = strtok(str, ",");
  while (token != NULL){
    label *my_label = (label *) malloc (sizeof (label));
    if (my_label ==NULL){
      printf ("Memory reallocation failed.\n");
      exit (1);
    }
    /*we create new label*/
    strcpy(my_label->name,token);
    my_label->name[strlen (my_label->name)] = '\0';
    my_label->address = NONE;
    my_label->symbol = EXTERN;
    my_label->type = NONE;
    my_label->err = OK;
    my_label->index = -1;
    insert_label (label_arr,label_size,label_cap,my_label,line_count);
    free (my_label);
    token = strtok(NULL, ",");
    }
  free(str);
  return 0;
}

/*We insert a new word into the array of DC words*/
void insert_to_dc(dc **dc_arr, int *size_dc, int *cap_dc, dc *my_dc)
{
  dc *new_dc = NULL;

  /*if the DC array is empty*/
  if(*size_dc ==0){
      *((dc_arr)[*size_dc]) = *my_dc;
      (*size_dc)++;
      return;
    }
  /*we resize the DC array*/
  new_dc = (dc *) realloc (*dc_arr, (1 + (*cap_dc)) * sizeof (dc));
  if (new_dc == NULL){
      printf ("Memory reallocation failed.\n");
      exit (1);
    }

  /*add new DC word into the DC array*/
  (new_dc)[*size_dc] = *my_dc;
  *dc_arr= new_dc;
  (*size_dc)++;
  (*cap_dc)++;
}





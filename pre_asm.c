
#include <stdio.h>
#include "pre_asm.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/*the pre_asm file handle the first level of the program which read the source
 * file and unfold all the macros in a new file.am*/

/*the function open the first source file as file.as from the input */
FILE *create_file (const char *file_name)
{
  FILE *file;
  const char *filename = file_name;
  const char *extension = ".as";
  char * arr = malloc ((strlen(filename) + LEN_EXTENTION + 1)* sizeof (char));
  strcpy(arr, filename);
  strcat(arr, extension);
  file = NULL;

  file = fopen (arr, "r+");
  if (file == NULL){
      printf ("file doesnt open\n");
    }
  free (arr);
  return file;
}

/*the function check if the line is empty or a comment line */
int check_my_line (line my_line)
{
  int i = 0;
  /*skip spaces*/
  while (i < my_line.val_length && isspace(my_line.value[i]) != 0) {
      i++;
    }
  if (my_line.value[i] == ';' || my_line.val_length == i ){
      return 0;
    }
  return 1;
}

/*According to the instruction of the assignment, we assume that there are no
 * errors in the phase of withdrawing the macros*/

/*the function checks if the line is a start of a definition of a new macro*/
void check_macro (line my_line,macro *my_macro)
{
  int i =0;
  int k =0;
  int j =0;
  char result[LINE_CAP];
  char name[LINE_CAP];
  /*skip spaces*/
  while (i < my_line.val_length && isspace(my_line.value[i]) != 0) {
      i++;
    }
  /*we are looking for the string "mcro"*/
  while (i < my_line.val_length && isspace(my_line.value[i]) == 0 && j < LINE_CAP-1){
      result[j] = my_line.value[i];
      i++;
      j++;
    }
  result[j] = '\0';

  if(strcmp(result,"mcro") == 0){
    /*skip spaces*/
    while (i < my_line.val_length && isspace(my_line.value[i]) != 0) {
        i++;
      }
    /*we are looking for the name of the mcro*/
    while (i < my_line.val_length && isspace(my_line.value[i]) == 0 && k < LINE_CAP-1) {
        name[k] = my_line.value[i];
        i++;
        k++;
      }
    if(k>0){
        name[k] = '\0';
      }
    strcpy(my_macro->name,name);
    strcpy(my_macro->value,"");
    return;
  }
  my_macro->name[0] = '\0';
}

/*the function checks if the line is the end of a definition of a macro*/
int check_endmcro (line my_line)
{
  int i = 0;
  int j = 0;
  char result[LINE_CAP];
  /*skip spaces*/
  while (i < my_line.val_length && isspace(my_line.value[i]) != 0){
      i++;
    }
  /*we are looking for the string "endmcro"*/
  while (i < my_line.val_length && isspace(my_line.value[i]) == 0 && j < LINE_CAP-1){
      result[j] = my_line.value[i];
      i++;
      j++;
    }
  if(j>0){
      result[j] = '\0';
    }
  if (strcmp (result, "endmcro") == 0){
      return 0;
    }
  return 1;
}

/*the function adds the value/text of the macro until it run into the "endmcro" command*/
char *add_mcro_val ( line cur_line,char *array,int flag)
{
  /*we add the first content of the macro*/
  if (flag == 0){
      strcpy(array,cur_line.value);
  }
    /*we add the rest content of the macro*/
  else{
    strcat(array,cur_line.value);}
    return array;
}


/*the function adds new type of macro to the macro array*/
void insert_macro (macro **macro_arr, int *size, int *capacity,macro* new_m_1)
{
  macro * new_macro =NULL;
  /*it's the first index of the macro array*/
  if(*size == 0){
    *((macro_arr)[*size]) = *new_m_1;
    (*size)++;
    return;
    }
  /*we resize the macro array*/
  new_macro = (macro *)realloc(*macro_arr, (1+(*capacity)) * sizeof(macro));
  if (new_macro == NULL){
  printf("Memory reallocation failed.\n");
      exit (1);
  }
  /*add new macro into the macro array*/
  (new_macro)[*size] = *new_m_1;
  *macro_arr = new_macro;
  (*size)++;
  (*capacity)++;
}

/*the function creates & open a new file.am*/
file1 pre_assambler_file (const char *file_name)
{
  file1 file_1;
  FILE *sec_file;
  const char *filename = file_name;
  const char *extension = ".am";
  char * arr = malloc ((strlen(filename) + LEN_EXTENTION + 1)* sizeof (char ));
  strcpy(arr, filename);
  strcat(arr, extension);

  sec_file = NULL;
  sec_file = fopen (arr, "w+");
  if (sec_file == NULL){
      printf ("file doesnt open\n");
    }
  file_1.name = arr;
  file_1.my_file = sec_file;
  return file_1;
}

/*the function writes into the new file.am, while unfold the macros in it*/
void pre_assambler (macro *macro_arr, line line_1, FILE *sec_file, int size)
{
  int i =0;
  int j =0;
  int k =0;
  char result[LINE_CAP];
  /*skip spaces*/
  while (i < line_1.val_length && isspace (line_1.value[i]) != 0){
          i++;
      }
  /*check if first word here is a macro*/
  while (i < line_1.val_length && isspace (line_1.value[i]) == 0 && j < LINE_CAP-1){
      result[j] = line_1.value[i];
      i++;
      j++;
    }
  result[j] = '\0';

  /*we check if the line is calling to a macro that needs to be unfolded */
  for(;k<size; k++){
    if(strcmp (result,macro_arr[k].name) == 0){
        fputs(macro_arr[k].value,sec_file);
        return;
      }
  }
  /*we write a regular line into the file */
  fputs(line_1.value,sec_file);
}

/*the function copy the use of the strdup, which is coping a string*/
char *my_strdup (const char *str)
{
  size_t len = strlen(str);
  char *dest = (char *)malloc(len + 1);
  if(dest == NULL){
      printf ("memory allocation failed\n");
      exit (1);
  }
  strcpy(dest, str);
  return dest;
}























#include <stdio.h>
#include "second_asm.h"
#define EXTERN_EXTENTION 4
#define OBJECT_EXTENTION 3
#define LINE_CAP 82
#include <ctype.h>
#include <printf.h>
#include <string.h>
#include <stdlib.h>


/*The function checks if the entry command is valid*/
char * check_entry (line my_line, int index, int line_count)
{
  int j =0;
  char result[LINE_CAP];
  char* string_copy = NULL;
  /*skip spaces*/
  while (index < my_line.val_length && isspace(my_line.value[index]) != 0) {
      index++;
    }

  /*collect the name of the label*/
  if(isalpha(my_line.value[index]) != 0){
      result[j] = my_line.value[index];
      index++;
      j++;
      while (index < my_line.val_length && isspace (my_line.value[index]) == 0 && j < LINE_CAP-1) {
          if(isalpha(my_line.value[index]) == 0 && isdigit (my_line.value[index])==0){
              printf ("illegal label. err line:  %d\n",line_count);
              return NULL;
            }
          result[j] = my_line.value[index];
          index++;
          j++;
        }
  }
  else{
      printf ("illegal label. err line:  %d\n",line_count);
      return NULL;
  }

  result[j] = '\0'; /*the name of the label after the entry command*/
  /*checking if there are chars after the first label*/
  while(index < my_line.val_length){
    if(isspace(my_line.value[index]) == 0){
        printf ("more than one label/illegal label in entry command. err line:  %d\n",line_count);
        return NULL;
    }
    index++;
  }
  string_copy = my_strdup(result);
  /*return the label if the line is legal*/
  return string_copy;
}

/*The function creates a new file.ent or file.ext for writing if necessary, for
 * the entry/extern lines*/
FILE *create_entry_extern_file(const char *file_name,const char *str,int type)
{
  FILE *file;
  const char *filename = file_name;
  char * arr = malloc ((strlen(filename) + EXTERN_EXTENTION + 1)* sizeof (char));
  strcpy(arr, filename);

  if (type == 1){ /* create entry file*/
     const char *extension = str;
      strcat(arr,extension);
    }
  else{ /* create extern file*/
      const char *extension = str;
      strcat(arr,extension);
  }
  file = fopen (arr, "w+");
  if (file == NULL){
      printf ("file doesnt open\n");
    }
  free (arr);
  return file;
}

/* The function creates a new file.ob that encode the words by using base64 */
FILE *create_object_file(const char *file_name)
{
  FILE *file;
  const char *filename = file_name;
  const char *extension = ".ob";
  char * arr = malloc ((strlen(filename) + OBJECT_EXTENTION + 1)* sizeof (char));
  strcpy(arr, filename);
  strcat(arr, extension);

  file = NULL;
  file = fopen (arr, "w+");
  if (file == NULL){
      printf ("file doesnt open\n");
    }
  free (arr);
  return file;
}

/*The function checks if the arrays are not empty and frees the allocated memory accordingly*/
void free_memory (ic *ic_arr, const int *size_ic, dc *dc_arr, const int *size_dc,
                  label *label_arr, const int *label_size,macro * macro_arr,const int *size)
{
  if (*size_ic > 0){
      free (ic_arr);
  }
  if (*size_dc > 0){
      free (dc_arr);
    }
  if (*label_size > 0){
      free (label_arr);
    }
  if (*size > 0){
      free (macro_arr);
    }
}





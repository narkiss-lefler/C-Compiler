#include <stdio.h>
#define LINE_CAP 82
#include <string.h>
#include <stdlib.h>
#include "second_asm.h"
#define MAC_VAL_CAP 4000
#define INITIAL_IC 99
#define INITIAL_DC 99
#define LENGTH_OF_NUM 5

/*The program implements a compiler which receives files with macros, commands,
 * instructions and data and translates them into Latin, and finally into base 64 */

/*we start numbering the lines after removing all the comment/empty lines */

/*we assume that the max line length is 80 (not including \n)*/

/*We assume that the macros are defined properly, as stated in the assignment*/

/*we assume that there is at least one space between the end of a label and the
 * instruction, Similar to the duty of at least one space between the action and the operator/s and as
 * appears in the structure shown in the assignment */

/*we assume strings cant get '"' inside the string, in order to avoid nested strings*/

/*we assume each *macro* length_value is max 50 lines which is 4000 chars*/

int main (int argc, char *argv[])
{
  int error_flag = 0; /*general error in the program which means no output*/
  int i = 1;
  int flag = 0; /*flag which means first line of content of a macro or additional lines of content */
  char cur_line[LINE_CAP]; /*cur line we read from the file*/
  char number_str_ic[LENGTH_OF_NUM]; /*presentation number of ic words for file ob*/
  char number_str_dc[LENGTH_OF_NUM]; /*presentation number of dc words for file ob*/
  int size = 0; /*current size of macro array*/
  int capacity = 1; /*current cap of macro array*/
  int size_label = 0; /*current size of label array*/
  int cap_label = 1; /*current cap of label array*/
  int size_dc = 0; /*current size of dc array*/
  int cap_dc = 1; /*current cap of dc array*/
  int size_ic = 0; /*current size of ic array*/
  int cap_ic = 1; /*current cap of ic array*/
  int entry_flag = 0; /*There is at least one entry label*/
  int extern_flag = 0; /*There is at least one external label*/
  int line_count = 1; /*Count the row number*/

  /*The array of known operations*/
  action act_arr[] = {{"mov"},{"cmp"},{"add"},
                      {"sub"},{"not"},{"clr"},
                      {"lea"},{"inc"},{"dec"},
                      {"jmp"},{"bne"},{"red"},
                      {"prn"},{"jsr"},{"rts"},
                      {"stop"}};

  /*The loop loops through all the files that the user enters from the command line*/
  for (; i < argc; i++)
    {
      FILE *file = create_file (argv[i]);
      if (file != NULL){
          int IC = INITIAL_IC; /*initial IC*/
          int DC = INITIAL_DC; /*initial DC*/
          dc *dc_arr = NULL; /*dc words array*/
          ic *ic_arr = NULL; /*ic words array*/
          line line_; /*cur line from the file*/
          FILE *file_2 = NULL; /*file.am*/
          macro *macro_arr = NULL; /*macro array*/
          label *label_arr = NULL; /*label array*/
          file1 sec_file; /*file.am*/
          sec_file = pre_assambler_file (argv[i]); /*create the file.am*/
          macro_arr = (macro *) malloc (sizeof (macro));

          /*This part of the program is responsible for interpreting the macros
           * that appear in the source file*/
          while (fgets (cur_line, sizeof (cur_line), file) != NULL){
              macro *cur_macro = NULL;
              cur_macro = (macro *) malloc (sizeof (macro));
              line_.value = cur_line;
              line_.val_length = (int) strlen (cur_line);

              /*check whether the line too long, a blank line, or a comment line*/
              if (check_my_line (line_) == 0){
                  free (cur_macro);
                  continue;
                }
              /*check whether the line is starting to define a new macro*/
              check_macro (line_, cur_macro);
              if (cur_macro->name[0] == '\0'){
                  /*If the line does not belong to the definition or content of
                   * a macro insert it into the file.am*/
                  pre_assambler (macro_arr, line_, sec_file.my_file, size);
                  free (cur_macro);
                  continue;
                }

              if (cur_macro->name[0] != '\0'){
                  char mac_val[MAC_VAL_CAP];
                  /*Save the lines of macro content until it encounter a
                   * macro end definition*/
                  while (fgets (cur_line, sizeof (cur_line), file) != NULL){
                      line line_2;
                      line_2.value = cur_line;
                      line_2.val_length = (int) strlen (cur_line);
                      /*if it's not the end of the macro content add the line*/
                      if (check_endmcro (line_2) != 0){
                          char *res;
                          res = add_mcro_val (line_2, mac_val, flag);
                          flag = 1;
                          strcpy(cur_macro->value, res);
                          continue;
                        }
                      break;
                    }
                  insert_macro (&macro_arr, &size, &capacity, cur_macro);
                  flag = 0;
                  if (cur_macro != NULL){
                      free (cur_macro);
                    }
                }
            }
              /*The step of withdrawing the macros is finished, the program
              starts to read the data lines and instructions lines from the
              newly created file.am*/

              
              
              file_2 = fopen (sec_file.name, "r+"); /*open the file.am*/
              dc_arr = (dc *) malloc (cap_dc * sizeof (dc));
              ic_arr = (ic *) malloc (cap_ic * sizeof (ic));
              label_arr = (label *) malloc (sizeof (label));
	      free(sec_file.name);
	      fclose (sec_file.my_file); /*close the file.am*/
	      fclose (file); /*close the file.as*/
	      

              /*we read line by line after unfolding the macros from file.am*/
              while (fgets (cur_line, sizeof (cur_line), file_2) != NULL)
                {
                  word res;
                  line line_1;
                  line_1.value = cur_line;
                  line_1.val_length = (int) strlen (cur_line);
                  res = check_line (line_1); /*func returns the first word in the line */

                  /*If the first word is a label*/
                  if (res.word[res.length - 1] == ':' && res.length < MAX_LBL_LEN){
                      label *my_label = NULL;
                      res.word[res.length - 1] = '\0';
                      my_label = (label *) malloc (sizeof (label));

                      /*Checking what type of command or data is in the line*/
                      check_label (line_1, res.index, line_count, IC, DC, res.word, act_arr, my_label);
                      if (my_label->err != ERROR){
                          if (my_label->type == DATA){
                              /*check if the line is valid data*/
                              char *my_res = check_data_lbl (line_count, line_1, my_label->index, 0);
                              if (my_res != NULL){
                                  int first_token = 0; /*the first data get the same address as the label*/
                                  int inx = 0;
                                  int check = 0;
                                  int count = DC + 1;
                                  char *token = NULL;
                                  token = strtok (my_res, ",");

                                  /*insert all the numbers in the "data" line*/
                                  while (token != NULL){
                                      dc *my_dc = (dc *) malloc (sizeof (dc));
                                      if (my_dc == NULL){
                                          printf ("Memory reallocation failed.\n");
                                          free (my_dc);
                                          exit (1);
                                        }
                                      if (first_token != 0){
                                          count++;
                                        }
                                      /*fill the new dc word*/
                                      my_dc->num = int_to_bin (atoi (token), TYP_5, 0, 0);
                                      my_dc->type = DATA;
                                      my_dc->address = count;
                                      insert_to_dc (&dc_arr, &size_dc, &cap_dc, my_dc);
                                      first_token = 1;
                                      free (my_dc);
                                      token = strtok (NULL, ",");
                                    }
                                  /*check if label already in the array*/
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if(my_res != NULL){
                                      free (my_res);
                                    }

                                  if (check == ERROR){
                                      free (my_label);
                                      error_flag = ERROR;
                                      line_count++;
                                      continue;
                                    }
                                  insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                  DC = count;
                                  line_count++;
                                  free (my_label);
                                  continue;
                                }
                              /*if the "data" line is illegal*/
                              free (my_label);
                              error_flag = ERROR;
                              line_count++;
                              continue;
                            }

                          if (my_label->type == STRING){
                              /*check if the line is valid string*/
                              char *my_res = check_string_lbl (line_count, line_1, my_label->index);
                              int b = 0;
                              if (my_res != NULL){
                                  int inx = 0;
                                  int check = 0;
                                  int count = DC + 1;

                                  /*insert all the chars in the "string" line*/
                                  while (b <= strlen (my_res)){
                                    dc *my_dc = (dc *) malloc (sizeof (dc));
                                    if (my_dc == NULL){
                                        printf ("Memory reallocation failed.\n");
                                        free (my_dc);
                                        exit (1);
                                      }
                                    if (b != 0){
                                        count++;
                                      }
                                    my_dc->type = STRING;
                                    my_dc->address = count;
                                    if (b < strlen (my_res)){
                                        my_dc->letter = (int) my_res[b];
                                      }
                                    else{
                                        my_dc->letter = 0;
                                      }
                                    insert_to_dc (&dc_arr, &size_dc, &cap_dc, my_dc);
                                    free (my_dc);
                                    b++;
                                    }

                                  /*check if label already in the array*/
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if(my_res != NULL){
                                    free(my_res);
                                  }
                                  if (check == ERROR){
                                      free (my_label);
                                      error_flag = ERROR;
                                      line_count++;
                                      continue;
                                    }
                                  insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                  DC = count;
                                  line_count++;
                                  free (my_label);
                                  continue;
                                }
                              /*if the "string" line is illegal*/
                              free (my_label);
                              error_flag = ERROR;
                              line_count++;
                              continue;
                            }

                          /*Under each condition, the legality of the instruction
                           * line is checked so that the number, type of parameters
                           * and their writing is correct. Then it is checked whether the
                           * label does not already exist in the array.
                           * If everything is in correct, we will insert the
                           * words into the array and the label into the symbol table.*/

                          /*Actions that doesn't accept operators*/
                          if (my_label->type == ACTION){
                              if (strcmp (my_label->action_name, "rts") == 0 || strcmp (my_label->action_name, "stop") == 0){
                                  int res_ad = check_line_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                  my_label->address, &ic_arr, &size_ic, &cap_ic, 0);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (res_ad != 0 && strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if (res_ad != 0 && check != ERROR){
                                      IC = res_ad;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              /*Actions that accept 2 operators*/
                              if (strcmp (my_label->action_name, "mov") == 0){
                                  int new_ic = check_two_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    IC+ 1, &ic_arr, &size_ic, &cap_ic, MOV-1);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "cmp") == 0){
                                  int new_ic = check_two_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, CMP);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "add") == 0){
                                  int new_ic = check_two_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, ADD);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name)== 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "sub") == 0){
                                  int new_ic = check_two_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, SUB);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "lea") == 0){
                                  int new_ic = check_two_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, LEA);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      free (my_label);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              /*Actions that accept only 1 operator*/
                              if (strcmp (my_label->action_name, "not") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, NOT);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "clr") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, CLR);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = ERROR;
                                      check = ERROR;
                                      break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "inc") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, INC);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "dec") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, DEC);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                    if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                        printf ("label already in the array. err in line: %d\n", line_count);
                                        error_flag = ERROR;
                                        check = ERROR;
                                        break;
                                      }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                    IC = new_ic;
                                    insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                    line_count++;
                                    free (my_label);
                                    continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "jmp") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, JMP);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                      if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                          printf ("label already in the array. err in line: %d\n", line_count);
                                          error_flag = ERROR;
                                          check = ERROR;
                                          break;
                                        }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                      IC = new_ic;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "bne") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, BNE);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                      if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                          printf ("label already in the array. err in line: %d\n", line_count);
                                          error_flag = ERROR;
                                          check = ERROR;
                                          break;
                                        }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                      IC = new_ic;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "red") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, RED);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                      if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                          printf ("label already in the array. err in line: %d\n", line_count);
                                          error_flag = ERROR;
                                          check = ERROR;
                                          break;
                                        }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                      IC = new_ic;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "prn") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, PRN);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                      if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                          printf ("label already in the array. err in line: %d\n", line_count);
                                          error_flag = ERROR;
                                          check = ERROR;
                                          break;
                                        }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                      IC = new_ic;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }

                              if (strcmp (my_label->action_name, "jsr") == 0){
                                  int new_ic = check_one_op_action (line_count, line_1, my_label->index, my_label->action_name,
                                                                    1+ IC, &ic_arr, &size_ic, &cap_ic, JSR);
                                  int inx = 0;
                                  int check = 0;
                                  for (; inx < size_label; inx++){
                                      if (strcmp (label_arr[inx].name, my_label->name) == 0){
                                          printf ("label already in the array. err in line: %d\n", line_count);
                                          error_flag = ERROR;
                                          check = ERROR;
                                          break;
                                        }
                                    }
                                  if (new_ic != 0 && check != ERROR){
                                      IC = new_ic;
                                      insert_label (&label_arr, &size_label, &cap_label, my_label, line_count);
                                      line_count++;
                                      free (my_label);
                                      continue;
                                    }
                                  free (my_label);
                                  error_flag = ERROR;
                                  line_count++;
                                  continue;
                                }
                            }
                        }
                      /*If the label itself is illegal*/
                      free (my_label);
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  /*Under each condition, the legality of the instruction
                  * line is checked so that the number, type of parameters
                  * and their writing is correct.
                  * If everything is in correct, we will insert the
                  * words into the array.*/

                  /*When the line does not contain a label as the first word*/

                  if (strcmp (res.word, ".string") == 0){
                      char *my_res = check_string_lbl (line_count, line_1, res.index);
                      int c = 0;
                      if (my_res != NULL){
                          int count = DC + 1;
                          while (c <= strlen (my_res)){
                              dc *my_dc = (dc *) malloc (sizeof (dc));
                              if (my_dc == NULL){
                                  printf ("Memory reallocation failed.\n");
                                  free (my_dc);
                                  exit (1);
                                }
                              if (c != 0){
                                  count++;
                                }
                              my_dc->type = STRING;
                              my_dc->address = count;
                              if (c < strlen (my_res)){
                                  my_dc->letter = (int) my_res[c];
                                }
                              else{
                                  my_dc->letter = 0;
                                }
                              insert_to_dc (&dc_arr, &size_dc, &cap_dc, my_dc);
                              free (my_dc);
                              c++;
                            }
                          if(my_res != NULL){
                              free(my_res);
                            }
                          DC = DC + (int) strlen (my_res) + 1;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, ".data") == 0){
                      char *my_res = check_data_lbl (line_count, line_1, res.index, 0);
                      if (my_res != NULL){
                          int first_token = 0;
                          int count = DC + 1;
                          char *token = NULL;
                          token = strtok (my_res, ",");
                          while (token != NULL){
                              dc *my_dc = (dc *) malloc (sizeof (dc));
                              if (my_dc == NULL){
                                  printf ("Memory reallocation failed.\n");
                                  free (my_dc);
                                  exit (1);
                                }
                              if (first_token != 0){
                                  count++;
                                }
                              my_dc->num = int_to_bin (atoi (token), TYP_5, 0, 0);
                              my_dc->type = DATA;
                              my_dc->address = count;
                              insert_to_dc (&dc_arr, &size_dc, &cap_dc, my_dc);
                              first_token = 1;
                              free (my_dc);
                              token = strtok (NULL, ",");
                            }
                          if(my_res != NULL){
                              free (my_res);
                            }
                          DC = count;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  /*Actions that doesn't accept operators*/
                  if (strcmp (res.word, "rts") == 0 || strcmp (res.word, "stop") == 0){
                      int res_ad = check_line_action (line_count, line_1, res.index, res.word,
                                                      IC, &ic_arr, &size_ic, &cap_ic, 1);
                      if (res_ad != 0){
                          IC = res_ad;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  /*Actions that accept 2 operators*/
                  if (strcmp (res.word, "mov") == 0){
                      int new_ic = check_two_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, MOV-1);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "cmp") == 0){
                      int new_ic = check_two_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, CMP);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "add") == 0){
                      int new_ic = check_two_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, ADD);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "sub") == 0){
                      int new_ic = check_two_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, SUB);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "lea") == 0){
                      int new_ic = check_two_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, LEA);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  /*Actions that accept only 1 operator*/
                  if (strcmp (res.word, "not") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, NOT);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "clr") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, CLR);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "inc") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, INC);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "dec") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, DEC);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }
                  if (strcmp (res.word, "jmp") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, JMP);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "bne") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, BNE);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }
                  if (strcmp (res.word, "red") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, RED);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "prn") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, PRN);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, "jsr") == 0){
                      int new_ic = check_one_op_action (line_count, line_1, res.index, res.word,
                                                        1+ IC, &ic_arr, &size_ic, &cap_ic, JSR);
                      if (new_ic != 0){
                          IC = new_ic;
                          line_count++;
                          continue;
                        }
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }

                  /*extern type check*/
                  if (strcmp (res.word, ".extern") == 0){
                      int check = 0;
                      char *my_res = check_data_lbl (line_count, line_1, res.index, 1);
                      if(my_res != NULL){
                          char* string_copy = my_strdup(my_res);
                          char* token;
                          token = strtok(my_res, ",");
                          while (token != NULL) {
                              int inx = 0;
                              for (; inx < size_label; inx++){
                                  if (strcmp (label_arr[inx].name, token) == 0){
                                      printf ("label already in the array. err in line: %d\n", line_count);
                                      error_flag = 1;
                                      check = 1;
                                      break;
                                    }
                                }
                              if(check == 1){
                                  break;
                                }
                              token = strtok(NULL, ",");
                          }
                          if (string_copy != NULL && check == 0){
                              insert_extern_label (&label_arr, &size_label, &cap_label, string_copy, line_count);
                              if (my_res != NULL){
                                  free (my_res);
                                }
                              line_count++;
                              continue;
                            }
                          else{
                              if (my_res != NULL){
                                  free (my_res);
                                }
                              error_flag =1;
                              line_count++;
                              continue;
                            }
                      }
                      error_flag =1;
                      line_count++;
                      continue;
                    }

                  if (strcmp (res.word, ".entry") == 0){
                    line_count++;
                      continue;
                    }

                  else {
                      printf ("Illegal row. err line: %d\n", line_count);
                      error_flag = ERROR;
                      line_count++;
                      continue;
                    }
                }

          /*if so far the file is legal, The program handles all the lines that contain the command "entry"*/
          if (error_flag == 0){
              int e = 0;
              int f = 0;
              int g = 0;
              int my_line_count = 1;

              fseek (file_2, 0, SEEK_SET);
              while (fgets (cur_line, sizeof (cur_line), file_2) != NULL){
                  word res;
                  line line_2;
                  line_2.value = cur_line;
                  line_2.val_length = (int) strlen (cur_line);
                  res = check_line (line_2);

                  if (strcmp (res.word, ".entry") == 0){
                      char *my_lbl = check_entry (line_2, res.index, my_line_count);
                      if (my_lbl != NULL){
                          int d = 0;
                          int lbl_flag = 0;
                          for (; d < size_label; d++){
                              if (strcmp (my_lbl, label_arr[d].name) == 0){
                                  if(label_arr[d].symbol == ENTRY || label_arr[d].symbol == EXTERN){
                                      printf ("label entry cant be defined twice or exit as extern as well. err line: %d\n", my_line_count);
                                      error_flag = 1;
                                      free (my_lbl);
                                      my_line_count++;
                                      lbl_flag = -1;
                                      break;
                                  }
                                  lbl_flag = 1;
                                  label_arr[d].symbol = ENTRY;
                                  entry_flag = 1;
                                  free (my_lbl);
                                  my_line_count++;
                                  break;
                                }
                            }
                          if (lbl_flag == 0 ){
                              error_flag = ERROR;
                              free (my_lbl);
                              printf ("label entry doesnt exit in the file. err line: %d\n", my_line_count);
                              my_line_count++;
                              continue;
                            }
                        }
                      else{
                          error_flag = ERROR;
                          free (my_lbl);
                          my_line_count++;
                        }
                    } else{
                      my_line_count++;
                    }
                }

                  /*We make another pass over the file.am in order to go over all the labels,
                  *encode their address and update the address of the dc words*/
                  if (error_flag != ERROR && size_ic != 0){
                      int last_address = size_ic;
                      for (; e < size_dc; e++){
                          dc_arr[e].address = dc_arr[e].address + last_address;
                        }
                      for (; f < size_label; f++){
                          if (label_arr[f].type == DATA || label_arr[f].type == STRING){
                              label_arr[f].address = label_arr[f].address + last_address;
                            }
                        }

                      /*Go through all the ic-words that contain label and
                       * code their address*/
                      for (; g < size_ic; g++){
                          if (ic_arr[g].name[0] != '\0'){
                              int j = 0;
                              int lbl_exist = 0;
                              for (; j < size_label; j++){
                                  if (strcmp (ic_arr[g].name, label_arr[j].name) == 0){
                                      if (label_arr[j].symbol == ENTRY || label_arr[j].symbol == NONE){
                                          ic_arr[g].bin_code = int_to_bin ((int) label_arr[j].address, TYP_7, BIN_NUM, 0);
                                          lbl_exist = 1;
                                          break;
                                        }
                                      if (label_arr[j].symbol == EXTERN){
                                          extern_flag = 1;
                                          ic_arr[g].bin_code = int_to_bin (0, TYP_8, BIN_NUM, 0);
                                          lbl_exist = 1;
                                          break;
                                        }
                                    }
                                }
                              /*There is a label in the ic-words array that is
                               * undefined and has no address*/
                              if (lbl_exist == 0){
                                  error_flag = ERROR;
                                  printf ("label without a proper definition. err line: %d\n", my_line_count);
                                }
                            }

                        }
                    }
                }

              /*After going through the entire file, if it is correct,
               * we will write the output files*/
              if (error_flag == 0){
                  int m = 0;
                  int o = 0;
                  char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                  FILE *file_ob = NULL;

                  /*If entry words were defined we will write an output file.ent*/
                  if (entry_flag == 1){
                      int h = 0;
                      char str1[MAX_ADDRESS_LEN];
                      FILE *file_en = NULL;
                      file_en = create_entry_extern_file (argv[i], ".ent", TYP_1);
                      for (; h < size_label; h++){
                          if (label_arr[h].symbol == ENTRY){
                              fputs (label_arr[h].name, file_en);
                              fputs ("\t", file_en);
                              sprintf(str1, "%d", label_arr[h].address);
                              fputs (str1, file_en);
                              fputs ("\n", file_en);
                            }
                        }
                      fclose (file_en);
                    }
                  /*If extern words were defined we will write an output file.ext*/
                  if (extern_flag == 1){
                      int h = 0;
                      FILE *file_ex = create_entry_extern_file (argv[i], ".ext", 0);
                      for (; h < size_ic; h++){
                          int n = 0;
                          if (ic_arr[h].name[0] != '\0'){
                              for (; n < size_label; n++){
                                  char str1[MAX_ADDRESS_LEN];
                                  if (strcmp (label_arr[n].name, ic_arr[h].name) == 0 && label_arr[n].symbol == 2){
                                      fputs (label_arr[n].name, file_ex);
                                      fputs ("\t", file_ex);
                                      sprintf(str1, "%d", ic_arr[h].address);
                                      fputs (str1, file_ex);
                                      fputs ("\n", file_ex);
                                      break;
                                    }
                                }
                            }
                        }
                      fclose (file_ex);
                    }

                  /* If the input is correct, we will write the encoding file.ob
                   * according to base64 */
                  file_ob = create_object_file (argv[i]);
		  /*we write the number of ic and dc words at the head of the file*/
		  sprintf(number_str_ic,"%d", size_ic);
		  fputs(number_str_ic, file_ob);
		  fputs (" ", file_ob);
		  sprintf(number_str_dc, "%d", size_dc);
		  fputs(number_str_dc, file_ob);
		  fputs ("\n", file_ob);

                  for (; m < size_ic; m++){
                  /*writing ic words*/
                      long int right = 0;
                      long int left = 0;
                      right = (ic_arr[m].bin_code & MOV_SIX_SPOTS);
                      left = ((ic_arr[m].bin_code >> MOV_6_BIT) & MOV_SIX_SPOTS);
                      fputc (b64chars[left], file_ob);
                      fputc (b64chars[right], file_ob);
                      fputs ("\n", file_ob);
                    }
                  /*writing dc words*/
                  for (; o < size_dc; o++){
                      /*data word*/
                      long int right = 0;
                      long int left = 0;
                      if (dc_arr[o].type == DATA){
                          right = (dc_arr[o].num & MOV_SIX_SPOTS);
                          left = ((dc_arr[o].num >> MOV_6_BIT) & MOV_SIX_SPOTS);
                          fputc (b64chars[left], file_ob);
                          fputc (b64chars[right], file_ob);
                          fputs ("\n", file_ob);
                        }
                      /*string word*/
                      if (dc_arr[o].type == STRING){
                          right = (dc_arr[o].letter & MOV_SIX_SPOTS);
                          left = ((dc_arr[o].letter >> MOV_6_BIT) & MOV_SIX_SPOTS);
                          fputc (b64chars[left], file_ob);
                          fputc (b64chars[right], file_ob);
                          fputs ("\n", file_ob);
                        }
                    }
                  fclose (file_ob);
                  fclose(file_2);
                } 
 	      
              free_memory (ic_arr, &size_ic, dc_arr, &size_dc, label_arr, &size_label, macro_arr, &size);

          error_flag = OK;
          flag = 0;
          size = 0;
          capacity = 1;
          size_label = 0;
          cap_label = 1;
          size_dc = 0;
          cap_dc = 1;
          size_ic = 0;
          cap_ic = 1;
          entry_flag = 0;
          extern_flag = 0;
          line_count = 1;
        }
    }
  return 0;
  }



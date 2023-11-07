myprog:pre_asm.h first_asm.h second_asm.h pre_asm.c first_asm.c second_asm.c main.c
	gcc -ansi -pedantic -g -Wall pre_asm.h first_asm.h second_asm.h pre_asm.c first_asm.c second_asm.c main.c -o myprog


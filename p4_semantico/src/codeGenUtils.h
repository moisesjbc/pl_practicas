/*This file intends to keep the defeinitions of the procedures and functions that will used in
the code generation. Also the global variables and data structures*/
#include "Q.h"
/*gc creates the code line that the code generation part passes*/
//gc(char *str);

/*Registers a new label and returns the identifier*/
int newLabel();

/*Returns an available register*/
int assignRegisters(int type);

/*Frees all the registers*/
int freeRegisters();

/*Frees the i-th register*/
int freeRegister(int i, int type);

/*Code generation for variables*/
int variableCodeGeneration;

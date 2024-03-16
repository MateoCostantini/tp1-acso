#include <stdio.h>


// agergar al makefile

typedef struct {
  const int *opcode;
  int opcode_length;
  void (*instruction_function)(int*);
  int* (*identify_params) (int*);
} Instruction;




// definir cada instruccion con su opcode 
void func(){
  const int instructions_quantity = 26; // la defini tambien en sim.c, identify_instruction
  Instruction **instructions = malloc(instructions_quantity * sizeof(Instruction*));
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion siempre se mantiene el mismo punetro a struct


  Instruction* ADDS_ER;
  const int op[] = {1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  ADDS_ER->opcode = op;
  //agregar las funciones para esa instruccion en los parametros de la struct
  instructions[0] = ADDS_ER;

  Instruction* ADDS_I;
  const int op[] = {1,0,1,1,0,0,0,1};
  ADDS_I->opcode = op;
  // agregar funciones
  instructions[1] = ADDS_I;

  Instruction* SUBS_ER;
  const int op[] = {1,1,1,0,1,0,1,1,0,0,1};
  SUBS_ER->opcode = op;
  // agregar funciones
  instructions[2] = SUBS_ER;

  Instruction* SUBS_I;
  const int op[] = {1,1,1,1,0,0,0,1};
  SUBS_I->opcode = op;
  // agregar funciones
  instructions[3] = SUBS_I;

  Instruction* HLT;
  const int op[] = {0}; //buscar
  HLT->opcode = op;
  // agregar funciones
  instructions[4] = HLT;

  Instruction* CMP_ER;
  const int op[] = {0}; //buscar
  CMP_ER->opcode = op;
  // agregar funciones
  instructions[5] = CMP_ER;

  Instruction* CMP_I;
  const int op[] = {0}; //buscar
  CMP_I->opcode = op;
  // agregar funciones
  instructions[6] = CMP_I;

  


  // seguir...
}
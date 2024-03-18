#include "shell.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// agergar al makefile
// falta tambien definir el length del opcode para cada instruccion

typedef struct {
  const int *opcode;
  const int *cond;
  const int *imms;
  int opcode_length;
  void (*instruction_function)(int *);
  int *(*identify_params)(int *);
} Instruction;

// me agarro la duda si la forma de acceder a los registros y memoria es con int
// binario y no decimal!!

// Hay funciones que se repiten (son iguales) habria que sacar las repetidas y
// generalizar

// habria que inicializar params como arreglos de 0s xq sino sumaria cualquier
// cosa

int *identify_params_1(int *instruction_base) { // chequear si el opction y imm3
                                                // hay que ponerlo como param
  // sirve para ADDS_ER, SUBS_ER, ANDS_SR, EOR_SR, ORR_SR
  int *params = (int *)calloc(3, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if (i >= (31 - 20) && i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if (i >= (31 - 9) && i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if (i >= (31 - 4) && i <= 31) {
      params[2] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_2(int *instruction_base) {
  // sirve para ADDS_I, SUBS_I
  int *params = (int *)calloc(4, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 23) <= i <= (31 - 22)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 22) - i));
    } else if ((31 - 21) <= i <= (31 - 10)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 10) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[2] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[3] += instruction_base[i] * (pow(2, (31 - i)));
    }
  }
  return params;
}

int *identify_params_HLT(int *instruction_base) {
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_CMP_ER(int *instruction_base) {
  int *params = (int *)calloc(2, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_CMP_I(int *instruction_base) {
  int *params = (int *)calloc(3, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 23) <= i <= (31 - 22)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 22) - i));
    } else if ((31 - 21) <= i <= (31 - 10)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 10) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[2] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_B(int *instruction_base) {
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 25) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_BR(int *instruction_base) {
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_Bcond(int *instruction_base) { // muchas dudas con este
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 3) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_3(int *instruction_base) {
  // sirve para LSR, LSL, STUR, STURB, STURH, LDUR, LDURH, LDURB
  int *params = (int *)calloc(2, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[1] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_MOVZ(int *instruction_base) {
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < 32; i++) {
    if ((31 - 4) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

void function_ADDS_ER(int *params) {
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 + x2;

  if (x0 == 0) {
    NEXT_STATE.FLAG_Z = 1;
    NEXT_STATE.FLAG_N = 0;
  } else if (x0 < 0){
    NEXT_STATE.FLAG_N = 1;
    NEXT_STATE.FLAG_Z = 0;
  }else{
    NEXT_STATE.FLAG_N = 0;
    NEXT_STATE.FLAG_Z = 0;
  }
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 32;
}






// definir cada instruccion con su opcode
Instruction **build_instructions() {
  const int instructions_quantity =
      27; // la defini tambien en sim.c, identify_instruction
  Instruction **instructions = malloc(sizeof(Instruction *) * 27);
  if (instructions == NULL) {
    return NULL;
  }
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion
  // siempre se mantiene el mismo punetro a struct
  Instruction *ADDS_ER;
  ADDS_ER->opcode = (const int[]){1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  ADDS_ER->identify_params = identify_params_1;
  // agregar las funciones para esa instruccion en los parametros de la struct
  instructions[0] = ADDS_ER;

  Instruction *ADDS_I;
  ADDS_I->opcode = (const int[]){1, 0, 1, 1, 0, 0, 0, 1};
  // agregar funciones
  ADDS_I->identify_params = identify_params_2;
  instructions[1] = ADDS_I;

  Instruction *SUBS_ER;
  SUBS_ER->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  SUBS_ER->identify_params = identify_params_1;
  // agregar funciones
  instructions[2] = SUBS_ER;

  Instruction *SUBS_I;
  SUBS_I->opcode = (const int[]){1, 1, 1, 1, 0, 0, 0, 1};
  SUBS_I->identify_params = identify_params_2;
  // agregar funciones
  instructions[3] = SUBS_I;

  Instruction *HLT;
  HLT->opcode =
      (const int[]){1, 1, 0, 1, 0, 1,
                    0, 0, 0, 1, 0}; // chequear si falta un 0 por la consigna
  HLT->identify_params = identify_params_HLT;
  // agregar funciones
  instructions[4] = HLT;

  Instruction *CMP_ER;
  CMP_ER->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  CMP_ER->identify_params = identify_params_CMP_ER;
  // agregar funciones
  instructions[5] = CMP_ER;

  Instruction *CMP_I;
  CMP_I->opcode = (const int[]){1, 1, 1, 1, 0, 0, 0, 1};
  CMP_I->identify_params = identify_params_CMP_I;
  // agregar funciones
  instructions[6] = CMP_I;

  Instruction *ANDS_SR;
  ANDS_SR->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 0};
  ANDS_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[7] = ANDS_SR;

  Instruction *EOR_SR;
  EOR_SR->opcode = (const int[]){1, 1, 0, 0, 1, 0, 1, 0};
  EOR_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[8] = EOR_SR;

  Instruction *ORR_SR;
  ORR_SR->opcode = (const int[]){1, 0, 1, 0, 1, 0, 1, 0};
  ORR_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[9] = ORR_SR;

  Instruction *B;
  B->opcode = (const int[]){0, 0, 0, 1, 0, 1};
  B->identify_params = identify_params_B;
  // agregar funciones
  instructions[10] = B;

  Instruction *BR;
  BR->opcode = (const int[]){1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0,
                             1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  BR->identify_params = identify_params_BR;
  // agregar funciones
  instructions[11] = BR;

  Instruction *BEQ;
  BEQ->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BEQ->cond = (const int[]){0, 0, 0, 0};
  BEQ->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[12] = BEQ;

  Instruction *BNE;
  BNE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BNE->cond = (const int[]){0, 0, 0, 1};
  BNE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[13] = BNE;

  Instruction *BGT;
  BGT->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BGT->cond = (const int[]){1, 1, 0, 0};
  BGT->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[14] = BGT;

  Instruction *BLT;
  BLT->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BLT->cond = (const int[]){1, 0, 1, 1};
  BLT->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[15] = BLT;

  Instruction *BGE;
  BGE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BGE->cond = (const int[]){1, 0, 1, 0};
  BGE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[16] = BGE;

  Instruction *BLE;
  BLE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BLE->cond = (const int[]){1, 1, 0, 1};
  BLE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[17] = BLE;

  Instruction *LSL_I;
  LSL_I->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 1, 0};
  LSL_I->imms = (const int[]){1, 1, 1, 1, 1, 1};
  LSL_I->identify_params = identify_params_3;
  // agregar funciones
  instructions[18] = LSL_I;

  Instruction *LSR_I;
  LSR_I->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 1, 0}; // no pusimos N
  LSR_I->imms = (const int[]){0, 0, 0, 0, 0, 0};
  LSR_I->identify_params = identify_params_3;
  // agregar funciones
  instructions[19] = LSR_I;

  Instruction *STUR;
  STUR->opcode = (const int[]){1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STUR->identify_params = identify_params_3;
  // agregar funciones
  instructions[20] = STUR;

  Instruction *STURB;
  STURB->opcode = (const int[]){0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURB->identify_params = identify_params_3;
  // agregar funciones
  instructions[21] = STURB;

  Instruction *STURH;
  STURH->opcode = (const int[]){0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURH->identify_params = identify_params_3;
  // agregar funciones
  instructions[22] = STURH;

  Instruction *LDUR;
  LDUR->opcode = (const int[]){1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDUR->identify_params = identify_params_3;
  // agregar funciones
  instructions[23] = LDUR;

  Instruction *LDURH;
  LDURH->opcode = (const int[]){0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURH->identify_params = identify_params_3;
  // agregar funciones
  instructions[24] = LDURH;

  Instruction *LDURB;
  LDURB->opcode = (const int[]){0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURB->identify_params = identify_params_3;
  // agregar funciones
  instructions[25] = LDURB;

  Instruction *MOVZ;
  MOVZ->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 0, 1};
  MOVZ->identify_params = identify_params_MOVZ;
  // agregar funciones
  instructions[26] = MOVZ;

  return instructions;
}
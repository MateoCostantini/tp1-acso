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
  } else if (x0 < 0) {
    NEXT_STATE.FLAG_N = 1;
    NEXT_STATE.FLAG_Z = 0;
  } else {
    NEXT_STATE.FLAG_N = 0;
    NEXT_STATE.FLAG_Z = 0;
  }
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 32;
}

void function_ADDS_I(int *params) {}

// definir cada instruccion con su opcode
Instruction** build_instructions() {
  const int instructions_quantity =
      27; // la defini tambien en sim.c, identify_instruction
  Instruction **instructions = malloc(sizeof(Instruction*) * 27);
  if (instructions == NULL){
    return NULL;
  }
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion
  // siempre se mantiene el mismo punetro a struct
  Instruction *ADDS_ER = malloc(sizeof(Instruction));
  if (ADDS_ER == NULL) return NULL;
  ADDS_ER->opcode = (const int[]){1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  ADDS_ER->identify_params = identify_params_1;
  // agregar las funciones para esa instruccion en los parametros de la struct
  instructions[0] = ADDS_ER;

  Instruction *ADDS_I = malloc(sizeof(Instruction));
  if (ADDS_I == NULL) return NULL;
  ADDS_I->opcode = (const int[]){1, 0, 1, 1, 0, 0, 0, 1};
  // agregar funciones
  ADDS_I->identify_params = identify_params_2;
  instructions[1] = ADDS_I;

  Instruction *SUBS_ER = malloc(sizeof(Instruction));
  if (SUBS_ER == NULL) return NULL;
  SUBS_ER->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  SUBS_ER->identify_params = identify_params_1;
  // agregar funciones
  instructions[2] = SUBS_ER;

  Instruction *SUBS_I = malloc(sizeof(Instruction));
  if (SUBS_I == NULL) return NULL;
  SUBS_I->opcode = (const int[]){1, 1, 1, 1, 0, 0, 0, 1};
  SUBS_I->identify_params = identify_params_2;
  // agregar funciones
  instructions[3] = SUBS_I;

  Instruction *HLT = malloc(sizeof(Instruction));
  if (HLT == NULL) return NULL;
  HLT->opcode = (const int[]){1, 1, 0, 1, 0, 1,0, 0, 0, 1, 0};  // chequear si falta un 0 por la consigna
  HLT->identify_params = identify_params_HLT;
  // agregar funciones
  instructions[4] = HLT;

  Instruction *CMP_ER = malloc(sizeof(Instruction));
  if (CMP_ER == NULL) return NULL;
  CMP_ER->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  CMP_ER->identify_params = identify_params_CMP_ER;
  // agregar funciones
  instructions[5] = CMP_ER;

  Instruction *CMP_I = malloc(sizeof(Instruction));
  if (CMP_I == NULL) return NULL;
  CMP_I->opcode = (const int[]) {1, 1, 1, 1, 0, 0, 0, 1};
  CMP_I->identify_params = identify_params_CMP_I;
  // agregar funciones
  instructions[6] = CMP_I;

  Instruction *ANDS_SR = malloc(sizeof(Instruction));
  if (ANDS_SR == NULL) return NULL;
  ANDS_SR->opcode = (const int[]){1, 1, 1, 0, 1, 0, 1, 0};
  ANDS_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[7] = ANDS_SR;

  Instruction *EOR_SR = malloc(sizeof(Instruction));
  if (EOR_SR == NULL) return NULL;
  EOR_SR->opcode = (const int[]){1, 1, 0, 0, 1, 0, 1, 0};
  EOR_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[8] = EOR_SR;

  Instruction *ORR_SR = malloc(sizeof(Instruction));
  if (ORR_SR == NULL) return NULL;
  ORR_SR->opcode = (const int[]){1, 0, 1, 0, 1, 0, 1, 0};
  ORR_SR->identify_params = identify_params_1;
  // agregar funciones
  instructions[9] = ORR_SR;

  Instruction *B = malloc(sizeof(Instruction));
  if (B == NULL) return NULL;
  B->opcode = (const int[]){0, 0, 0, 1, 0, 1};
  B->identify_params = identify_params_B;
  // agregar funciones
  instructions[10] = B;

  Instruction *BR = malloc(sizeof(Instruction));
  if (BR == NULL) return NULL;
  BR->opcode = (const int[]){1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0,1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  BR->identify_params = identify_params_BR;
  // agregar funciones
  instructions[11] = BR;

  Instruction *BEQ = malloc(sizeof(Instruction));
  if (BEQ == NULL) return NULL;
  BEQ->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BEQ->cond = (const int[]){0, 0, 0, 0};
  BEQ->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[12] = BEQ;

  Instruction *BNE = malloc(sizeof(Instruction));
  if (BNE == NULL) return NULL;
  BNE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BNE->cond = (const int[]){0, 0, 0, 1};
  BNE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[13] = BNE;

  Instruction *BGT = malloc(sizeof(Instruction));
  if (BGT == NULL) return NULL;
  BGT->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BGT->cond = (const int[]){1, 1, 0, 0};
  BGT->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[14] = BGT;

  Instruction *BLT = malloc(sizeof(Instruction));
  if (BLT == NULL) return NULL;
  BLT->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BLT->cond = (const int[]){1, 0, 1, 1};
  BLT->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[15] = BLT;

  Instruction *BGE = malloc(sizeof(Instruction));
  if (BGE == NULL) return NULL;
  BGE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BGE->cond = (const int[]){1, 0, 1, 0};
  BGE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[16] = BGE;

  Instruction *BLE = malloc(sizeof(Instruction));
  if (BLE == NULL) return NULL;
  BLE->opcode = (const int[]){0, 1, 0, 1, 0, 1, 0, 0};
  BLE->cond = (const int[]){1, 1, 0, 1};
  BLE->identify_params = identify_params_Bcond;
  // agregar funciones
  instructions[17] = BLE;

  Instruction *LSL_I = malloc(sizeof(Instruction));
  if (LSL_I == NULL) return NULL;
  LSL_I->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 1, 0};
  LSL_I->imms = (const int[]){1, 1, 1, 1, 1, 1};
  LSL_I->identify_params = identify_params_3;
  // agregar funciones
  instructions[18] = LSL_I;

  Instruction *LSR_I = malloc(sizeof(Instruction));
  if (LSR_I == NULL) return NULL;
  LSR_I->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 1, 0};// no pusimos N
  LSR_I->imms = (const int[]){0, 0, 0, 0, 0, 0};
  LSR_I->identify_params = identify_params_3;
  // agregar funciones
  instructions[19] = LSR_I;

  Instruction *STUR = malloc(sizeof(Instruction));
  if (STUR == NULL) return NULL;
  STUR->opcode = (const int[]){1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STUR->identify_params = identify_params_3;
  // agregar funciones
  instructions[20] = STUR;

  Instruction *STURB = malloc(sizeof(Instruction));
  if (STURB == NULL) return NULL;
  STURB->opcode = (const int[]){0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURB->identify_params = identify_params_3;
  // agregar funciones
  instructions[21] = STURB;

  Instruction *STURH = malloc(sizeof(Instruction));
  if (STURH == NULL) return NULL;
  STURH->opcode = (const int[]){0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURH->identify_params = identify_params_3;
  // agregar funciones
  instructions[22] = STURH;

  Instruction *LDUR = malloc(sizeof(Instruction));
  if (LDUR == NULL) return NULL;
  LDUR->opcode = (const int[]){1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDUR->identify_params = identify_params_3;
  // agregar funciones
  instructions[23] = LDUR;

  Instruction *LDURH = malloc(sizeof(Instruction));
  if (LDURH == NULL) return NULL;
  LDURH->opcode = (const int[]){0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURH->identify_params = identify_params_3;
  // agregar funciones
  instructions[24] = LDURH;

  Instruction *LDURB = malloc(sizeof(Instruction));
  if (LDURB == NULL) return NULL;
  LDURB->opcode = (const int[]){0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURB->identify_params = identify_params_3;
  // agregar funciones
  instructions[25] = LDURB;

  Instruction *MOVZ = malloc(sizeof(Instruction));
  if (MOVZ == NULL) return NULL;
  MOVZ->opcode = (const int[]){1, 1, 0, 1, 0, 0, 1, 0, 1};
  MOVZ->identify_params = identify_params_MOVZ;
  // agregar funciones
  instructions[26] = MOVZ;

  return instructions;
}
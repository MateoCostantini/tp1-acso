#include "shell.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// agergar al makefile
// falta tambien definir el length del opcode para cada instruccion

typedef struct {
  uint32_t opcode;
  void (*instruction_function)(int *);
  int *(*identify_params)(uint32_t);
} Instruction;

// me agarro la duda si la forma de acceder a los registros y memoria es con int
// binario y no decimal!!

// Hay funciones que se repiten (son iguales) habria que sacar las repetidas y
// generalizar

// habria que inicializar params como arreglos de 0s xq sino sumaria cualquier
// cosa

// los contenidos de los registros en que base?

// El PC esta en bytes o bits para ver si sumarle 4 o 32

void update_flags(int n) {
  if (n == 0) {
    NEXT_STATE.FLAG_Z = 1;
    NEXT_STATE.FLAG_N = 0;
  } else if (n < 0) {
    NEXT_STATE.FLAG_N = 1;
    NEXT_STATE.FLAG_Z = 0;
  } else {
    NEXT_STATE.FLAG_N = 0;
    NEXT_STATE.FLAG_Z = 0;
  }
}

// revisar esto bien
int64_t sign_extend(int64_t n, int n_bits) {
  int64_t mask = (int64_t)1 << (n_bits - 1);
  n = n & (((int64_t)1 << n_bits) - 1);
  return (n ^ mask) - mask;
}

int32_t sign_extend32(int32_t n, int n_bits) {
  int32_t mask = (int32_t)1 << (n_bits - 1);
  n = n & ((1 << n_bits) - 1);

  return (n ^ mask) - mask;
}

int *identify_params_1(
    uint32_t instruction_base) { // chequear si el opction y imm3
                                 // hay que ponerlo como param
  // sirve para ADDS_ER, SUBS_ER, ANDS_SR, EOR_SR, ORR_SR
  uint32_t masks[] = {0b00000000000111110000000000000000,
                      0b00000000000000000000001111100000,
                      0b00000000000000000000000000011111};
  int *params = (int *)calloc(3, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 16;
  params[1] = (masks[1] & instruction_base) >> 5;
  params[2] = (masks[2] & instruction_base);
  return params;
}

int *identify_params_2(uint32_t instruction_base) {
  // sirve para ADDS_I, SUBS_I
  uint32_t masks[] = {
      0b00000000110000000000000000000000, 0b00000000001111111111110000000000,
      0b00000000000000000000001111100000, 0b00000000000000000000000000011111};
  int *params = (int *)calloc(4, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 22;
  params[1] = (masks[1] & instruction_base) >> 10;
  params[2] = (masks[2] & instruction_base) >> 5;
  params[3] = (masks[3] & instruction_base);
  return params;
}

int *identify_params_HLT(uint32_t instruction_base) {
  uint32_t masks[] = {0b00000000000111111111111111100000};
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 5;
  return params;
}

/*int *identify_params_CMP_ER(uint32_t instruction_base) {
  uint32_t masks[] = {0b00000000000111110000000000000000,
                      0b00000000000000000000001111100000};
  int *params = (int *)calloc(2, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 16;
  params[1] = (masks[1] & instruction_base) >> 5;
  return params;
}*/

/*int *identify_params_CMP_I(
  uint32_t instruction_base) { // se podria unificar con identify 2
  uint32_t masks[] = {
      0b00000000110000000000000000000000,
      0b00000000001111111111110000000000,
      0b00000000000000000000001111100000,
  };
  int *params = (int *)calloc(3, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 22;
  params[1] = (masks[1] & instruction_base) >> 10;
  params[2] = (masks[2] & instruction_base) >> 5;
  return params;
}*/

int *identify_params_B(uint32_t instruction_base) {
  uint32_t masks[] = {
      0b00000011111111111111111111111111,
  };
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = masks[0] & instruction_base;
  return params;
}

int *identify_params_BR(uint32_t instruction_base) {
  uint32_t masks[] = {
      0b00000000000000000000001111100000,
  };
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = masks[0] & instruction_base >> 5;
  return params;
}

int *identify_params_Bcond(
    uint32_t instruction_base) { // tiene imm19 de parametro
  uint32_t masks[] = {
      0b00000000111111111111111111100000,
  };
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 5;
  return params;
}

int *identify_params_LSL_LSR_I(uint32_t instruction_base) {
  uint32_t masks[] = {
      0b00000000001111110000000000000000, 0b00000000000000001111110000000000,
      0b00000000000000000000001111100000, 0b00000000000000000000000000011111};
  int *params = (int *)calloc(4, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 16;
  params[1] = (masks[1] & instruction_base) >> 10;
  params[2] = (masks[2] & instruction_base) >> 5;
  params[3] = masks[3] & instruction_base;
  return params;
}

int *identify_params_3(uint32_t instruction_base) {
  // sirve para STUR, STURB, STURH, LDUR, LDURH, LDURB
  uint32_t masks[] = {0b00000000000111111111000000000000, // chequear
                      0b00000000000000000000001111100000,
                      0b00000000000000000000000000011111};
  int *params = (int *)calloc(3, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 12;
  params[1] = (masks[1] & instruction_base) >> 5;
  params[2] = masks[2] & instruction_base;
  return params;
}

int *identify_params_MOVZ(uint32_t instruction_base) {
  printf("entre a identify params movz\n\n");
  uint32_t masks[] = {0b00000000000111111111111111100000,
                      0b00000000000000000000000000011111};
  int *params = (int *)calloc(2, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 5;
  params[1] = masks[1] & instruction_base;
  return params;
}

void function_ADDS_ER(int *params) { // el bit 21 de output es 0, no 1
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 + x2;

  update_flags(x0);
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 4;
}

void function_ADDS_I(int *params) {
  int x1 = CURRENT_STATE.REGS[params[2]];
  int imm = params[1];
  if (params[0] == 1) {
    int imm = imm << 12;
  }
  int x0 = x1 + imm;

  update_flags(x0);
  NEXT_STATE.REGS[params[3]] = x0;
  NEXT_STATE.PC += 4;
}

void function_SUBS_CMP_ER(int *params) {
  // subs_er el bit 21 de output es 0, no 1
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 - x2;

  if (params[2] != 31) { // esto en binario o que?
    NEXT_STATE.REGS[params[2]] = x0;
  }
  update_flags(x0);
  NEXT_STATE.PC += 4;
}

void function_SUBS_CMP_I(int *params) {
  int x1 = CURRENT_STATE.REGS[params[2]];
  int imm = params[1];
  if (params[0] == 1) {
    int imm = imm << 12;
  }
  int x0 = x1 - imm;

  if (params[2] != 31) { // esto en binario o que?
    NEXT_STATE.REGS[params[3]] = x0;
  }
  update_flags(x0);
  NEXT_STATE.PC += 4;
}

void function_HLT(int *params) {
  RUN_BIT = 0; // habria que chequear params?
  NEXT_STATE.PC += 4;
}

void function_ANDS_SR(int *params) {
  // hay un shift en el la instruccion y en la consigna no dice que no lo
  // usemos. (igual para eor)
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 & x2;
  update_flags(x0);
  NEXT_STATE.REGS[params[2]] = x0; // tiene que ser con 64 bits
  NEXT_STATE.PC += 4;
}

void function_EOR_SR(int *params) { // aclaracion: no actualiza flags
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 ^ x2;
  NEXT_STATE.REGS[params[2]] = x0; // tiene que ser con 64 bits
  NEXT_STATE.PC += 4;
}

void function_ORR_SR(int *params) { // aclaracion: no actualiza flags
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 | x2;
  NEXT_STATE.REGS[params[2]] = x0; // tiene que ser con 64 bits
  NEXT_STATE.PC += 4;
}

void function_B(int *params) { // no entiendo la aclaracion de la consigna
  int32_t imms = sign_extend32(params[0], 26);
  int offset = (imms) * 4; // signed
  NEXT_STATE.PC += offset; // chequear
}

void function_BR(int *params) { NEXT_STATE.PC = CURRENT_STATE.REGS[params[0]]; }

void function_BEQ(int *params) {
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 1) {
    NEXT_STATE.PC += (imms) * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BNE(int *params) {
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 0) {
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BGT(int *params) {
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 0 && CURRENT_STATE.FLAG_N == 0) {
    NEXT_STATE.PC = imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BLT(int *params) {
  if (CURRENT_STATE.FLAG_N == 1) {
    int32_t imms = sign_extend32(params[0], 19);
    NEXT_STATE.PC = imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BGE(int *params) {
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_N == 0) {
    NEXT_STATE.PC = imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BLE(int *params) {
  int32_t imms = sign_extend32(params[0], 19);
  if (!(CURRENT_STATE.FLAG_N == 0 && CURRENT_STATE.FLAG_Z == 0)) {
    NEXT_STATE.PC = imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_LSL_LSR_I(int *params) {
  int x1 = CURRENT_STATE.REGS[params[2]];
  int shift = params[0];
  if (params[1] == 0b111111) {
    NEXT_STATE.REGS[params[3]] = x1 >> shift;
  } else {
    NEXT_STATE.REGS[params[3]] = x1 << shift;
  }
  NEXT_STATE.PC += 4;
}

void function_STUR(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  address = address + offset;

  mem_write_32(address, value);
  NEXT_STATE.PC += 4;
}

void function_STURB(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int64_t address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  uint32_t least_sig_byte =
      value & 0xFF; // asumiendo que quiere los menos significativos
  uint32_t shifted = least_sig_byte; // << 24;
  address = address + offset;
  uint32_t saved = mem_read_32(address);
  saved &= 0xFFFFFF00;
  saved |= shifted; // cambiar nombre a shifted
  printf("\naddress: %lx\n", address);
  printf("\nsaved: %x\n", saved);
  printf("\nshifted: %x\n", shifted);
  printf("\noffset: %lx\n", offset);
  mem_write_32(address, saved);
  NEXT_STATE.PC += 4;
}

void function_STURH(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int64_t address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  uint32_t least_sig_2bytes =
      value & 0xFFFF; // asumiendo que quiere los menos significativos
  uint32_t shifted = least_sig_2bytes << 16;
  address = address + offset;
  uint32_t saved = mem_read_32(address);
  saved &= 0x0000FFFF;
  saved |= shifted;

  mem_write_32(address, saved);
  NEXT_STATE.PC += 4;
}

void function_LDUR(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  address = address + offset;

  uint32_t lower = mem_read_32(address);

  uint32_t upper = mem_read_32(address + 4);
  int64_t combined = ((int64_t)upper << 32) | lower;

  NEXT_STATE.REGS[params[2]] = combined;
  NEXT_STATE.PC += 4;

}

void function_LDURB(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  address = address + offset;

  uint32_t saved = mem_read_32(address);

  int first_byte = saved & 0xFF;

  NEXT_STATE.REGS[params[2]] = first_byte;
  NEXT_STATE.PC += 4;
}

void function_LDURH(int *params) {
  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return; // ver que hacemos si el address es 31

  address = address + offset;

  uint32_t saved = mem_read_32(address);

  int first2_bytes = saved & 0xFFFF;

  NEXT_STATE.REGS[params[2]] = first2_bytes;
  NEXT_STATE.PC += 4;
}

void function_MOVZ(int *params) {
  printf("entre a movz\n\n");
  int imm = params[0]; // ver tipo
  NEXT_STATE.REGS[params[1]] = imm;
  NEXT_STATE.PC += 4;
}

// definir cada instruccion con su opcode
Instruction **build_instructions() {
  const int instructions_quantity =
      23; // la defini tambien en sim.c, identify_instruction
  Instruction **instructions = malloc(sizeof(Instruction *) * 27);
  if (instructions == NULL) {
    return NULL;
  }
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion
  // siempre se mantiene el mismo punetro a struct
  Instruction *ADDS_ER = malloc(sizeof(Instruction));
  if (ADDS_ER == NULL)
    return NULL;
  ADDS_ER->opcode = (uint32_t)0b10101011000000000000000000000000;
  ADDS_ER->identify_params = identify_params_1;
  ADDS_ER->instruction_function = function_ADDS_ER;
  instructions[0] = ADDS_ER;

  Instruction *ADDS_I = malloc(sizeof(Instruction));
  if (ADDS_I == NULL)
    return NULL;
  ADDS_I->opcode = (uint32_t)0b10110001000000000000000000000000;
  ADDS_I->identify_params = identify_params_2;
  ADDS_I->instruction_function = function_ADDS_I;
  instructions[1] = ADDS_I;

  Instruction *SUBS_CMP_ER = malloc(sizeof(Instruction));
  if (SUBS_CMP_ER == NULL)
    return NULL;
  SUBS_CMP_ER->opcode = (uint32_t)0b11101011000000000000000000000000;
  SUBS_CMP_ER->identify_params = identify_params_1;
  SUBS_CMP_ER->instruction_function = function_SUBS_CMP_ER;
  instructions[2] = SUBS_CMP_ER;

  Instruction *SUBS_CMP_I = malloc(sizeof(Instruction));
  if (SUBS_CMP_I == NULL)
    return NULL;
  SUBS_CMP_I->opcode = (uint32_t)0b11110001000000000000000000000000;
  SUBS_CMP_I->identify_params = identify_params_2;
  SUBS_CMP_I->instruction_function = function_SUBS_CMP_I;
  instructions[3] = SUBS_CMP_I;

  Instruction *HLT = malloc(sizeof(Instruction));
  if (HLT == NULL)
    return NULL;
  HLT->opcode =
      (uint32_t)0b11010100010000000000000000000000; // chequear si falta un 0
                                                    // por la consigna
  HLT->identify_params = identify_params_HLT;
  HLT->instruction_function = function_HLT;
  instructions[4] = HLT;

  Instruction *ANDS_SR = malloc(sizeof(Instruction));
  if (ANDS_SR == NULL)
    return NULL;
  ANDS_SR->opcode = (uint32_t)0b11101010000000000000000000000000;
  ANDS_SR->identify_params = identify_params_1;
  ANDS_SR->instruction_function = function_ANDS_SR;
  instructions[5] = ANDS_SR;

  Instruction *EOR_SR = malloc(sizeof(Instruction));
  if (EOR_SR == NULL)
    return NULL;
  EOR_SR->opcode = (uint32_t)0b11001010000000000000000000000000;
  EOR_SR->identify_params = identify_params_1;
  EOR_SR->instruction_function = function_EOR_SR;
  instructions[6] = EOR_SR;

  Instruction *ORR_SR = malloc(sizeof(Instruction));
  if (ORR_SR == NULL)
    return NULL;
  ORR_SR->opcode = (uint32_t)0b10101010000000000000000000000000;
  ORR_SR->identify_params = identify_params_1;
  ORR_SR->instruction_function = function_ORR_SR;
  instructions[7] = ORR_SR;

  Instruction *B = malloc(sizeof(Instruction));
  if (B == NULL)
    return NULL;
  B->opcode = (uint32_t)0b00010100000000000000000000000000;
  B->identify_params = identify_params_B;
  B->instruction_function = function_B;
  instructions[8] = B;

  Instruction *BR = malloc(sizeof(Instruction));
  if (BR == NULL)
    return NULL;
  BR->opcode = (uint32_t)0b11010110000111110000000000000000;
  BR->identify_params = identify_params_BR;
  BR->instruction_function = function_BR;
  instructions[9] = BR;

  Instruction *BEQ = malloc(sizeof(Instruction));
  if (BEQ == NULL)
    return NULL;
  BEQ->opcode = (uint32_t)0b01010100000000000000000000000000;
  BEQ->identify_params = identify_params_Bcond;
  BEQ->instruction_function = function_BEQ;
  instructions[10] = BEQ;

  Instruction *BNE = malloc(sizeof(Instruction));
  if (BNE == NULL)
    return NULL;
  BNE->opcode = (uint32_t)0b01010100000000000000000000000001;
  BNE->identify_params = identify_params_Bcond;
  BNE->instruction_function = function_BNE;
  instructions[11] = BNE;

  Instruction *BGT = malloc(sizeof(Instruction));
  if (BGT == NULL)
    return NULL;
  BGT->opcode = (uint32_t)0b01010100000000000000000000001100;
  BGT->identify_params = identify_params_Bcond;
  BGT->instruction_function = function_BGT;
  instructions[12] = BGT;

  Instruction *BLT = malloc(sizeof(Instruction));
  if (BLT == NULL)
    return NULL;
  BLT->opcode = (uint32_t)0b01010100000000000000000000001011;
  BLT->identify_params = identify_params_Bcond;
  BLT->instruction_function = function_BLT;
  // agregar funciones
  instructions[13] = BLT;

  Instruction *BGE = malloc(sizeof(Instruction));
  if (BGE == NULL)
    return NULL;
  BGE->opcode = (uint32_t)0b01010100000000000000000000001010;
  BGE->identify_params = identify_params_Bcond;
  BGE->instruction_function = function_BGE;
  instructions[14] = BGE;

  Instruction *BLE = malloc(sizeof(Instruction));
  if (BLE == NULL)
    return NULL;
  BLE->opcode = (uint32_t)0b01010100000000000000000000001101;
  BLE->identify_params = identify_params_Bcond;
  BLE->instruction_function = function_BLE;
  instructions[15] = BLE;

  Instruction *LSL_LSR_I = malloc(sizeof(Instruction));
  if (LSL_LSR_I == NULL)
    return NULL;
  LSL_LSR_I->opcode = (uint32_t)0b11010011000000000000000000000000; // imms
  LSL_LSR_I->identify_params = identify_params_LSL_LSR_I;
  LSL_LSR_I->instruction_function = function_LSL_LSR_I;
  instructions[16] = LSL_LSR_I;

  Instruction *STUR = malloc(sizeof(Instruction));
  if (STUR == NULL)
    return NULL;
  STUR->opcode = (uint32_t)0b11111000000000000000000000000000;
  STUR->identify_params = identify_params_3;
  STUR->instruction_function = function_STUR;
  instructions[17] = STUR;

  Instruction *STURB = malloc(sizeof(Instruction));
  if (STURB == NULL)
    return NULL;
  STURB->opcode = (uint32_t)0b00111000000000000000000000000000;
  STURB->identify_params = identify_params_3;
  STURB->instruction_function = function_STURB;
  instructions[18] = STURB;

  Instruction *STURH = malloc(sizeof(Instruction));
  if (STURH == NULL)
    return NULL;
  STURH->opcode = (uint32_t)0b01111000000000000000000000000000;
  STURH->identify_params = identify_params_3;
  STURH->instruction_function = function_STURH;
  instructions[19] = STURH;

  Instruction *LDUR = malloc(sizeof(Instruction));
  if (LDUR == NULL)
    return NULL;
  LDUR->opcode = (uint32_t)0b11111000010000000000000000000000;
  LDUR->identify_params = identify_params_3;
  LDUR->instruction_function = function_LDUR;
  instructions[20] = LDUR;

  Instruction *LDURH = malloc(sizeof(Instruction));
  if (LDURH == NULL)
    return NULL;
  LDURH->opcode = (uint32_t)0b01111000010000000000000000000000;
  LDURH->identify_params = identify_params_3;
  LDURH->instruction_function = function_LDURH;
  instructions[21] = LDURH;

  Instruction *LDURB = malloc(sizeof(Instruction));
  if (LDURB == NULL)
    return NULL;
  LDURB->opcode = (uint32_t)0b00111000010000000000000000000000;
  LDURB->identify_params = identify_params_3;
  LDURB->instruction_function = function_LDURB;
  instructions[22] = LDURB;

  Instruction *MOVZ = malloc(sizeof(Instruction));
  if (MOVZ == NULL)
    return NULL;
  MOVZ->opcode = (uint32_t)0b11010010100000000000000000000000;
  MOVZ->identify_params = identify_params_MOVZ;
  MOVZ->instruction_function = function_MOVZ;
  instructions[23] = MOVZ;

  return instructions;
}
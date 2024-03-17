#include <stdio.h>

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

int* identify_params_ADDS_ER(int* instruction_base){
  int params[3];
  for (size_t i; i < 32; i++){
    if (i >= (31 - 20) && i <= (31 - 16)){
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if (i >= (31 - 9) && i <= (31 - 5)){
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if (i >= (31 - 4) && i <= 31){
      params[2] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}



// definir cada instruccion con su opcode
void build_instructions() {
  const int instructions_quantity = 27; // la defini tambien en sim.c, identify_instruction
  Instruction *instructions[27];
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion
  // siempre se mantiene el mismo punetro a struct
  Instruction *ADDS_ER;
  const int op[] = {1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  ADDS_ER->opcode = op;
  // agregar las funciones para esa instruccion en los parametros de la struct
  instructions[0] = ADDS_ER;

  Instruction *ADDS_I;
  const int op[] = {1, 0, 1, 1, 0, 0, 0, 1};
  ADDS_I->opcode = op;
  // agregar funciones
  instructions[1] = ADDS_I;

  Instruction *SUBS_ER;
  const int op[] = {1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  SUBS_ER->opcode = op;
  // agregar funciones
  instructions[2] = SUBS_ER;

  Instruction *SUBS_I;
  const int op[] = {1, 1, 1, 1, 0, 0, 0, 1};
  SUBS_I->opcode = op;
  // agregar funciones
  instructions[3] = SUBS_I;

  Instruction *HLT;
  const int op[] = {1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0}; 
  HLT->opcode = op;
  // agregar funciones
  instructions[4] = HLT;

  Instruction *CMP_ER;
  const int op[] = {1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  CMP_ER->opcode = op;
  // agregar funciones
  instructions[5] = CMP_ER;

  Instruction *CMP_I;
  const int op[] = {1, 1, 1, 1, 0, 0, 0, 1};
  CMP_I->opcode = op;
  // agregar funciones
  instructions[6] = CMP_I;

  Instruction *ANDS_SR;
  const int op[] = {1, 1, 1, 0, 1, 0, 1, 0};
  ANDS_SR->opcode = op;
  // agregar funciones
  instructions[7] = ANDS_SR;

  Instruction *EOR_SR;
  const int op[] = {1, 1, 0, 0, 1, 0, 1, 0};
  EOR_SR->opcode = op;
  // agregar funciones
  instructions[8] = EOR_SR;

  Instruction *ORR_SR;
  const int op[] = {1, 0, 1, 0, 1, 0, 1, 0};
  ORR_SR->opcode = op;
  // agregar funciones
  instructions[9] = ORR_SR;

  Instruction *B;
  const int op[] = {0, 0, 0, 1, 0, 1};
  B->opcode = op;
  // agregar funciones
  instructions[10] = B;

  Instruction *BR;
  const int op[] = {1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0,
                    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  BR->opcode = op;
  // agregar funciones
  instructions[11] = BR;

  Instruction *BEQ;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {0, 0, 0, 0};
  BEQ->opcode = op;
  BEQ->cond = cond_;
  // agregar funciones
  instructions[12] = BEQ;

  Instruction *BNE;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {0, 0, 0, 1};
  BNE->opcode = op;
  BNE->cond = cond_;
  // agregar funciones
  instructions[13] = BNE;

  Instruction *BGT;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {1, 1, 0, 0};
  BGT->opcode = op;
  BGT->cond = cond_;
  // agregar funciones
  instructions[14] = BGT;

  Instruction *BLT;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {1, 0, 1, 1};
  BLT->opcode = op;
  BLT->cond = cond_;
  // agregar funciones
  instructions[15] = BLT;

  Instruction *BGE;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {1, 0, 1, 0};
  BGE->opcode = op;
  BGE->cond = cond_;
  // agregar funciones
  instructions[16] = BGE;

  Instruction *BLE;
  const int op[] = {0, 1, 0, 1, 0, 1, 0, 0};
  const int cond_[] = {1, 1, 0, 1};
  BLE->opcode = op;
  BLE->cond = cond_;
  // agregar funciones
  instructions[17] = BLE;

  Instruction* LSL_I;
  const int op[] = {1,1,0,1,0,0,1,1,0};
  const int imms_[] = {1,1,1,1,1,1};
  LSL_I->opcode = op;
  LSL_I->imms = imms_;
  // agregar funciones
  instructions[18] = LSL_I;

  Instruction* LSR_I;
  const int op[] = {1,1,0,1,0,0,1,1,0}; // no pusimos N
  const int imms_[] = {0,0,0,0,0,0};
  LSR_I->opcode = op;
  LSR_I->imms = imms_;
  // agregar funciones
  instructions[19] = LSR_I;

  Instruction* STUR;
  const int op[] = {1,1,1,1,1,0,0,0,0,0,0};
  STUR->opcode = op;
  // agregar funciones
  instructions[20] = STUR;

  Instruction* STURB;
  const int op[] = {0,0,1,1,1,0,0,0,0,0,0};
  STURB->opcode = op;
  // agregar funciones
  instructions[21] = STURB;

  Instruction* STURH;
  const int op[] = {0,1,1,1,1,0,0,0,0,0,0};
  STURH->opcode = op;
  // agregar funciones
  instructions[22] = STURH;

  Instruction* LDUR;
  const int op[] = {1,1,1,1,1,0,0,0,0,1,0};
  LDUR->opcode = op;
  // agregar funciones
  instructions[23] = LDUR;

  Instruction* LDURH;
  const int op[] = {0,1,1,1,1,0,0,0,0,1,0};
  LDURH->opcode = op;
  // agregar funciones
  instructions[24] = LDURH;

  Instruction* LDURB;
  const int op[] = {0,0,1,1,1,0,0,0,0,1,0};
  LDURB->opcode = op;
  // agregar funciones
  instructions[25] = LDURB;

  Instruction* MOVZ;
  const int op[] = {1,1,0,1,0,0,1,0,1};
  MOVZ->opcode = op;
  // agregar funciones
  instructions[26] = MOVZ;
}
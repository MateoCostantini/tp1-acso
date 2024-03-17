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

// me agarro la duda si la forma de acceder a los registros y memoria es con int
// binario y no decimal!!

// Hay funciones que se repiten (son iguales) habria que sacar las repetidas y
// generalizar

// habria que inicializar params como arreglos de 0s xq sino sumaria cualquier cosa

int *identify_params_ADDS_ER(
    int *instruction_base) { // chequear si el opction y imm3 hay que ponerlo
                             // como param
  int params[3];
  for (size_t i; i < 32; i++) {
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

int *identify_params_ADDS_I(int *instruction_base) {
  int params[4];
  for (size_t i; i < 32; i++) {
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

int *identify_params_SUBS_ER(int *instruction_base) {
  int params[3];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[3] += instruction_base[i] * (pow(2, (31 - i)));
    }
  }
  return params;
}

int *identify_params_SUBS_I(int *instruction_base) {
  int params[4];
  for (size_t i; i < 32; i++) {
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
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_CMP_ER(int *instruction_base) {
  int params[2];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_CMP_I(int *instruction_base) {
  int params[3];
  for (size_t i; i < 32; i++) {
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

int *identify_params_ANDS_SR(int *instruction_base) {
  int params[3];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[2] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_EOR_SR(int *instruction_base) {
  int params[3];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[2] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_ORR_SR(int *instruction_base) {
  int params[3];
  for (size_t i; i < 32; i++) {
    if ((31 - 20) <= i <= (31 - 16)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 16) - i));
    } else if ((31 - 9) <= i <= (31 - 5)) {
      params[1] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {
      params[2] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_B(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 25) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_BR(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    }
  }
  return params;
}

int *identify_params_Bcond(int *instruction_base) { // muchas dudas con este
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 3) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_LS(int *instruction_base) { // si el imms lo vemos antes, es lo mismo lsl y lsr
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) { // El imms que usamos para comparar asumo que
      params[1] += instruction_base[i] * (pow(2, 31 - i)); // lo tenemos que saber de antes para distinguir lsl y lsr
    }
  }
  return params;
}

int *identify_params_STUR(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) { // en este no hay que chequear, necesitamos los 2 en 64bits
      params[1] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}


int *identify_params_STURB(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {  // chequear si hay que guardar el param Rt o no
      params[1] += instruction_base[i] * (pow(2, 31 - i)); // el manual dice que es algo de 32 bits
    }
  }
  return params;
}

int *identify_params_STURH(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {  // chequear si hay que guardar el param Rt o no
      params[1] += instruction_base[i] * (pow(2, 31 - i)); // el manual dice que es algo de 32 bits
    }
  }
  return params;
}

int *identify_params_LDUR(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) { // en este no hay que chequear, necesitamos los 2 en 64bits
      params[1] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}

int *identify_params_LDURH(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {  // chequear si hay que guardar el param Rt o no
      params[1] += instruction_base[i] * (pow(2, 31 - i)); // el manual dice que es algo de 32 bits
    }
  }
  return params;
}

int *identify_params_LDURB(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 9) <= i <= (31 - 5)) {
      params[0] += instruction_base[i] * (pow(2, (31 - 5) - i));
    } else if ((31 - 4) <= i <= 31) {  // chequear si hay que guardar el param Rt o no
      params[1] += instruction_base[i] * (pow(2, 31 - i)); // el manual dice que es algo de 32 bits
    }
  }
  return params;
}

int *identify_params_MOVZ(int *instruction_base) {
  int params[1];
  for (size_t i; i < 32; i++) {
    if ((31 - 4) <= i <= 31) {
      params[0] += instruction_base[i] * (pow(2, 31 - i));
    }
  }
  return params;
}



// definir cada instruccion con su opcode
void build_instructions() {
  const int instructions_quantity =
      27; // la defini tambien en sim.c, identify_instruction
  Instruction *instructions[27];
  // tal vez en vez de hacerlo malloc hacerlo estatico total en cada posicion
  // siempre se mantiene el mismo punetro a struct
  Instruction *ADDS_ER;
  const int op[] = {1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1};
  ADDS_ER->opcode = op;
  ADDS_ER->identify_params = identify_params_ADDS_ER;
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
  const int op[] = {1, 1, 0, 1, 0, 1,
                    0, 0, 0, 1, 0}; // chequear si falta un 0 por la consigna
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

  Instruction *LSL_I;
  const int op[] = {1, 1, 0, 1, 0, 0, 1, 1, 0};
  const int imms_[] = {1, 1, 1, 1, 1, 1};
  LSL_I->opcode = op;
  LSL_I->imms = imms_;
  // agregar funciones
  instructions[18] = LSL_I;

  Instruction *LSR_I;
  const int op[] = {1, 1, 0, 1, 0, 0, 1, 1, 0}; // no pusimos N
  const int imms_[] = {0, 0, 0, 0, 0, 0};
  LSR_I->opcode = op;
  LSR_I->imms = imms_;
  // agregar funciones
  instructions[19] = LSR_I;

  Instruction *STUR;
  const int op[] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STUR->opcode = op;
  // agregar funciones
  instructions[20] = STUR;

  Instruction *STURB;
  const int op[] = {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURB->opcode = op;
  // agregar funciones
  instructions[21] = STURB;

  Instruction *STURH;
  const int op[] = {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
  STURH->opcode = op;
  // agregar funciones
  instructions[22] = STURH;

  Instruction *LDUR;
  const int op[] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDUR->opcode = op;
  // agregar funciones
  instructions[23] = LDUR;

  Instruction *LDURH;
  const int op[] = {0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURH->opcode = op;
  // agregar funciones
  instructions[24] = LDURH;

  Instruction *LDURB;
  const int op[] = {0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0};
  LDURB->opcode = op;
  // agregar funciones
  instructions[25] = LDURB;

  Instruction *MOVZ;
  const int op[] = {1, 1, 0, 1, 0, 0, 1, 0, 1};
  MOVZ->opcode = op;
  // agregar funciones
  instructions[26] = MOVZ;
}
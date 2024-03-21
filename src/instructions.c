#include "shell.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t opcode;
  void (*instruction_function)(int *);
  int *(*identify_params)(uint32_t);
} Instruction;

void update_flags(int n) {
  /*
  Updates the flags Z and N based on the number n.
  If n is a negative, the flag N is 1 and Z is 0
  If n is 0, the flag Z is 1 and N is 0
  Else, both flags are 0.

  params:
    n: the number to update the flags to

  returns:
    None
  */ 

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


int64_t sign_extend(int64_t n, int n_bits) {
  /*
  Extends a number n from n_bits to 64 bits

  params:
    n: a n_bits long number
    n_bits: lenght to convert from

  returns:
    (n ^ mask) - mask: n extended to 64 bits with its corresponding sign
  */

  int64_t mask = (int64_t)1 << (n_bits - 1);
  n = n & (((int64_t)1 << n_bits) - 1);
  return (n ^ mask) - mask;
}

int32_t sign_extend32(int32_t n, int n_bits) {
  /*
  Extends a number n from n_bits to 32 bits

  params:
    n: a n_bits long number
    n_bits: lenght to convert from

  returns:
    (n ^ mask) - mask: n extended to 32 bits with its corresponding sign
  */
  int32_t mask = (int32_t)1 << (n_bits - 1);
  n = n & ((1 << n_bits) - 1);

  return (n ^ mask) - mask;
}

int *identify_params_1(uint32_t instruction_base) { 
  /*
  Identifies the parameters for the functions: ADDS_ER, 
  SUBS_CMP_ER, ANDS_SR, EOR_SR and ORR_SR

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    None
  */ 

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
  /*
  Identifies the parameters for the functions: ADDS_I, 
  and SUBS_CMP_I

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */

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
  /*
  Identifies the parameters for the function HLT

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */

  uint32_t masks[] = {0b00000000000111111111111111100000};
  int *params = (int *)calloc(1, sizeof(int));
  if (params == NULL) {
    return NULL;
  }
  params[0] = (masks[0] & instruction_base) >> 5;
  return params;
}

int *identify_params_B(uint32_t instruction_base) {
  /*
  Identifies the parameters for the function B

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */ 
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
  /*
  Identifies the parameters for the function BR

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */
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

int *identify_params_Bcond(uint32_t instruction_base) {
  /*
  Identifies the parameters for the function B.cond

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */ 

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
  /*
  Identifies the parameters for the function LSL_LSR_I

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */ 

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
  /*
  Identifies the parameters for the functions: STUR,
  STURB, STURH, LDUR, LDURH and LDURB

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */ 

  uint32_t masks[] = {0b00000000000111111111000000000000,
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
  /*
  Identifies the parameters for the function MOVZ

  params:
    instruction_base: the instruction to identify parameters from

  returns:
    params: every parameter needed to run the instruction properly
  */ 

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

void function_ADDS_ER(int *params) {
  /*
  Adds the registers params[1] and params[0] and saves it in the
  register params[2]

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 + x2;

  update_flags(x0);
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 4;
}

void function_ADDS_I(int *params) {
  /*
  Adds the register params[2] and the immediate params[1] and 
  saves it in the register params[3]. It also checks params[0] == 1
  to see if a shift is needed to be performed on the immediate.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

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
  /*
  Substratcs the registers params[1] and params[0] and saves it in the
  register params[2]. If the destination register params[2] is the register
  31, it runs as a CMP function, therefore, it updates only the flags and
  does not save anything to the registers.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 
  
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 - x2;

  if (params[2] != 31) {
    NEXT_STATE.REGS[params[2]] = x0;
  }
  update_flags(x0);
  NEXT_STATE.PC += 4;
}

void function_SUBS_CMP_I(int *params) {
  /*
  Substratcs the register params[2] and the immediate params[1] and saves it 
  in the register params[3]. If the destination register params[2] is the
  register 31, it runs as a CMP function, therefore, it updates only the flags
  and does not save anything to the registers. It also checks params[0] == 1
  to see if a shift is needed to be performed on the immediate.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int x1 = CURRENT_STATE.REGS[params[2]];
  int imm = params[1];
  if (params[0] == 1) {
    int imm = imm << 12;
  }
  int x0 = x1 - imm;

  if (params[2] != 31) {
    NEXT_STATE.REGS[params[3]] = x0;
  }
  update_flags(x0);
  NEXT_STATE.PC += 4;
}

void function_HLT(int *params) {
  /*
  Sets RUN_BIT to 0, and stops the simulation.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 
  RUN_BIT = 0;
  NEXT_STATE.PC += 4;
}

void function_ANDS_SR(int *params) {
  /*
  Performs a bitwise AND operation between the registers params[1] and
  params[0], and saves it in the register params[2].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  
  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 & x2;
  update_flags(x0);
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 4;
}

void function_EOR_SR(int *params) {
  /*
  Performs a bitwise XOR operation between the registers params[1] and
  params[0], and saves it in the register params[2].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */

  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 ^ x2;
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 4;
}

void function_ORR_SR(int *params) {
  /*
  Performs a bitwise OR operation between the registers params[1] and
  params[0], and saves it in the register params[2].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int x1 = CURRENT_STATE.REGS[params[1]];
  int x2 = CURRENT_STATE.REGS[params[0]];
  int x0 = x1 | x2;
  NEXT_STATE.REGS[params[2]] = x0;
  NEXT_STATE.PC += 4;
}

void function_B(int *params) {
  /*
  Unconditional branch, set program counter to a new adress which depends on
  current program counter and params[0].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 26);
  int offset = (imms) * 4;
  NEXT_STATE.PC += offset;
}

void function_BR(int *params) {
  /*
  Unconditional branch, set program counter to a new adress which does not
  depend on current program counter.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  NEXT_STATE.PC = CURRENT_STATE.REGS[params[0]];
}

void function_BEQ(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Condition: flag z == 1.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 1) {
    NEXT_STATE.PC += (imms) * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BNE(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Condition: flag z == 0.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 0) {
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BGT(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Conditions: flag Z == 0 and flag N ==
  flag V

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_Z == 0 && CURRENT_STATE.FLAG_N == 0) {
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BLT(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Conditions: flag N != flag V.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  if (CURRENT_STATE.FLAG_N == 1) {
    int32_t imms = sign_extend32(params[0], 19);
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BGE(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Conditions: flag N == flag V.

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 19);
  if (CURRENT_STATE.FLAG_N == 0) {
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_BLE(int *params) {
  /*
  Conditional branch, set program counter to a new adress which depends on
  current program counter and params[0]. Conditions: not(flag Z == 0 and flag N
  == flag V).

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int32_t imms = sign_extend32(params[0], 19);
  if (!(CURRENT_STATE.FLAG_N == 0 && CURRENT_STATE.FLAG_Z == 0)) {
    NEXT_STATE.PC += imms * 4;
  } else {
    NEXT_STATE.PC += 4;
  }
}

void function_LSL_LSR_I(int *params) {
  /*
  Makes a logical shift to the value of the register params[2]. The shift can be to the right or left depending on params[1].
  The amount of shifts is defined in params[0] and the result is stored in the register params[3].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
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
  /*
  Saves the value saved in the register params[2] to the memory value stored in
  the register params[1] after aplying an offset (params[0]) to said address.
  Before that, the offset is extended to 64 bits using sign_extend().

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  address = address + offset;

  mem_write_32(address, value);
  NEXT_STATE.PC += 4;
}

void function_STURB(int *params) {
  /*
  Saves the least significant byte value saved in the register params[2] to 
  the memory value stored in the register params[1] after aplying an offset
  (params[0]) to said address. Before that, the offset is extended to 64 bits
  using sign_extend().

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int64_t address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  uint32_t least_sig_byte =
      value & 0xFF;
  address = address + offset;
  uint32_t saved = mem_read_32(address);
  saved &= 0xFFFFFF00;
  saved |= least_sig_byte;
  mem_write_32(address, saved);
  NEXT_STATE.PC += 4;
}

void function_STURH(int *params) {
  /*
  Saves the two least significant byte value saved in the register params[2]
  to the memory value stored in the register params[1] after aplying an offset
  (params[0]) to said address. Before that, the offset is extended to 64 bits
  using sign_extend().

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int64_t address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  uint32_t least_sig_2bytes =
      value & 0xFFFF;
  address = address + offset;
  uint32_t saved = mem_read_32(address);
  saved &= 0xFFFF0000;
  saved |= least_sig_2bytes;

  mem_write_32(address, saved);
  NEXT_STATE.PC += 4;
}

void function_LDUR(int *params) {
  /*
  Saves the value saved in the address params[1] after aplying an offset
  (params[0]) to said address. Before that, the offset is extended to 64 bits using 
  sign_extend(). The value is stored in the register params[2].


  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  address = address + offset;

  uint32_t lower = mem_read_32(address);
  uint32_t upper = mem_read_32(address + 4);
  int64_t combined = ((int64_t)upper << 32) | lower;

  NEXT_STATE.REGS[params[2]] = combined;
  NEXT_STATE.PC += 4;
}

void function_LDURB(int *params) {
  /*
  Saves the least significant byte saved in the address params[1] after 
  aplying an offset (params[0]) to said address. Before that, the offset
  is extended to 64 bits using sign_extend(). The value is stored in the 
  register params[2].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  address = address + offset;

  uint32_t saved = mem_read_32(address);

  int first_byte = saved & 0xFF;

  NEXT_STATE.REGS[params[2]] = first_byte;
  NEXT_STATE.PC += 4;
}

void function_LDURH(int *params) {
  /*
  Saves the two least significant bytes saved in the address params[1] after
  aplying an offset (params[0]) to said address. Before that, the offset is
  extended to 64 bits using sign_extend(). The value is stored in the
  register params[2].


  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */ 

  int value = CURRENT_STATE.REGS[params[2]];
  int address = CURRENT_STATE.REGS[params[1]];
  int64_t offset = sign_extend(params[0], 9);

  if (address == 31)
    return;

  address = address + offset;

  uint32_t saved = mem_read_32(address);

  int first2_bytes = saved & 0xFFFF;

  NEXT_STATE.REGS[params[2]] = first2_bytes;
  NEXT_STATE.PC += 4;
}

void function_MOVZ(int *params) {
  /*
  Saves and immediate (params[0]) value in the register params[1].

  params:
    params: the parameters needed to perform the instruction correctly

  returns:
    None
  */
  int imm = params[0];
  NEXT_STATE.REGS[params[1]] = imm;
  NEXT_STATE.PC += 4;
}

Instruction **build_instructions() {
  /*
  Creates every possible instruction and generates and array with them.
  It links every instruction with it's opcode, function to idenrify it's
  parameters, and a function to execute the instruction.

  params:
    None

  Returns:
    instructions: Array of pointers to Instructions.
  */

  Instruction **instructions = malloc(sizeof(Instruction *) * 27);
  if (instructions == NULL) {
    return NULL;
  }
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
  HLT->opcode = (uint32_t)0b11010100010000000000000000000000;
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
  LSL_LSR_I->opcode = (uint32_t)0b11010011000000000000000000000000;
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
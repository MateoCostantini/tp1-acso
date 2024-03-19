#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "instructions2.c"
#define NUMBER_INSTRUCTIONS 23

/*
typedef struct {
  const int *opcode;
  const int *cond;
  const int *imms;
  int opcode_length;
  void (*instruction_function)(int *);
  int *(*identify_params)(int *);
} Instruction;
*/



// Hacer una struct que haga el decode y se guarde las partes que interpreta
//  Hacer como un diccionario para mapear el opcode con la intruccion y que en (ya esta)
//  el value corra la funcion de la instruccion (ya esta)

/*int* uint32_to_binary_array(uint32_t num) {
    int i;
    int* binary = malloc(32 * sizeof(int));
    if (binary == NULL){
      return NULL;
    }
    for (i = 31; i >= 0; i--) {
        binary[i] = (num >> i) & 1;
    }
  return binary;
}*/



Instruction* identify_instruction(uint32_t instruction_base, Instruction** instructions) {
  uint32_t masks[] = {
      0b11111111111000000000000000000000,
      0b11111111000000000000000000001111,
      0b11111111000000000000000000000000, // entraria BEQ, hacer if != a opcode bcond
      0b11111100000000000000000000000000,
      0b11111111111111111111110000000000,
      0b11111111100000000000000000000000}; // entraria al imms 00000, hacer if
  for (size_t i = 0; i < 6; i++){
      uint32_t opcode_elegible = masks[i] & instruction_base;
      for (size_t j = 0; j < NUMBER_INSTRUCTIONS; j++){
        if (instructions[j]->opcode == opcode_elegible){
          return instructions[j];
        }
      }
  }
}


void execute(uint32_t instruction_base, Instruction* chosen_instruction){
  int* params = chosen_instruction->identify_params(instruction_base); // Esto seria parte del decode en realidad
  chosen_instruction->instruction_function(params);
}


void process_instruction() {
  
  Instruction** instructions_list = build_instructions(); // que se haga solo una vez
  //char* instruction_hex = mem_read_32(CURRENT_STATE.PC);
  uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
  //int* instruction_base = uint32_to_binary_array(instruction_hex);
  //int* instruction_base = hex2bin(instruction_hex);
  Instruction* chosen_instruction = identify_instruction(instruction, instructions_list);
  execute(instruction, chosen_instruction);

  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory.
   *
   * Sugerencia: hagan una funcion para decode()
   *             y otra para execute()
   *
   * */
}
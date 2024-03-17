#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  int *opcode;
  int opcode_length;
  void (*instruction_function)(int*);
  int* (*identify_params) (int*);
} Instruction;

void process_instruction() {
  /* execute one instruction here. You should use CURRENT_STATE and modify
   * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
   * access memory.
   *
   * Sugerencia: hagan una funcion para decode()
   *             y otra para execute()
   *
   * */
}

// Hacer una struct que haga el decode y se guarde las partes que interpreta
//  Hacer como un diccionario para mapear el opcode con la intruccion y que en (ya esta)
//  el value corra la funcion de la instruccion (ya esta)
int *hex2bin(char *instruction) {
  int *binary_instruction = malloc(32 * sizeof(int));
  // if binary_instruction == NULL ...
  for (size_t i = 0; i < 8; i++) {
    char digit_hex = instruction[i];
    int digit_dec;
    if (digit_hex == 'a') {
      digit_dec = 10;
    } else if (digit_hex == 'b') {
      digit_dec = 11;
    } else if (digit_hex == 'c') {
      digit_dec = 12;
    } else if (digit_hex == 'd') {
      digit_dec = 13;
    } else if (digit_hex == 'e') {
      digit_dec = 14;
    } else if (digit_hex == 'f') {
      digit_dec = 15;
    } else {
      digit_dec = (int)digit_hex;
    }
    for (size_t j = 0; j < 4; j++) {
      int bit = digit_dec % 2;
      digit_dec = digit_dec / 2;
      binary_instruction[(3 - j) + i * 4] = bit;
    }
  }
  return binary_instruction;
}


Instruction* identify_instruction(int *instruction_base, Instruction* instructions) {
  Instruction* chosen_instruction;
  const int instructions_quantity = 27; //chequear
  for (size_t i = 0; i < instructions_quantity; i++){
    bool aux = true;
    for (size_t j = 0; j < instructions[i].opcode_length; j++){
      if (instructions[i].opcode[j] != instruction_base[j]){
        aux = false;
        break;
      }
    }
    chosen_instruction = &instructions[i];
    return chosen_instruction;
  }
}


void execute(int* instruction_base, Instruction* chosen_instruction){
  int* params = chosen_instruction->identify_params(instruction_base); // Esto seria parte del decode en realidad
  chosen_instruction->instruction_function(params);
}
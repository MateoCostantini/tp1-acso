#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "instructions.c"
#define NUMBER_INSTRUCTIONS 24



Instruction* identify_instruction(uint32_t instruction_base, Instruction** instructions) {
    /*
    Identify the appropriate instruction based on the opcode and the input instruction.

    Params:
        instruction_base: The instruction recieved from the input.
        instructions: List of available instructions.

    Returns:
        Instruction: The identified instruction.
    */
    
  uint32_t masks[] = {
      0b11111111000000000000000000001111, 
      0b11111111111111111111110000000000,
      0b11111111111000000000000000000000,
      0b11111111100000000000000000000000,
      0b11111111000000000000000000000000, 
      0b11111100000000000000000000000000,
      }; 
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
  /*
    Gets the parameters and excecutes de instruction.

    Params:
        instruction_base: The instruction recieved from the input.
        chosen_instruction: The instruction chosen in the identify_instruction() function.

    Returns:
        None.
  */
  int* params = chosen_instruction->identify_params(instruction_base); 
  chosen_instruction->instruction_function(params);
}


void process_instruction() {
  /*
    Main function, reads the instruction from memory, identifies the appropriate
    instruction, and executes it.

    Params:
        None

    Returns:
        None.
  */
  Instruction** instructions_list = build_instructions(); // que se haga solo una vez
  uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
  Instruction* chosen_instruction = identify_instruction(instruction, instructions_list);
  execute(instruction, chosen_instruction);
}
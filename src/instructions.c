
// agergar al makefile

typedef struct {
  int *opcode;
  int opcode_length;
  int* params;
  void (*instruction_function)(int*);
} Instruction;

// definir cada instruccion con su opcode 
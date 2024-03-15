#include <stdio.h>
#include <assert.h>
#include <string.h>

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. 
     * 
     * Sugerencia: hagan una funcion para decode()
     *             y otra para execute()
     * 
     * */

}


void decode(char* instruction){
    int potencias = {8, 4, 2, 1, 0};
    for (size_t i = 0; i < 8; i++){
        char character = instruction[i];
        int digit;
        if (strcmp(character,'a') == 0){
            digit = 10;
        } else if (strcmp(character,'b') == 0){
            digit = 11;
        } else if (strcmp(character,'c') == 0){
            digit = 12;
        } else if (strcmp(character,'d') == 0){
            digit = 13;
        } else if (strcmp(character,'e') == 0){
            digit = 14;
        } else if (strcmp(character,'f') == 0){
            digit = 15;
        } else{
            digit = (int) character;
        }

        //for (size_t j = 0; )
    }
}
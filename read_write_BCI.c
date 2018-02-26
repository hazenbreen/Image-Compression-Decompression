/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/19/16
 *
 * Purpose: This program contains functions that are capable of reading 
 *          data from the binary compressed image format and writing 
 *          data to binary compressed image format. 
 */

#include "read_write_BCI.h"
#include "bitpack.h"

/*
 * Name: print_word
 *
 * Purpose: prints the word to the Binarcy Compressed Image 
 *          in big-endian order.
 */
void print_word(uint64_t word)
{
        uint64_t byte;

        for (int i = 3; i >= 0 ; i--) {
                byte = Bitpack_getu(word, 8, (i * 8));
                putchar((char)byte);
        }
}

/* 
 * Name: read_word
 *
 * Purpose: reads the word from from the Binary Compressed Image 
 *          in big-endian order.
 */
uint64_t read_word(FILE *input)
{
        uint64_t byte = 0;
        uint64_t word = 0;

        for (int i = 3; i >= 0 ; i--) {
                byte = getc(input);
                word = Bitpack_newu(word, 8, (i * 8), byte); 
        }

        return word;        
}

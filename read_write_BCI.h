#ifndef READ_WRITE_BCI_INCLUDED
#define READ_WRITE_BCI_INCLUDED

/*
 * Name: Hazen Breen | Jordan Stone 
 *
 * Date: 10/19/16
 *
 * Purpose: This is the interface for the program that contains 
 *              functions that read and write to binary compressed image format.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>


void print_word(uint64_t word);

uint64_t read_word(FILE *input);


#endif
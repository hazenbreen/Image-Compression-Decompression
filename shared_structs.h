#ifndef SHARED_STRUCTS_INCLUDED
#define SHARED_STRUCTS_INCLUDED

/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Purpose: This file contains structs that will pass values 
 *          between different source files that need to share the same 
 *          secrets. They will be part of the interface between the 
 *          compress40/decompress40 functions in compress40.c and
 *          the conversion, DCT, and quiantization modules.
 */



#include <stdint.h>
#include "pnm.h"

typedef struct RGB_pixel_block {
        Pnm_rgb pix1;
        Pnm_rgb pix2;
        Pnm_rgb pix3;
        Pnm_rgb pix4;

        unsigned denominator;
} RGB_pixel_block;



typedef struct Quantized_values {
        uint16_t quant_a;
        int8_t quant_b;
        int8_t quant_c;
        int8_t quant_d;

        unsigned quant_Pb;
        unsigned quant_Pr;
} Quantized_values;

#endif
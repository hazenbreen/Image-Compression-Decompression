#ifndef DATA_CONVERSION_INCLUDED
#define DATA_CONVERSION_INCLUDED


/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/15/16
 *
 * Purpose: This is the header file for the functions that perform the 
 *                      math that converts the cololspaces, performs Discrete
 *                      and Inverse Discrete Cosine Transform, and quantizes 
 *                      and dequantizes the values. This source file also 
 *                      writes the compressed image to .ppm format.
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "arith40.h"
#include "pnm.h"
#include "compress40.h"

typedef struct Component_video Component_video;

typedef struct DCT_values DCT_values;

typedef struct Inverse_DCT_values Inverse_DCT_values;



Quantized_values comp_data_conversion(RGB_pixel_block block_pixels);

void decomp_data_conversion(Quantized_values quant_vals, Pnm_ppm dest_image, 
        unsigned col, unsigned row);


/* convert colorspace */
Component_video RGB_to_YPbPr(Pnm_rgb pixel, unsigned denominator);

void YPbPr_to_RGB(Inverse_DCT_values inv_dct_vals, Pnm_ppm dest_image, 
        unsigned col, unsigned row); 

struct Pnm_rgb fill_pixel(Inverse_DCT_values inv_dct_vals, float luma, 
        int denominator);



/* Discrete Cosine Transform */
float calc_avg_chroma(char color, Component_video pix1, Component_video pix2,
        Component_video pix3, Component_video pix4);

DCT_values DCT(Component_video pix1, 
        Component_video pix2, Component_video pix3, Component_video pix4);

Inverse_DCT_values inverse_DCT(DCT_values dct_vals, float avg_pb, 
                float avg_pr);


/* quantize values */
Quantized_values quant_coeffs(DCT_values dct_vals);
int8_t quant_AC(float coeff);

DCT_values dequant_coeffs(Quantized_values quant_vals);
float dequant_AC(int8_t quant_coeff); 

#endif


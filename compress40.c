/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/15/16
 *
 * Purpose: This is the source file for the functions that call the functions 
 *                      that perform the compression and decompression of an
 *                      image.
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "arith40.h"
#include "compress40.h"
#include "read_PPM.h"
#include "data_conversion.h"
#include "bitpack.h"
#include "read_write_BCI.h"
#include "shared_structs.h"



Pnm_ppm initialize_new_image_dimensions(Pnm_ppm dest_image, unsigned width, 
        unsigned height, A2Methods_T methods);

uint64_t pack_word(Quantized_values quant_vals);

Quantized_values unpack_word(uint64_t word);


/*
 * Name: trim_width
 *
 * Purpose: This function trims the width of the inputted .ppm so that it 
 *      has an even number of pixels horizontally.
 */
static inline unsigned trim_width(unsigned width) 
{
        if (width % 2 != 0) {
                return (width - 1); 
        }
        
        return width;
}

/*
 * Name: trim_height
 *
 * Purpose: This function trims the height of the inputted .ppm so that it 
 *      has an even number of pixels vertically.
 */
static inline unsigned trim_height(unsigned height) 
{
        if (height % 2 != 0) {
                return (height - 1); 
        }
        
        return height;
}


/*
 * Name: compress40
 *
 * Purpose: This function iterates through each 2x2 block of pixels in the 
 *      .ppm and calls the functions necessary to compress to Binary 
 *      Compressed Image. 
 */
void compress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        Pnm_ppm source_image = Pnm_ppmread(input, methods);

        RGB_pixel_block block_pixels;
        Quantized_values quant_vals;
        uint64_t word;

        unsigned t_width = trim_width(source_image->width);
        unsigned t_height = trim_height(source_image->height);
       
        printf("COMP40 Compressed image format 2\n%u %u\n", t_width, 
                t_height);

        for (unsigned row = 0; row < t_height; row += 2) {
                for (unsigned col = 0; col < t_width; col += 2) {
                                        
                        block_pixels = get_block_data(source_image, col, row);

                        quant_vals = comp_data_conversion(block_pixels);  

                        word = pack_word(quant_vals);

                        print_word(word);
                }
        }
        Pnm_ppmfree(&source_image);
}


/*
 * Name: decompress40
 *
 * Purpose: This function iterates through the Binary Compressed Image and 
 *      calls the functions necessary to decompress and create a .ppm.
 */
void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u\n",
                &width, &height);
        assert(read == 2);

        unsigned t_width = trim_width(width);
        unsigned t_height = trim_height(height);        

        uint64_t word;
        Quantized_values quant_vals;

        Pnm_ppm dest_image = malloc(sizeof(*dest_image));
        dest_image = initialize_new_image_dimensions(dest_image, t_width, 
                t_height, methods);

        for (unsigned row = 0; row < t_height; row += 2) {
                for (unsigned col = 0; col < t_width; col += 2) {
                        word = 0;

                        word = read_word(input);

                        quant_vals = unpack_word(word);

                        decomp_data_conversion(quant_vals, dest_image, col, row);
                }
        }
        Pnm_ppmwrite(stdout, dest_image);
        Pnm_ppmfree(&dest_image);
}


/*
 * Name: initialize_new_image_dimensions
 *
 * Purpose: This function will create a new Pnm_ppm which will  store
 *      the new image data.
 */
Pnm_ppm initialize_new_image_dimensions(Pnm_ppm dest_image, unsigned width, 
        unsigned height, A2Methods_T methods) 
{
        dest_image->width       = width;
        dest_image->height      = height;
        dest_image->denominator = 255; 

        dest_image->methods = methods;
        dest_image->pixels = dest_image->methods->new(dest_image->width, 
                dest_image->height, sizeof(struct Pnm_rgb));
        

        return dest_image;
}


/*
 * Name: pack_word
 *
 * Purpose: This function takes the quantized values for a block of 
 *      pixels and packs them into single word using the Bitpack interface.
 */
uint64_t pack_word(Quantized_values quant_vals)
{
        uint64_t word = 0;

        word = Bitpack_newu(word, 6, 26,  quant_vals.quant_a);
        word = Bitpack_news(word, 6, 20,  quant_vals.quant_b); 
        word = Bitpack_news(word, 6, 14,  quant_vals.quant_c);
        word = Bitpack_news(word, 6,  8,  quant_vals.quant_d);
        word = Bitpack_newu(word, 4,  4, quant_vals.quant_Pb); 
        word = Bitpack_newu(word, 4,  0, quant_vals.quant_Pr);      

        return word;
}

/*
 * Name: unpack_word
 *
 * Purpose: This function takes a word representing a block of pixels and 
 *      unpacks them into quantized values using the Bitpack interface.
 */
Quantized_values unpack_word(uint64_t word)
{
        Quantized_values quant_vals;

        quant_vals.quant_a  = Bitpack_getu(word, 6, 26);
        quant_vals.quant_b  = Bitpack_gets(word, 6, 20); 
        quant_vals.quant_c  = Bitpack_gets(word, 6, 14);
        quant_vals.quant_d  = Bitpack_gets(word, 6,  8);
        quant_vals.quant_Pb = Bitpack_getu(word, 4,  4); 
        quant_vals.quant_Pr = Bitpack_getu(word, 4,  0);      

        return quant_vals;
}

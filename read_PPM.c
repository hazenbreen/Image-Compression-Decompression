/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/18/16
 *
 * Purpose: This program reads data to ppm format.
 */

#include "read_PPM.h"

/*
 * Name: get_block_data
 *
 * Purpose: This function reads a single block of data from Pnm_ppm
 */
RGB_pixel_block get_block_data(Pnm_ppm source_image, int col, int row)
{

        RGB_pixel_block block_pixels;
        
        block_pixels.pix1 = source_image->methods->at(source_image->pixels, 
                col, row);

        block_pixels.pix2 = source_image->methods->at(source_image->pixels, 
                col + 1, row);

        block_pixels.pix3 = source_image->methods->at(source_image->pixels, 
                col, row + 1);

        block_pixels.pix4 = source_image->methods->at(source_image->pixels, 
                col + 1, row + 1);

        block_pixels.denominator = source_image->denominator;
          

        return block_pixels; 
}

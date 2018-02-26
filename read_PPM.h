#ifndef READ_WRITE_PPM_INCLUDED
#define READ_WRITE_PPM_INCLUDED

/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/18/16
 *
 * Purpose: This is the interface for the source file 
 *          that reads data to ppm format.
 */



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h> 

#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "shared_structs.h"


RGB_pixel_block get_block_data(Pnm_ppm source_image, int col, int row);

#endif

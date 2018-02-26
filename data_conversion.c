/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/15/16
 *
 * Purpose: This is the source file for the functions that perform the 
 *                      math that converts the cololspaces, performs Discrete
 *                      and Inverse Discrete Cosine Transform, and quantizes 
 *                      and dequantizes the values. This source file also 
 *                      writes the compressed image to .ppm format.
 */

#include "shared_structs.h"
#include "read_PPM.h"
#include "data_conversion.h"



struct Component_video {
        float luma;
        float chroma_b;
        float chroma_r;
};

struct DCT_values {
        float a, b, c, d;
};

struct Inverse_DCT_values {
        float y1, y2, y3, y4; 
        float avg_pb, avg_pr;  
};


/*
 * Name: calc_red
 *
 * Purpose: This function calculates the red value to be put into the RGB 
 *      struct from the Y, Pb, and Pr.
 */
static inline float calc_red(float luma, float avg_pb, float avg_pr)
{
        float red_val = (1.0 * luma) + (0.0 * avg_pb) + (1.402 * avg_pr);
        if (red_val > 1.0) {
                red_val = 1.0;
        } else if (red_val < 0.0) {
                red_val = 0.0;
        }

        return red_val;
}

/*
 * Name: calc_green
 *
 * Purpose: This function calculates the green value to be put into the RGB 
 *      struct from the Y, Pb, and Pr.
 */
static inline float calc_green(float luma, float avg_pb, float avg_pr)
{
        float green_val = (1.0 * luma) - (0.344136 * avg_pb) - 
                (0.714136 * avg_pr);
        if (green_val > 1.0) {
                green_val = 1.0;
        } else if (green_val < 0.0) {
                green_val = 0.0;
        }

        return green_val;
}

/*
 * Name: calc_blue
 *
 * Purpose: This function calculates the blue value to be put into the RGB 
 *      struct from the Y, Pb, and Pr.
 */
static inline float calc_blue(float luma, float avg_pb, float avg_pr)
{
        float blue_val = (1.0 * luma) + (1.772 * avg_pb) + (0.0 * avg_pr);
        if (blue_val > 1.0) {
                blue_val = 1.0;
        } else if (blue_val < 0.0) {
                blue_val = 0.0;
        }

        return blue_val;
}

/* 
 * Name: check_luma_range
 *     
 * Purpose: This function will increase the accuracy of the decompression by
 *      bounding the luma to the expected value.
 */
static inline float check_luma_range(float luma)
{
        if (luma > 1.0) {
                luma = 1.0;
        } else if (luma < 0.0){
                luma = 0.0;
        }

        return luma;
}

/* 
 * Name: check_chroma_range
 *     
 * Purpose: THis function will increase the accuracy of the decompression by
 *      bouding the chroma to the expected value.
 */
static inline float check_chroma_range(float chroma)
{
        if (chroma > 0.5) {
                chroma = 0.5;
        } else if (chroma < -0.5){
                chroma = -0.5;
        }

        return chroma;
}



/*
 * Name: comp_data_conversion
 *
 * Purpose: This function calls the functions to perform the colorspace 
 *      conversion, the discrete cosine transformation, and then the 
 *      quantization.
 */
Quantized_values comp_data_conversion(RGB_pixel_block block_pixels)
{
        DCT_values dct_vals;
        Quantized_values quant_vals;
        float avg_pb, avg_pr;

        /* convert colorspace */
        Component_video YPbPr1 = RGB_to_YPbPr(block_pixels.pix1, 
                block_pixels.denominator);
        Component_video YPbPr2 = RGB_to_YPbPr(block_pixels.pix2, 
                block_pixels.denominator);
        Component_video YPbPr3 = RGB_to_YPbPr(block_pixels.pix3, 
                block_pixels.denominator);
        Component_video YPbPr4 = RGB_to_YPbPr(block_pixels.pix4, 
                block_pixels.denominator);

        /* Discrete Cosine Transformation */
        avg_pb = calc_avg_chroma('b', YPbPr1, YPbPr2, 
                YPbPr3, YPbPr4); 
        avg_pr = calc_avg_chroma('r', YPbPr1, YPbPr2, 
                YPbPr3, YPbPr4); 

        dct_vals = DCT(YPbPr1, YPbPr2, YPbPr3, YPbPr4);

        /* quantize values */
        quant_vals = quant_coeffs(dct_vals);

        quant_vals.quant_Pb = Arith40_index_of_chroma(avg_pb);
        quant_vals.quant_Pr = Arith40_index_of_chroma(avg_pr);


        return quant_vals;
}

/*
 * Name: decomp_data_conversion
 *
 * Purpose: This function calls the functions to perform the dequantization, 
 *      the inverse discrete cosine transformation, and then the 
 *      colorspace conversion.
 */
void decomp_data_conversion(Quantized_values quant_vals, Pnm_ppm dest_image, 
        unsigned col, unsigned row)
{
        float avg_pb, avg_pr;
        // RGB_pixel_block block_pixels;
        DCT_values dct_vals;
        Inverse_DCT_values inv_dct_vals;

        /* de-quantize values */
        avg_pb = Arith40_chroma_of_index(quant_vals.quant_Pb);
        avg_pr = Arith40_chroma_of_index(quant_vals.quant_Pr);

        dct_vals = dequant_coeffs(quant_vals);

        /* inverse discrete cosine transformation */
        inv_dct_vals = inverse_DCT(dct_vals, avg_pb, avg_pr);

        /* convert colorspace */
        YPbPr_to_RGB(inv_dct_vals, dest_image, col, row);
}



/**************************************************************************
*************************   CONVERT COLORSPACE ****************************
**************************************************************************/

/*
 * Name: RGB_to_YPbPr
 *
 * Purpose: This function converts from RGB values to YPbPr values.
 */
Component_video RGB_to_YPbPr(Pnm_rgb pixel, unsigned denominator) 
{
        float f_red, f_green, f_blue;
        float denom = (float)denominator;

        f_red = (pixel->red) / (denom);
        f_green = (pixel->green) / (denom);
        f_blue = (pixel->blue) / (denom);

        Component_video YPbPr;

        YPbPr.luma = (0.299 * f_red) + (0.587 * f_green) + 
                (0.114 * f_blue);
        YPbPr.chroma_b = (-0.168736 * f_red) - (0.331264 * f_green) +
                (0.5 * f_blue);
        YPbPr.chroma_r = (0.5 * f_red) - (0.418688 * f_green) - 
                (0.081312 * f_blue);

        return YPbPr;
}

/*
 * Name: YPbPr_to_RGB
 *
 * Purpose: This function converts the colorspace from YPbRp to RGB by calling
 *      functions that perform the math.  The function then uses the method
 *      suite to set the RGB values within the struct.
 */
void YPbPr_to_RGB(Inverse_DCT_values inv_dct_vals, Pnm_ppm dest_image, 
        unsigned col, unsigned row) 
{
        int denominator = 255;


        Pnm_rgb pix1 = dest_image->methods->at(dest_image->pixels, col, row);
        struct Pnm_rgb temp1 = fill_pixel(inv_dct_vals, 
                inv_dct_vals.y1, denominator);
        *(pix1) = temp1;


        Pnm_rgb pix2 = dest_image->methods->at(dest_image->pixels, col + 1,
                row);
        struct Pnm_rgb temp2 = fill_pixel(inv_dct_vals, 
                inv_dct_vals.y2, denominator);
        *(pix2) = temp2;


        Pnm_rgb pix3 = dest_image->methods->at(dest_image->pixels, col, 
                row + 1);
        struct Pnm_rgb temp3 = fill_pixel(inv_dct_vals, 
                inv_dct_vals.y3, denominator);
        *(pix3) = temp3;


        Pnm_rgb pix4 = dest_image->methods->at(dest_image->pixels, col + 1, 
                row + 1);
        struct Pnm_rgb temp4 = fill_pixel(inv_dct_vals, 
                inv_dct_vals.y4, denominator);
        *(pix4) = temp4;
}

/*
 * Name: fill_pixel
 *
 * Purpose: This function gets the red, green, and blue value for a pixel. It
 *      also calls the functions that calculate this value.
 */
struct Pnm_rgb fill_pixel(Inverse_DCT_values inv_dct_vals, float luma, 
        int denominator)
{
        struct Pnm_rgb pixel;

        pixel.red = (denominator) * calc_red(luma,
                        inv_dct_vals.avg_pb, inv_dct_vals.avg_pr);

        pixel.green = (denominator) * calc_green(luma,
                        inv_dct_vals.avg_pb, inv_dct_vals.avg_pr);

        pixel.blue = (denominator) * calc_blue(luma,
                        inv_dct_vals.avg_pb, inv_dct_vals.avg_pr);

        return pixel;
}



/**************************************************************************
********************  Discrete Cosine Transformation  *********************
**************************************************************************/

/*
 * Name: calc_avg_chroma
 *
 * Purpose: This function averages the chroma values passed in based on 
 *      specified chroma value.
 */
float calc_avg_chroma(char color, Component_video pix1, Component_video pix2,
        Component_video pix3, Component_video pix4)
{
        float num_pixs = 4.0; 
        if (color == 'b') {
                return ((pix1.chroma_b + pix2.chroma_b + pix3.chroma_b 
                        + pix4.chroma_b) / num_pixs); 
        } else {                                                  
                return ((pix1.chroma_r + pix2.chroma_r + pix3.chroma_r 
                        + pix4.chroma_r) / num_pixs);     
        }
}

/*
 * Name: DCT
 *
 * Purpose: This function performs the discrete cosine transformation on the
 *      pixels in the block.
 */
DCT_values DCT(Component_video pix1, Component_video pix2,
                Component_video pix3, Component_video pix4)
{
        float num_pixs = 4.0; 
        DCT_values dct_vals;

        dct_vals.a = (pix4.luma + pix3.luma + pix2.luma + pix1.luma) 
                / num_pixs;
        dct_vals.b = (pix4.luma + pix3.luma - pix2.luma - pix1.luma) 
                / num_pixs;
        dct_vals.c = (pix4.luma - pix3.luma + pix2.luma - pix1.luma) 
                / num_pixs;
        dct_vals.d = (pix4.luma - pix3.luma - pix2.luma + pix1.luma) 
                / num_pixs;

        return dct_vals;
}

/*
 * Name: inverse_DCT
 *
 * Purpose: This function trims performs the inverse discrete cosine 
 *      transformation on the DCT values passed in.
 */
Inverse_DCT_values inverse_DCT(DCT_values dct_vals, float avg_pb, 
        float avg_pr)
{
        Inverse_DCT_values inv_dct_vals;

        inv_dct_vals.y1 = dct_vals.a - dct_vals.b - dct_vals.c + dct_vals.d;
        inv_dct_vals.y1 = check_luma_range(inv_dct_vals.y1);

        inv_dct_vals.y2 = dct_vals.a - dct_vals.b + dct_vals.c - dct_vals.d;
        inv_dct_vals.y2 = check_luma_range(inv_dct_vals.y2);
        

        inv_dct_vals.y3 = dct_vals.a + dct_vals.b - dct_vals.c - dct_vals.d;
        inv_dct_vals.y3 = check_luma_range(inv_dct_vals.y3);

        inv_dct_vals.y4 = dct_vals.a + dct_vals.b + dct_vals.c + dct_vals.d;
        inv_dct_vals.y4 = check_luma_range(inv_dct_vals.y4);


        inv_dct_vals.avg_pb = avg_pb;
        inv_dct_vals.avg_pb = check_chroma_range(inv_dct_vals.avg_pb);

        inv_dct_vals.avg_pr = avg_pr;
        inv_dct_vals.avg_pr = check_chroma_range(inv_dct_vals.avg_pr);


        return inv_dct_vals;
}



/**************************************************************************
*************************   Quantize     **********************************
**************************************************************************/
/*
 * Name: quant_coeffs
 *
 * Purpose: This function will take the DCT coefficients and will convert
 *       them to quantized values.
 */
Quantized_values quant_coeffs(DCT_values dct_vals)
{
        float dc_range = 63.0;
        Quantized_values quant_vals;
        
        quant_vals.quant_a = ((uint16_t) roundf(dct_vals.a * dc_range));

        quant_vals.quant_b = quant_AC(dct_vals.b);
        quant_vals.quant_c = quant_AC(dct_vals.c);
        quant_vals.quant_d = quant_AC(dct_vals.d);

        return quant_vals;
}

/* 
 * Name: quant_AC
 *
 * Purpose: This function will take a floating point number and convert 
 *          it to a 5-bit signed integer. It will do this by converting the 
 *          input value to a number between -15 and 15.
 */
int8_t quant_AC(float coeff) 
{
        float ac_range = 31.0;
        float ac_max = 0.3;
        float ac_min = -0.3;
        float denom = (ac_max)/(ac_range);

        if (coeff < ac_min) {
                coeff = ac_min;
        } else if (coeff > ac_max) {
                coeff = ac_max;
        }

        return ((int8_t) roundf(coeff / denom));
}

/* 
 * Name: dequant_coeffs
 *
 * Purpose: This function will take the quantized values and will convert
 *       them to DCT coefficients.
 */
DCT_values dequant_coeffs(Quantized_values quant_vals)
{
        DCT_values dct_vals;
        float dc_range = 63.0;
        
        dct_vals.a = (float)(quant_vals.quant_a / dc_range);

        dct_vals.b = dequant_AC(quant_vals.quant_b);
        dct_vals.c = dequant_AC(quant_vals.quant_c);
        dct_vals.d = dequant_AC(quant_vals.quant_d);

        return dct_vals;
}

/* 
 * Name: dequant_AC
 *
 * Purpose: This function will take a 5-bit signed integer and convert 
 *          it to a floating point number. It will do this by converting the 
 *          input value to a number between -0.3 and 0.3 .
 */
float dequant_AC(int8_t quant_coeff) 
{
        float ac_range = 31.0;
        float ac_max = 0.3;
        float scale = (ac_max)/(ac_range);

        return ((float) (quant_coeff * scale));
}

README
Image Compression and Decompression
 
By: Hazen Breen and Jordan Stone
 
Date: 10/21/16


Architecture
---------------------------------------------------------------------------
We broke the this problem into functional modules. With this problem, 
our solution would have to perform both image compression and decompression. 
While image compression and decompression are related, image decompression 
utilizes the exact reciprocal functions of image compression. 

Therefore, in our architecture we decided to create each functional module 
so that it would contain the pertinent functions for both compression 
and decompression. 

For example the quantization module, that is located within 
data_conversion.c, takes care of both the quantization of values and 
the de-quantization of values. These functions share the same secrets. 
Therefore by keeping them in the same module we save memory by not passing 
around necessary variables. 

Below is a breakdown of the source files used for our solution.


Source Files
---------------------------------------------------------------------------
40image.c:
__________
-This source file reads commandline inputs and determines either 
 decompression or compression.


compress40.c:
_____________
-This sourcefile contains the main mapping mechanisms for both the 
 compression solution and the decompression solution.

-It also trims the image dimensions if necessary.

-Delegates the tasks to appropriate modules. 


read_PPM.c:
________________
-This source file is responsible for reading from a ppm image format. 

-This file does not contain the reciprocal function of the module for 
 writing to a ppm due to architectural limitations that were not foreseen 
 (see data_conversion.c). Ideally, we would plan to fix this issue by 
 determining a method for passing the RGB values of the destination 
 image throughout the decompression process in a way that is more logical. 



data_conversion.c:
__________________
-This sourcefile contains three modules, these modules were designed to be 
 grouped together in order to optimize the number of shared secrets. Since all 
 of these modules deal with simple mathematical calculations on a set of 
 shared variables, it is more modular to keep them within the same source 
 file. However, each module has individual functions that are separate 
 from other modules in the source file.  

-Responsible for the colorspace conversion module which will convert 
 between the RGB and Component Video colorspaces.

-Responsible for the discrete cosine transform module which will perform 
 both DCT(compression) and inverse-DCT(decompression).

-Responsible for the quantization module which will quantize the 
 values(compression) and de-quanitze the values(decompression).

-This source file also is responsible for writing the decompressed image 
 to .ppm format. 


bitpack.c:
__________
-This sourcefile contains functions necessary for packing values into a 
 64-bit word and extracting values from a 64-bit word.


read_write_BCI.c:
_________________
-This source file contains a single module that is responsible for 
 reading binary compressed images and writing data to binary compressed 
 image format.



Time Spent
---------------------------------------------------------------------------
 We spent approximately 25 hours analyzing the problem posed in 
    the assignment.


 We spent approximately 38 hours solving the problems after your analysis.



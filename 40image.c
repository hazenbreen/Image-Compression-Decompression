/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/15/16
 *
 * Purpose: This program is an image compressor and decomspressor. 
 *          It takes command line arguments to determine whether the 
 *          program will act as a compressor or a decompressor. If 
 *          the user does not specify, the default setting is the 
 *          compression setting.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "pnm.h"

#include "compress40.h"
#include "read_PPM.h"

#include "a2methods.h"
#include "a2plain.h"

static void (*compress_or_decompress)(FILE *input) 
        = compress40;

int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }

        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }
}

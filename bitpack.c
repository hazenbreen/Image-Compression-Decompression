/*
 * Name: Hazen Breen and Jordan Stone 
 *
 * Date: 10/11/16
 *
 * Purpose: This source file contains functions that extract values from
 *          64-bit words, both signed values and unsigned values. Also,
 *          this file has functions which can create new 64-bit words. 
 *          The purpose of bit packing is to conveniently package data. 
 */


#include <stdbool.h>
#include <stdint.h>
#include "bitpack.h"
#include <assert.h>
#include "except.h"

#define MAX_WIDTH 64
#define MASK -1

Except_T Bitpack_Overflow = { "Overflow packing bits" };


/*
 * Name: shift_left
 *
 * Purpose: This function will perform a left bit shift on the word. It will
 *          shift the word left by shift.
 */
static inline uint64_t shift_left(uint64_t word, uint8_t shift)
{
        if (shift >= MAX_WIDTH) {
                word = 0;
        } else {
                word <<= shift;
        }

        return word;
}

/*
 * Name: logical_right
 *
 * Purpose: This function will perform a logical right bit shift on the 
 *          word. It will shift the word right by shift.
 */
static inline uint64_t logical_right(uint64_t word, uint8_t shift)
{
        if (shift >= MAX_WIDTH) {
                word = 0;
        } else {
                word >>= shift;
        }

        return word;
}

/*
 * Name: arithmetic_right
 *
 * Purpose: This function will perform an arithmetic right bit shift on the 
 *          word. It will shift the word right by shift.
 */
static inline uint64_t arithmetic_right(uint64_t word, uint8_t shift)
{
        if (shift >= MAX_WIDTH) {
                word = 0;
        } else {
                int64_t temp_word = word;
                temp_word >>=  shift;
                word = temp_word;
        }

        return word;
}

/*
 * Name: Bitpack_fitsu
 *
 * Purpose: This function will determine if the unsigned number n can be 
 *          represented with width number of bits.
 *
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);

        if (width == 0) {
                return false; 
        }

        uint64_t original_n = n;

        n = shift_left(n, (MAX_WIDTH - width) );
        n = logical_right(n, (MAX_WIDTH - width) );

        if (original_n != n) {
                return false;
        } else {
                return true;
        } 
}

/*
 * Name: Bitpack_fitss
 *
 * Purpose: This function will determine if the signed number n can be 
 *          represented with width number of bits.
 */
bool Bitpack_fitss( int64_t n, unsigned width)
{
        assert(width <= 64);

        if (width == 0) {
                return false;
        } else if (n < 0) {
                n = n * (-1);
        }

        uint64_t new_n = n;

        return Bitpack_fitsu(new_n, width - 1) == true;
}

/*
 * Name: Bitpack_getu
 *
 * Purpose: This function extract values from a word. The values are
 *          extracted by first creating a mask for the value desired that
 *          starts at lsb and is width wide. Then the (word & mask) is
 *          right shifted by lsb to return the value.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);

        uint64_t mask = MASK;

        mask = shift_left(mask, (MAX_WIDTH - width) );
        mask = logical_right(mask, 
                ((MAX_WIDTH - width) - lsb) );

        return logical_right((word & mask), lsb);
}

/*
 * Name: Bitpack_gets
 *
 * Purpose: This function extract signed values from a word. The values are
 *          extracted by first creating a mask for the value desired that
 *          starts at lsb and is width wide. Then the (word & mask) is
 *          right shifted by lsb to return the value.
 */
int64_t  Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        
        uint64_t mask = MASK;
        
        mask = shift_left(mask, (MAX_WIDTH - width) );
        mask = logical_right(mask, (MAX_WIDTH - width - lsb) );

        int64_t mod_word = (word & mask);

        mod_word = shift_left(mod_word, 
                (MAX_WIDTH - width - lsb) );
        mod_word = arithmetic_right(mod_word, 
                (MAX_WIDTH - width) );

        return mod_word;
}

/*
 * Name: Bitpack_newu
 *
 * Purpose: This function returns a new word which is identical to the 
 *          original word, except that the field that is width many bits
 *          with least significant bit at lsb will have been replaced by the 
 *          unsigned value.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
        uint64_t value)
{
        bool can_fit = Bitpack_fitsu(value, width);
        if ((width > MAX_WIDTH) || (width + lsb) > MAX_WIDTH || (!can_fit)) {
                RAISE(Bitpack_Overflow);
        }

        value = shift_left(value, lsb);

        return (word | value);
}

/*
 * Name: Bitpack_news
 *
 * Purpose: This function returns a new word which is identical to the 
 *          original word, except that the field that is width many bits
 *          with least significant bit at lsb will have been replaced by the 
 *          signed value.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
        int64_t value)
{
        bool can_fit = Bitpack_fitss(value, width);
        if ((width > MAX_WIDTH) || (width + lsb) > MAX_WIDTH || (!can_fit)) {
                RAISE(Bitpack_Overflow);
        }

        value = shift_left(value, 
                (MAX_WIDTH - width) );
        value = logical_right(value, 
                ((MAX_WIDTH - width) - lsb) );

        return (word | value);
}

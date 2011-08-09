/*
 * -------------------------------------------------------------------------
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
 *
 * ANIMAL - AN IMAging Library
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * LICENSE NOTE: THIS FILE CONTAINS CODE DERIVED FROM THE GANDALF LIBRARY
 *
 * Modifications started in 27 may 2004 from Gandalf v. 1.3.2 found at
 * http://gandalf-library.sf.net
 *
 * All original Gandalf code was (c) 2000 Imagineer Software Limited and 
 *                                        Industrial Research Limited
 *
 * The authors of the original, non-modified code were:
 * Phil McLauchlan, Allan Jaenicke, and Phillip Ngan
 *
 * Those authors are NOT responsible for the changes found herein.
 * The Animal authors assume their own animal incompetence for what
 * worsened, and thank the original authors for inspiring the rest.
 * -------------------------------------------------------------------------
 */ 





#ifndef ANIMAL_BIT_ARRAY_H
#define ANIMAL_BIT_ARRAY_H 1

#include <stdio.h>
#include "../common.h"
#include "misc_defs.h"
#include "allocate.h"
#include "memory_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonArray
 * \{
 */

#ifdef ANIMAL_UI64_MAX

/// 64-bit word
typedef animal_ui64 Animal_BitWord;
#define ANIMAL_BITWORD_SIZE 64
#define ANIMAL_BITWORD_FULL (animal_ui64) ANIMAL_UI64_MAX
#define ANIMAL_MSB_SET (animal_ui64) 0x8000000000000000 /*(1<<63)*/
#define ANIMAL_LSB_SET (animal_ui64) 1

#else

/// 32-bit word
typedef animal_ui32 Animal_BitWord;
#define ANIMAL_BITWORD_SIZE 32
#define ANIMAL_BITWORD_FULL (animal_ui32) ANIMAL_UI32_MAX
#define ANIMAL_MSB_SET (animal_ui32) (1<<31)
#define ANIMAL_LSB_SET (animal_ui32) 1

#endif

/**
 * \brief Alignment options when computing bounds of a bit array.
 */
typedef enum {ANIMAL_WORD_ALIGNMENT, ANIMAL_BYTE_ALIGNMENT, ANIMAL_BIT_ALIGNMENT}
   Animal_Alignment;

/// 1-dimensional array of bits
typedef struct Animal_BitArray
{
   Animal_BitWord *data;
   unsigned int no_bits;
   unsigned int no_words;

   /* allocated number of words */
   unsigned int words_alloc;

   /* whether the data array was dynamically allocated */
   Animal_Bool data_alloc;

   /* memory stack pointer or NULL */
   Animal_MemoryStack *memory_stack;

   /* whether this structure was dynamically allocated */
   Animal_Bool alloc;
} Animal_BitArray;

AnimalExport Animal_BitArray
   *animal_bit_array_form_data ( Animal_BitArray *ba,
                                 Animal_BitWord *data, unsigned data_words,
                                 unsigned int no_bits ),
   *animal_bit_array_ms_form ( Animal_MemoryStack *ms, Animal_BitArray *ba,
                               unsigned int no_bits );

AnimalExport Animal_Bool 
   animal_bit_array_set_size ( Animal_BitArray *ba, unsigned int no_bits );

AnimalExport void     
   animal_bit_array_free    ( Animal_BitArray *ba ),
   animal_bit_array_free_va ( Animal_BitArray *ba, ... );

/* Logic Functions */

AnimalExport Animal_BitArray 
   *animal_bit_array_invert_s ( Animal_BitArray *ba ),
   *animal_bit_array_and_s    ( Animal_BitArray *ba1, Animal_BitArray *ba2 ),
   *animal_bit_array_nand_s   ( Animal_BitArray *ba1, Animal_BitArray *ba2 ),
   *animal_bit_array_or_s     ( Animal_BitArray *ba1, Animal_BitArray *ba2 ),
   *animal_bit_array_eor_s    ( Animal_BitArray *ba1, Animal_BitArray *ba2 ),
   *animal_bit_array_andnot_s ( Animal_BitArray *ba1, Animal_BitArray *ba2 );


AnimalExport Animal_Bool
   animal_bit_array_invert_i ( Animal_BitArray *ba ),
   animal_bit_array_and_i    ( Animal_BitArray *ba_dst, Animal_BitArray *ba ),
   animal_bit_array_nand_i   ( Animal_BitArray *ba_dst, Animal_BitArray *ba ),
   animal_bit_array_or_i     ( Animal_BitArray *ba_dst, Animal_BitArray *ba ),
   animal_bit_array_eor_i    ( Animal_BitArray *ba_dst, Animal_BitArray *ba ),
   animal_bit_array_andnot_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba ),
/* insert part of src bit array into dst bit array */
   animal_bit_array_insert ( Animal_BitArray *source, unsigned int offset_s,
                             Animal_BitArray *dest,   unsigned int offset_d,
                             unsigned int no_bits ),
/* set all bits in a bit array */
   animal_bit_array_fill ( Animal_BitArray *ba, Animal_Bool val ),
/* copy one bit array to another */
   animal_bit_array_copy_q ( Animal_BitArray *ba_source,
                                     Animal_BitArray *ba_dest );

AnimalExport Animal_BitArray *
   animal_bit_array_copy_s ( Animal_BitArray *ba_source );

AnimalExport Animal_Bool      
   animal_bit_array_expand_q ( Animal_BitArray *ba,
                               Animal_BitArray *ref_ba,
                               Animal_BitArray *exp_ba );

AnimalExport Animal_BitArray *
   animal_bit_array_expand_s ( Animal_BitArray *ba,
                               Animal_BitArray *ref_ba );

/* fill part of a bit array */
AnimalExport Animal_Bool 
   animal_bit_array_fill_part ( Animal_BitArray *ba, unsigned int offset,
                                unsigned int no_bits, Animal_Bool val );

/* invert part of a bit array */
AnimalExport Animal_Bool 
   animal_bit_array_invert_part ( Animal_BitArray *ba, unsigned int offset,
                                  unsigned int no_bits );

/* print bit array in ASCII to file */
AnimalExport void 
   animal_bit_array_fprint ( FILE *fp, Animal_BitArray *ba, int indent );

/**
 * \brief Macro: Number of bit-words given number of bits.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
unsigned ANIMAL_NO_BITWORDS ( unsigned no_bits );
#else
#define ANIMAL_NO_BITWORDS(nb) ((nb+ANIMAL_BITWORD_SIZE-1)/ANIMAL_BITWORD_SIZE)
#endif

/**
 * \brief Macro: Form bit array.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_BitArray *
 animal_bit_array_form ( Animal_BitArray *ba, unsigned int no_bits );
#else
#define animal_bit_array_form(ba,nb) animal_bit_array_form_data(ba,NULL,0,nb)
#endif

/**
 * \brief Macro: Allocate new bit array.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_BitArray *animal_bit_array_alloc ( unsigned int no_bits );
#else
#define animal_bit_array_alloc(nb) animal_bit_array_form_data(NULL,NULL,0,nb)
#endif

/**
 * \brief Macro: Allocate new bit array using stack-style memory allocation.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_BitArray *animal_bit_array_ms_malloc ( unsigned int no_bits );
#else
#define animal_bit_array_ms_malloc(nb) animal_bit_array_ms_form(NULL,nb)
#endif

/**
 * \brief Macro: Print bit array in ASCII to standard output.
 *
 * Print bit array in ASCII to standard output. Implemented as a macro call
 * to animal_bit_array_fprint().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool animal_bit_array_print ( Animal_BitArray *bit_array, int indent );
#else
#define animal_bit_array_print(ba,i) animal_bit_array_fprint(stdout,ba,i)
#endif

/**
 * \brief Macro: Set bit in bit list to 1 (true).
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool animal_bit_array_set_bit ( Animal_BitArray *bit_array, int pos );
#else
#ifdef NDEBUG
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_set_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] |= ( ANIMAL_MSB_SET >>( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE)
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_set_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] |= ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE)
#endif /* #ifdef WORDS_BIGENDIAN */
#else /* #ifndef NDEBUG */
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_set_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_set_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] |= ( ANIMAL_MSB_SET >>( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE))
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_set_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_set_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] |= ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE))
#endif /* #ifdef WORDS_BIGENDIAN */
#endif /* #ifdef NDEBUG */
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: get bit of bit array.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool animal_bit_array_get_bit ( Animal_BitArray *bit_array, int pos );
#else
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_get_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] & ( ANIMAL_MSB_SET >> ( (p) % ANIMAL_BITWORD_SIZE)))
#else
#define animal_bit_array_get_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] & ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE)))
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: clear bit in bit list to 0 (false).
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool animal_bit_array_clear_bit ( Animal_BitArray *bit_array, int pos );
#else
#ifdef NDEBUG
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_clear_bit(ba,p) \
    ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] &= (ANIMAL_BITWORD_FULL ^ ( ANIMAL_MSB_SET >> ( (p) % ANIMAL_BITWORD_SIZE))),ANIMAL_TRUE)
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_clear_bit(ba,p) \
    ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] &= (ANIMAL_BITWORD_FULL ^ ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE))),ANIMAL_TRUE)
#endif /* #ifdef WORDS_BIGENDIAN */
#else /* #ifndef NDEBUG */
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_clear_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_clear_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] &= (ANIMAL_BITWORD_FULL ^ ( ANIMAL_MSB_SET >> ( (p) % ANIMAL_BITWORD_SIZE))),ANIMAL_TRUE))
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_clear_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_clear_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] &= (ANIMAL_BITWORD_FULL ^ ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE))),ANIMAL_TRUE))
#endif /* #ifdef WORDS_BIGENDIAN */
#endif /* #ifdef NDEBUG */
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \brief Macro: set/clear bit depending on Boolean argument.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool
 animal_bit_array_twiddle_bit ( Animal_BitArray *bit_array, int pos, Animal_Bool val );
#else
#ifdef NDEBUG
#define animal_bit_array_twiddle_bit(ba,p,val) \
    (((val) ? animal_bit_array_set_bit(ba,p) : animal_bit_array_clear_bit(ba,p)),\
     ANIMAL_TRUE)
#else
#define animal_bit_array_twiddle_bit(ba,p,val) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_twiddle_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      (((val) ? animal_bit_array_set_bit(ba,p) : animal_bit_array_clear_bit(ba,p)),\
       ANIMAL_TRUE))
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */


/**
 * \brief Macro: Invert bit.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_Bool
 animal_bit_array_invert_bit ( Animal_BitArray *bit_array, int pos );
#else
#ifdef NDEBUG
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_invert_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] ^= ( ANIMAL_MSB_SET >>( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE)
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_invert_bit(ba,p) \
            ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] ^= ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE)
#endif /* #ifdef WORDS_BIGENDIAN */
#else /* #ifndef NDEBUG */
#ifdef WORDS_BIGENDIAN
#define animal_bit_array_invert_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_invert_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] ^= ( ANIMAL_MSB_SET >>( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE))
#else /* #ifndef WORDS_BIGENDIAN */
#define animal_bit_array_invert_bit(ba,p) \
     ((p)>=(ba)->no_bits \
      ? (animal_err_flush_trace(),\
         animal_err_register("animal_bit_array_invert_bit",\
                          ANIMAL_ERROR_TOO_LARGE,""),\
         ANIMAL_FALSE) :\
      ((ba)->data[(p)/ANIMAL_BITWORD_SIZE] ^= ( ANIMAL_LSB_SET << ( (p) % ANIMAL_BITWORD_SIZE)),ANIMAL_TRUE))
#endif /* #ifdef WORDS_BIGENDIAN */
#endif /* #ifdef NDEBUG */
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ANIMAL_BIT_ARRAY_H */

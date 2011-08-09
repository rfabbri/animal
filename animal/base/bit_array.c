/*
 * -------------------------------------------------------------------------
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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





#include "bit_array.h"
#include "allocate.h"
#include "misc_error.h"
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonArray
 * \{
 */

/**
 * \brief Form new bit array using pre-allocated data array.
 * \param ba Existing bit array or \c NULL
 * \param data Block of memory to use for bit array
 * \param data_words Number of words in \a data array
 * \param no_bits Number of bits in bit array
 * \return Formed bit array, or \c NULL on error.
 *
 * Form new bit array of size \a no_bits using pre-allocated data array
 * \a data. If \a data is not \c NULL, the number of \a data_words must be
 * enough to store \a no_bits bits.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_form_data ( Animal_BitArray *ba,
                           Animal_BitWord *data, unsigned data_words,
                           unsigned int no_bits )
{
   if ( ba == NULL )
   {
      ba = animal_malloc_object ( struct Animal_BitArray );
      if ( ba == NULL )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_bit_array_form_data", ANIMAL_ERROR_MALLOC_FAILED,
                            "" );
         return NULL;
      }
      
      ba->alloc = ANIMAL_TRUE;
   }
   else
      ba->alloc = ANIMAL_FALSE;

   ba->no_bits = no_bits;
   ba->no_words = ANIMAL_NO_BITWORDS(no_bits);
   if ( data == NULL )
   {
      if ( ba->no_words == 0 )
      {
         ba->data = NULL;
         ba->data_alloc = ANIMAL_FALSE;
      }
      else
      {
         ba->data = animal_malloc_array ( Animal_BitWord, ba->no_words );
         if ( ba->data == NULL )
         {
            animal_err_flush_trace();
            animal_err_register ( "animal_bit_array_form_data",
                               ANIMAL_ERROR_MALLOC_FAILED, "" );
            return NULL;
         }

         ba->data_alloc = ANIMAL_TRUE;
      }

      ba->words_alloc = ba->no_words;
   }
   else
   {
      if ( data_words < ba->no_words )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_bit_array_form_data", ANIMAL_ERROR_INCOMPATIBLE,
                            "" );
         return NULL;
      }

      ba->data = data;
      ba->words_alloc = data_words;
      ba->data_alloc = ANIMAL_FALSE;
   }

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* no memory stack for this array */
   ba->memory_stack = NULL;

   return ba;
}

/**
 * \brief Form new bit array using stack memory style memory allocation
 * \param ms Pointer to memory stack structure
 * \param ba Existing bit array or \c NULL
 * \param no_bits Number of bits in bit array
 * \return Formed bit array, or \c NULL on error.
 *
 * Form new bit array of size \a no_bits using stack-style memory allocation.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_ms_form ( Animal_MemoryStack *ms, Animal_BitArray *ba,
                         unsigned int no_bits )
{
   if ( ba == NULL )
   {
      ba = animal_ms_malloc_object ( ms, struct Animal_BitArray );
      if ( ba == NULL )
      {
         animal_err_register ( "animal_bit_array_tform", ANIMAL_ERROR_FAILURE, "" );
         return NULL;
      }
      
      ba->alloc = ANIMAL_TRUE;
   }
   else
      ba->alloc = ANIMAL_FALSE;

   ba->no_bits = no_bits;
   ba->no_words = ANIMAL_NO_BITWORDS(no_bits);
   ba->data = animal_ms_malloc_array ( ms, Animal_BitWord, ba->no_words );
   if ( ba->data == NULL )
   {
      animal_err_register ( "animal_bit_array_tform", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }

   ba->words_alloc = ba->no_words;
   ba->data_alloc = ANIMAL_TRUE;

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* set memory stack pointer to freeing this structure */
   ba->memory_stack = ms;

   return ba;
}

/**
 * \brief Reset size of existing bit array.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Reset size of existing bit array \a ba to \a no_bits.
 */
AnimalExport Animal_Bool
 animal_bit_array_set_size ( Animal_BitArray *ba, unsigned int no_bits )
{
   ba->no_words = ANIMAL_NO_BITWORDS(no_bits);
   if ( ba->no_words > ba->words_alloc )
   {
      if ( !ba->data_alloc || ba->memory_stack != NULL )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_bit_array_set_size",
                            ANIMAL_ERROR_CANT_REALLOC, "" );
         return ANIMAL_FALSE;
      }
         
      ba->data = animal_realloc_array ( Animal_BitWord, ba->data, ba->no_words );
      if ( ba->data == NULL )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_bit_array_set_size",
                            ANIMAL_ERROR_MALLOC_FAILED, "" );
         return ANIMAL_FALSE;
      }

      ba->words_alloc = ba->no_words;
   }

   ba->no_bits = no_bits;

   /* set last word to zero to fill in unused bits */
   if ( ba->no_words > 0 ) ba->data[ba->no_words-1] = 0;

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Free bit array.
 */
AnimalExport void
 animal_bit_array_free ( Animal_BitArray *ba )
{
   if ( ba->memory_stack == NULL )
   {
      /* use normal free function */
      if ( ba->data_alloc ) free ( ba->data );
      if ( ba->alloc      ) free ( ba );
   }
   else
   {
      /* use memory stack free function */
      if ( ba->data_alloc ) animal_ms_free ( ba->memory_stack, ba->data );
      if ( ba->alloc      ) animal_ms_free ( ba->memory_stack, ba );
   }
}

/**
 * \brief Free \c NULL terminated list of bit arrays.
 * \return No value.
 *
 * Free list of bit arrays starting with \a ba and terminated with \c NULL.
 */
AnimalExport void
 animal_bit_array_free_va ( Animal_BitArray *ba, ... )
{
   va_list ap;

   va_start ( ap, ba );
   while ( ba != NULL )
   {
      /* free next matrix */
      animal_bit_array_free ( ba );

      /* get next matrix in list */
      ba = va_arg ( ap, Animal_BitArray * );
   }

   va_end ( ap );
}

/**
 * \brief Set all bits in a bit array.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Set all bits in bit array \a ba to the given boolean value \a val.
 */
AnimalExport Animal_Bool
 animal_bit_array_fill ( Animal_BitArray *ba , Animal_Bool val )
{
   unsigned int ctr = ba->no_words;
   Animal_BitWord valword;

   valword = val ? ANIMAL_BITWORD_FULL : 0;

   while(ctr-- != 0)
      ba->data[ctr] = valword;

   return ANIMAL_TRUE;
}

/**
 * \brief Invert all the bits in a bit array.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Invert all the bits in a bit array \a ba.
 */
AnimalExport Animal_Bool
 animal_bit_array_invert_i ( Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   while(ctr-- != 0)
      ba->data[ctr] = ba->data[ctr] ^ ANIMAL_BITWORD_FULL;

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Invert all the bits in a bit array.
 * \return A new inverted bit array.
 *
 * Invert all the bits in a bit array \a ba, returning the result as anew bit
 * array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_invert_s ( Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;
   Animal_BitArray *ba_dst;

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_invert_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] ^ ANIMAL_BITWORD_FULL;

   /* success */
   return ba_dst;
}

/**
 * \brief Perform binary AND operation between two bit arrays.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Perform binary AND operation between bit arrays \a ba and \a ba_dst,
 * writing the result into \a ba_dst.
 */
AnimalExport Animal_Bool
 animal_bit_array_and_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_and_i", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return ANIMAL_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] & ba_dst->data[ctr];

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Perform binary NAND (not and) operation between bit arrays.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Perform binary NAND (not and) operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
AnimalExport Animal_Bool
 animal_bit_array_nand_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_nand_i", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return ANIMAL_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = (ba->data[ctr] & ba_dst->data[ctr])
                          ^ ANIMAL_BITWORD_FULL;

   /* success */
   return ANIMAL_TRUE;
}

/**
 * Perform binary OR operation between bit arrays.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Perform binary OR operation between bit arrays \a ba and \a ba_dst,
 * writing the result into \a ba_dst.
 */
AnimalExport Animal_Bool
 animal_bit_array_or_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_or_i", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return ANIMAL_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] | ba_dst->data[ctr];

   /* success */
   return ANIMAL_TRUE;
}

/**
 * Perform binary EOR (exclusive or) operation between bit arrays.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Perform binary EOR (exclusive or) operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
AnimalExport Animal_Bool
 animal_bit_array_eor_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_eor_i", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return ANIMAL_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba->data[ctr] ^ ba_dst->data[ctr];

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Perform binary AND-NOT operation between bit arrays.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Perform binary AND-NOT operation between bit arrays \a ba and
 * \a ba_dst, writing the result into \a ba_dst.
 */
AnimalExport Animal_Bool
 animal_bit_array_andnot_i ( Animal_BitArray *ba_dst, Animal_BitArray *ba )
{
   unsigned int ctr = ba->no_words;

   /* check that the bit arrays are the same size */
   if ( ba_dst->no_bits != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_andnot_i", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return ANIMAL_FALSE;
   }

   while(ctr-- != 0)
      ba_dst->data[ctr] = ba_dst->data[ctr] & (ba->data[ctr] ^ ANIMAL_BITWORD_FULL);

   /* success */
   return ANIMAL_TRUE;
}

/**
 * Perform binary AND operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary AND operation between bit arrays \a ba1 and \a ba2,
 * returning the result as a new bit array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_and_s  ( Animal_BitArray *ba1, Animal_BitArray *ba2 )
{
   Animal_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_and_s", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_and_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] & ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary NAND (not and) operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary NAND (not and) operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_nand_s ( Animal_BitArray *ba1, Animal_BitArray *ba2 )
{
   Animal_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_nand_s", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_nand_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = (ba1->data[ctr] & ba2->data[ctr])
                          ^ ANIMAL_BITWORD_FULL;

   /* success */
   return ba_dst;
}

/**
 * Perform binary OR operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary OR operation between bit arrays \a ba1 and \a ba2,
 * returning the result as a new bit array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_or_s   ( Animal_BitArray *ba1, Animal_BitArray *ba2 )
{
   Animal_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_or_s", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_or_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] | ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary EOR (exclusive or) operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary EOR (exclusive or) operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_eor_s  ( Animal_BitArray *ba1, Animal_BitArray *ba2 )
{
   Animal_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_eor_s", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_eor_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] ^ ba2->data[ctr];

   /* success */
   return ba_dst;
}

/**
 * Perform binary AND-NOT operation between bit arrays.
 * \return The result as a new bit array.
 *
 * Perform binary AND-NOT operation between bit arrays \a ba1 and
 * \a ba2, returning the result as a new bit array.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_andnot_s ( Animal_BitArray *ba1, Animal_BitArray *ba2 )
{
   Animal_BitArray *ba_dst;
   unsigned int ctr = ba1->no_words;

   /* check that the bit arrays are the same size */
   if ( ba1->no_bits != ba2->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_andnot_s", ANIMAL_ERROR_INCOMPATIBLE, "" );
      return NULL;
   }

   /* allocate result bit array */
   ba_dst = animal_bit_array_alloc ( ba1->no_bits );
   if ( ba_dst == NULL )
   {
      animal_err_register ( "animal_bit_array_andnot_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }
   
   while(ctr-- != 0)
      ba_dst->data[ctr] = ba1->data[ctr] & (ba2->data[ctr] ^ ANIMAL_BITWORD_FULL);

   /* success */
   return ba_dst;
}

/**
 * \brief Insert part of one bit array in another.
 * \param src Source bit array
 * \param offset_src Offset of start of insertion region in \a src
 * \param dst Destination bit array
 * \param offset_dst Offset of start of insertion region in \a dst
 * \param no_bits Number of bits to insert
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Insert part of bit array \a src in bit array \a dst
 */
AnimalExport Animal_Bool
 animal_bit_array_insert ( Animal_BitArray *src, unsigned int offset_src,
                        Animal_BitArray *dst, unsigned int offset_dst, 
                        unsigned int no_bits )
{
   unsigned int lbits, ubits, lmask, umask,
                no_word_boundaries_in_src, no_word_boundaries_in_dst, ctr = 2;
   Animal_BitWord *ptr_faw_src, *ptr_faw_dst;
   int skew, tmp;
   div_t res_src_div, res_dst_div;

   animal_err_test_bool ( offset_src+no_bits <= src->no_bits &&
                       offset_dst+no_bits <= dst->no_bits,
                       "animal_bit_array_insert", ANIMAL_ERROR_ILLEGAL_ARGUMENT,
                       "" );

#if 0

   for ( tmp = 0; tmp < no_bits; tmp++ )
      if ( animal_bit_array_get_bit ( src, offset_src+tmp ) )
      {
         animal_bit_array_set_bit ( dst, offset_dst+tmp ) ;
      }
      else
      {
         animal_bit_array_clear_bit ( dst, offset_dst+tmp ) ;
      }

      return ANIMAL_TRUE;

#endif


   /* Evaluate the quotient and remainder of offset divided by word length */
   res_src_div = div (offset_src, ANIMAL_BITWORD_SIZE); 
   res_dst_div = div (offset_dst, ANIMAL_BITWORD_SIZE); 

   /* Determine the difference in offset within the first word of src and dst.
      If skew < 0 a left shift is required to align, otherwise a right shift */
   skew = res_dst_div.rem - res_src_div.rem;

   /* Number of word boundaries that are crossed in the src and dst array */
   no_word_boundaries_in_src = (offset_src + no_bits - 1) / ANIMAL_BITWORD_SIZE - res_src_div.quot;
   no_word_boundaries_in_dst = (offset_dst + no_bits - 1) / ANIMAL_BITWORD_SIZE - res_dst_div.quot;

   ptr_faw_src = &(src->data[res_src_div.quot]);
   ptr_faw_dst = &(dst->data[res_dst_div.quot]);

   switch(no_word_boundaries_in_src)
   {
      case 0:        /* Bits are contained within a single word in the src array */
                        
        switch(no_word_boundaries_in_dst)
        {
           case 0:        /* Bits are contained within a single word in the dst array */

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             if(skew < 0)
                lbits = (*ptr_faw_src) << abs(skew);        
             else
                lbits = (*ptr_faw_src) >> abs(skew);        

             /* Make mask */
             tmp = ANIMAL_BITWORD_SIZE - no_bits;

             lmask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
             lmask = lmask >> tmp;
             lmask = lmask << (tmp - res_dst_div.rem);
#else
             /* Shift bits so they can be copied into dst words */
             if(skew < 0)
                lbits = (*ptr_faw_src) >> abs(skew);        
             else
                lbits = (*ptr_faw_src) << abs(skew);        

             /* Make mask */
             tmp = ANIMAL_BITWORD_SIZE - no_bits;

             lmask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
             lmask = lmask << tmp;
             lmask = lmask >> (tmp - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~lmask) | (lbits & lmask);

             return ANIMAL_TRUE;

           case 1: /* Bits are spread over two words in the dst array - skew must be > 0 */
                                        
#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> skew;
             lbits = (*ptr_faw_src) << (ANIMAL_BITWORD_SIZE - skew);

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
             lmask = ANIMAL_BITWORD_FULL << ((ANIMAL_BITWORD_SIZE << 1) - res_dst_div.rem - no_bits);
#else
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << skew;
             lbits = (*ptr_faw_src) >> (ANIMAL_BITWORD_SIZE - skew);

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
             lmask = ANIMAL_BITWORD_FULL >> ((ANIMAL_BITWORD_SIZE << 1) - res_dst_div.rem - no_bits);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) |  (ubits & umask);
             *(ptr_faw_dst + 1) = (*(ptr_faw_dst + 1) & ~lmask) |  (lbits & lmask);                                        

             return ANIMAL_TRUE;
        }
        break;

      case 1:

        switch(no_word_boundaries_in_dst)
        {
           case 0:

             /* Shift bits so they can be copied into dst words (skew can only be negative) */
#ifdef WORDS_BIGENDIAN
             ubits = (*ptr_faw_src) << abs(skew);
             lbits = (*(ptr_faw_src+1)) >> (ANIMAL_BITWORD_SIZE + skew);

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> res_src_div.rem;
             umask = umask << abs(skew);

             tmp = res_src_div.rem + no_bits;
             lmask = ANIMAL_BITWORD_FULL >> ((ANIMAL_BITWORD_SIZE << 1) - tmp );
             lmask = lmask << (abs(skew) + ANIMAL_BITWORD_SIZE - tmp);
#else
             ubits = (*ptr_faw_src) >> abs(skew);
             lbits = (*(ptr_faw_src+1)) << (ANIMAL_BITWORD_SIZE + skew);

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << res_src_div.rem;
             umask = umask >> abs(skew);

             tmp = res_src_div.rem + no_bits;
             lmask = ANIMAL_BITWORD_FULL << ((ANIMAL_BITWORD_SIZE << 1) - tmp );
             lmask = lmask >> (abs(skew) + ANIMAL_BITWORD_SIZE - tmp);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask ) | (ubits & umask);
             *ptr_faw_dst = (*ptr_faw_dst & ~lmask ) | (lbits & lmask);

             return ANIMAL_TRUE;

           case 1:


             if(skew < 0)
             {
                /* first word */

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << abs(skew);
                lbits = (*(ptr_faw_src+1)) >> (ANIMAL_BITWORD_SIZE + skew);
                                                
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
                lmask = ~(ANIMAL_BITWORD_FULL << abs(skew));

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

                /* second word */
                ubits = (*(ptr_faw_src+1)) << abs(skew);

                /* Make mask */
                umask = ANIMAL_BITWORD_FULL << ( (ANIMAL_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#else
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> abs(skew);
                lbits = (*(ptr_faw_src+1)) << (ANIMAL_BITWORD_SIZE + skew);
                                                
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
                lmask = ~(ANIMAL_BITWORD_FULL >> abs(skew));

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

                /* second word */
                ubits = (*(ptr_faw_src+1)) >> abs(skew);

                /* Make mask */
                umask = ANIMAL_BITWORD_FULL >> ( (ANIMAL_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */
                                                
                /* Apply mask and insert */
                *(ptr_faw_dst+1) = (*(ptr_faw_dst+1) & ~umask) | (ubits & umask);
             }
             else
             {
                /* first word */
                tmp = ANIMAL_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
                /* Make Mask */
                umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;

                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> skew;
                                                
                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* second word */
                ubits = (*ptr_faw_src) << tmp;
                lbits = (*(ptr_faw_src+1)) >> skew;

                /* Make mask */
                umask = ANIMAL_BITWORD_FULL << tmp;
                lmask = ANIMAL_BITWORD_FULL << ((ANIMAL_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#else
                /* Make Mask */
                umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;

                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << skew;
                                                
                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* second word */
                ubits = (*ptr_faw_src) >> tmp;
                lbits = (*(ptr_faw_src+1)) << skew;

                /* Make mask */
                umask = ANIMAL_BITWORD_FULL >> tmp;
                lmask = ANIMAL_BITWORD_FULL >> ((ANIMAL_BITWORD_SIZE << 1) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst+1) = ((*(ptr_faw_dst+1) & ~(umask | lmask)) ) | (ubits & umask) | (lbits & lmask);
             }
                                        
             return ANIMAL_TRUE;
                                        
           case 2: /* skew can only be positive */

             /* first word */
             tmp = ANIMAL_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
             /* Make Mask */        
             umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> skew;
                                        
             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);

             /* middle word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << tmp;
             lbits = (*(ptr_faw_src+1)) >> skew;
                                        
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << tmp;

             /* Apply mask and insert */
             *(ptr_faw_dst+1) = (ubits & umask) | (lbits & ~umask);

             /* last word */

             /* Shift bits so they can be copied into dst words */
             lbits = *(ptr_faw_src+1) << tmp;

             /* Make mask */
             lmask = ANIMAL_BITWORD_FULL << ((ANIMAL_BITWORD_SIZE + (ANIMAL_BITWORD_SIZE<<1)) - no_bits - res_dst_div.rem);
#else
             /* Make Mask */        
             umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << skew;
                                        
             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);

             /* middle word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> tmp;
             lbits = (*(ptr_faw_src+1)) << skew;
                                        
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> tmp;

             /* Apply mask and insert */
             *(ptr_faw_dst+1) = (ubits & umask) | (lbits & ~umask);

             /* last word */

             /* Shift bits so they can be copied into dst words */
             lbits = *(ptr_faw_src+1) >> tmp;

             /* Make mask */
             lmask = ANIMAL_BITWORD_FULL >> ((ANIMAL_BITWORD_SIZE + (ANIMAL_BITWORD_SIZE<<1)) - no_bits - res_dst_div.rem);
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *(ptr_faw_dst + 2) = (*(ptr_faw_dst + 2) & ~lmask) |  (lbits & lmask);                                        

             return ANIMAL_TRUE;
        }

      default:

        switch(no_word_boundaries_in_dst - no_word_boundaries_in_src)
        {
           case -1: /* skew can only be negative */

             /* first word */
             tmp = ANIMAL_BITWORD_SIZE + skew;

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) << abs(skew);
             lbits = (*(ptr_faw_src+1)) >> tmp;
                                                
             /* Make mask */
             lmask = ~(ANIMAL_BITWORD_FULL << abs(skew));
             umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
#else
             /* Shift bits so they can be copied into dst words */
             ubits = (*ptr_faw_src) >> abs(skew);
             lbits = (*(ptr_faw_src+1)) << tmp;
                                                
             /* Make mask */
             lmask = ~(ANIMAL_BITWORD_FULL >> abs(skew));
             umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                

             /* middle words */

#ifdef WORDS_BIGENDIAN
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << abs(skew);

             for(;ctr < no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << abs(skew);
                lbits = (*(ptr_faw_src + ctr)) >> tmp;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr - 1) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*(ptr_faw_src + ctr - 1)) << abs(skew);
             lbits = (*(ptr_faw_src + ctr)) >> tmp;

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << abs(skew);
             lmask = ANIMAL_BITWORD_FULL << (-skew - (no_bits + offset_src) % ANIMAL_BITWORD_SIZE );
#else
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> abs(skew);

             for(;ctr < no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> abs(skew);
                lbits = (*(ptr_faw_src + ctr)) << tmp;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr - 1) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = (*(ptr_faw_src + ctr - 1)) >> abs(skew);
             lbits = (*(ptr_faw_src + ctr)) << tmp;

             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> abs(skew);
             lmask = ANIMAL_BITWORD_FULL >> (-skew - (no_bits + offset_src) % ANIMAL_BITWORD_SIZE );
#endif /* #ifdef WORDS_BIGENDIAN */
                                                
             /* Apply mask and insert */
             *(ptr_faw_dst + ctr - 1) = (*(ptr_faw_dst + ctr - 1) & ~lmask) | (ubits & umask) | (lbits & (umask ^ lmask));

             return ANIMAL_TRUE;

           case 0: 
             if(skew < 0)
             {
                /* first word */
                tmp = ANIMAL_BITWORD_SIZE + skew;

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) << abs(skew);
                lbits = (*(ptr_faw_src+1)) >> tmp;
                                                
                /* Make mask */
                lmask = ~(ANIMAL_BITWORD_FULL << abs(skew));
                umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
#else
                /* Shift bits so they can be copied into dst words */
                ubits = (*ptr_faw_src) >> abs(skew);
                lbits = (*(ptr_faw_src+1)) << tmp;
                                                
                /* Make mask */
                lmask = ~(ANIMAL_BITWORD_FULL >> abs(skew));
                umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & (umask & ~lmask)) | (lbits & lmask);
                                                
                /* middle words */

#ifdef WORDS_BIGENDIAN
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL << abs(skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr) << abs(skew);
                   lbits = *(ptr_faw_src + ctr + 1) >> tmp;

                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr) << abs(skew);

                /* Make mask */
                umask = ~(ANIMAL_BITWORD_FULL >> ((res_dst_div.rem + no_bits) % ANIMAL_BITWORD_SIZE)); 
#else
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL >> abs(skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr) >> abs(skew);
                   lbits = *(ptr_faw_src + ctr + 1) << tmp;

                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr) >> abs(skew);

                /* Make mask */
                umask = ~(ANIMAL_BITWORD_FULL << ((res_dst_div.rem + no_bits) % ANIMAL_BITWORD_SIZE)); 
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~umask) | (ubits & umask);

                return ANIMAL_TRUE;
             }
             else /* skew > 0 */
             {
                /* first word */
                tmp = ANIMAL_BITWORD_SIZE - skew;

#ifdef WORDS_BIGENDIAN
                /* Shift bits so they can be copied into dst words */
                ubits = *ptr_faw_src >> skew;
                                                
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
#else
                /* Shift bits so they can be copied into dst words */
                ubits = *ptr_faw_src << skew;
                                                
                /* Make mask */
                umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
                /* middle words */

#ifdef WORDS_BIGENDIAN
                /* Make mask */
                umask = ~(ANIMAL_BITWORD_FULL >> skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr - 1) << (tmp);
                   lbits = *(ptr_faw_src + ctr) >> skew;
                                        
                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << tmp;
                lbits = *(ptr_faw_src + ctr) >> skew;

                /* Make mask */
                lmask = ~(ANIMAL_BITWORD_FULL >> ((res_dst_div.rem + no_bits) % ANIMAL_BITWORD_SIZE)); 
#else
                /* Make mask */
                umask = ~(ANIMAL_BITWORD_FULL << skew);

                for(ctr = 1;ctr < no_word_boundaries_in_src; ctr++)        
                {
                   /* Shift bits so they can be copied into dst words */
                   ubits = *(ptr_faw_src + ctr - 1) >> (tmp);
                   lbits = *(ptr_faw_src + ctr) << skew;
                                        
                   /* Apply mask and insert */
                   *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
                }

                /* last word */

                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                lbits = *(ptr_faw_src + ctr) << skew;

                /* Make mask */
                lmask = ~(ANIMAL_BITWORD_FULL << ((res_dst_div.rem + no_bits) % ANIMAL_BITWORD_SIZE)); 
#endif /* #ifdef WORDS_BIGENDIAN */

                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~lmask) | (ubits & umask) | (lbits & (lmask ^umask));
                                                        
                return ANIMAL_TRUE;
             }
                                
           case 1: /* skew can only be positive */

             /* first word */
             tmp = ANIMAL_BITWORD_SIZE - skew;        

#ifdef WORDS_BIGENDIAN
             /* Shift bits so they can be copied into dst words */
             ubits = *ptr_faw_src >> skew;
                                                
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> res_dst_div.rem;
#else
             /* Shift bits so they can be copied into dst words */
             ubits = *ptr_faw_src << skew;
                                                
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << res_dst_div.rem;
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *ptr_faw_dst = (*ptr_faw_dst & ~umask) | (ubits & umask);
                                                
             /* middle words */

#ifdef WORDS_BIGENDIAN
             /* Make mask */
             umask = ~(ANIMAL_BITWORD_FULL >> skew);

             for(ctr = 1;ctr <= no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) << tmp;
                lbits = *(ptr_faw_src + ctr) >> skew;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = *(ptr_faw_src + ctr - 1) << tmp;
                                                
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL << ( ANIMAL_BITWORD_SIZE - ( (no_bits + offset_src) % ANIMAL_BITWORD_SIZE + skew) );
#else
             /* Make mask */
             umask = ~(ANIMAL_BITWORD_FULL << skew);

             for(ctr = 1;ctr <= no_word_boundaries_in_src; ctr++)        
             {
                /* Shift bits so they can be copied into dst words */
                ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                lbits = *(ptr_faw_src + ctr) << skew;
                                        
                /* Apply mask and insert */
                *(ptr_faw_dst + ctr) = (ubits & umask) | (lbits & ~umask);
             }

             /* last word */

             /* Shift bits so they can be copied into dst words */
             ubits = *(ptr_faw_src + ctr - 1) >> tmp;
                                                
             /* Make mask */
             umask = ANIMAL_BITWORD_FULL >> ( ANIMAL_BITWORD_SIZE - ( (no_bits + offset_src) % ANIMAL_BITWORD_SIZE + skew) );
#endif /* #ifdef WORDS_BIGENDIAN */

             /* Apply mask and insert */
             *(ptr_faw_dst + ctr) = (*(ptr_faw_dst + ctr) & ~umask) | (ubits & umask);

             return ANIMAL_TRUE;
        }
   }

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Copy one bit array to another.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Copy bit array \a ba_source to \a ba_dest.
 */
AnimalExport Animal_Bool
 animal_bit_array_copy_q ( Animal_BitArray *ba_source, Animal_BitArray *ba_dest )
{
   /* make sure destination bit array can hold source bit array */
   if ( ba_dest->words_alloc < ba_source->no_words &&
        !animal_bit_array_set_size ( ba_dest, ba_source->no_bits ) )
   {
      animal_err_register ( "animal_bit_array_copy_q", ANIMAL_ERROR_FAILURE, "" );
      return ANIMAL_FALSE;
   }

   /* copy bit array data */
   memcpy(ba_dest->data, ba_source->data, ba_source->no_words * sizeof(Animal_BitWord));

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Return a copy of a bit array.
 * \return A new copy of the bit array \a ba_source.
 *
 * Return a copy of bit array \a ba_source.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_copy_s ( Animal_BitArray *ba_source )
{
   Animal_BitArray *ba_dest = animal_bit_array_alloc(ba_source->no_bits);

   if ( ba_dest == NULL ||
        !animal_bit_array_copy_q ( ba_source, ba_dest ) )
   {
      animal_err_register ( "animal_bit_array_copy_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return ba_dest;
}

/**
 * \brief Expand bit array by padding it with zeros in a reference bit array.
 * \param ba Input bit array
 * \param ref_ba Reference bit array specifying padded bits
 * \param exp_ba Expanded bit array or \c NULL.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Expand bit array \a ba by padding it with zeros in a reference bit array
 * \a ref_ba.
 */
AnimalExport Animal_Bool
 animal_bit_array_expand_q ( Animal_BitArray *ba, Animal_BitArray *ref_ba,
                          Animal_BitArray *exp_ba )
{
   unsigned int i, j;

   if ( exp_ba == NULL )
   {
      /* allocate new bit array */
      exp_ba = animal_bit_array_alloc ( ref_ba->no_bits );
      if ( exp_ba == NULL )
      {
         animal_err_register ( "animal_bit_array_expand_q", ANIMAL_ERROR_FAILURE, "" );
         return ANIMAL_FALSE;
      }
   }
   else
      /* resize bit array if necessary */
      if ( exp_ba->no_bits != ref_ba->no_bits &&
           !animal_bit_array_set_size ( exp_ba, ref_ba->no_bits ) )
      {
         animal_err_register ( "animal_bit_array_expand_q", ANIMAL_ERROR_FAILURE, "" );
         return ANIMAL_FALSE;
      }
   
   animal_bit_array_fill ( exp_ba, ANIMAL_FALSE );
   for ( i = 0, j = 0; i < ref_ba->no_bits; i++ )
      if ( animal_bit_array_get_bit ( ref_ba, i ) )
      {
         if ( animal_bit_array_get_bit ( ba, j ) )
            animal_bit_array_set_bit ( exp_ba, i );

         j++;
      }

   /* make sure we've reached the end of the bit array */
   if ( j != ba->no_bits )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_bit_array_expand_q", ANIMAL_ERROR_INCOMPATIBLE,
                         "" );
      return ANIMAL_FALSE;
   }

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Expand bit array by padding it with zeros in a reference bit array.
 * \param ba Input bit array
 * \param ref_ba Reference bit array specifying padded bits
 * \return Expanded bit array as a new bit array, or \c NULL on failure.
 *
 * Expand bit array \a ba by padding it with zeros in a reference bit array
 * \a ref_ba.
 */
AnimalExport Animal_BitArray *
 animal_bit_array_expand_s ( Animal_BitArray *ba, Animal_BitArray *ref_ba )
{
   Animal_BitArray *exp_ba = animal_bit_array_alloc(ref_ba->no_bits);

   if ( ref_ba == NULL ||
        !animal_bit_array_expand_q ( ba, ref_ba, exp_ba ) )
   {
      animal_err_register ( "animal_bit_array_expand_s", ANIMAL_ERROR_FAILURE, "" );
      return NULL;
   }

   /* success */
   return exp_ba;
}

 /**
 * \brief Fill part of a bit array.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Fill part of a bit array, specified by the start point \a offset and the
 * given number of bits \a no_bits. The part is filled with the given boolean
 * value \a val.
 */
AnimalExport Animal_Bool
 animal_bit_array_fill_part ( Animal_BitArray *ba, unsigned int offset,
                           unsigned int no_bits, Animal_Bool val )
{
   div_t start_div, end_div;

   animal_err_test_bool ( offset+no_bits <= ba->no_bits,
                       "animal_bit_array_fill_part", ANIMAL_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* return immediately if no bits are to be filled */
   if ( no_bits == 0 ) return ANIMAL_TRUE;

   /* compute word at start and end of filled part of row */
   start_div = div ( (int)offset, ANIMAL_BITWORD_SIZE );
   end_div = div ( (int)(offset+no_bits-1), ANIMAL_BITWORD_SIZE );

   if ( val )
   {
      /* fill with ones */
      if ( start_div.quot == end_div.quot )
         /* fill single word */
#ifdef WORDS_BIGENDIAN
         ba->data[start_div.quot] |=
               ((ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> start_div.rem);
#else      
         ba->data[start_div.quot] |=
               ((ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#endif
      else
      {
         int word;

         /* fill first word */
#ifdef WORDS_BIGENDIAN
         ba->data[start_div.quot] |= (ANIMAL_BITWORD_FULL >> start_div.rem);
#else      
         ba->data[start_div.quot] |= (ANIMAL_BITWORD_FULL << start_div.rem);
#endif
         
         /* fill middle words */
         for ( word = start_div.quot+1; word < end_div.quot; word++ )
            ba->data[word] = ANIMAL_BITWORD_FULL;

         /* fill end word */
#ifdef WORDS_BIGENDIAN
         ba->data[end_div.quot] |= (ANIMAL_BITWORD_FULL <<
                                    (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#else      
         ba->data[end_div.quot] |= (ANIMAL_BITWORD_FULL >>
                                    (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#endif
      }
   }
   else
   {
      if ( start_div.quot == end_div.quot )
      {
         /* fill single word */
         if ( start_div.rem == 0 )
         {
            if ( end_div.rem == ANIMAL_BITWORD_SIZE-1 )
               ba->data[start_div.quot] = 0;
            else
#ifdef WORDS_BIGENDIAN
               ba->data[start_div.quot] &= (ANIMAL_BITWORD_FULL >>
                                            (end_div.rem+1));
            
#else      
               ba->data[start_div.quot] &= (ANIMAL_BITWORD_FULL <<
                                            (end_div.rem+1));
#endif
         }
         else if ( end_div.rem == ANIMAL_BITWORD_SIZE-1 )
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                      (ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE - start_div.rem));
#else      
            ba->data[start_div.quot] &=
                      (ANIMAL_BITWORD_FULL >> (ANIMAL_BITWORD_SIZE - start_div.rem));
#endif
         }
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                ((ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE - start_div.rem))
                 | (ANIMAL_BITWORD_FULL >> (end_div.rem + 1)));
#else      
            ba->data[start_div.quot] &=
                ((ANIMAL_BITWORD_FULL >> (ANIMAL_BITWORD_SIZE - start_div.rem))
                 | (ANIMAL_BITWORD_FULL << (end_div.rem + 1)));
#endif
         }
      }
      else
      {
         int word;

         /* fill first word */
         if ( start_div.rem == 0 )
            ba->data[start_div.quot] = 0;
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[start_div.quot] &=
                      (ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE - start_div.rem));
#else      
            ba->data[start_div.quot] &=
                      (ANIMAL_BITWORD_FULL >> (ANIMAL_BITWORD_SIZE - start_div.rem));
#endif
         }

         /* fill middle words */
         for ( word = start_div.quot+1; word < end_div.quot; word++ )
            ba->data[word] = 0;

         /* fill end word */
         if ( end_div.rem == ANIMAL_BITWORD_SIZE-1 )
            ba->data[end_div.quot] = 0;
         else
         {
#ifdef WORDS_BIGENDIAN
            ba->data[end_div.quot] &= (ANIMAL_BITWORD_FULL >> (end_div.rem + 1));
#else      
            ba->data[end_div.quot] &= (ANIMAL_BITWORD_FULL << (end_div.rem + 1));
#endif
         }
      }
   }

   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Invert part of a bit array.
 * \return #ANIMAL_TRUE on success, #ANIMAL_FALSE on failure.
 *
 * Invert part of a bit array, specified by the start point \a offset and the
 * given number of bits \a no_bits.
 */
AnimalExport Animal_Bool
 animal_bit_array_invert_part ( Animal_BitArray *ba, unsigned int offset,
                             unsigned int no_bits )
{
   div_t start_div, end_div;

   animal_err_test_bool ( offset+no_bits <= ba->no_bits,
                       "animal_bit_array_invert_part", ANIMAL_ERROR_ILLEGAL_ARGUMENT,
                       "" );

   /* return immediately if no bits are to be filled */
   if ( no_bits == 0 ) return ANIMAL_TRUE;

   /* compute word at start and end of filled part of row */
   start_div = div ( (int)offset, ANIMAL_BITWORD_SIZE );
   end_div = div ( (int)(offset+no_bits-1), ANIMAL_BITWORD_SIZE );

   if ( start_div.quot == end_div.quot )
      /* fill single word */
#ifdef WORDS_BIGENDIAN
      ba->data[start_div.quot] ^=
               ((ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> start_div.rem);
#else      
      ba->data[start_div.quot] ^=
               ((ANIMAL_BITWORD_FULL << (ANIMAL_BITWORD_SIZE
                                      - end_div.rem + start_div.rem - 1))
                >> (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#endif
   else
   {
      int word;

      /* fill first word */
#ifdef WORDS_BIGENDIAN
      ba->data[start_div.quot] ^= (ANIMAL_BITWORD_FULL >> start_div.rem);
#else      
      ba->data[start_div.quot] ^= (ANIMAL_BITWORD_FULL << start_div.rem);
#endif
      /* fill middle words */
      for ( word = start_div.quot+1; word < end_div.quot; word++ )
         ba->data[word] ^= ANIMAL_BITWORD_FULL;

      /* fill end word */
#ifdef WORDS_BIGENDIAN
      ba->data[end_div.quot] ^= (ANIMAL_BITWORD_FULL <<
                                 (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#else      
      ba->data[end_div.quot] ^= (ANIMAL_BITWORD_FULL >>
                                 (ANIMAL_BITWORD_SIZE - end_div.rem - 1));
#endif
   }
   
   /* success */
   return ANIMAL_TRUE;
}

/**
 * \brief Print bit array in ASCII to file.
 */
AnimalExport void
 animal_bit_array_fprint ( FILE *fp, Animal_BitArray *ba, int indent )
{
   unsigned int i;

   /* print indentation */
   for ( indent--; indent >= 0; indent-- ) fprintf ( fp, " " );

   /* print bit array */
   for ( i = 0; i < ba->no_bits; i++ )
      fprintf ( fp, "%c", animal_bit_array_get_bit(ba,i) ? '1' : '0' );

   /* print end of line character */
   fprintf ( fp, "\n" );
}

/**
 * \}
 */

/**
 * \}
 */

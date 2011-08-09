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





/* Module for temporary stack storage. Memory must be freed in reverse of the
 * order it was allocated. Much faster than malloc().
 */
#include "../common.h"
#include "memory_stack.h"
#include "misc_defs.h"
#include "misc_error.h"
#include "allocate.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonAllocate
 * \{
 */

/* set TM_TEST_MAGIC to 0 to eliminate tests for overwriting a block */
#define TM_TEST_MAGIC 1

#if TM_TEST_MAGIC
#define TM_MAGIC_NUMBER 0x2f3ee7b1 /* value checked for block overwrite */
#endif

/**
 * \brief Initialise temporary memory allocation structure.
 * \param ms Memoru stack structure pointer
 * \param nblocks Maximum number of blocks of memory to allow
 * \param bsize Size of each block in bytes
 * \return Pointer to initialised structure, or \c NULL on failure.
 */
AnimalExport Animal_MemoryStack *
 animal_memory_stack_form ( Animal_MemoryStack *ms, int nblocks, size_t bsize )
{
   if ( ms == NULL )
   {
      ms = animal_malloc_object(Animal_MemoryStack);
      if ( ms == NULL )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_memory_stack_form", ANIMAL_ERROR_MALLOC_FAILED,
                            "" );
         return NULL;
      }

      ms->alloc = ANIMAL_TRUE;
   }
   else
      ms->alloc = ANIMAL_FALSE;
   
   if ( nblocks <= 0 )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_memory_stack_form", ANIMAL_ERROR_ILLEGAL_ARGUMENT,
                         "" );
      return NULL;
   }

   ms->block_ptr = (Animal_BigType **) malloc ( nblocks*sizeof(Animal_BigType *) );
   ms->block_end = (int          *) malloc ( nblocks*sizeof(int) );
   if ( ms->block_ptr == NULL || ms->block_end == NULL )
   {
      animal_err_flush_trace();
      animal_err_register ( "animal_memory_stack_form", ANIMAL_ERROR_MALLOC_FAILED,
                         "" );
      return NULL;
   }

   /* compute block size in terms of Animal_BigType's */
   ms->tm_bsize = 1 + (bsize-1)/sizeof(Animal_BigType);

   ms->max_tm_blocks = nblocks;

   /* see if you we can get magic number and size into a single Animal_BigType */
   if ( sizeof(size_t) == sizeof(animal_ui32) &&
        sizeof(size_t) + sizeof(animal_ui32) == sizeof(Animal_BigType) )
      ms->squeeze_OK = ANIMAL_TRUE;
   else
      ms->squeeze_OK = ANIMAL_FALSE; /* can't manage it */

   /* initialise other fields */
   ms->last_free = NULL;
   ms->tm_total = 0;
   ms->current_tm_block = 0;
   ms->next_start = 0;
   ms->alloc_tm_blocks = 0;

   /* success */
   return ms;
}

/**
 * \brief Temporary memory allocation routine, faster than malloc().
 * \param ms Pointer to memory stack structure
 * \param size Amount of memory to allocate in bytes
 * \return Non \c NULL successfully allocated block, \c NULL on failure
 *
 * For allocating chunks of memory which can be freed in reverse order,
 * The module allocates large blocks at a time using \a malloc(), the blocks
 * being "a lot" bigger than the chunks to be \a animal_ms_malloc()'d, to avoid
 * wasting memory. Calls to animal_ms_malloc() then fill up the current block, and
 * when the end is reached another block is \a malloc()'d. There's a dirty bit
 * concerning memory alignment where I assume that everything should be
 * aligned according to a "big" type (double). \a malloc() guarantees that
 * it returns pointers that may be safely cast to any pointer type, but I
 * (PFM) don't know how to do that properly in C.
 */
AnimalExport void *
 animal_ms_malloc ( Animal_MemoryStack *ms, size_t size )
{
   Animal_BigType *tm_ptr = NULL;

   /* let's decide to return a valid if you want a pointer to zero bytes,
      and leave NULL for errors */
   if ( size == 0 )
      return ( (void *)(ms->block_ptr[ms->current_tm_block]
                        + ms->next_start) );

   /* make size into number of sizeof(Animal_BigType)'s rather than bytes */
   size = 1 + (size-1)/sizeof(Animal_BigType);

   /* increment size for magic number and stored size */
   if ( ms->squeeze_OK ) size++;
   else                  size += 2;

   /* check whether there is no block currently allocated, i.e. this is the
      first call to animal_ms_malloc() since the call to animal_memory_stack_form()
      or animal_memory_stack_free() */
   if ( ms->alloc_tm_blocks == 0 )
   {
      if ( size > ms->tm_bsize )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_ms_malloc", ANIMAL_ERROR_FAILURE,
                            "requested size too big" );
         return NULL;
      }

      ms->block_ptr[0] = animal_malloc_array ( Animal_BigType, ms->tm_bsize );
      tm_ptr = ms->block_ptr[0];
      ms->block_end[0] = 0;
      ms->current_tm_block = 0;
      ms->alloc_tm_blocks = 1;
      ms->next_start = size;
   }
   else if ( ms->next_start + size > ms->tm_bsize )
   /* we have reached the  end of the current temporary memory block */
   {
      if ( size > ms->tm_bsize )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_ms_malloc", ANIMAL_ERROR_FAILURE,
                            "requested size too big" );
         return NULL;
      }
         
      ms->block_end[ms->current_tm_block++] = ms->next_start;

      /* see whether we need to allocate a new block */
      if ( ms->current_tm_block == ms->alloc_tm_blocks )
      {
         if ( ms->alloc_tm_blocks >= ms->max_tm_blocks )
         {
            animal_err_flush_trace();
            animal_err_register ( "animal_ms_malloc", ANIMAL_ERROR_FAILURE,
                               "too many blocks allocated" );
         }

         ms->block_ptr[ms->current_tm_block] =
                           animal_malloc_array ( Animal_BigType, ms->tm_bsize );
         if ( ms->block_ptr[ms->current_tm_block] == NULL )
         {
            animal_err_flush_trace();
            animal_err_register ( "animal_ms_malloc", ANIMAL_ERROR_MALLOC_FAILED, "" );
            return NULL;
         }

         ms->alloc_tm_blocks++;
      }

      ms->next_start = size;
      tm_ptr = ms->block_ptr[ms->current_tm_block];
   }
   else
   {
      ms->next_start += size;
      tm_ptr = ms->block_ptr[ms->current_tm_block] + ms->next_start - size;
   }

   /* store magic number to test for illegal overwriting test */
   *((animal_ui32 *) tm_ptr) = TM_MAGIC_NUMBER;

   /* store size */
   if ( ms->squeeze_OK )
   {
      *(((size_t *) tm_ptr)+1) = size;
      tm_ptr++;
   }
   else
   {
      *((size_t *) (tm_ptr+1)) = size;
      tm_ptr += 2;
   }

   /* keep a running total */
   ms->tm_total += size;

   /* there is no meaning for last freed pointer */
   ms->last_free = NULL;

#if 0
   printf ( "talloc of %d words at %x (total=%d)\n",
            size, (animal_ui32) tm_ptr, ms->tm_total );
#endif

   return ( (void *) tm_ptr );
}

/**
 * \brief Temporary memory free routine.
 * \param ms Pointer to memory stack structure
 * \param ptr Pointer to memory area to free, as returned by animal_ms_malloc()
 *
 * Temporary memory free routine, Memory must be freed in reverse of the
 * order it was allocated using animal_ms_malloc(). animal_ms_free() does not
 * actually free any memory, but allows the marked memory to be used in
 * subsequent animal_ms_malloc() calls. After using the temporary memory, call
 * animal_memory_stack_free() to actually free the memory blocks.
 */
AnimalExport void
 animal_ms_free ( Animal_MemoryStack *ms, void *ptr )
{
   Animal_BigType *tm_ptr = (Animal_BigType *) ptr;
   size_t size;

   if ( tm_ptr == NULL ) return;

   if ( ms->squeeze_OK )
   {
      tm_ptr--;
      size = *(((size_t *) tm_ptr)+1);
   }
   else
   {
      tm_ptr -= 2;
      size = *((size_t *) (tm_ptr+1));
   }

   /* check for illegal overwriting */
   animal_assert ( *((animal_ui32 *) tm_ptr) == TM_MAGIC_NUMBER,
                "talloc magic number overwritten" );

   /* subtract stored size from running total */
   ms->tm_total -= size;

#if 0
   printf ( "ms_free of %d words at %x (total=%d)\n",
            size, (animal_ui32) (tm_ptr+(ms->squeeze_OK ? 1 : 2)), ms->tm_total );
#endif

   /* check whether calls to animal_ms_free() have wound back to the
      start of a block */
   if ( tm_ptr == ms->block_ptr[ms->current_tm_block] )
      if ( ms->current_tm_block == 0 )
      {
         ms->next_start = 0;
         ms->last_free = tm_ptr;
      }
      else
      {
         ms->next_start = ms->block_end[--ms->current_tm_block];
         ms->last_free = NULL;
      }
   else
   {
      animal_assert ( ms->last_free == NULL || tm_ptr <= ms->last_free,
                   "temporary memory freed in wrong order" );
      ms->next_start = tm_ptr - ms->block_ptr[ms->current_tm_block];
      ms->last_free = tm_ptr;
   }
}

/**
 * \brief Frees a list of temporaray blocks terminated by \c NULL.
 * \param ms Pointer to memory stack structure
 * \param ptr The first memory block to free
 * \param ... List of other blocks to free, terminated by \c NULL
 *
 * Frees a list of temporaray blocks allocated by animal_ms_malloc(), teminated
 * by \c NULL. animal_ms_free() is called for each block, preserving the order
 * of the arguments in the calls.
 */
AnimalExport void
 animal_ms_free_va ( Animal_MemoryStack *ms, void *ptr, ... )
{
   va_list ap;

   if ( ptr == NULL ) return;

   animal_ms_free ( ms, ptr );
   va_start ( ap, ptr );
   for(;;)
   {
      ptr = va_arg ( ap, void * );
      if ( ptr == NULL ) break;

      animal_ms_free ( ms, ptr );
   }

   va_end ( ap );
}

/**
 * \brief Frees all temporary memory.
 * \param ms Pointer to memory stack structure
 *
 * Frees all memory allocated using calls to animal_ms_malloc().
 */
AnimalExport void
 animal_memory_stack_free ( Animal_MemoryStack *ms )
{
   int blk;

   for ( blk = 0; blk < ms->alloc_tm_blocks; blk++ )
      free ( ms->block_ptr[blk] );

   free ( ms->block_end );
   free ( ms->block_ptr );
   if ( ms->alloc ) free(ms);
}

/**
 * \brief Frees unused temporary memory.
 * \param ms Pointer to memory stack structure
 *
 * Frees memory stack down to the last animal_ms_free() call.
 */
AnimalExport void
 animal_memory_stack_clean ( Animal_MemoryStack *ms )
{
   int blk;

   for ( blk = ms->current_tm_block + 1; blk < ms->alloc_tm_blocks; blk++ )
      free ( ms->block_ptr[blk] );

   ms->alloc_tm_blocks = ms->current_tm_block+1;
}

/**
 * \brief Returns the total temporary memory currently allocated.
 * \param ms Pointer to memory stack structure
 *
 * Returns the total temporary memory currently allocated by calls
 * to animal_ms_malloc().
 */
AnimalExport size_t
 animal_memory_stack_total ( Animal_MemoryStack *ms )
{
   return ms->tm_total;
}

/**
 * \}
 */

/**
 * \}
 */

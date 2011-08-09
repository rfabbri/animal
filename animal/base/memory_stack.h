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





#ifndef _ANIMAL_MEMORY_STACK_H
#define _ANIMAL_MEMORY_STACK_H

#include "misc_defs.h"
#include <stddef.h> /* defines size_t */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonAllocate
 * \{
 */

/**
 * \brief Big C type for byte alignment
 */
typedef double Animal_BigType;

/**
 * \brief Structure to hold state of stack memory blocks
 */
typedef struct Animal_MemoryStack
{
   Animal_BigType **block_ptr, *last_free;
   size_t tm_bsize, tm_total;
   int current_tm_block, next_start, alloc_tm_blocks;
   int *block_end, max_tm_blocks;
   Animal_Bool squeeze_OK;

   Animal_Bool alloc; /* whether this structure was dynamically allocated */
} Animal_MemoryStack;

AnimalExport Animal_MemoryStack
   *animal_memory_stack_form ( Animal_MemoryStack *ms,
                              int nblocks, size_t bsize );

AnimalExport void
   *animal_ms_malloc ( Animal_MemoryStack *ms, size_t size ),
    animal_ms_free ( Animal_MemoryStack *ms, void *ptr ),
    animal_ms_free_va ( Animal_MemoryStack *ms, void *ptr, ... ),
    animal_memory_stack_free ( Animal_MemoryStack *ms ),
    animal_memory_stack_clean ( Animal_MemoryStack *ms );

AnimalExport size_t animal_memory_stack_total ( Animal_MemoryStack *ms );

/**
 * \brief Macro: Allocate and initialise temporary memory allocation structure.
 * \param nblocks Maximum number of blocks of memory to allow
 * \param bsize Size of each block in bytes
 * \return Pointer to new structure, or \c NULL on failure.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
Animal_MemoryStack *animal_memory_stack_alloc ( int nblocks, size_t bsize );
#else
#define animal_memory_stack_alloc(nb,bs) animal_memory_stack_form(NULL,nb,bs)
#endif

/**
 * \brief Macro: Dynamically allocate one item of a specific type with "stack" memory.
 * \param ms Memory stack structure
 * \param obj_type The type of objects to be allocated
 * \return The allocated object, or \c NULL on failure.
 *
 * Allocation function for allocating 1 item of the given \a obj_type in
 * temporary stack-style memory. Implemented as a macro call to
 * animal_ms_malloc().
 *
 * \sa animal_ms_malloc().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_ms_malloc_object ( Animal_MemoryStack *ms, TYPE obj_type );
#else
#define animal_ms_malloc_object(ms,obj_type) \
        ((obj_type *) animal_ms_malloc(ms,sizeof(obj_type)))
#endif

/**
 * \brief Macro: Dynamically allocate \a n items of a specific type with "stack" memory.
 * \param ms Memory stack structure
 * \param obj_type The type of objects to be allocated
 * \param size The number of objects to allocate for in the array
 * \return The allocated array of objects, or \c NULL on failure.
 *
 * Allocation function for allocating \a size items of the given \a obj_type in
 * temporary stack-style memory.
 * Implemented as a macro call to animal_ms_malloc().
 *
 * \sa animal_ms_malloc_object(), animal_ms_malloc().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_ms_malloc_array ( Animal_MemoryStack *ms, TYPE obj_type, size_t size );
#else
#define animal_ms_malloc_array(ms,obj_type,size) \
        ((obj_type *) animal_ms_malloc ( ms, (size)*sizeof(obj_type) ))
#endif

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ANIMAL_MEMORY_STACK_H */

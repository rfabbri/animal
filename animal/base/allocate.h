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





#ifndef _ANIMAL_ALLOCATE_H
#define _ANIMAL_ALLOCATE_H

#include <stdlib.h>
#include "../common.h"
#include "misc_defs.h"

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
 * \brief Generic free function type.
 */
typedef void (*Animal_FreeFunc) (void *);

#ifdef ANIMAL_GENERATE_DOCUMENTATION
#define obj_type int x
#endif

/**
 * \brief Macro: Allocate an object of a specific type using \c malloc().
 * \param obj_type The type of object to be allocated
 * \return Pointer to the allocated object, or \c NULL on failure.
 *
 * Invokes \c malloc() to dynamically allocate an object of a specific type.
 *
 * \sa animal_malloc_array().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_malloc_object ( TYPE obj_type );
#else
#define animal_malloc_object(obj_type) ((obj_type *) malloc(sizeof(obj_type)))
#endif

/**
 * \brief Macro: Allocate an array of objects of a specific type using \c malloc().
 * \param obj_type The type of objects to be allocated
 * \param size The number of objects to allocate for in the array
 * \return The allocated array of objects, or \c NULL on failure.
 *
 * Invokes \c malloc() to dynamically allocate an array of objects of a
 * specific type.
 *
 * \sa animal_malloc_object(), animal_realloc_array().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_malloc_array ( TYPE obj_type, unsigned size );
#else
#define animal_malloc_array(obj_type,size) \
           ((obj_type *)malloc((size)*sizeof(obj_type)))
#endif

/**
 * \brief Macro: Reallocate an array of objects using \c realloc().
 * \param obj_type The type of objects to be allocated
 * \param ptr The existing array to be reallocated or NULL
 * \param size The number of objects to allocate for in the array
 * \return The reallocated array of objects, or \c NULL on failure.
 *
 * Invokes \c realloc() to dynamically reallocate an array of objects of a
 * specific type. If \a ptr is passed as \c NULL then it is equivalent to
 * calling animal_malloc_array().
 *
 * \sa animal_malloc_array().
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void *animal_realloc_array ( TYPE obj_type, void *ptr, unsigned size );
#else
#define animal_realloc_array(type,ptr,n) \
           ((type *)realloc((void *)ptr,(n)*sizeof(type)))
#endif

#ifdef ANIMAL_GENERATE_DOCUMENTATION
#undef obj_type
#endif

AnimalExport void animal_free_va ( void *ptr, ... );

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ANIMAL_ALLOCATE_H */

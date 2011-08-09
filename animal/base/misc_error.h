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





#ifndef _ANIMAL_MISC_ERROR_H
#define _ANIMAL_MISC_ERROR_H

#include "../common.h"
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include "err.h"
#include "misc_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonError
 * \{
 */

/* Animal error codes. If you change them, remember to change the switch
 * statement in print_error() in misc_error.c.
 */
#define ANIMAL_ERROR_FAILURE                   1
#define ANIMAL_ERROR_NOT_IMPLEMENTED           2
#define ANIMAL_ERROR_REF_OUTSIDE_MATRIX        3
#define ANIMAL_ERROR_CANT_REALLOC              4
#define ANIMAL_ERROR_MALLOC_FAILED             5
#define ANIMAL_ERROR_DIFFERENT_DIMS            6
#define ANIMAL_ERROR_INCOMPATIBLE              7
#define ANIMAL_ERROR_MATRIX_NOT_SQUARE         8
#define ANIMAL_ERROR_INPLACE_TRANSPOSE         9
#define ANIMAL_ERROR_INPLACE_MULTIPLY         10
#define ANIMAL_ERROR_ARRAY_TOO_SMALL          11
#define ANIMAL_ERROR_ILLEGAL_TYPE             12
#define ANIMAL_ERROR_NO_IMPLICIT_INV          13
#define ANIMAL_ERROR_CLAPACK_ILLEGAL_ARG      14
#define ANIMAL_ERROR_CBLAS_ILLEGAL_ARG        15
#define ANIMAL_ERROR_CBLAS_FAILED             16
#define ANIMAL_ERROR_NOT_POSITIVE_DEFINITE    17
#define ANIMAL_ERROR_DIVISION_BY_ZERO         18
#define ANIMAL_ERROR_SQRT_OF_NEG_NUMBER       19
#define ANIMAL_ERROR_NO_CONVERGENCE           20
#define ANIMAL_ERROR_SINGULAR_MATRIX          21
#define ANIMAL_ERROR_IMAGE_TOO_SMALL          22
#define ANIMAL_ERROR_NO_DATA                  23
#define ANIMAL_ERROR_NOT_ENOUGH_DATA          24
#define ANIMAL_ERROR_OPENING_FILE             25
#define ANIMAL_ERROR_CORRUPTED_FILE           26
#define ANIMAL_ERROR_OUTSIDE_RANGE            27
#define ANIMAL_ERROR_TOO_LARGE                28
#define ANIMAL_ERROR_TOO_SMALL                29
#define ANIMAL_ERROR_TRUNCATED_FILE           30
#define ANIMAL_ERROR_NOT_INITIALISED          31
#define ANIMAL_ERROR_ILLEGAL_ARGUMENT         32
#define ANIMAL_ERROR_CCMATH_FAILED            33
#define ANIMAL_ERROR_NO_SOLUTION              34
#define ANIMAL_ERROR_READING_FROM_FILE        35
#define ANIMAL_ERROR_WRITING_TO_FILE          36

AnimalExport void animal_err_default_reporter(void);

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ANIMAL_MISC_ERROR_H */

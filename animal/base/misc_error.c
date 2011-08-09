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





#include <string.h>
#include "misc_error.h"
#include "allocate.h"

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonError
 * \{
 */

#if 0

typedef struct Animal_ErrorStruct
{
   char *funcname;
   int   code;
   char *message;
   char *filename;
   int   line;
   struct Animal_ErrorStruct *next;
} Animal_ErrorStruct;

static Animal_ErrorStruct *
 animal_error_stack_add ( Animal_ErrorStruct *error_stack,
                       const char *funcname, int code, char *message,
                       const char *filename, int line )
{
   Animal_ErrorStruct *new = animal_malloc_object(Animal_ErrorStruct);

   animal_assert ( new != NULL, "malloc() failed in animal_error_stack_add()" );
   new->funcname = animal_malloc_array ( char, strlen(funcname)+1 );
   strcpy ( new->funcname, funcname );
   new->code = code;
   new->message = animal_malloc_array ( char, strlen(message)+1 );
   strcpy ( new->message, message );
   new->filename = animal_malloc_array ( char, strlen(filename)+1 );
   strcpy ( new->filename, filename );
   new->line = line;
 
   new->next = error_stack;
   return new;
}

static void
 animal_error_stack_free ( Animal_ErrorStruct *error_stack )
{
   Animal_ErrorStruct *next;

   if ( error_stack == NULL ) return;
   next = error_stack->next;
   free ( error_stack );
   animal_error_stack_free ( next );
}

static Animal_ErrorStruct *error_stack = NULL;

void
 animal_err_flush_trace(void)
{
   animal_error_stack_free ( error_stack );
   error_stack = NULL;
}

/* not a user function */
void animal_err_register_expanded ( const char *funcname, int code, char *message,
                                 const char *filename, int line )
{
#if 1
   print_error ( funcname, code, message, filename, line );
   exit(-1);
#endif
   error_stack = animal_error_stack_add ( error_stack, funcname, code, message,
                                       filename, line );
}
#endif

static void print_error ( const char *funcname, int code, const char *message,
                          const char *filename, int line )
{
   fprintf ( stderr, "Animal error %d in function %s(): ",
             code, funcname );
   switch ( code )
   {
      case ANIMAL_ERROR_FAILURE:
        fprintf ( stderr, "failed:" );
        break;

      case ANIMAL_ERROR_NOT_IMPLEMENTED:
        fprintf ( stderr, "not implemented" );
        break;

      case ANIMAL_ERROR_REF_OUTSIDE_MATRIX:
        fprintf ( stderr, "reference outside matrix" );
        break;

      case ANIMAL_ERROR_CANT_REALLOC:
        fprintf ( stderr, "can't reallocate" );
        break;

      case ANIMAL_ERROR_MALLOC_FAILED:
        fprintf ( stderr, "memory allocation failed" );
        break;

      case ANIMAL_ERROR_DIFFERENT_DIMS:
        fprintf ( stderr, "dimensions don't match" );
        break;

      case ANIMAL_ERROR_INCOMPATIBLE:
        fprintf ( stderr, "incompatible" );
        break;

      case ANIMAL_ERROR_MATRIX_NOT_SQUARE:
        fprintf ( stderr, "matrix not square" );
        break;

     case ANIMAL_ERROR_INPLACE_TRANSPOSE:
        fprintf ( stderr, "illegal in-place matrix transpose" );
        break;

     case ANIMAL_ERROR_INPLACE_MULTIPLY:
        fprintf ( stderr, "illegal in-place matrix multiply" );
        break;

      case ANIMAL_ERROR_ARRAY_TOO_SMALL:
        fprintf ( stderr, "array too small" );
        break;

      case ANIMAL_ERROR_ILLEGAL_TYPE:
        fprintf ( stderr, "illegal type" );
        break;

      case ANIMAL_ERROR_NO_IMPLICIT_INV:
        fprintf ( stderr, "can't do implicit inverse" );
        break;

      case ANIMAL_ERROR_CLAPACK_ILLEGAL_ARG:
        fprintf ( stderr, "LAPACK function illegal argument" );
        break;

      case ANIMAL_ERROR_CBLAS_ILLEGAL_ARG:
        fprintf ( stderr, "BLAS function illegal argument" );
        break;

      case ANIMAL_ERROR_CBLAS_FAILED:
        fprintf ( stderr, "BLAS function failed" );
        break;

      case ANIMAL_ERROR_NOT_POSITIVE_DEFINITE:
        fprintf ( stderr, "matrix not positive definite" );
        break;

      case ANIMAL_ERROR_DIVISION_BY_ZERO:
        fprintf ( stderr, "attempted division by zero" );
        break;

      case ANIMAL_ERROR_SQRT_OF_NEG_NUMBER:
        fprintf ( stderr, "attempted square-root of negative number" );
        break;

      case ANIMAL_ERROR_NO_CONVERGENCE:
        fprintf ( stderr, "no convergence" );
        break;

      case ANIMAL_ERROR_SINGULAR_MATRIX:
        fprintf ( stderr, "singular matrix" );
        break;

      case ANIMAL_ERROR_IMAGE_TOO_SMALL:
        fprintf ( stderr, "image too small" );
        break;

      case ANIMAL_ERROR_NO_DATA:
        fprintf ( stderr, "no data" );
        break;

      case ANIMAL_ERROR_NOT_ENOUGH_DATA:
        fprintf ( stderr, "not enough data" );
        break;

      case ANIMAL_ERROR_OPENING_FILE:
        fprintf ( stderr, "couldn't open file" );
        break;

      case ANIMAL_ERROR_CORRUPTED_FILE:
        fprintf ( stderr, "corrupted file" );
        break;

      case ANIMAL_ERROR_OUTSIDE_RANGE:
        fprintf ( stderr, "outside range" );
        break;

      case ANIMAL_ERROR_TOO_LARGE:
        fprintf ( stderr, "too large" );
        break;

      case ANIMAL_ERROR_TOO_SMALL:
        fprintf ( stderr, "too small" );
        break;

      case ANIMAL_ERROR_TRUNCATED_FILE:
        fprintf ( stderr, "truncated file" );
        break;

      case ANIMAL_ERROR_NOT_INITIALISED:
        fprintf ( stderr, "not initialised" );
        break;

      case ANIMAL_ERROR_ILLEGAL_ARGUMENT:
        fprintf ( stderr, "illegal argument" );
        break;

      case ANIMAL_ERROR_CCMATH_FAILED:
        fprintf ( stderr, "CCM library function failed" );
        break;

      case ANIMAL_ERROR_NO_SOLUTION:
        fprintf ( stderr, "no solution found" );
        break;

      case ANIMAL_ERROR_READING_FROM_FILE:
        fprintf ( stderr, "reading from file" );
        break;

      case ANIMAL_ERROR_WRITING_TO_FILE:
        fprintf ( stderr, "writing to file" );
        break;

      default:
        fprintf ( stderr, "unknown error" );
        break;
   }

   fprintf ( stderr, " %s\n", message );
   fprintf ( stderr, "Line %d of file %s\n", line, filename );
}

/**
 * \brief Default Animal error handling function.
 * \return No value.
 *
 * Call animal_err_set_reporter(animal_err_default_reporter); in your Animal 
 * program before calling any other Animal functions, if you want to use this
 * error handling routine, which just prints an error message and exits.
 */
AnimalExport void
 animal_err_default_reporter(void)
{
   int n, i;

   n = animal_err_get_error_count();
   for (i = 1; i<=n; i++)
   {
      const char *func_name, *message, *file_name;
      int code, line;

      if ((animal_err_get_error(i, &func_name, &code, &file_name, 
                             &line, &message ) != ANIMAL_EC_OK))
         fprintf(stderr, "**** animal_err_get_error unsuccessful\n");
        
      fprintf(stderr, "\nError number %d\n", i );
      print_error ( func_name, code, message, file_name, line );
   }

   abort();
}

/**
 * \}
 */

/**
 * \}
 */

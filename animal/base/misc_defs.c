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





#define ANIMAL_DONT_DEFINE_MALLOC

#include "misc_defs.h"
#include "bit_array.h"
#include "misc_error.h"
#include <string.h>

/**
 * \mainpage Animal: AN IMAging Library
 */

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonMisc Miscellaneous Definitions and Functions
 * \{
 */

#ifndef NDEBUG
/* Debug flag: off by default. Switching it on in the debugger
 * (set animal_debug=1 in gdb) turns on debugging messages.
 */
Animal_Bool animal_debug = ANIMAL_FALSE;
#endif

/**
 * \brief Array of sizes of simple types used in Gandalf/Animal.
 */
AnimalExport const size_t animal_type_sizes[] =
{
   sizeof(char),           /**< corresponding to label #ANIMAL_CHAR */
   sizeof(unsigned char),  /**< corresponding to label #ANIMAL_UCHAR */
   sizeof(short),          /**< corresponding to label #ANIMAL_SHORT */
   sizeof(unsigned short), /**< corresponding to label #ANIMAL_USHORT */
   sizeof(int),            /**< corresponding to label #ANIMAL_INT */
   sizeof(unsigned int),   /**< corresponding to label #ANIMAL_UINT */
   sizeof(long),           /**< corresponding to label #ANIMAL_LONG */
   sizeof(unsigned long),  /**< corresponding to label #ANIMAL_ULONG */
#if (ANIMAL_SIZEOF_LONG_LONG != 0)
   sizeof(long long),      /**< corresponding to label #ANIMAL_LONGLONG */
#endif
   sizeof(float),          /**< corresponding to label #ANIMAL_FLOAT */
   sizeof(double),         /**< corresponding to label #ANIMAL_DOUBLE */
   sizeof(long double),    /**< corresponding to label #ANIMAL_LONGDOUBLE */
   sizeof(char *),         /**< corresponding to label #ANIMAL_STRING */
   sizeof(Animal_Bool),       /**< corresponding to label #ANIMAL_BOOL */
   sizeof(void *)          /**< corresponding to label #ANIMAL_POINTER */
};

#ifdef ANIMAL_MALLOC_DEBUG

#include <animal/base/linked_list.h>

typedef enum { ANIMAL_MALLOC, ANIMAL_CALLOC, ANIMAL_DOUBLELOC } Animal_MallocType;
typedef struct Animal_MallocStackStruct
{
   int line;
   char *file;
   struct Animal_MallocStackStruct *next;
} Animal_MallocStackStruct;

typedef struct Animal_MallocStruct
{
   unsigned line;
   char *file;
   Animal_MallocType type;
   void *ptr;
   size_t size, bsize;
   Animal_MallocStackStruct *stack;

   struct Animal_MallocStruct *next;
} Animal_MallocStruct;

static Animal_MallocStackStruct *stack = NULL;
static Animal_MallocStruct *list = NULL;

static void
 stack_element_free ( Animal_MallocStackStruct *stack )
{
   free ( stack->file );
   free ( stack );
}

static void
 stack_free ( Animal_MallocStackStruct *stack )
{
   if ( stack == NULL ) return;
   stack_free ( stack->next );
   stack_element_free ( stack );
}

static Animal_MallocStackStruct *
 copy_reversed_stack ( Animal_MallocStackStruct *stack )
{
   Animal_MallocStackStruct *stack_copy = NULL;

   for ( ; stack != NULL; stack = stack->next )
   {
      Animal_MallocStackStruct *new = (Animal_MallocStackStruct *)
                                   malloc(sizeof(Animal_MallocStackStruct));

      assert ( new != NULL );
      new->line = stack->line;
      new->file = (char *) malloc((strlen(stack->file)+1)*sizeof(char));
      assert ( new->file != NULL );
      strcpy ( new->file, stack->file );
      new->next = stack_copy;
      stack_copy = new;
   }      
      
   return stack_copy;
}

static void
 add_to_malloc_list ( unsigned line, const char *file, Animal_MallocType type,
                      void *ptr, size_t size, size_t bsize )
{
   Animal_MallocStruct *new = (Animal_MallocStruct *)
                           malloc(sizeof(Animal_MallocStruct));

   assert ( new != NULL );

   /* fill fields of new structure */
   new->line = line;
   new->file = (char *) malloc((strlen(file)+1)*sizeof(char));
   assert ( new->file != NULL );
   strcpy ( new->file, file );
   new->type = type;
   new->ptr = ptr;
   new->size = size*bsize;
   new->bsize = bsize;
   new->stack = copy_reversed_stack(stack);

   /* insert new block in list */
   new->next = list;
   list = new;
}

static void
 remove_from_malloc_list ( void *ptr )
{
   Animal_MallocStruct *lptr, *prev;

   /* search through list for given pointer */
   for ( prev = NULL, lptr = list; lptr != NULL;
         prev = lptr, lptr = lptr->next )
      if ( lptr->ptr == ptr )
      {
         if ( prev == NULL ) list = lptr->next;
         else                prev->next = lptr->next;

         stack_free(lptr->stack);
         free(lptr->file);
         free(lptr);
         break;
      }

   assert ( lptr != NULL );
}

/**
 * \brief Animal debugging version of \c calloc().
 * \param file The file in which \c calloc() was called
 * \param line The line number that \c calloc() was called from
 * \param nmemb The number of blocks of data to allocate
 * \param size The size of each block
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c calloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
AnimalExport void *
 animal_calloc  ( const char *file, unsigned line, size_t nmemb, size_t size )
{
   void *ptr = calloc(nmemb, size);

   add_to_malloc_list ( line, file, ANIMAL_CALLOC, ptr, nmemb, size );
   return ptr;
}

/**
 * \brief Animal debugging version of \c malloc().
 * \param file The file in which \c malloc() was called
 * \param line The line number that \c malloc() was called from
 * \param size The size of the block of memory to be allocated
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c malloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
AnimalExport void *
 animal_malloc  ( const char *file, unsigned line, size_t size )
{
   void *ptr = malloc(size);

   add_to_malloc_list ( line, file, ANIMAL_MALLOC, ptr, size, 1 );
   return ptr;
}

/**
 * \brief Animal debugging version of \c realloc().
 * \param file The file in which realloc() was called
 * \param line The line number that realloc() was called from
 * \param ptr A previously allocated memory block or NULL
 * \param size The size of the block of memory to be allocated
 * \return Pointer allocated from the heap, or \c NULL on failure.
 *
 * Puts details of the \c malloc() call into a linked list for subsequent
 * analysis of the heap.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
AnimalExport void *
 animal_realloc ( const char *file, unsigned line, void *ptr, size_t size )
{
   if ( ptr != NULL ) remove_from_malloc_list ( ptr );
   ptr = realloc(ptr,size);
   add_to_malloc_list ( line, file, ANIMAL_DOUBLELOC, ptr, size, 1 );
   return ptr;
}

/**
 * \brief Animal debugging version of \c free().
 * \param ptr A previously allocated memory block
 * \return No value.
 *
 * Removes details of a previous \c malloc(), \c calloc() or \c realloc() call
 * from a linked list.
 *
 * This function is for debug purposes only and is not thread-safe.
 */
AnimalExport void 
 animal_free ( void *ptr )
{
   remove_from_malloc_list ( ptr );
   free ( ptr );
}

/**
 * \brief Prints a report on the current heap status to \c stderr.
 * \param file A file to which to print the report or \c NULL
 * \return No value.
 *
 * Prints a report on the current heap status to the given \a file, or to
 * \c stderr if \a file is passed as \c NULL.
 */
AnimalExport Animal_Bool
 animal_heap_report(const char *file)
{
   Animal_MallocStruct *lptr;
   FILE *fp;

   if ( file == NULL )
      fp = stderr;
   else
   {
      fp = fopen ( file, "w" );
      if ( fp == NULL )
      {
         animal_err_flush_trace();
         animal_err_register ( "animal_heap_report", ANIMAL_ERROR_OPENING_FILE, "" );
         return ANIMAL_FALSE;
      }
   }
   
   for ( lptr = list; lptr != NULL; lptr = lptr->next )
   {
      Animal_MallocStackStruct *ptr;

      switch ( lptr->type )
      {
         case ANIMAL_MALLOC:
           fprintf ( fp,
                     "malloc of %d bytes (%p) at line %d of file %s\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file );
           break;

         case ANIMAL_CALLOC:
           fprintf ( fp, "calloc of %d bytes (%p) at line %d of file %s (block size %d)\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file, lptr->bsize );
           break;

         case ANIMAL_DOUBLELOC:
           fprintf ( fp,
                     "realloc of %d bytes (%p) at line %d of file %s\n",
                     lptr->size, lptr->ptr, lptr->line, lptr->file );
           break;

         default:
           assert(0);
           break;
      }

      for ( ptr = lptr->stack; ptr != NULL; ptr = ptr->next )
         fprintf ( fp, "  called from line %d of file %s\n",
                   ptr->line, ptr->file );
   }

   /* success */
   if ( file != NULL ) fclose(fp);
   return ANIMAL_TRUE;
}

AnimalExport void
 animal_heap_push_fileline ( const char *file, int line )
{
   Animal_MallocStackStruct *new = (Animal_MallocStackStruct *)
                                malloc(sizeof(Animal_MallocStackStruct));

   assert ( new != NULL );
   new->file = (char *) malloc(strlen(file)+1);
   new->line = line;
   assert ( new->file != NULL );
   strcpy ( new->file, file );
   new->next = stack;
   stack = new;
}

AnimalExport void
 animal_heap_pop(void)
{
   Animal_MallocStackStruct *next;

   assert ( stack != NULL );
   next = stack->next;
   stack_element_free ( stack );
   stack = next;
}

#endif /* #ifdef ANIMAL_MALLOC_DEBUG */

/* define memcpy if it is not part of the C library */
#ifndef ANIMAL_HAVE_MEMCPY
AnimalExport void *
 memcpy ( void *dest, const void *src, size_t n )
{
   char *chdest = (char *)dest, *chsrc = (char *)src;
   int intn;
   
   for ( intn = (int)n-1; intn >= 0; intn-- )
      *chdest++ = *chsrc++;

   return dest;
}
#endif /* #ifndef HAVE_MEMCPY */

/**
 * \}
 */

/**
 * \}
 */

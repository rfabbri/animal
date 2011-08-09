/*
 * -------------------------------------------------------------------------
 * ANIMAL - ANIMAL IMage Processing LibrarY
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------------
 */ 
  

#ifndef ANIMAL_COMMON_H
#define ANIMAL_COMMON_H 1

//#ifdef HAVE_CONFIG_H
// usually put this around bellow, however when this is included in a new
// application, animal-config will not be included anymore, causing header
// errors. The right solution is to prefix HAVE_CONFIG_H with
// ANIMAL_HAVE_CONFIG_H, then append -DANIMAL_HAVE_CONFIG_H to cflags in
// animal-config script
//#endif
//# include <animal/animal-config.h>
# include "animal-config.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>


#if !ANIMAL_HAVE_HYPOT
#  define hypot(x,y) sqrt(x*x + y*y)
#endif

#if ANIMAL_HAVE_STDBOOL_H 
# include <stdbool.h> 
#else 
# if ! ANIMAL_HAVE__BOOL 
#  ifdef __cplusplus 
     typedef bool _Bool; 
#  else 
     typedef unsigned char _Bool; 
#  endif 
# endif 
# define bool _Bool 
# define false 0 
# define true 1 
# define __bool_true_false_are_defined 1 
#endif

#if defined(WIN32) &&  !defined(__CYGWIN__)
#  if defined (_DLL) && !defined(ANIMAL_DLL)
#      define ANIMAL_DLL
#  endif
#  if defined(ANIMAL_DLL)
#      if !defined(ANIMAL_LIB)
#        define AnimalExport __declspec(dllimport)
#      else
#        define AnimalExport __declspec(dllexport)
#      endif
#   else
#      define AnimalExport
#   endif
#else
#  define AnimalExport
#endif


#if defined(__cplusplus) || defined(c_plusplus)
#  define BEGIN_C_DECLS    extern "C" {
#  define END_C_DECLS      }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

/* Type definitions
 * @@@ maybe in the future use glib from GTK for common typedefs 
 */
typedef char pint8;
typedef unsigned char puint8;
typedef int pint32;
typedef unsigned puint32;
typedef puint8 pixval;    /* default pixel type */
#define PIXVAL_MAX UCHAR_MAX
#define PUINT32_MAX UINT_MAX

/* round for positive numbers: */
#define PROUND(inttype,val)  (inttype) ((val)+0.5)

#include "base/allocate.h"
#include "base/misc_error.h"

/*
 *  Convenience wrapper macros to be used inside library code.
 *  TODO: include this into Gandalf. 
 */
#define ANIMAL_MALLOC_OBJECT(ptr, obj_type, fname, retval)  do { \
   ptr = animal_malloc_object(obj_type);                       \
   if (!ptr) {                                              \
      animal_err_flush_trace();                                \
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  \
      return (retval);                                      \
   }                                                        \
   } while(0)

#define ANIMAL_MALLOC_ARRAY(ptr, obj_type, size, fname, retval) do {   \
   ptr = animal_malloc_array(obj_type,size);                   \
   if (!ptr) {                                              \
      animal_err_flush_trace();                                \
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  \
      return (retval);                                      \
   }                                                        \
   } while(0)

#define ANIMAL_REALLOC_ARRAY(ptr, obj_type, size, fname, retval) do {  \
   ptr = animal_realloc_array(obj_type, ptr, size);            \
   if (!ptr) {                                              \
      animal_err_flush_trace();                                \
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  \
      return (retval);                                      \
   }                                                        \
   } while(0)

/* 
 *  This macro is used when an error has been created by the current function
 */
#define ANIMAL_ERR_FIRST(fname, code, msg, retval)  do { \
   animal_err_flush_trace();                             \
   animal_err_register (fname, (code), msg);             \
   return (retval);                                   \
   } while(0)

/*
 * The following macros are used to test the return status of a function
 * that returns a status value. The return value of the function
 * must be stored in a variable called "stat" and the function name
 * must be stored in a variable called "fname".
 */
#define CHECK_RET_STATUS(retval) if (!stat) {         \
   animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");    \
   return (retval);                                   \
   }

#endif /* !ANIMAL_COMMON_H */

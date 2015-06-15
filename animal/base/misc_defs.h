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





#ifndef ANIMAL_MISC_DEFS_H
#define ANIMAL_MISC_DEFS_H 1

#include "../common.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonMisc
 * \{
 */

/// Boolean data type
typedef enum
{
    /// Boolean false value
    ANIMAL_FALSE=0,

    /// Boolean true value
    ANIMAL_TRUE=1
} Animal_Bool;

/* expresion to be evaluated only if NDEBUG is not defined */
#ifdef NDEBUG
#define animal_eval(x) ((void) 0)
#else
#define animal_eval(x) (x)
#endif

/* define if you want to check for memory leaks */
/* NOT THREAD-SAFE */
#ifndef ANIMAL_THREAD_SAFE
#if 0
#define ANIMAL_MALLOC_DEBUG
#endif
#endif

/// labels for simple types used throughout Animal
typedef enum
{
   ANIMAL_CHAR,       /**< signed character */
   ANIMAL_UCHAR,      /**< unsigned character */
   ANIMAL_SHORT,      /**< signed short integer */
   ANIMAL_USHORT,     /**< unsigned short integer */
   ANIMAL_INT,        /**< signed integer */
   ANIMAL_UINT,       /**< unsigned integer */
   ANIMAL_LONG,       /**< signed long integer */
   ANIMAL_ULONG,      /**< unsigned long integer */
#if (ANIMAL_SIZEOF_LONG_LONG != 0)
   ANIMAL_LONGLONG,   /**< signed extra-long integer */
#endif
   ANIMAL_FLOAT,      /**< single precision floating point */
   ANIMAL_DOUBLE,     /**< double precision floating point */
   ANIMAL_LONGDOUBLE, /**< long double precision floating point */
   ANIMAL_STRING,     /**< string (array of characters) */
   ANIMAL_BOOL,       /**< boolean */
   ANIMAL_POINTER     /**< generic pointer */
} Animal_Type;

/// array of sizeof()'s of each Animal type, one for each value in a Animal_Type
AnimalExport extern const size_t animal_type_sizes[];


/* ANIMAL_UINT8/16/32/64_MAX is the maximum value a 8/16/32/64 bit unsigned
 * type can hold. */

/* set floating-point versions of integer type limits */
#if (ANIMAL_SIZEOF_CHAR == 1)
#define ANIMAL_CHAR_MAXD 127.0
#define ANIMAL_CHAR_MAXF 127.0F
#define ANIMAL_UCHAR_MAXD 255.0
#define ANIMAL_UCHAR_MAXF 255.0F
#endif

#if (ANIMAL_SIZEOF_SHORT == 2)
#define ANIMAL_SHRT_MAXD 32767.0
#define ANIMAL_SHRT_MAXF 32767.0F
#define ANIMAL_USHRT_MAXD 65535.0
#define ANIMAL_USHRT_MAXF 65535.0F
#elif (ANIMAL_SIZEOF_SHORT == 4)
#define ANIMAL_SHRT_MAXD 2147483647.0
#define ANIMAL_SHRT_MAXF 2147483647.0F
#define ANIMAL_USHRT_MAXD 4294967295.0
#define ANIMAL_USHRT_MAXF 4294967295.0F
#elif (ANIMAL_SIZEOF_SHORT == 8)
#define ANIMAL_SHRT_MAXD 9.2233720368547758e18
#define ANIMAL_SHRT_MAXF 9.2233720368547758e18F
#define ANIMAL_USHRT_MAXD 1.8446744073709552e19
#define ANIMAL_USHRT_MAXF 1.8446744073709552e19F
#endif

#if (ANIMAL_SIZEOF_INT == 2)
#define ANIMAL_INT_MAXD 32767.0
#define ANIMAL_INT_MAXF 32767.0F
#define ANIMAL_UINT_MAXD 65535.0
#define ANIMAL_UINT_MAXF 65535.0F
#elif (ANIMAL_SIZEOF_INT == 4)
#define ANIMAL_INT_MAXD 2147483647.0
#define ANIMAL_INT_MAXF 2147483647.0F
#define ANIMAL_UINT_MAXD 4294967295.0
#define ANIMAL_UINT_MAXF 4294967295.0F
#elif (ANIMAL_SIZEOF_INT == 8)
#define ANIMAL_INT_MAXD 9.2233720368547758e18
#define ANIMAL_INT_MAXF 9.2233720368547758e18F
#define ANIMAL_UINT_MAXD 1.8446744073709552e19
#define ANIMAL_UINT_MAXF 1.8446744073709552e19F
#endif

#if (ANIMAL_SIZEOF_LONG == 4)
#define ANIMAL_ULONG_MAXD 4294967295.0
#define ANIMAL_ULONG_MAXF 4294967295.0F
#elif (ANIMAL_SIZEOF_LONG == 8)
#define ANIMAL_LONG_MAXD 9.2233720368547758e18
#define ANIMAL_LONG_MAXF 9.2233720368547758e18F
#define ANIMAL_ULONG_MAXD 1.8446744073709552e19
#define ANIMAL_ULONG_MAXF 1.8446744073709552e19F
#endif

/* 8-bit integer types */
#if (ANIMAL_SIZEOF_CHAR == 1)
typedef          char animal_i8;
typedef unsigned char animal_ui8;
#define ANIMAL_INT8  ANIMAL_CHAR
#define ANIMAL_UINT8 ANIMAL_UCHAR
#define ANIMAL_I8_MIN  CHAR_MIN
#define ANIMAL_I8_MAX  CHAR_MAX
#define ANIMAL_UI8_MAX UCHAR_MAX
#define ANIMAL_UI8_MAXD ANIMAL_UCHAR_MAXD
#define ANIMAL_UI8_MAXF ANIMAL_UCHAR_MAXF
#endif /* #if (ANIMAL_SIZEOF_CHAR == 1) */

/* 16-bit integer types */
#if (ANIMAL_SIZEOF_SHORT == 2)
typedef          short animal_i16;
typedef unsigned short animal_ui16;
#define ANIMAL_INT16  ANIMAL_SHORT
#define ANIMAL_UINT16 ANIMAL_USHORT
#define ANIMAL_I16_MIN  SHRT_MIN
#define ANIMAL_I16_MAX  SHRT_MAX
#define ANIMAL_UI16_MAX USHRT_MAX
#define ANIMAL_UI16_MAXD ANIMAL_USHRT_MAXD
#define ANIMAL_UI16_MAXF ANIMAL_USHRT_MAXF
#elif (ANIMAL_SIZEOF_INT == 2)
typedef          int animal_i16;
typedef unsigned int animal_ui16;
#define ANIMAL_INT16  ANIMAL_INT
#define ANIMAL_UINT16 ANIMAL_UINT
#define ANIMAL_I16_MIN  INT_MIN
#define ANIMAL_I16_MAX  INT_MAX
#define ANIMAL_UI16_MAX UINT_MAX
#define ANIMAL_UI16_MAXD ANIMAL_UINT_MAXD
#define ANIMAL_UI16_MAXF ANIMAL_UINT_MAXF
#endif /* #if (ANIMAL_SIZEOF_SHORT == 2) */

/* 32-bit integer types */
#if (ANIMAL_SIZEOF_INT == 4)
typedef          int animal_i32;
typedef unsigned int animal_ui32;
#define ANIMAL_INT32  ANIMAL_INT
#define ANIMAL_UINT32 ANIMAL_UINT
#define ANIMAL_I32_MIN  INT_MIN
#define ANIMAL_I32_MAX  INT_MAX
#define ANIMAL_UI32_MAX UINT_MAX
#define ANIMAL_UI32_MAXD ANIMAL_UINT_MAXD
#define ANIMAL_UI32_MAXF ANIMAL_UINT_MAXF
#elif (ANIMAL_SIZEOF_LONG == 4)
typedef          long animal_i32;
typedef unsigned long animal_ui32;
#define ANIMAL_INT32  ANIMAL_LONG
#define ANIMAL_UINT32 ANIMAL_ULONG
#define ANIMAL_I32_MIN  LONG_MIN
#define ANIMAL_I32_MAX  LONG_MAX
#define ANIMAL_UI32_MAX ULONG_MAX
#define ANIMAL_UI32_MAXD ANIMAL_ULONG_MAXD
#define ANIMAL_UI32_MAXF ANIMAL_ULONG_MAXF
#endif /* #if (ANIMAL_SIZEOF_INT == 4) */

/* 64-bit unsigned type */
#if (ANIMAL_SIZEOF_INT == 8)
typedef          int animal_i64;
typedef unsigned int animal_ui64;
#define ANIMAL_INT64  ANIMAL_INT
#define ANIMAL_UINT64 ANIMAL_UINT
#define ANIMAL_I64_MIN  INT_MIN
#define ANIMAL_I64_MAX  INT_MAX
#define ANIMAL_UI64_MAX UINT_MAX
#define ANIMAL_UI64_MAXD ANIMAL_UINT_MAXD
#define ANIMAL_UI64_MAXF ANIMAL_UINT_MAXF
#elif (ANIMAL_SIZEOF_LONG == 8)
typedef          long animal_i64;
typedef unsigned long animal_ui64;
#define ANIMAL_INT64  ANIMAL_LONG
#define ANIMAL_UINT64 ANIMAL_ULONG
#define ANIMAL_I64_MIN  LONG_MIN
#define ANIMAL_I64_MAX  LONG_MAX
#define ANIMAL_UI64_MAX ULONG_MAX
#define ANIMAL_UI64_MAXD ANIMAL_ULONG_MAXD
#define ANIMAL_UI64_MAXF ANIMAL_ULONG_MAXF

#if 0 /* removed by PM: not sure using 64-bit words on PCs is a good idea... */
#elif (ANIMAL_SIZEOF_LONG_LONG == 8 && ANIMAL_SIZEOF_LONG < 8)
typedef          long long animal_i64;
typedef unsigned long long animal_ui64;

/* ULONGLONG_MAX not normally defined, but we know the value anyway */
#define ANIMAL_UI64_MAX 0xffffffffffffffff
#endif

#endif /* #if (ANIMAL_SIZEOF_INT == 8) */

/* check sizes of objects we just defined */
#ifdef ANIMAL_UI8_MAX
#if (ANIMAL_UI8_MAX != 0xff)
#error Inconsistent 8-bit integer size
#endif
#endif

#ifdef ANIMAL_UI16_MAX
#if (ANIMAL_UI16_MAX != 0xffff)
#error Inconsistent 16-bit integer size
#endif
#endif

#ifdef ANIMAL_UI32_MAX
#if (ANIMAL_UI32_MAX != 0xffffffff)
#error Inconsistent 32-bit integer size
#endif
#endif

#ifdef ANIMAL_UI64_MAX
#if (ANIMAL_UI64_MAX != 0xffffffffffffffff)
#error Inconsistent 64-bit integer size
#endif
#endif

/**
 * \brief Macro: Applies test and aborts program on false result with a message.
 * \param expr The test expression to be evaluated
 * \param message The message printed when the test fails
 * \return No value.
 *
 * Evaluates a test expression and aborts the program if the expression \a expr
 * evaluates as false, printing the given error \a message.
 * Ignored if \c NDEBUG is defined.
 */
#ifdef ANIMAL_GENERATE_DOCUMENTATION
void animal_assert ( Animal_Bool expr, const char*message );
#else
#ifndef NDEBUG
#define animal_assert(expr, message) \
    if (!(expr)) { \
           fprintf(stderr, "Assertion (%s) failed: %s\n File %s(%d).\n", \
                  #expr, message, __FILE__, __LINE__); \
           abort(); }
#else
#define animal_assert(expr, mes) ((void) 0)
#endif
#endif /* #ifdef ANIMAL_GENERATE_DOCUMENTATION */

#ifndef NDEBUG
/* debug flag; can be set interactively in debugger to switch on
 * various messages */
AnimalExport extern Animal_Bool animal_debug;
#endif

/* in case you want to check the status of your malloc()/free() calls... */
#ifdef ANIMAL_MALLOC_DEBUG

#ifndef ANIMAL_DONT_DEFINE_MALLOC
#define malloc(n) animal_malloc(__FILE__,__LINE__,n)
#define free animal_free
#define calloc(n,bs) animal_calloc(__FILE__,__LINE__,n,bs)
#define realloc(ptr,n) animal_realloc(__FILE__,__LINE__,ptr,n)
#endif

void *
   animal_calloc   ( const char *file, unsigned line,
                    size_t nmemb, size_t size ),
   *animal_malloc  ( const char *file, unsigned line, size_t size ),
   *animal_realloc ( const char *file, unsigned line, void *ptr, size_t size ),
   animal_free     ( void *ptr ),
   animal_heap_push_fileline ( const char *file, int line ),
   animal_heap_pop(void);

AnimalExport Animal_Bool animal_heap_report(const char *file);
#define animal_heap_push() animal_heap_push_fileline(__FILE__,__LINE__)

#else
#define animal_heap_report(f)  ((void) 0)
#define animal_heap_push()    ((void) 0)
#define animal_heap_pop()     ((void) 0)
#endif /* #ifdef ANIMAL_MALLOC_DEBUG */

/* check whether memcpy is part of the standard C library */
#ifndef ANIMAL_HAVE_MEMCPY
AnimalExport void *memcpy ( void *dest, const void *src, size_t n );
#endif

/* Necessary for Visual C++ compilation */
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#ifndef M_SQRT3
#define M_SQRT3 1.73205080756887719317
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

/* extra useful constants */
#define ANIMAL_ONE_THIRD   0.3333333333333333333333
#define ANIMAL_ONE_THIRD_F 0.3333333333333333333333F

/* powf and sqrtf are not available on all platforms: */
#ifndef HAVE_POWF
#define powf(x,y) (float)pow((float)(x), (float)(y))
#endif

#ifndef HAVE_FABSF
#define fabsf(x) (float)fabs((float)(x))
#endif

#ifndef HAVE_SQRTF
#define sqrtf(x) (float)sqrt((float)(x))
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

#endif /* #ifndef _ANIMAL_MISC_DEFS_H */

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
  

#ifndef UTIL_H
#define UTIL_H 1

#include "common.h"
#include <float.h>   


BEGIN_C_DECLS
/* Data types */ 

typedef struct {
   int x;
   int y;
} point;

typedef struct {
   double x;
   double y;
} point_double;

#define ASSIGN_POINTS(to,from) (to).x = (from).x; (to).y = (from).y

typedef struct _curve {
   double *X;
   double *Y;
   unsigned n;
} curve;

/* animal_error messages */
#define MEMERROR1 "Cannot allocate memory."

/* 
 *  General Utilities 
 */
#define MAX(x,y) ( (x > y)?(x):(y) )
#define MIN(x,y) ( (x < y)?(x):(y) )
#define non_negative_dbl(x) ( x>0 || fabs(x) < DBL_EPSILON )
#define non_positive_dbl(x) ( x<0 || fabs(x) < DBL_EPSILON )
#define non_zero_dbl(x) ( fabs(x) >= DBL_EPSILON )

/* Circular increment mod "m" */
#define incr(x,n,m) ( (x + n) % m )

/* (Row, Col) indexing of 1D arrays */
#define RCbyR(a,i,j,ncols) a[(j) + (i)*ncols]
#define RCbyC(a,i,j,nrows) a[(i) + (j)*nrows]
#define RC3DbyC(a,i,j,k,rows,cols) a[(i) + (j)*(rows) + \
                                  (k) * (rows)*(cols)]


/* Converts linear index "n" to (row,col) index */
#define RIND2(n,nrow) ( (n)%(nrow) )
#define CIND2(n,nrow) ( (n)/(nrow) )
#define RIND2byR(n,ncol) ( (n)/(ncol) ) 
#define CIND2byR(n,ncol) ( (n)%(ncol) )

/* print error message and halt */
AnimalExport void 
   animal_error(char *what,char *where),
   free_double_m(double ***M, int row),
   mply2dfloat(float *M1, int r1, float *M2, int c2, int col, float *M1M2),
   free_curve(curve **c),
   print_curve(curve *c),
   show_int_v(int *v, int dim),
   rgb2hsv(float r, float g, float b, float *h, float *s, float *v),
   hsv2rgb(float h, float s, float v, float *r, float *g, float *b)
   ;
                                
/* swap a and b */
AnimalExport inline void 
   intswap(int *a, int *b); 

AnimalExport double
   **alloc_double_m(int row, int col);


AnimalExport curve
   *new_curve(unsigned n);

END_C_DECLS

#endif /* !UTIL_H */

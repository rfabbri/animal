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
  

#include "array.h"
#include <stddef.h>
#include <stdlib.h>

/*
    General routines for working with 2d arrays. These are almost like images, but
    just carry the "nrows" and "ncols" attributes. Therefore these are more
    adequate for intermediate/auxiliary usage.
*/

AnimalExport DblM *
new_dbl_m(int nrows, int ncols)
{
   DblM *m=NULL;
   int i;

   /* --- Allocations --- */
   m = (DblM *) malloc(sizeof(DblM));
   if (m == NULL) return NULL;

   m->data = (double *) calloc(nrows*ncols, sizeof(double));
   if (m->data == NULL) return NULL;

   m->lut = (int *) calloc(nrows, sizeof(int));
   if (m->lut == NULL) return NULL;

   /* --- Settings --- */
   for (i=0; i<nrows; i++)
      m->lut[i] = i*ncols;
   m->rows = nrows;
   m->cols = ncols;

   return m;
}


AnimalExport void 
free_dbl_m(DblM **m)
{
   DblM *aux;

   aux = *m;
   if (aux != NULL) { 
      if (aux->data != NULL)  
         free(aux->data);
      if (aux->lut != NULL)  
         free(aux->lut);
      free(aux);
      *m = NULL;
   }
}

AnimalExport int 
normal_dbl_m(DblM *m, double lower, double upper)
{
   double max, min, denom;
   int i;
   double ratio;

   max = max_dbl_m(m);
   min = min_dbl_m(m);
   denom = max-min;
   if (denom == 0) 
      return 0;
   
   ratio = upper-lower / denom;
   for (i=0; i<m->rows*m->cols; i++)
      m->data[i] = lower + ratio*(m->data[i] - min) ;

   return 1;
}

AnimalExport double
max_dbl_m(DblM *m)
{
   int i;
   double max;

   max = m->data[0];
   for (i=1; i < m->rows*m->cols; i++) 
      max = (m->data[i] > max)? m->data[i] : max;

   return max;
}

AnimalExport double
min_dbl_m(DblM *m)
{
   int i;
   double min;

   min = m->data[0];
   for (i=1; i < m->rows*m->cols; i++) 
      min = (m->data[i] < min)? m->data[i] : min;

   return min;
}

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
  

#ifndef ANIMAL_ARRAY_H
#define ANIMAL_ARRAY_H 1 

#include "common.h"
BEGIN_C_DECLS

typedef struct _dbl_m {
   int rows;
   int cols;                
   int *lut; /* Lookup table with column positions. Used for (row,col) indexing */
   double *data; 
} DblM;

AnimalExport DblM
   *new_dbl_m(int nrows, int ncols);

AnimalExport void
   free_dbl_m(DblM **m);

AnimalExport int
   normal_dbl_m(DblM *m, double lower, double upper);

AnimalExport double
   max_dbl_m(DblM *m),
   min_dbl_m(DblM *img);

END_C_DECLS
#endif /* !ARRAY_H */

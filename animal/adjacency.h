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
  

#ifndef ADJ_H_
#define ADJ_H_ 1
/*
#include <animal/common.h>
#include <animal/util.h>
#include <animal/img.h>
*/

#include "common.h"
#include "util.h"
#include "img.h"



BEGIN_C_DECLS

#define DEFAULT_NHOOD NULL
/* The above macro is useful to pass in any nhood
 * parameter of functions to indicate that the user
 * wants to use the default nhood natural to the function */

/* --- Globals --- */
extern int n8[8][2];
extern int n4[4][2];

typedef struct _nhood {
  int *dx;
  int *dy;
  unsigned n;
} nhood;

AnimalExport nhood
   *new_nhood(int n),
   *circular_nhood(double radius),
   *square_nhood(unsigned n),
   *get_4_nhood(),
   *get_8_nhood();

AnimalExport void 
   free_nhood(nhood **A),
   framesize(nhood *nh, int *left, int *right, int *up, int *down),
   print_nhood(nhood *nh);

AnimalExport int 
nhcount_np(Img *im, int r, int c, nhood *nh, pixval val),
nh8count_np(Img *im, int r, int c, pixval val),
crossing_index_np(Img *im, int r, int c);

END_C_DECLS

#endif /* !ADJACENCY_H */

/*
 * -------------------------------------------------------------------------
 *
 * S. E. D. R. (Sorted Euclidean Distance Representation) --  HEADER
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
 *
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
#ifndef SEDR_H
#define SEDR_H 1
#include "common.h"

BEGIN_C_DECLS

/*
   ATTENTION:
      This SEDR stores the (r,c) coordinates, NOT (x,y). 
*/
typedef struct _sedr {
   int *pt;             /* an array of point coordinates */
   unsigned *npts;      /* total number of point coordinates inside
                           disc with given distance. This i used for
                           indexing into the "pt" array */
   unsigned total_npts; /* the total #points kept in sedr */
   double *distance;    /* exact euclidean distances */
   unsigned *sqrd_dist; /* pre-computed squared distances */
   unsigned length;     /* number of distinct distances */
   double maxradius;    /* maximum distance */
} SEDR;


/* --- Globals --- */
AnimalExport extern SEDR 
   *global_sedr;
/* The user may call free_sedr on global_sedr when he's finished
   with euclidean calculations.  */


/* --- API --- */
AnimalExport SEDR
   *grow_sedr(double radius),
   *enlarge_sedr(SEDR *sdr, double radius);

AnimalExport void 
   free_sedr(SEDR **sedr),
   print_sedr(SEDR *sedr, int howmanydists);

#define SQR_AREA(r) (unsigned) ((2*round(r)+1)*(2*round(r)+1))
#define sizeof_sedr(s) s->npts[s->length-1]

END_C_DECLS
#endif /* !SEDR_H */

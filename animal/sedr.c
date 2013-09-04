/*
 * -------------------------------------------------------------------------
 *
 * S. E. D. R. (Sorted Euclidean Distance Representation) 
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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

#include "sedr.h"
#include "util.h"
#include <stdlib.h>
#include <stdlib.h>


/* Global external variables */

SEDR *global_sedr=NULL;


/* Private declarations */
static int 
   compare_v_distance(const void *a, const void *b);
static SEDR *
   new_sedr( unsigned ndists, unsigned npts, double maxradius);
static SEDR *
   realloc_sedr(  SEDR *sedr, 
                  double radius, 
                  unsigned length, 
                  unsigned npts    );


/*-----------------------------------------------------------------
 * SEDR Alocation routine
 *    -returns NULL if allocation fails
 *-----------------------------------------------------------------*/
SEDR *
new_sedr( unsigned ndists, unsigned npts, double maxradius)
{
   char *fname="new_sedr";
   SEDR *s;

   ANIMAL_MALLOC_OBJECT(s, SEDR, fname,NULL);
   ANIMAL_MALLOC_ARRAY(s->npts, unsigned, ndists, fname,NULL);
   ANIMAL_MALLOC_ARRAY(s->pt, int, 2*npts, fname,NULL);
   ANIMAL_MALLOC_ARRAY(s->distance, double, ndists, fname,NULL);
   ANIMAL_MALLOC_ARRAY(s->sqrd_dist, unsigned, ndists, fname,NULL);

   s->total_npts = npts;
   s->length = ndists;
   s->maxradius = maxradius;
   return s;
}

/*-----------------------------------------------------------------
 * SEDR Dealocation routine
 *    - returns NULL if allocation fails
 *-----------------------------------------------------------------*/
AnimalExport void 
free_sedr(SEDR **sedr)
{
   SEDR *s;

   s = *sedr;
   free(s->pt);
   free(s->npts);
   free(s->distance);
   free(s->sqrd_dist);
   free(*sedr);
}


/*-----------------------------------------------------------------
 * Routine for building the SEDR 
 *    - returns NULL if allocation fails
 *-----------------------------------------------------------------*/
AnimalExport SEDR *
grow_sedr(double radius)
{
   char *fname="grow_sedr";
   int   row, col, 
         r, c,
         irad, *pt;
   unsigned i, n, nd;
   register unsigned j;
   double **v, d;
   SEDR *sedr;


   // Decide if global_sedr may be re-used, and if it needs to be 
   // enlarged to contain grater distances.
   if (global_sedr != NULL) {
      if ( non_positive_dbl(radius - global_sedr->maxradius) )
         return global_sedr; 
      else
         return enlarge_sedr(NULL,radius);
   }

   /* 
      Allocate a vector to temporarily store point-distance information.
      This vector has dimension n x 3, where first two columns are the (r,c)
      coordinate of the n-th point, and the third column is the distance of the
      n-th point to the center of a (2*irad+1) x (2*irad+1) square.  
   */
   irad = (int) ceil(radius);
   row = col = 2*irad+1;
   v = alloc_double_m(row*col, 3); 
       if (!v) {
          animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
          return NULL;                                    
       }

   n = 0;
   for (i=0; i < (unsigned) row; i++)
      for (j=0; j < (unsigned) col; j++) {
         r = i-irad;
         c = j-irad;
         d = hypot(r,c);
         if (non_positive_dbl(d-radius)) {
            v[n][0] = r;
            v[n][1] = c;
            v[n][2] = d;
            n++;   
         }
      }

   /* sort points based on distance */
   qsort (v, n, sizeof(double *), compare_v_distance);

   radius=v[n-1][2];  // e.g. radius was 1.4, but now it's 1.4132136

   nd = 1; d = v[0][2];
   /* one pass to discover the number of different distances */
   for (i=1; i<n; i++) { 
      nd += (d != v[i][2]);
      d = v[i][2];
   }

   /* allocates SEDR */
   sedr = new_sedr(nd, n, radius);
          if (!sedr) {
             animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
             return NULL;                                    
          }
    
   pt = sedr->pt;


   /* --- Build SEDR from "v" --- */
   pt[0] = 0;
   pt[1] = 0;
   sedr->distance[0] = 0;
   sedr->sqrd_dist[0] = 0;

   j=2; d=0; nd=0; i=1;
   if (i<n) {
      do {
         if (d != v[i][2]) {
            sedr->npts[nd] = j;
            sedr->distance[++nd] = d = v[i][2];
            sedr->sqrd_dist[nd] = PROUND(unsigned, d*d);
         }
         pt[j] = v[i][0];
         pt[++j] = v[i][1];
         j++;
         i++;
   
         // unrolling
         pt[j] = v[i][0];
         pt[++j] = v[i][1];
         j++;
         i++;
   
         pt[j] = v[i][0];
         pt[++j] = v[i][1];
         j++;
         i++;
   
         pt[j] = v[i][0];
         pt[++j] = v[i][1];
         j++;
         i++;
      } while (i < n);
   }
   sedr->npts[nd] = j;
   free_double_m(&v, row*col);
   global_sedr=sedr;

   return sedr;
}


/*-----------------------------------------------------------------
 * Enlarges a previously created SEDR
 *    - if parameter sedr is NULL, enlarges global_sedr
 *    - if both are NULL, runs grows_sedr on global_sedr
 *-----------------------------------------------------------------*/
AnimalExport SEDR * 
enlarge_sedr(SEDR *sdr, double radius)
{
   char *fname="enlarge_sedr";
   int   n, 
         row, col, 
         i, j, 
         r, c, 
         nd,
         ndist, irad, max_npts;
   double **v, d, maxrad;
   SEDR *sedr;

   // if parameter sedr is NULL, enlarges global_sedr
   // if both are NULL, runs grow_sedr on global_sedr
   if (sdr == NULL) {
      if (global_sedr == NULL) {
         global_sedr=grow_sedr(radius);
         return global_sedr;
      }
      sedr = global_sedr;
   } else
      sedr = sdr;


   /* start growing on top of sedr */
   maxrad = sedr->maxradius;
   irad   = (int) ceil(radius);

   /* 
      Allocate a vector to temporarily store point-distance information.
      This vector has dimension n x 3, where first two columns are the (r,c)
      coordinate of the n-th point, and the third column is the distance of the
      n-th point to the center of a (2*irad+1) x (2*irad+1) square.  
   */
   max_npts = SQR_AREA(radius) - SQR_AREA(maxrad); 
   v = alloc_double_m(max_npts, 3);
       if (!v) {
          animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
          return NULL;                                    
       }
   n = -1;
   row = col = 2*irad+1;
   for (i=0; i < row; i++)
      for (j=0; j < col; j++) {
         r = i-irad;
         c = j-irad;
         d = hypot(r,c);
         if (non_positive_dbl(d-radius) && d > maxrad) {
            n++;   
            v[n][0] = r;
            v[n][1] = c;
            v[n][2] = d;
         }
      }

   /* sort points based on distance */
   qsort (v, ++n, sizeof(double *), compare_v_distance);

   radius=v[n-1][2];  // e.g. radius was 1.4, but now it's 1.4132136

   nd = 1; d = v[0][2];
   /* one pass to discover the number of different distances */
   for (i=1; i<n; i++) {
      nd += (d != v[i][2]);
      d = v[i][2];
   }

   ndist = sedr->length;
   sedr = realloc_sedr(sedr, radius, ndist+nd, n);
          if (!sedr) {
             animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
             return NULL;                                    
          }

   /* build SEDR from "v" */
   sedr->distance[ndist]  = d = v[0][2];
   sedr->sqrd_dist[ndist] = PROUND(unsigned,d*d);
   j = sedr->npts[ndist-1];
   i=0;
   do {
      if (d != v[i][2]) {
         sedr->npts[ndist] = j;
         sedr->distance[++ndist] = d = v[i][2];
         sedr->sqrd_dist[ndist]  = PROUND(unsigned,d*d);
      }
      sedr->pt[j] = v[i][0];
      sedr->pt[++j] = v[i][1];
      j++;
      i++;

      // unrolling
      sedr->pt[j] = v[i][0];
      sedr->pt[++j] = v[i][1];
      j++;
      i++;

      sedr->pt[j] = v[i][0];
      sedr->pt[++j] = v[i][1];
      j++;
      i++;

      sedr->pt[j] = v[i][0];
      sedr->pt[++j] = v[i][1];
      j++;
      i++;
   } while (i < n);
   sedr->npts[ndist] = j;

   free_double_m(&v, max_npts);

   return sedr;
}

/*-----------------------------------------------------------------
 *
 * Re-alocation of SEDR structure
 *
 *-----------------------------------------------------------------*/
SEDR *
realloc_sedr(SEDR *sedr, double radius, unsigned length, unsigned npts)
{
   char *fname="realloc_sedr";

   sedr->total_npts += npts;

   ANIMAL_MALLOC_ARRAY(sedr->pt, int, 2*sedr->total_npts, fname, NULL);
   ANIMAL_MALLOC_ARRAY(sedr->npts, unsigned, length, fname, NULL);
   ANIMAL_MALLOC_ARRAY(sedr->distance, double, length, fname, NULL);
   ANIMAL_MALLOC_ARRAY(sedr->sqrd_dist, unsigned, length, fname, NULL);

   sedr->length = length;
   sedr->maxradius = radius;
   return sedr;
}

/*-----------------------------------------------------------------
 * Print a SEDR to stdout. 
 *    - useful for validation of SEDR values
 *    - "howmanydists" is negative if all sedr is to be printed
 *-----------------------------------------------------------------*/
AnimalExport void
print_sedr(SEDR *sedr, int howmanydists)
{
   int i;
   unsigned j, jini;

   if (howmanydists <0 || howmanydists > (int)sedr->length)
      howmanydists = (int)sedr->length;

   printf("        Maximum distance: %g\n",sedr->maxradius);
   printf("# of different distances: %d\n",sedr->length);
   printf("       Total # of points: %d\n",sedr->total_npts);
   j = 0;
   for (i=0; i < howmanydists; i++) {
      printf("----------\n");
      printf("Dist: %f\n", sedr->distance[i]);

      jini = j;
      while (j < sedr->npts[i]) {
         printf("x: %d\ty: %d\n", sedr->pt[j], sedr->pt[j+1]);
         j+=2;
      }

      printf("Npts: %d\n",(j - jini)/2);
   }
}


/*-----------------------------------------------------------------
 *
 *  Auxiliary comparision routine to be used with qsort
 *
 *-----------------------------------------------------------------*/
int
compare_v_distance(const void *a, const void *b)
{
  const double **pa = (const double **) a;
  const double **pb = (const double **) b;

  return ( (*pa)[2] > (*pb)[2] ) - ( (*pa)[2] < (*pb)[2] );
}

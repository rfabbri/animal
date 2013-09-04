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
  

#include "adjacency.h"
#include "sedr.h"
#include <stdlib.h>

/*
 *  Globals
 */

/* 8-neighborhood coordinates
    -1  0  1
     _______
 -1 |3  4  5|
  0 |2  *  6|
  1 |1  0  7|
     -------
*/
int n8[8][2] = {
  { 0,  1}, // 0
  {-1,  1}, // 1
  {-1,  0}, // 2
  {-1, -1}, // 3
  { 0, -1}, // 4
  { 1, -1}, // 5
  { 1,  0}, // 6
  { 1,  1}  // 7
};

/* 4-neighborhood coordinates
    -1  0  1
     _______ 
 -1 |   2   |
  0 |1  *  3|
  1 |   0   |
     ------- 
*/
int n4[4][2] = {
   { 0,  1}, // 0
   {-1,  0}, // 1
   { 0, -1}, // 2
   { 1,  0}  // 3
};

static void realloc_nhood(nhood *nh, unsigned n);

AnimalExport nhood *
new_nhood(int n)
{
  nhood *A=NULL;
  char *fname="new_nhood";

  ANIMAL_MALLOC_OBJECT(A, nhood, fname,NULL);

  ANIMAL_MALLOC_ARRAY(A->dx, int, n, fname,NULL);

  ANIMAL_MALLOC_ARRAY(A->dy, int, n, fname,NULL);

  A->n  = n;

  return A;
}

void
realloc_nhood(nhood *nh, unsigned n)
{
   
   nh->dx = (int *) realloc(nh->dx, n*sizeof(int));
   assert(nh->dx != NULL); // @@@

   nh->dy = (int *) realloc(nh->dy, n*sizeof(int));
   assert(nh->dy != NULL); // @@@

   nh->n  = (int)n;
}

AnimalExport void
free_nhood(nhood **A)
{
  nhood *aux;

  aux = *A;
  if (aux != NULL) {
    if (aux->dx != NULL) 
       free(aux->dx);
    if (aux->dy != NULL) 
       free(aux->dy);
    free(aux);
    *A = NULL;
  }   
}

AnimalExport nhood *
square_nhood(unsigned n)
{
   nhood *nh;
   int i,j, min, max;
   unsigned pos;
   char *fname="square_nhood";

   assert(n % 2 == 1);
   nh = new_nhood(n*n-1);
   if (!nh) {
      animal_err_register(fname,ANIMAL_ERROR_FAILURE,"");
      return NULL;
   }

   max = (n-1)/2;
   min = -max;
   pos = 0;
   for (i=min; i<=max; ++i)
      for (j=min; j<=max; ++j)
         if (i != 0 || j != 0) {
            nh->dx[pos]   = i;
            nh->dy[pos++] = j;
         }

   return nh;
}

AnimalExport nhood *
circular_nhood(double rad)
{
   SEDR *s;
   int *dx, *dy;
   unsigned k, p, n, *pt;
   nhood *nhood;

   s = grow_sedr(rad);
      if (!s) return NULL;
   pt = s->pt;

   nhood = new_nhood( (2*ceil(rad)+1)*(2*ceil(rad)+1) );
   dx = nhood->dx;
   dy = nhood->dy;


   /* -- distance 0 -- */ 
   dy[0] = 0;
   dx[0] = 0;

   n=1;
   p=2;
   /* -- remaining distances -- */             
   for (k=1; k < s->length && non_negative_dbl(rad - s->distance[k]); k++) {
      do {
         dy[n] = pt[p];
         dx[n++] = pt[++p];
         ++p;

         // unrolling
         dy[n] = pt[p];
         dx[n++] = pt[++p];
         ++p;

         dy[n] = pt[p];
         dx[n++] = pt[++p];
         ++p;

         dy[n] = pt[p];
         dx[n++] = pt[++p];
         ++p;
      } while (p < s->npts[k]);
   }

   realloc_nhood(nhood, n);

   /* @@@ seria eficiente ordenar pontos na ordem "raster" */
   return nhood;
}

AnimalExport nhood *
get_4_nhood()
{
   nhood *nh;
   int i;
   
   nh = new_nhood(4);
   if (!nh) {
      animal_err_register ("get_4_nhood", ANIMAL_ERROR_FAILURE,""); 
      return NULL;
   }
   for (i=0; i<4; i++) {
      nh->dx[i] = n4[i][0];
      nh->dy[i] = n4[i][1];
   }
   return nh;
}

AnimalExport nhood *
get_8_nhood()
{
   nhood *nh;
   int i;
   
   nh = new_nhood(8);
   if (!nh) {
      animal_err_register ("get_8_nhood", ANIMAL_ERROR_FAILURE,""); 
      return NULL;
   }
   for (i=0; i<8; i++) {
      nh->dx[i] = n8[i][0];
      nh->dy[i] = n8[i][1];
   }
   return nh;
}

/* 
    Given a nhood, returns the frame around the image into which it fits.
*/
AnimalExport void
framesize(nhood *nh, int *left, int *right, int *up, int *down)
{
   int l, r, u, d;
   unsigned i;

   l = r = u = d = 0;
   for (i=0; i<nh->n; i++) {
      l = MIN(l,nh->dx[i]);
      r = MAX(r,nh->dx[i]);
      u = MIN(l,nh->dy[i]);
      d = MAX(r,nh->dx[i]);
   }
   *left  = -l;
   *right =  r;
   *up    = -u;
   *down  =  d;
}

/*
   This routine counts the number of neighbors of a pixel (r,c)
   in the image with value val.

   DO NOT provide (r,c) in 1-pix-wide frame of the image. It will
   cause a segfault.

*/ 
AnimalExport int 
nhcount_np(Img *im, int r, int c, nhood *nh, pixval val) 
{
   unsigned i, n=0;

   for (i=0; i < nh->n; i++)
      if (RC(im, r + nh->dy[i], c + nh->dx[i]) == val)
         n++;

   return n;
}

AnimalExport int 
nh8count_np(Img *im, int r, int c, pixval val) 
{
   int i,n=0;

   for (i=0; i < 8; i++)
      if (RC(im, r + n8[i][1], c + n8[i][0]) == val)
         n++;

   return n;
}

/*
   This function tells the number N of regions that would exist if the
   pixel P(r,c) where changed from FG to BG. It returns 2*N, that is,
   the number of pixel transitions in the neighbourhood.
*/
AnimalExport int
crossing_index_np(Img *im, int r, int c)
{
   int n=0, i, idx,next8[8] = {1,2,3,4,5,6,7,0};
   pixval curr;

   curr=RC(im, r+n8[0][1], c+n8[0][0]);
   for (i=0,idx=0;  i<=8;  i++,idx=next8[idx]) {
      if (RC(im, r + n8[idx][1], c + n8[idx][0]) != curr ) {
         n++;
         curr = RC(im, r + n8[idx][1],c + n8[idx][0]);
      }
   }

   return n;
}

AnimalExport void
print_nhood(nhood *nh)
{
   unsigned i;
   printf("dx\tdy\n");
   for (i=0; i< (unsigned)nh->n; ++i)
      printf("%d\t%d\n", nh->dx[i], nh->dy[i]);
}

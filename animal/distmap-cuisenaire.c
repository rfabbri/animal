/*
 * =============================================================================
 *
 * O.   C U I S E N A I R E's    D I S T A N C E    M A P S
 *
 *
 * This is an enhancement on Olivier Cuisenaire's code published in his
 * homepage:
 *
 *    http://ltswww.epfl.ch/~cuisenai/DT/
 *
 * Reference for the algorithm: 
 * 
 *  O. Cuisenaire and B. Macq, "Fast and exact signed Euclidean distance 
 *  transformation with linear complexity", ICASSP'99 - IEEE Intl Conference
 *  on Acoustics, Speech and Signal Processing, Vol. 6, pp. 3293-3296. 
 *  
 * Reference for the implementation:
 *
 *  Chapter 3 of "Distance transformations: fast algorithms and applications 
 *  to medical image processing", Olivier Cuisenaire's Ph.D. Thesis, October 
 *  1999, Université catholique de Louvain, Belgium.
 *
 * SOME IMPROVEMENTS OVER THE ORIGINAL:
 *    - The original is only suited for square N x N images; this one is
 *    suited for non-square images.
 *    - Improved error treatment
 *    - Improved code documentation
 *    - More descriptive variable names
 *
 * $Revision: 1.4 $ $Date: 2010-04-19 20:13:21 $
 *
 * ANIMAL - ANIMAL IMage Processing LibrarY
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri <rfabbri@users.sourceforge.net>
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
 * =============================================================================
 */ 

#include "distmaps.h"

/* 
 * Internal structures / types
 */


#define CHUNK_SIZE 16  /* chunk size to alloc memory for lists */

#define MIN_ERROR_4SED 2
#define MIN_ERROR_8SED 116
#define NUMBER_OF_MASKS 36
puint32 edt_min_error[NUMBER_OF_MASKS] = { 2, 116, 520, 2017, 4610, 10600,
  18752, 34217, 52882, 84676, 120392, 167248, 238130, 314689, 426400, 567025,
  680576, 912052, 1073250, 1394801, 1669282, 2047816, 2411840, 2745585, 3378290,
  4012100, 4494386, 5405737, 6150482, 6817284, 7700552, 9247841, 10355650,
  11799412, 13126280, 14845825};

/*puint32 edt_min_error[NUMBER_OF_MASKS] = {*/
/*   1,      100,    120,     1017,    1610,    10600,   11752,  14217,*/
/*   12882,  14676,  100392,  107248,  138130,  114689,  126400, 167025,*/
/*   180576, */
/*   212052, 973250, 1194801, 1169282, 1247816, 1411840, 1745585, */
/*   2378290, */
/*   2912100};*/


/*puint32 edt_min_error[NUMBER_OF_MASKS] = {*/
/*   1,1,    1,1,    1,1,   1,1,*/
/*   1,1,  1,1,  1,1,  1,1,*/
/*   1, */
/*   1,1, 1,1, 1,1, 1, */
/*   1, 100000000};*/

/* TODO: increase number of masks for perfect EDT w/huge images */

/* Cuisenaire's original code:
 *
#define MIN_ERROR_4SED 2
#define MIN_ERROR_8SED 116
#define NUMBER_OF_MASKS 18
int min_error[NUMBER_OF_MASKS] = { 2,116,520,2017,4610,10600,18752,34217,52882,
				   84676,120392,167248,238130,314689,426400,
				   567025,680576,912052};
 * */


/* 
 * LIST STRUCTURE: we have a "bucket" array called edt_master_list. 
 * At each position of edt_master_list, say edt_master_list[i], we have a list
 * of points. The points are allocated in chunks of size CHUNK_SIZE.
 * Inside a chunk, we have an ordinary static list; 
 * Two or more chunks are liked together forming a dynamic liked list.
 * */
typedef struct grid *ptrgrid;

typedef struct grid { 
   int x[CHUNK_SIZE], y[CHUNK_SIZE];
   int dx[CHUNK_SIZE], dy[CHUNK_SIZE];
   int cnt;
   ptrgrid nxt;
} grid;

ptrgrid *edt_master_list;  /* Global list */


static bool 
   cuisenaire_initmasterlist(ImgPUInt32 *im, puint32 *max, puint32 **sq),
   cuisenaire_addtolist(int x, int y, int dx, int dy, puint32 val),
   edt_cuisenaire_p4sed(ImgPUInt32 *im, puint32 *max, puint32 *sq),
   edt_cuisenaire_p8sed(ImgPUInt32 *im, puint32 max, puint32 *sq),
   edt_cuisenaire_pNxN(ImgPUInt32 *im, puint32 max, puint32 *sq),
   edt_cuisenaire_poNxN(ImgPUInt32 *im, puint32 max, puint32 *sq);

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Cuisenaire's simple 4-neighbor approximate EDT by
 *               propagation.
 *
 * INPUT
 *    - im: binary image in puint32 storage.
 *    
 * OUTPUT 
 *    - im: distance map, where im(px,py) = min distance from (px,py)
 *    to the pixels with value 0 in the input.
 *    - return value: true if everything went ok, false otherwise.
 *    
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_psn4(ImgPUInt32 *im)
{
   puint32 *sq, max_dist = (puint32) -1;
   char *fname="edt_cuisenaire_psn4";
   bool stat;

   assert(im->isbinary);

   stat = cuisenaire_initmasterlist(im, &max_dist, &sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p4sed(im, &max_dist, sq);
   CHECK_RET_STATUS(false);

   free(sq-2*MAX(im->rows,im->cols));
   free(edt_master_list);

   im->isbinary = false;
   return true;
}

/* TODO: max_dist variants for other algorithms beyond PMON
AnimalExport bool
edt_cuisenaire_psn4(ImgPUInt32 *im)
{
  return edt_cuisenaire_psn4_max_dist(im, (puint32) -1);
}
*/

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Cuisenaire's simple 8-neighbor approximate EDT by
 *               propagation.
 *
 * INPUT
 *    - im: binary image in puint32 storage.
 *    
 * OUTPUT 
 *    - im: distance map, where im(px,py) = min distance from (px,py)
 *    to the pixels with value 0 in the input.
 *    - return value: true if everything went ok, false otherwise.
 *    
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_psn8(ImgPUInt32 *im)
{
   puint32 *sq, max_dist = (puint32) -1;
   char *fname="edt_cuisenaire_psn8";
   bool stat;

   assert(im->isbinary);

   stat = cuisenaire_initmasterlist(im, &max_dist, &sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p4sed(im, &max_dist, sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p8sed(im, max_dist, sq);
   CHECK_RET_STATUS(false);

   free(sq-2*MAX(im->rows,im->cols));
   free(edt_master_list);

   im->isbinary = false;
   return true;
}

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Cuisenaire's multiple neighborhood exact EDT by
 *               propagation.
 *
 * INPUT
 *    - im: binary image in puint32 storage.
 *    - max_dist: maximum (squared) distance to be computed; if (puint32)-1, a
 *    representation of infinity, then this is the maximum possible distance 
 *    
 * OUTPUT 
 *    - im: distance map, where im(px,py) = min distance from (px,py)
 *    to the pixels with value 0 in the input.
 *    - return value: true if everything went ok, false otherwise.
 *    
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_pmn_max_dist(ImgPUInt32 *im, puint32 max_dist)
{
   puint32 *sq;
   char *fname="edt_cuisenaire_psn";
   bool stat;

/*   printf("*****  HERE!!! ************ \n");*/
   assert(im->isbinary);

   stat = cuisenaire_initmasterlist(im, &max_dist, &sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p4sed(im, &max_dist, sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p8sed(im, max_dist, sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_pNxN(im, max_dist, sq);
   CHECK_RET_STATUS(false);

   free(sq-2*MAX(im->rows,im->cols));
   free(edt_master_list);

   im->isbinary = false;
   return true;
}

/*----FUNCTION----------------------------------------------------------------
 *
 * Description: Shorthand for edt_cuisenaire_pmn_max_dist to compute all
 * possible distances
 *
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_pmn(ImgPUInt32 *im)
{
  return edt_cuisenaire_pmn_max_dist(im, (puint32) -1);
}

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Cuisenaire's multiple ORIENTED neighborhood exact EDT by
 *               propagation.
 *
 * INPUT
 *    - im: binary image in puint32 storage.
 *    - max_dist: maximum (squared) distance to be computed; if (puint32)-1, a
 *    representation of infinity, then this is the maximum possible distance
 *    
 * OUTPUT 
 *    - im: distance map, where im(px,py) = min distance from (px,py)
 *    to the pixels with value 0 in the input.
 *    - return value: true if everything went ok, false otherwise.
 *    
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_pmon_max_dist(ImgPUInt32 *im, puint32 max_dist)
{
   puint32 *sq;
   char *fname="edt_cuisenaire_psn";
   bool stat;

   assert(im->isbinary);

   stat = cuisenaire_initmasterlist(im, &max_dist, &sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p4sed(im, &max_dist, sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_p8sed(im, max_dist, sq);
   CHECK_RET_STATUS(false);

   stat = edt_cuisenaire_poNxN(im, max_dist, sq);
   CHECK_RET_STATUS(false);

   free(sq-2*MAX(im->rows,im->cols));
   free(edt_master_list);

   im->isbinary = false;
   return true;
}

/*----FUNCTION----------------------------------------------------------------
 *
 * Description: Shorthand for edt_cuisenaire_pmon_max_dist to compute all
 * possible distances
 *
 *----------------------------------------------------------------------------*/
AnimalExport bool
edt_cuisenaire_pmon(ImgPUInt32 *im)
{
  return edt_cuisenaire_pmon_max_dist(im, (puint32) -1);
}

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description: Initialization for for Cuisenaire's propagation
 *
 * INPUT
 *    - im: binary image in puint32 storage.
 *    
 * OUTPUT 
 *    - max: maximum distance to be computed; if (puint32)-1, a representation
 *    of infinity, then this is the maximum possible distance, which is one
 *    plus the diagonal diameter of the image
 *    - ssq: lookup table for squares
 *    - return value: true if everything went ok, false otherwise.
 *    
 *----------------------------------------------------------------------------*/
bool
cuisenaire_initmasterlist(ImgPUInt32 *im, puint32 *max, puint32 **ssq)
{
   char *fname = "cuisenaire_initmasterlist";
   bool stat;

   extern ptrgrid *edt_master_list; /* global */
   ptrgrid *list_ptr;
   puint32 maxd, *pt, *dummy, *sq;
   int r=im->rows, c = im->cols, M = MAX(r,c), i,
       maxx = c-1, maxy = r-1,
       x,y;


   /* lookup table for squares */
   ANIMAL_MALLOC_ARRAY(dummy, puint32, 4*M + 1, fname, false);

   sq = dummy + 2*M;  // we want sq[0] to be the middle element of the array
                      // so that sq[-x] == sq[x] == x*x
   
   *ssq = sq;
   for (i=0; i<= 2*M; ++i)
      sq[-i] = sq[i] = i*i;


   /* list allocation */
   maxd = r*r + c*c + 1;
   ANIMAL_MALLOC_ARRAY(edt_master_list, ptrgrid, maxd + 2*c, fname, false);
                                        /* @@@ why 2*c ?? */

   /* initialization */
   for (list_ptr = edt_master_list+maxd; list_ptr >= edt_master_list; --list_ptr)
      *list_ptr=NULL;

   /* assign INFTY to all foreground pixels */
   pt = DATA(im);
   for (i=0; i< r*c; ++i)
      if(pt[i]) pt[i] = maxd;

   /* Insert in list every foreground pixel at distance 1 of the
    * background, which are foreground with at least one background
    * pixel. This is different from the paper, which unnecessarily 
    * inserts all background pixels */
   for (y=0; y <= maxy; ++y)
      for (x=0; x <= maxx; ++x, ++pt)
         if (*pt == 0) {
            if (x < maxx && pt[1]  == maxd) {
               stat = cuisenaire_addtolist(x+1,y,1,0,1);
               CHECK_RET_STATUS(false);
               pt[1] = 1;
            }
            if (x > 0    && pt[-1] == maxd) {
               stat = cuisenaire_addtolist(x-1,y,-1,0,1);
               CHECK_RET_STATUS(false);
               pt[-1] = 1;
            }
            if (y < maxy && pt[c]  == maxd) {
               stat = cuisenaire_addtolist(x,y+1,0,1,1);
               CHECK_RET_STATUS(false);
               pt[c] = 1;
            }
            if (y > 0    && pt[-c] == maxd) {
               stat = cuisenaire_addtolist(x,y-1,0,-1,1);
               CHECK_RET_STATUS(false);
               pt[-c] = 1;
            }
         }
         
   if (*max == (puint32) -1)
      *max = maxd;    /* "infinity" */
   else
      *max = *max + 1; /* one plus the maximum desired distance */

   return true;
}

/*
 * Simple propagation with 4-neighborhood
 */
bool
edt_cuisenaire_p4sed(ImgPUInt32 *im, puint32 *max, puint32 *sq)
{
   extern ptrgrid *edt_master_list; /* global */

   char *fname = "edt_cuisenaire_p4sed";
   ptrgrid tmp, old;

   int   cnt, counter=0,       /* counters */
         n_empty_list=0,
         r = im->rows, c = im->cols,
         maxx = c - 1, maxy = r - 1, 
         x, y, *X, *Y, dx, dy, *DX, *DY, *imlut;

   puint32 maxd=*max, *pt, *testpt, currdist, newdist, *imdata;

   bool propa, stat;

   imlut  = LUT(im); /* internal lookup table for 2D indexing */
   imdata = DATA(im);

   /* main loop */

   for (currdist = 1; currdist < maxd; ++currdist) {
      tmp = edt_master_list[currdist];
      edt_master_list[currdist] = NULL;

      if (tmp == NULL) {
         n_empty_list++;
         if (n_empty_list > 2*sqrt(currdist) + 1)
            maxd = currdist;
      } else {
         n_empty_list = 0;

         /* traverse the linkedlist in bucket[currdist] */
         while (tmp != NULL) {
            X = tmp->x;  Y = tmp->y;  DX = tmp->dx;  DY = tmp->dy;

            /* traverse internal static list (remember: dynamic list 
             * elements are  allocated in contiguous chunks) */
            for (cnt = tmp->cnt-1; cnt >= 0; --cnt) {
               x = X[cnt]; y = Y[cnt];
               pt = imdata + index1(y,x,imlut);
               dx = DX[cnt]; dy = DY[cnt];
               propa = false;

               /* x+1 */
               if (dx > 0) {
                  if (x < maxx) {
                     newdist = sq[dx+1] + sq[dy];
                     testpt = pt + 1;
                     if (newdist < *testpt) {
                        stat = cuisenaire_addtolist(x+1, y, dx+1, dy, newdist);
                        CHECK_RET_STATUS(false);
                        *testpt = newdist;
                        propa = true;
                     }
                  }
               /* x-1 */
               } else if (dx < 0) {
                  if (x > 0) {
                     newdist = sq[dx-1] + sq[dy];
                     testpt = pt - 1;
                     if (newdist < *testpt) {
                        stat = cuisenaire_addtolist(x-1, y, dx-1, dy, newdist);
                        CHECK_RET_STATUS(false);
                        *testpt = newdist;
                        propa = true;
                     }
                  }
               /* dx==0 only needs to be propagated transversally for
                * the first two iterations*/
               } else if (currdist == 1) {
                  if (x < maxx) {
                     testpt = pt + 1;
                     if (2 < *testpt) {
                        stat = cuisenaire_addtolist(x+1, y, 1, dy, 2);
                        CHECK_RET_STATUS(false);
                        *testpt = 2;
                        propa = true;
                     }
                  }
                  if (x > 0) {
                     testpt = pt - 1;
                     if (2 < *testpt) {
                        stat = cuisenaire_addtolist(x-1, y, -1, dy, 2);
                        CHECK_RET_STATUS(false);
                        *testpt = 2;
                        propa = true;
                     }
                  }
               }

               /* y+1 */
               if (dy > 0) {
                  if (y < maxy) {
                     newdist = sq[dx] + sq[dy+1];
                     testpt = pt + c;
                     if (newdist < *testpt) {
                        stat = cuisenaire_addtolist(x, y+1, dx, dy+1, newdist);
                        CHECK_RET_STATUS(false);
                        *testpt = newdist;
                        propa = true;
                     }
                  }
               /* y-1 */
               } else if (dy < 0) {
                  if (y > 0) {
                     newdist = sq[dx] + sq[dy-1];
                     testpt = pt - c;
                     if (newdist < *testpt) {
                        stat = cuisenaire_addtolist(x, y-1, dx, dy-1, newdist);
                        CHECK_RET_STATUS(false);
                        *testpt = newdist;
                        propa = true;
                     }
                  }
               /* dy==0 only needs to be propagated transversally for
                * the first two iterations*/
               } else if (currdist == 1) {
                  if (y < maxy) {
                     testpt = pt + c;
                     if (2 < *testpt) {
                        stat = cuisenaire_addtolist(x, y+1, dx, 1, 2);
                        CHECK_RET_STATUS(false);
                        *testpt = 2;
                        propa = true;
                     }
                  }
                  if (y > 0) {
                     testpt = pt - c;
                     if (2 < *testpt) {
                        stat = cuisenaire_addtolist(x, y-1, dx, -1, 2);
                        CHECK_RET_STATUS(false);
                        *testpt = 2;
                        propa = true;
                     }
                  }
               }

               if (!propa && currdist >= MIN_ERROR_4SED) {
                  stat = cuisenaire_addtolist(x,y,dx,dy,0);
                  /* bucket 0 is temporary */
                  CHECK_RET_STATUS(false);
                  counter++;
               }
            }
            old = tmp;
            tmp = tmp->nxt;
            X=Y=DX=DY = NULL;
            free(old);
         }
         edt_master_list[currdist] = edt_master_list[0];
         edt_master_list[0] = NULL;
      }

   }

   *max = maxd;

   return true;
}

/*
 * Propagation with direct diagonal elements; to be applied over
 * 4-nhood simple propagation (edt_cuisenaire_p4sed). It corrects
 * distances up to MIN_ERROR_8SED
 */
bool
edt_cuisenaire_p8sed(ImgPUInt32 *im, puint32 max, puint32 *sq)
{
   extern ptrgrid *edt_master_list; /* global */

   char *fname = "edt_cuisenaire_p8sed";
   ptrgrid tmp, old;

   int   cnt,
         r = im->rows, c = im->cols,
         maxx = c - 1, maxy = r - 1, cp1 = c+1, cm1 = c-1,
         x, y, *X, *Y, dx, dy, *DX, *DY, *imlut;

   puint32 *pt, *testpt, currdist, newdist, *imdata;
   bool stat;


   imlut  = LUT(im); /* internal lookup table for 2D indexing */
   imdata = DATA(im);


   /* main loop */

   for (currdist = MIN_ERROR_4SED; currdist < MIN(max,MIN_ERROR_8SED); ++currdist) {
      tmp = edt_master_list[currdist];
      edt_master_list[currdist] = NULL;

      /* traverse the linkedlist in bucket[currdist] */
      while (tmp != NULL) {
         X = tmp->x;  Y = tmp->y;  DX = tmp->dx;  DY = tmp->dy;

         /* traverse internal static list (remember: dynamic list 
          * elements are  allocated in contiguous chunks) */
         for (cnt = tmp->cnt-1; cnt >= 0; --cnt) {
            x = X[cnt]; y = Y[cnt];
            pt = imdata + index1(y,x,imlut);
            dx = DX[cnt]; dy = DY[cnt];

            if (dx > 0) {
               if (dy > 0) {
                  if (x < maxx && y < maxy) {
                     newdist = sq[dx+1] + sq[dy+1];
                     testpt = pt + cp1;
                     if (newdist < *testpt) {
                        *testpt = newdist;
                        stat = cuisenaire_addtolist(x+1, y+1, dx+1, dy+1, newdist);
                        CHECK_RET_STATUS(false);
                     }
                  }
               } else if (dy < 0  &&  x < maxx  &&  y > 0) {
                     newdist = sq[dx+1] + sq[dy-1];
                     testpt = pt - cm1;
                     if (newdist < *testpt) {
                        *testpt = newdist;
                        stat = cuisenaire_addtolist(x+1, y-1, dx+1, dy-1, newdist);
                        CHECK_RET_STATUS(false);
                     }
                  }
            } else if (dx < 0) {
               if (dy > 0) {
                  if (x > 0 && y < maxy) {
                     newdist = sq[dx-1] + sq[dy+1];
                     testpt = pt + cm1;
                     if (newdist < *testpt) {
                        *testpt = newdist;
                        stat = cuisenaire_addtolist(x-1, y+1, dx-1, dy+1, newdist);
                        CHECK_RET_STATUS(false);
                     }
                  }

               } else if (dy<0 && x>0 && y>0) {
                  newdist = sq[dx-1] + sq[dy-1];
                  testpt = pt - cp1;
                  if (newdist < *testpt) {
                     *testpt = newdist;
                     stat = cuisenaire_addtolist(x-1, y-1, dx-1, dy-1, newdist);
                     CHECK_RET_STATUS(false);
                  }

               }

            }
         }
         old = tmp;
         tmp = tmp->nxt;
         X=Y=DX=DY = NULL;
         free(old);
      }
   }
   return true;
}

bool
edt_cuisenaire_pNxN(ImgPUInt32 *im, puint32 max, puint32 *sq)
{  
   /* global variables */
   extern ptrgrid *edt_master_list; 
   extern puint32 edt_min_error[NUMBER_OF_MASKS];

   /* internal variables */

   char *fname = "edt_cuisenaire_pNxN";
   ptrgrid tmp, old;

   int   cnt, mask,
         r = im->rows, c = im->cols, 
         maxx = c - 1, maxy = r - 1,  i, j, si, sj, end_i, end_j,
         x, y, *X, *Y, dx, dy, *DX, *DY, sdx, sdy, *imlut;

   puint32 *pt, *testpt, startdist, currdist, newdist, enddist, *imdata;
   bool stat;


   imlut  = LUT(im); /* internal lookup table for 2D indexing */
   imdata = DATA(im);

   if (max > edt_min_error[NUMBER_OF_MASKS-1]) {
      /* TODO use better error treatment */
      printf("Warning: perfect map not guaranteed \n");
      printf("     max: %d max(dnp_table): %d\n",max, edt_min_error[NUMBER_OF_MASKS-1]);
      fprintf(stderr,"Warning, perfect map not guaranteed \n");
      fprintf(stderr,"     max: %d max(dnp_table): %d\n",max, edt_min_error[NUMBER_OF_MASKS-1]);

      edt_min_error[NUMBER_OF_MASKS-1]=max+1;
   }

   /* main loop */

   for (mask = 2; mask < NUMBER_OF_MASKS; mask++) {
      startdist = edt_min_error[mask-1]; enddist = edt_min_error[mask];
      if (enddist > max) 
         enddist=max;

      /* traverse buckets */
      for (currdist = startdist; currdist < enddist; ++currdist) {
         tmp = edt_master_list[currdist];
         edt_master_list[currdist] = NULL;

         /* traverse the linkedlist in bucket[currdist] */
         while (tmp != NULL) {
            X = tmp->x;  Y = tmp->y;  DX = tmp->dx;  DY = tmp->dy;

            /* traverse internal static list (remember: dynamic list 
             * elements are  allocated in static chunks) */
            for (cnt = tmp->cnt-1; cnt >= 0; --cnt) {
               x = X[cnt]; y = Y[cnt];
               pt = imdata + index1(y,x,imlut);
               dx = DX[cnt]; dy = DY[cnt];

               if(dx>0) { sdx=1; end_j=maxx-x; }
               else { sdx=-1; end_j=x; }
               if(end_j>mask) end_j=mask;

               if(dy>0) { sdy=1; end_i=maxy-y; }
               else { sdy=-1;  end_i=y; }
               if(end_i>mask) end_i=mask;

               for (i=1; i <= end_i; ++i)
                  for (j=1; j <= end_j; ++j) {
                     sj = sdx*j; si = sdy*i;  /* si,sj == signed i,j */

                     newdist = sq[dx+sj] + sq[dy+si];

                     testpt = pt + index1_mult(si,sj,c);
                     if (newdist < *testpt) {
                        *testpt = newdist;
                        stat = cuisenaire_addtolist(x+sj,y+si,dx+sj,dy+si,newdist);
                        CHECK_RET_STATUS(false);
                     }
                  }
            }
            old = tmp;
            tmp = tmp->nxt;
            X=Y=DX=DY = NULL;
            free(old);
         }
      }
   }

   return true;
}

bool
edt_cuisenaire_poNxN(ImgPUInt32 *im, puint32 maxdist, puint32 *sq)
{  
   /* global variables */
   extern ptrgrid *edt_master_list; 
   extern puint32 edt_min_error[NUMBER_OF_MASKS];

   /* internal variables */

   char *fname = "edt_cuisenaire_poNxN";
   ptrgrid tmp, old;

   int   cnt, mask, max,
         r = im->rows, c = im->cols, 
         maxx = c - 1, maxy = r - 1,  i, j, si, sj, end_main,
         x, y, *X, *Y, dx, dy, *DX, *DY, sdx, sdy, *imlut;

   puint32 *pt, *testpt, startdist, currdist, newdist, enddist, *imdata;
   bool stat;

   float adx, ady, start, step_start, end, step_end;


   imlut  = LUT(im); /* internal lookup table for 2D indexing */
   imdata = DATA(im);

   if(maxdist > edt_min_error[NUMBER_OF_MASKS-1]) {
      /* TODO use better error treatment */
      fprintf(stderr,"Warning: perfect map not guaranteed \n");
      edt_min_error[NUMBER_OF_MASKS-1]=maxdist+1;
   }
   /* main loop */

   for (mask = 2; mask < NUMBER_OF_MASKS; mask++) {
      startdist = edt_min_error[mask-1]; enddist = edt_min_error[mask];
      if (enddist > maxdist) 
         enddist=maxdist;

      /* traverse buckets */
      for (currdist = startdist; currdist < enddist; ++currdist) {
         tmp = edt_master_list[currdist];
         edt_master_list[currdist] = NULL;

         /* traverse the linkedlist in bucket[currdist] */
         while (tmp != NULL) {
            X = tmp->x;  Y = tmp->y;  DX = tmp->dx;  DY = tmp->dy;

            /* traverse internal static list (remember: dynamic list 
             * elements are  allocated in static chunks) */
            for (cnt = tmp->cnt-1; cnt >= 0; --cnt) {
               x = X[cnt]; y = Y[cnt];
               pt = imdata + index1(y,x,imlut);
               dx = DX[cnt]; dy = DY[cnt];

               /* sdx, sdy: signs of dx and dy;
                * adx, ady: absolute values of dx and dy */
               if(dx>0) { sdx=1; adx=dx; }
               else { sdx=-1; adx=-dx; }

               if(dy>0) { sdy=1; ady=dy; }
               else { sdy=-1; ady=-dy; }


               if (adx > ady) {
                  end_main = (dy > 0)? maxy - y:y;
                  if (end_main > mask) end_main = mask;

                  step_start = ady/adx;
                  step_end   = ady/(adx-1);
                  start      = -step_start;
                  end = 1;

                  max = (dx > 0)? maxx-x : x;

                  for (i=1; i <= end_main; ++i) {
                     start += step_start;
                     end   += step_end;
                     if (end > max)
                        end = max;
                     for (j=start; j <= end; ++j) {
                        sj = sdx*j; si = sdy*i;  /* si,sj == signed i,j */

                        newdist = sq[dx+sj] + sq[dy+si];
                        testpt  = pt + index1_mult(si,sj,c);
                        if (newdist < *testpt) {
                           *testpt = newdist;
                           stat = cuisenaire_addtolist(x+sj,y+si,dx+sj,dy+si,newdist);
                           CHECK_RET_STATUS(false);
                        }
                     }
                  }
               } else  /* ady >= adx */ {
                  end_main = (dx > 0)? maxx - x : x - x;
                  if (end_main > mask) end_main = mask;

                  step_start = adx/ady;
                  step_end   = adx/(ady-1);
                  start      = -step_start;
                  end = 1;

                  max = (dy > 0)? maxy-y : y;

                  for (j=1; j <= end_main; ++j) {
                     start += step_start;
                     end   += step_end;
                     if (end > max)
                        end = max;

                     for (i=start; i <= end; ++i) {
                        sj = sdx*j; si = sdy*i;  /* si,sj == signed i,j */

                        newdist = sq[dx+sj] + sq[dy+si];
                        testpt = pt + index1_mult(si,sj,c);
                        if (newdist < *testpt) {
                           *testpt = newdist;
                           stat = cuisenaire_addtolist(x+sj,y+si,dx+sj,dy+si,newdist);
                           CHECK_RET_STATUS(false);
                        }
                     }
                  }

               }
            }
            old = tmp;
            tmp = tmp->nxt;
            X=Y=DX=DY = NULL;
            free(old);
         }
      }
   }

   return true;
}


bool
cuisenaire_addtolist(int x, int y, int dx, int dy, puint32 val)
{
/* 
 * REMEMBER THE LIST STRUCTURE: we have a "bucket" array called edt_master_list.
 * At each position of edt_master_list, say edt_master_list[i], we have a list
 * of points. The points are allocated in chunks of size CHUNK_SIZE.
 * Inside a chunk, we have an ordinary static list; 
 * Two or more chunks are liked together forming a dynamic liked list.
 */
   extern ptrgrid *edt_master_list; /* global */

   ptrgrid tmp;
   int cnt;
   char *fname="cuisenaire_addtolist";


   tmp = edt_master_list[val];

   if (tmp == NULL) {  /* bucket is empty */
      ANIMAL_MALLOC_OBJECT(tmp, grid, fname, false);
      cnt = tmp->cnt = 0;
      tmp->nxt = NULL;
      edt_master_list[val] = tmp;
   } 
   else if ( (cnt = tmp->cnt) == CHUNK_SIZE) {  /* chunk is full */
      ANIMAL_MALLOC_OBJECT(tmp, grid, fname, false);
      cnt = tmp->cnt = 0;
      tmp->nxt = edt_master_list[val];
      edt_master_list[val] = tmp;
   }

   tmp->x[cnt]  = x;
   tmp->y[cnt]  = y;
   tmp->dx[cnt] = dx;
   tmp->dy[cnt] = dy;
   tmp->cnt++;
   return true;
}


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Determine the smallest location leading to an error
 *
 * - uses an exhaustive search
 * - See Cuisenaire's thesis, page 61.
 * 
 * INPUT
 *    - nhood structure N
 *    - address of output variables
 *    - start_dpx: starting x coordinate for searching error
 *    locations. If equal to 0 then a default and safe value will be
 *    used
 *    - start_dpy: starting y coordinate for searching error
 *    locations. Make sure this is smaller than start_dpx
 *
 * OUTPUT 
 *    - Derr_p: smallest possible distance where an error can occur
 *    for neighborhood N
 *    - derr = (derr_x, derr_y): the relative location where an error can occur
 *    for the given neighborhood N.
 *
 * RETURN VALUE
 *    - true if success, false in case some execution error occurs
 *----------------------------------------------------------------------------*/
bool
smallest_error_location(nhood *N, puint32 *Derr_p, 
      puint32 *derr_x, 
      puint32 *derr_y, puint32 start_dpx, puint32 start_dpy)
{
   int     dpx, dpy, dp_n_x, dp_n_y,
           dqx, dqy, dq_n_x, dq_n_y; /*, *dprod; */
           
   puint32 D, Derr, Dqn, Dpn;
   char *fname="smallest_error_location";
   unsigned i;
   bool *test;


   Derr = PUINT32_MAX;

   ANIMAL_MALLOC_ARRAY(test, bool, N->n, fname, false);
//   ANIMAL_MALLOC_ARRAY(dprod, int, N->n, fname, false);

   dpx = (start_dpx == 0) ? 1 : start_dpx;

   assert((puint32)dpx > start_dpy);

   do {
      for (dpy = start_dpy; dpy <= dpx; ++dpy) {
         D = dpx*dpx + dpy*dpy;
         if (D < Derr) {
            for (i=0; i<N->n; ++i) {
//               dprod[i] = N->dx[i] * dpx + N->dy[i] * dpy;
               test[i] = false;
            }
            // Errors in Cuisenaire's thesis, page 61: 
            // max value for dqx is wrong (can lead to square root of
            // negative values) 
            // and also the formula for dqy (which must use ceil)
            for (dqx = 0; dqx <= ceil(sqrt(D + 1)); ++dqx) { 
               dqy =(int)D + 1 - dqx*dqx;

               if (dqy < 0)
                 dqy = 0;
               else
                 dqy = ceil(sqrt((double)dqy));

               for (i=0; i< N->n; ++i) 
                  if (!test[i]) {
//                     if (dprod[i] < 0) {
                        dq_n_x = dqx + N->dx[i];
                        dq_n_y = dqy + N->dy[i];
                        dp_n_x = dpx + N->dx[i];
                        dp_n_y = dpy + N->dy[i];
                        Dqn = dq_n_x*dq_n_x + dq_n_y*dq_n_y;
                        Dpn = dp_n_x*dp_n_x + dp_n_y*dp_n_y;
                        if (Dqn <= Dpn)
                           test[i] = true;
//                     } else
//                        test[i] = true;
                  }
            }
            for (i=0; i<N->n && test[i]; ++i)
               ;

            if (i == N->n) {
               Derr = D;
               *derr_x = dpx;
               *derr_y = dpy;
            }
         }
      }
      ++dpx;
   } while ((unsigned)(dpx*dpx) <= Derr);

   free(test);
//   free(dprod);
   *Derr_p = Derr;

   return true;
}
/*
bool
smallest_error_location(nhood *N, puint32 *Derr_p, 
      puint32 *derr_x, 
      puint32 *derr_y, puint32 start_dpx, puint32 start_dpy)
{
   int     dpx, dpy, dp_n_x, dp_n_y,
           dqx, dqy, dq_n_x, dq_n_y, *dprod; 
           
   puint32 D, Derr, Dqn, Dpn;
   char *fname="smallest_error_location";
   unsigned i;
   bool *test;


   Derr = PUINT32_MAX;

   ANIMAL_MALLOC_ARRAY(test, bool, N->n, fname, false);
//   ANIMAL_MALLOC_ARRAY(dprod, int, N->n, fname, false);

   dpx = (start_dpx == 0) ? 1 : start_dpx;

   assert((puint32)dpx > start_dpy);

   do {
      for (dpy = start_dpy; dpy <= dpx; ++dpy) {
         D = dpx*dpx + dpy*dpy;
         if (D < Derr) {
            for (i=0; i<N->n; ++i) {
//               dprod[i] = N->dx[i] * dpx + N->dy[i] * dpy;
               test[i] = false;
            }
            // Errors in Cuisenaire's thesis, page 61: 
            // max value for dqx is wrong (can lead to square root of
            // negative values) 
            // and also the formula for dqy (which must use ceil)
            for (dqx = 0; dqx <= (int) sqrt(D + 1); ++dqx) { 
               dqy = ceil(sqrt(D + 1 - dqx*dqx));
               for (i=0; i< N->n; ++i) 
                  if (!test[i]) {
//                     if (dprod[i] < 0) {
                        dq_n_x = dqx + N->dx[i];
                        dq_n_y = dqy + N->dy[i];
                        dp_n_x = dpx + N->dx[i];
                        dp_n_y = dpy + N->dy[i];
                        Dqn = dq_n_x*dq_n_x + dq_n_y*dq_n_y;
                        Dpn = dp_n_x*dp_n_x + dp_n_y*dp_n_y;
                        if (Dqn <= Dpn)
                           test[i] = true;
//                     } else
//                        test[i] = true;
                  }
            }
            for (i=0; i<N->n && test[i]; ++i)
               ;

            if (i == N->n) {
               Derr = D;
               *derr_x = dpx;
               *derr_y = dpy;
            }
         }
      }
      ++dpx;
   } while ((unsigned)(dpx*dpx) <= Derr);

   free(test);
//   free(dprod);
   *Derr_p = Derr;

   return true;
}
*/


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Determines the closest non-propagating pixel and also
 * the smallest relative location where an error can occur in
 * propagation algorithms
 *
 * - This implementation is not described by Cuisenaire in his PhD
 *   thesis nor in the article for the PMN method.
 *
 * INPUT
 *    n: Describes the size of the n x n square neighborhood
 *
 * OUTPUT 
 *    Dnp: distance of the non-propagating pixel
 *    Derr: distance where an error in the EDT can occur
 *    npx_p, npy_p: pointers to the coordinates of the non-propag. pixel
 *    derr_x, derr_y: pointers to the coordinates of a possible error
 *
 * RETURN VALUE
 *    - true if success, false in case some execution error occurs
 *
 *----------------------------------------------------------------------------*/

bool 
closest_non_propagating_pixel (
      puint32 n, 
      puint32 *Dnp, puint32 *Derr, 
      puint32 *npx_p, puint32 *npy_p, 
      puint32 *derr_x, puint32 *derr_y, 
      puint32 start_dpx, puint32 start_dpy)
{
   nhood *nh;
   char *fname="closest_non_propagating_pixel";
   bool stat;
   puint32 npx, npy;
   double m;

   nh = square_nhood(n);
   if (!nh) {
      animal_err_register(fname, ANIMAL_ERROR_FAILURE,"");  
      return false;                                          
   }

   stat = smallest_error_location(nh, Derr, derr_x, derr_y, start_dpx, start_dpy);
   CHECK_RET_STATUS(false);

   assert(*derr_x != 0);
   m = (double)*derr_y / *derr_x;

   if (m < 1) {
      npx = *derr_x - (n-1)/2 - 1;
      for (npy=*derr_y-1; (double)npy/npx > m; --npy)
         ;
   } else if (m > 1) {
      npy = *derr_y - (n-1)/2 - 1;
      for (npx=*derr_x-1; (double)npy/npx < m; --npx)
         ;
   } else /* m == 1 */ {
      npx = *derr_x - (n-1)/2 - 1;
      npy = *derr_y - (n-1)/2 - 1;
   }

   *npx_p = npx;
   *npy_p = npy;
   *Dnp = npx*npx + npy*npy;

   free_nhood(&nh);
   return true;
}

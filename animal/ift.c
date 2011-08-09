/*
 * ============================================================================
 * 
 * I M A G E   F O R E S T I N G  T R A N S F O R M  (IFT)
 * 
 * $Revision: 1.1 $  $Date: 2005-07-20 19:15:47 $
 *
 * Thanks to Dr. Alexandre Falcao for his code, which aided the present one.
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
 * 
 * =============================================================================
 */

#include "ift.h"
#include "ift_pqueue.h"

/*----------------------------------------------------------------------
 *  IFT cost routines 
 *----------------------------------------------------------------------*/

/* For approximate EDT and related operators */

AnimalExport puint32 
pcost_edt(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q)
{
  puint32 dx,dy,r;
  point u,v;
  int cols;
 
  if (p==q)
    if (DATA(lambda)[p] > 0) /* seed pixel */
      return 0;
    else
      return PUINT32_MAX;
  else {
    r = get_root(aimg->pred,p);   /* FIXME: this' a little ineficient */
    cols = aimg->img->cols;
    u.x  = r%cols;  u.y = r/cols; 
    v.x  = q%cols;  v.y = q/cols;
    dx = v.x-u.x;
    dy = v.y-u.y;
    return dx*dx + dy*dy; 
  }
}

/* For regional minima */
AnimalExport puint32
pcost_ini(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q)
{
  if (p==q)
      return DATA(aimg->img)[p];
  else if (DATA(aimg->img)[p] <= DATA(aimg->img)[q])
      return  DATA(aimg->cost)[p];
  else
      return PUINT32_MAX;
}

/* For watershed (superior reconstruction) 
 * The cost of the path is the greatest arc cost. */
AnimalExport puint32
pcost_suprec(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q)
{
   if (p==q) 
      if (DATA(lambda)[p] > 0) /* seed pixel */      
         return (puint32) (DATA(handicap)[p]);
      else
         return PUINT32_MAX;
   else 
      return MAX(DATA(aimg->cost)[p], DATA(aimg->img)[q]);
}



/*----------------------------------------------------------------------
 *  IFT core propagation routines
 *----------------------------------------------------------------------*/

AnimalExport ann_img *
ift_fifo(Img *image, ImgPUInt32 *imlambda, Img *handicap, nhood *adj, path_cost pcost)
{
   puint32 *cost, *label, *pred, *lambda, tmp;
   pixval *img;
   int r,c,n, p,px,py, q,qx,qy, *idxlut;
   unsigned i;
   PQ *pq;
   ann_img *aimg;
   bool stat;
   char *fname="ift_fifo";

   aimg = new_ann_img(image);

   /* auxiliary variables for avoiding indirections */
   img    = DATA(image);
   cost   = DATA(aimg->cost);
   label  = DATA(aimg->label);
   pred   = DATA(aimg->pred);
   lambda = DATA(imlambda);
   idxlut = image->lut;

   r = aimg->img->rows;
   c = aimg->img->cols;
   n = r*c;

   pq = new_pqueue(QSIZE+1, n, cost);
      if (!pq) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }

   /* @@@ switch side? */
   for (p=0; p < n; p++) {
      pred[p]  = p;
      label[p] = lambda[p];
      cost[p]  = pcost(aimg, imlambda, handicap, p, p);
      if (cost[p] < PUINT32_MAX) {
         stat = insert_pqueue(&pq,p);
         if (!stat) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
      }
   }

   while (!is_empty_pqueue(pq)) {
      p = remove_pqueue(pq);
         if (p == NIL) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
      px = p % c;
      py = p / c;
      for (i=0; i < adj->n; i++) {
         qx = px + adj->dx[i]; 
         qy = py + adj->dy[i]; 
         if (valid_pixel(r,c,qx,qy)) {
            q  = index1(qy,qx,idxlut);
            if (cost[p] < cost[q]) {
               tmp = pcost(aimg,imlambda,handicap,p,q);
               if (tmp < cost[q]) { 
                  if (cost[q] == PUINT32_MAX) {
                     cost[q] = tmp;
                     stat = insert_pqueue(&pq,q);
                     if (!stat) {
                        animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
                        return NULL;
                     }
                  } else {
                     stat = update_pqueue(pq,q,tmp);
                     if (!stat) {
                        animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
                        return NULL;
                     }
                  }
                  pred[q] = p;
                  label[q] = label[p];
               }
            }
         }
      }
   }

   free_pqueue(&pq);
   return aimg;
}

/* LIFO tiebreaking */
AnimalExport ann_img *
ift_lifo(Img *image, ImgPUInt32 *imlambda, Img *handicap, nhood *adj, path_cost pcost)
{
   puint32 *cost, *label, *pred, *lambda, tmp;
   pixval *img;
   int r,c,n, p,px,py, q,qx,qy, *idxlut;
   unsigned i;
   PQ *pq;
   ann_img *aimg;
   bool stat;
   char *fname="ift_fifo";

   aimg = new_ann_img(image);

   /* auxiliary variables for avoiding indirections */
   img    = DATA(image);
   cost   = DATA(aimg->cost);
   label  = DATA(aimg->label);
   pred   = DATA(aimg->pred);
   lambda = DATA(imlambda);
   idxlut = image->lut;

   r = aimg->img->rows;
   c = aimg->img->cols;
   n = r*c;

   pq = new_pqueue(QSIZE+1, n, cost);
      if (!pq) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }
   set_pqueue_tiebreak(pq, IFT_LIFOBREAK);

   for (p=0; p < n; p++) {
      pred[p]  = p;
      label[p] = lambda[p];
      cost[p]  = pcost(aimg, imlambda, handicap, p, p);
      stat = insert_pqueue(&pq,p); /* insert everybody in queue */
         if (!stat) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
   }

   while (!is_empty_pqueue(pq)) {
      p = remove_pqueue(pq);
         if (p == NIL) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
      px = p % c;
      py = p / c;
      for (i=0; i < adj->n; i++) {
         qx = px + adj->dx[i]; 
         qy = py + adj->dy[i]; 
         if (valid_pixel(r,c,qx,qy)) {
            q  = index1(qy,qx,idxlut);
            if (pq->l.elem[q].tag == INSERTED) {
               tmp = pcost(aimg,imlambda,handicap,p,q);
               if (tmp <= cost[q]) { 
                     stat = update_pqueue(pq,q,tmp);
                     if (!stat) {
                        animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
                        return NULL;
                     }
                  pred[q] = p;
                  label[q] = label[p];
               }
            }
         }
      }
   }

   free_pqueue(&pq);
   return aimg;
}


/*
   Specific IFT euclidean distance propagation routine
*/
AnimalExport ann_img *
ift_euclidean_propagation(Img *image, ImgPUInt32 *imlambda, nhood *adj, char side)
{
   puint32 *Dx, *Dy, *cost, *label, *pred, *lambda;
   pixval *img;
   int r,c, p,px,py, q,qx,qy, *idxlut, 
       left,right,up,down, max_vert, max_hor, nbuck;

   unsigned *sq, dx, dy, tmp, i, n;
   PQ *pq;
	ann_img *aimg;
	bool stat;
	char *fname="ift_euclidean_propagation";


   aimg = new_ann_img(image); 

   /* auxiliary variables for avoiding indirections */
   img    = DATA(image);
   cost   = DATA(aimg->cost);
   label  = DATA(aimg->label);
   lambda = DATA(imlambda);
   pred   = DATA(aimg->pred);
   idxlut = image->lut;

   r = image->rows;
   c = image->cols;
   n  = MAX(r,c);

   ANIMAL_MALLOC_ARRAY(sq, unsigned, n, fname,NULL);
   for (i=0; i<n; i++)
      sq[i] = (unsigned)i*i;
   n = r*c;

   Dx = (puint32 *) calloc(n,sizeof(puint32));
   if (!Dx) {
      animal_err_flush_trace();                                
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"Variable Dx");  
      return false;                                          
   }

   Dy = (puint32 *) calloc(n,sizeof(puint32));
   if (!Dy) {
      animal_err_flush_trace();                                
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"Variable Dy");  
      return false;                                          
   }

   framesize(adj,&left,&right,&up,&down);
   max_vert = MAX(up,down);
   max_hor  = MAX(left,right);
   /* Number of buckets is maximum arc cost + 1 */
   nbuck = 2*((r-1)*max_vert + (c-1)*max_hor) \
           - max_vert*max_vert - max_hor*max_hor + 1;

//   nbuck = max_vert*max_vert + max_hor*max_hor + \
//                          2*((r-1)*max_vert + (c-1)*max_hor);
   pq = new_pqueue(nbuck,n,cost);
	if (!pq) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
		return NULL;
	}

   switch (side) {
      case BOTH:
         for (i=0; i<n; i++) {
            pred[i] = i;
            if (lambda[i] == 0) {
               cost[i] = PUINT32_MAX;
            } else {
               label[i] = lambda[i];
					stat = insert_pqueue(&pq,i);
               CHECK_RET_STATUS(NULL);;
            }
         }
			break;
      case INTERIOR:
         for (i=0; i<n; i++) {
				pred[i]  = i;
            if (lambda[i] == 0) {
               if (img[i] == FG)
                  cost[i] = PUINT32_MAX;
            } else { /* seed */
               label[i] = lambda[i];
					stat = insert_pqueue(&pq,i);
               CHECK_RET_STATUS(NULL);;
				}
			}
         break;
      case EXTERIOR:
         for (i=0; i<n; i++) {
				pred[i]  = i;
            if (lambda[i] == 0) {
               if (img[i] == BG)
                  cost[i] = PUINT32_MAX;
            } else { /* seed */
               label[i] = lambda[i];
					stat = insert_pqueue(&pq,i);
               CHECK_RET_STATUS(NULL);;
				}
			}
         break;
   }

   while (!is_empty_pqueue(pq)) {
      p = remove_pqueue(pq);
		if (p == NIL) {
			animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
			return NULL;
		}
      px = p % c;
      py = p / c;
      for (i=0; i < adj->n; i++) { 
         qx = px + adj->dx[i]; 
         qy = py + adj->dy[i]; 
         if (valid_pixel(r,c,qx,qy)) {
            q  = index1(qy,qx,idxlut);
            if (pq->l.elem[q].tag != REMOVED) {
               dx  = Dx[p]  + abs(px - qx);
               dy  = Dy[p]  + abs(py - qy);
               tmp = sq[dx] + sq[dy];
               if ( (tmp < cost[q]) || 
                    (tmp == cost[q] && label[p] < label[q]) ) {  /* @@@ */
                  if (pq->l.elem[q].tag == NOT_INSERTED) {
                     cost[q]  = tmp;
                     stat = insert_pqueue(&pq,q);
                     CHECK_RET_STATUS(NULL);;
						} else {
                     stat = update_pqueue(pq,q,tmp);
                     CHECK_RET_STATUS(NULL);;
                     cost[q]  = tmp;
						}
                  pred[q]  = p;
                  label[q] = label[p];
                  Dx[q] = dx;
                  Dy[q] = dy;
               }
            }
         }
      }
   }

   // Free memory
   free(sq);
   free(Dx);
   free(Dy);
   free_pqueue(&pq);
   return aimg;
}

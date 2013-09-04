/*
 * -------------------------------------------------------------------------
 *
 * S H A P E   A N A L Y S Y S   R O U T I N E S
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------------
 */ 

#include <stdlib.h>
#include <limits.h>
#include "analysis.h"
#include "segment.h"
#include "ift.h"

/*-----------------------------------------------------------------
 * Binary Contour Follower 
 *    - Extracts parametric contours from binary objects
 *    - nhood parameter may be 4 or 8 (defaults to 8)
 *    - bottom-left to upper-right coordinates
 *-----------------------------------------------------------------*/
AnimalExport int 
follow(Img *img, int **xlist, int **ylist, int nhood)
{
   int   (*path)[2],
         next8[8] = {1,2,3,4,5,6,7,0}, invert8[8]  = {4,5,6,7,0,1,2,3},
         next4[4] = {1,2,3,0},         invert4[4]  = {2,3,0,1},
         neighbours[8][2] = {},
         *next, *invert,
         cane,  cini, 
         i, j, k, a, b,
         *x=NULL, *y=NULL, t;  /* t: index into x and y */
   bool finished;

   switch (nhood) {
      case 4:
         path  = n4;
         next  = next4;
         invert= invert4;
         cane  = 1;
         break;
      default: 
         nhood = 8;
         path  = n8;
         next  = next8;
         invert= invert8;
         cane  = 2;
         break;
   }

   // looks for object
   for (k=0; (k < img->rows*img->cols) && (img->data[k] != 1); k++)
      ;

   if (k == img->rows*img->cols)  /* no object */
      return 0;

   // get 2D coordinate from index k
   i = k / img->cols;
   j = k % img->cols;
   i++; j++;

   img = impad(img,1,1,0); // Assume 0 out of the image 

   // get absolute coordinate of the neighbours
   for (k=0; k<nhood; k++) {
      neighbours[k][0] = path[k][0]+ i;
      neighbours[k][1] = path[k][1]+ j;
   }

   t=0; finished=false;
   while ( !finished ) {
      // realloc x,y 
      x = (int *)realloc(x,(t+1)*sizeof(int));
      y = (int *)realloc(y,(t+1)*sizeof(int));
      x[t] = j;
      y[t] = i;
      t++; 

      // Twist the "blindman's cane" to locate the next contour point
      cini=cane;
      do {
         cane=next[cane];
         a = neighbours[cane][0];
         b = neighbours[cane][1];
      } while (RC(img,a,b) == 0  &&  cane != cini);

      if (cane == cini)
         finished = true;
      else {
         i += path[cane][0];
         j += path[cane][1];
         for (k=0; k<nhood; k++) {  // Move to a new contour point
            neighbours[k][0] += path[cane][0];
            neighbours[k][1] += path[cane][1];
         }
         cane = invert[cane];
         finished =  j==x[0] && i==y[0];
      }

   }

   for (i=0; i<t; i++) {  // bottom-left to upper-right coordinates
      x[i] -= 1;
      y[i] = img->rows - y[i] -2; 
   }

   *xlist = x;
   *ylist = y;

   imfree(&img);
   return t;
}


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Multiscale Skeletonization
 *
 *    - use im2bw to threshold result.
 *    - use msskl_filter to filter shape.
 *    - opt may be INTERIOR or BOTH
 * 
 * INPUT
 *    - image: input binary image
 *    - label: labeled seeds from which to start propagation.  Seeds have value 
 *    greater than zero. Elements with zero label are not seeds.
 *    - side: may be INTERIOR, EXTERIOR, or BOTH.
 *
 * OUTPUT 
 *    - A record (ann_img) with cost (squared euclidean distance),
 *    label transform, 
 *
 * TODO
 *    - optmize memory usage: parameters "label" and "seed" are
 *    redundant.
 *    - improve error treatment
 *----------------------------------------------------------------------------*/
#define INVALID_DIST PUINT32_MAX

AnimalExport ann_img *
exact_propagation(Img *image, ImgPUInt32 *label, list *seed, char side)
{
   char *fname="exact_propagation";
   SEDR *sedr;
   int maxrad, r, c, prev_col, n, rw, cl, i, j, 
       x, y, *pt, *pi, *pf, *lut, d, cp;
   unsigned maxdist, k, pos;
   int *p;  /* @@@ perhaps a register variable is faster */
   ImgPUInt32 *lbl_img, *dist, *pred;
   puint32 *lbl_data, *dist_data, *pred_data;
   pixval *img_data;
   Img *img;
   list_ptr ptr;
	ann_img *aimg;

   r = image->rows; c = image->cols;

   /* estimate the maximum distance to grow */
   if (side != INTERIOR)
      maxrad = ceil(hypot(r,c)); 
   else { 
      maxrad = (int) (ceil((float)MIN(r,c)/2)); 
      for (n=0,i=0; i<r*c; i++)
         n += (image->data[i] == FG);
      maxrad = MIN(maxrad,(int) ceil(sqrt((double)n) / 2));
   }

   sedr = grow_sedr(maxrad);
   if (!sedr) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return NULL;                                    
   }
   img  = impad(image, maxrad, maxrad, 0); 
   img_data = img->data;
   prev_col = c;
   r = img->rows; c = img->cols;
   n=r*c;


   dist = new_img_puint32(r,c);
   dist_data = dist->data;
   lut = dist->lut;     /* table for (r,c) indexing */
	lbl_img = impad_puint32(label, maxrad, maxrad, 0);
   lbl_data = lbl_img->data;
	pred = new_img_puint32(r,c);
   pred_data = pred->data;

   /*
      We must mark as INVALID_DIST only those pixels that _will_ be 
      processed by the propagation. 
   */
   switch (side) {
      case INTERIOR:
         for (i=0; i<n; i++) {
            pred_data[i] = prev_col*(i/c-maxrad) + i%c - maxrad;
            if (img_data[i] == FG && lbl_data[i] == 0) /* (**) */
               dist_data[i] = INVALID_DIST;
         }
         /* OBS: condition (**) tests for lbl_data[i] == 0 because the
          * seed pixels don't need processing. In fact, the for loop
          * for the propagation starts at i=1 (distance =1), not i=0
          * (distance == 0). */
         break;
      case EXTERIOR:
         for (i=0; i<n; i++) {
            pred_data[i] = prev_col*(i/c-maxrad) + i%c - maxrad;
            if (img_data[i] == BG) 
               dist_data[i] = INVALID_DIST;
         }
         break;
      case BOTH:
         for (i=0; i<n; i++) {
            pred_data[i] = prev_col*(i/c-maxrad) + i%c - maxrad;
            if (lbl_data[i] == 0)
               dist_data[i] = INVALID_DIST;
         }
			break;
      default:
         ANIMAL_ERR_FIRST("exact_propagation", ANIMAL_ERROR_FAILURE, "Invalid side parameter", NULL);
   }

   maxdist = (unsigned) maxrad*maxrad;

   /* -- distances >= 1 -- */             
   pt = sedr->pt;
   p = pt+2;
   for (i=1; i < (int)sedr->length && maxdist >= sedr->sqrd_dist[i]; i++) {
      d = (int)sedr->sqrd_dist[i];
      k=1;
      ptr = get_list_head(seed);
      pi = p;
      pf = pt + sedr->npts[i];
      do {  /* loop the contour */
         cp = get_list_point(ptr);
         x = cp % prev_col + maxrad;
         y = cp / prev_col + maxrad;
         p = pi;
         do { /* loop displacements with distance d */
            rw = y + *p;
            cl = x + *(p+1);
            p+=2;
            pos = cl + lut[rw];
            if (dist_data[pos] == INVALID_DIST) {
               dist_data[pos] = d;
               lbl_data[pos] = k;
               pred_data[pos] = cp;
            }

            /* 
                Four-fold unroll: # of pts at any distance is a multiple of 4
            */
            rw = y + *p;
            cl = x + *(p+1);
            p+=2;
            pos = cl + lut[rw];
            if (dist_data[pos] == INVALID_DIST) {
               dist_data[pos] = d;
               lbl_data[pos] = k;
               pred_data[pos] = cp;
            }

            rw = y + *p;
            cl = x + *(p+1);
            p+=2;
            pos = cl + lut[rw];
            if (dist_data[pos] == INVALID_DIST) {
               dist_data[pos] = d;
               lbl_data[pos] = k;
               pred_data[pos] = cp;
            }

            rw = y + *p;
            cl = x + *(p+1);
            p+=2;
            pos = cl + lut[rw];
            if (dist_data[pos] == INVALID_DIST) {
               dist_data[pos] = d;
               lbl_data[pos] = k;
               pred_data[pos] = cp;
            }
         } while (p < pf);
         k++;
         ptr = next_list_node(ptr);
     } while (not_nil(ptr));
   }


	aimg = new_ann_img(image);
   for (i=maxrad; i<r-maxrad; i++)
      for (j=maxrad; j<c-maxrad; j++) {
         RC(aimg->label,i-maxrad, j-maxrad) = RC(lbl_img, i, j);
         RC(aimg->cost, i-maxrad, j-maxrad) = RC(dist, i, j);
         RC(aimg->pred, i-maxrad, j-maxrad) = RC(pred, i, j);
      }

   /* Liberate memory */   
   imfree(&img);
   imfree_puint32(&lbl_img);
   imfree_puint32(&dist);
   imfree_puint32(&pred);

   return aimg;
}

/*-----------------------------------------------------------------
 *
 * Allocation of annotated skeleton structure
 *
 *-----------------------------------------------------------------*/
AnimalExport annotated_skl*
new_ann_skl(int r, int c, int maxrad)
{
   annotated_skl *s=NULL;

   s = (annotated_skl*) malloc(sizeof(annotated_skl));
      if (!s) return NULL;

   s->skl=new_img_puint32(r,c);
      if (!s->skl) return NULL;

   s->lbl=new_img_puint32(r,c);
      if (!s->lbl) return NULL;

   s->dt=new_img_puint32(r,c);
      if (!s->dt) return NULL;

   s->maxradius = maxrad;

   return s;
}


/*-----------------------------------------------------------------
 *
 * Deallocation of annotated skeleton structure
 *
 *-----------------------------------------------------------------*/
AnimalExport void
free_ann_skl(annotated_skl **s)
{
   annotated_skl *aux;
   aux = *s;
   if (aux != NULL) {
      if (aux->skl != NULL)
         imfree_puint32(&aux->skl);
      if (aux->lbl != NULL)
         imfree_puint32(&aux->lbl);
      if (aux->dt != NULL)
         imfree_puint32(&aux->dt);
      free(aux);
      *s = NULL;
   }
}


/*---------------------------------------------------------------------
 * Multiscale Shape Reconstruction using Multiscale Skeletons
 *    - if thresh is negative, assume already thresholded skeleton
 *    - if thresh is positive, sklimg is not used at all (may be null)
 *---------------------------------------------------------------------*/
AnimalExport Img *
msskl_filter(Img *sklimg, annotated_skl *skl, int thresh)
{
   Img *shp, *result;
   int maxrad, r, c;

   r = skl->skl->rows;
   c = skl->skl->cols;
   maxrad = skl->maxradius;
   shp= new_img(r + 2*maxrad,c + 2*maxrad);
   shp->isbinary=true;

   // calls core routine
   if (!msskl_filter_np(shp, sklimg, skl, thresh))
      return NULL;

   result = imtrim(shp,maxrad,maxrad);
   imfree(&shp);
   return result;
}


/*-----------------------------------------------------------------
 * Core routine for msskl_filter
 *  - suffix "np" stands for "no padding"
 *  - assumes image is properly padded!
 *  - does not trimm result
 *  - assume shp already allocated
 *  - useful inside loops
 *-----------------------------------------------------------------*/
AnimalExport int
msskl_filter_np(Img *shp, Img *sklimg, annotated_skl *skl, int thresh)
{
   int i,j,k;
   int maxrad, r, c, *pt;
   SEDR *sedr;
   unsigned d;
   unsigned p, pf;

   r = skl->skl->rows;
   c = skl->skl->cols;
   maxrad = skl->maxradius;

   if (thresh > 0) {
      sklimg = new_img(r, c);
      for (i=0; i < r * c; i++)
         sklimg->data[i] = (skl->skl->data[i] >= (puint32) thresh);
   }

   sedr = grow_sedr(maxrad);   
   if (!sedr)
      return false;
   pt = sedr->pt;
   for (i=maxrad; i<shp->rows-maxrad; i++) 
      for (j=maxrad; j<shp->cols-maxrad; j++)
         if ( RC(sklimg, i-maxrad, j-maxrad) == 1 ) {  // found skeleton
            /* 
                Paints digital circle around the skeleton with radius equal 
                to the distance-transform value at each point. There should be a
                more efficient way to do this without using SEDR.
             */
            d = RC(skl->dt, i-maxrad, j-maxrad);

            /* distance 0 */
            RC(shp,i,j) = FG;
            p = 2;

            for (k=1; k < (int)sedr->length && sedr->sqrd_dist[k] <= d; k++) {
               pf = sedr->npts[k];
                  do {
                     RC(shp,i + pt[p], j + pt[p+1]) = FG;
                     p+=2;

                     // unrolling
                     RC(shp,i + pt[p], j + pt[p+1]) = FG;
                     p+=2;

                     RC(shp,i + pt[p], j + pt[p+1]) = FG;
                     p+=2;

                     RC(shp,i + pt[p], j + pt[p+1]) = FG;
                     p+=2;
                  } while (p < pf);
            }
         }
   return true;
}



/*---------------------------------------------------------------------
 * Indicates if the image is connected from side to side (percolation).
 *    - returns 1 if image is percolated; 0 otherwise. 
 *    - opt is 1 for percolation in horizontal-direction;
 *             0 for vertical-direction.
 *---------------------------------------------------------------------*/

AnimalExport int
percol(Img *img, int opt)
{
   int i,lbl;
   point pt;
   bool percolated=false;

   img = impad(img,1,1,BG);

   if (opt == 1) { // test connectivity in horizontal direction
      for (i=1,lbl=2,pt.x=1;  i<img->rows-1;  i++,lbl++)
         if (RC(img,i,1) == FG) {
            pt.y = i;
            imfill(img,&pt,lbl,FG,DEFAULT_NHOOD);
         }
   
      for (i=1; i<img->rows-1; i++)
         if ( RC(img,i,img->cols-2) >= 2)
            percolated = true;

   } else {  // test connectivity in vertical direction
      for (i=1,lbl=2,pt.y=1;  i<img->cols-1;  i++,lbl++)
         if (RC(img,1,i) == 1) {
            pt.x = i; 
            imfill(img,&pt,lbl,FG,DEFAULT_NHOOD);
         }
   
      for (i=1; i<img->cols-1; i++)
         if ( RC(img,img->rows-2,i) >= 2)
            percolated = true;
   }
   imfree(&img);
   return percolated;
}



/*-----------------------------------------------------------------
 * Calculates the area of a binary shape.
 *    - at the moment it only counts the number of pixels inside 
 *      the shape
 *-----------------------------------------------------------------*/
AnimalExport unsigned long
bwarea(Img *img)
{
   int i;
   unsigned n=0;
   for (i=0; i<img->rows*img->cols; i++)
      n += (img->data[i] != 0);
   return n;
}


AnimalExport annotated_skl *
msskl(Img *bin, char side, skl_algorithm alg)
{
   ImgPUInt32 *cont;
   nhood *nh;
   ann_img *aimg;
   annotated_skl *askl;
   list *lst;
   int r,c;

   r = bin->rows; 
   c = bin->cols;
   cont = label_contour_pixel(bin,&lst); 

   if (alg == SKL_IFT) {
      nh   = get_8_nhood();
      aimg = ift_euclidean_propagation(bin, cont, nh, side);
         if (!aimg) {
            animal_err_register ("msskl", ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
      free_nhood(&nh);
   } else {
   	aimg = exact_propagation(bin, cont, lst, side);
         if (!aimg) {
            animal_err_register ("msskl", ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
   }

   free_list(&lst);
   imfree_puint32(&cont);

   askl = (annotated_skl *) malloc(sizeof(annotated_skl));
      if (!askl) return NULL;
   
   askl->lbl  = aimg->label;
   askl->dt   = aimg->cost;
   askl->skl = msskl_difference(aimg);
   askl->maxradius  = (int)ceil(sqrt(max_img_puint32(askl->dt)));

   imfree_puint32(&aimg->pred);
   free(aimg);

   return askl;
}


/*
   TODO
   - label_contour is called two times; only once suffices
*/
AnimalExport ImgPUInt32 *
msskl_difference(ann_img *aimg)
{
   ImgPUInt32 *imcont, *immsskel, *imseed, *imperim;
   puint32 *seed, *cont, *perim, *pred, *label, *msskel,
            maxd1, maxd2, MaxD;
   int r,c,i,j,k,qx,qy,p,q, d1,d2,
       *idxlut,n;

   r = aimg->label->rows; 
   c = aimg->label->cols;
   n = r*c;
   idxlut   = aimg->label->lut;
   imcont   = label_contour(aimg->img);  
   imperim  = perimeter(aimg->img);
   immsskel = new_img_puint32(r,c);
   imseed   = root_map(aimg->pred);

   seed     = imseed->data;
   cont     = imcont->data;
   perim    = imperim->data;
   msskel   = immsskel->data;
   pred     = aimg->pred->data;
   label    = aimg->label->data;

   MaxD = 0;
   for (i=0; i<r; i++) 
         for (j=0; j<c; j++) {
            p = index1(i,j,idxlut);
            // @@@ why eliminate the contours??
            if (pred[p] != (unsigned)p) {/* Eliminates the contours and
                                            considers the side option */
               maxd1 = maxd2 = 0;
               for (k=0; k < 4; k++) {
                  qy = n4[k][0] + i;
                  qx = n4[k][1] + j;
                  if (valid_pixel(r,c,qx,qy)) {
                     q = index1(qy,qx,idxlut);
                     if (cont[seed[p]] == cont[seed[q]]) { // not a SKIZ
                        d2 = label[q] - label[p];
                        if (d2 > (int)perim[seed[p]]-d2)
                           d2 = (int)perim[seed[p]]-d2;
                        if (d2 > (int)maxd2)
                           maxd2 = (unsigned)d2;
                     } else { // a SKIZ
                        d1 = cont[seed[q]] - cont[seed[p]];
                        if (d1 > (int)maxd1)
                           maxd1 = (unsigned)d1;
                     }
                  }
               }
               if (maxd1 > 0)
                  msskel[p] = UINT_MAX;
               else {
                  msskel[p] = maxd2;
                  if (msskel[p] > MaxD)
                     MaxD = msskel[p];
               }
            }
         }

   for (p=0; p < n; p++)
      if (msskel[p] == UINT_MAX)
         msskel[p] = MaxD + 1;

   imfree_puint32(&imcont);
   imfree_puint32(&imperim);
   imfree_puint32(&imseed);
   return immsskel;
}

AnimalExport ImgPUInt32 *
perimeter(Img *bin)
{
   int p,n;
   ImgPUInt32 *imcont, *perim;
   puint32 *cont;
   curve *hist;

   imcont = label_contour(bin);
   n = imcont->rows*imcont->cols;
   perim = new_img_puint32(imcont->rows,imcont->cols);
   hist = histogram_puint32(imcont);
   cont = imcont->data;
   for (p=0; p<n; p++)
      if (cont[p])
         perim->data[p] = hist->Y[cont[p]];

   free_curve(&hist);
   imfree_puint32(&imcont);
   return perim;
}

/*
   Zhang-Suen thinning

   Assumes image is not padded, ie, it will modify only on inner pixels.
   Processing is done in-place, but an auxiliary image is used.
   Returns NULL if any error.
*/

AnimalExport int
thinzs_np(Img *A)
{
   bool repeat;
   Img *B;
   int i,j,n;

   B = new_img(A->rows,A->cols);
   if (!B) return false;

   do {
      repeat=false;
      for (i=1; i < A->rows-1; i++)  for (j=1; j < A->cols-1; j++)
         if ((RC(B,i,j) = RC(A,i,j)) == FG) {
            n = nh8count_np(A,i,j,FG);
            if ( n>=2 && n<=6 && crossing_index_np(A,i,j) == 2 )
               if((RC(A,i-1,j)==BG || RC(A,i,j+1)==BG || RC(A,i+1,j)==BG)
               && (RC(A,i-1,j)==BG || RC(A,i+1,j)==BG || RC(A,i,j-1)==BG)){
                  RC(B,i,j) = BG;
                  repeat=true;
               }
         }

      for (i=1; i < A->rows-1; i++)  for (j=1; j < A->cols-1; j++)
         if ((RC(A,i,j) = RC(B,i,j)) == FG) {
            n = nh8count_np(B,i,j,FG);
            if ( n>=2 && n<=6 && crossing_index_np(B,i,j) == 2 )
               if((RC(B,i,j+1)==BG || RC(B,i,j-1)==BG || RC(B,i-1,j)==BG)
               && (RC(B,i-1,j)==BG || RC(B,i,j-1)==BG || RC(B,i+1,j)==BG)){
                  RC(A,i,j) = BG;
                  repeat=true;
               }
         }
   } while (repeat);

   imfree(&B);
   return true;
}

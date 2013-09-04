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
  

#include "segment.h"
#include "pstk.h"
#include <stdlib.h>


#define FINISHED     2
#define INSERTED     1
#define NOT_INSERTED 0


/*
	This one is based on the same algorithm as the "follow" function
   - only for Jordan curves
   - FIXME not working for lots of contours!!
*/

AnimalExport ImgPUInt32 *
label_contour_pixel(Img *img, list **bel_seq)
{
   int   (*path)[2],
         next[8] = {1,2,3,4,5,6,7,0}, invert[8]  = {4,5,6,7,0,1,2,3},
         neighbours[8][2] = {},
         cane,  cini, 
         k, a, b, t;
   unsigned p,x,y,i,j,r,c;
   bool finished;
   Img *tmp, *mark; 
   ImgPUInt32 *label, *tmp_u;
   int *idxlut;


   path  = n8;
   cane  = 2;
   img = impad(img,1,1,BG); // Assume 0 out of the image 
   r   = (unsigned)img->rows; 
   c   = (unsigned)img->cols;
   tmp = img;
   img = bwborder_np(img);
   imfree(&tmp);
   label  = new_img_puint32(r,c);
   mark   = new_img(r,c);
   idxlut = img->lut;
   *bel_seq = new_list();

	for (y=1; y<r-1; y++)   for (x=1; x<c-1; x++) {
      p = index1(y,x,idxlut);
      if (img->data[p] == FG && label->data[p] == 0) {
         i = y;
         j = x;
		   // get absolute coordinate of the neighbours
		   for (k=0; k<8; k++) {
		      neighbours[k][0] = path[k][0]+ i;
		      neighbours[k][1] = path[k][1]+ j;
		   }
		
		   t=1; finished=false;
         do {
            p = index1(i,j,idxlut);
	         label->data[p] = t;
            if(!list_append(bel_seq, p - c - (i-1)*2 - 1 ))
               return NULL;
		      // Twist the "blindman's cane" to locate the next contour point
		      cini=cane;
		      do {
		         cane=next[cane];
		         a = neighbours[cane][0];
		         b = neighbours[cane][1];
		      } while (RC(img,a,b) == 0  &&  cane != cini);
		
		      if (cane == cini)
		         finished = true;
		      else {   // Move to a new contour point
		         i += path[cane][0];
		         j += path[cane][1];
		         for (k=0; k<8; k++) {  
		            neighbours[k][0] += path[cane][0];
		            neighbours[k][1] += path[cane][1];
		         }
		         cane = invert[cane];
		         finished =  j==x && i==y;
		      }
		
		      t++; 
		   } while(!finished);
      }
	}

   tmp_u = label;
   label = imtrim_puint32(label,1,1);

   imfree_puint32(&tmp_u);
   imfree(&img);
   return label;
}


/*---------------------------------------------------------------------------
 * Labels each contour point with a different image. 
 *    - Accepts many objects, with or w/o holes, 1-pix-wide entrances 
 *    - Implemented as a depth-first traversal of the boundary pixels. 
 *    - The boundary is considered internal to the objects. That leads
 *    to some problems with one-pixel-wide object parts
 *
 *  TODO
 *    - It is not working if two objects are connected by a single 1-pix-wide 
 *    path
 *    - It is not working for an L object:    * *
 *															 * *
 *														  * * *
 *									                 * * *
 *---------------------------------------------------------------------------*/
/*
AnimalExport ImgPUInt32 *
label_contour_pixel(Img *imbin)
{
   unsigned r,c,n,i,j,k,l, t,cp,p,px,py,q,qx,qy; 
   int *idxlut, diag_neighbours[4][2]={{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
   Img *imborder, *immark;
   pixval *border, *mark;
   ImgPUInt32 *impred, *imlabel, *tmp;
   puint32 *pred, *label;
   int_stk *stk;
   bool found_n4;

   n = imbin->rows * imbin->cols;
   imbin = impad(imbin,1,1,BG);
   imborder = bwborder_np(imbin);

   r = imbin->rows;  c = imbin->cols; 
   immark  = new_img(r,c);
   imlabel = new_img_puint32(r,c);
   impred  = new_img_puint32(r,c);

   // variables to avoid indirections:
   idxlut = imbin->lut;
   border = imborder->data;
   label  = imlabel->data;
   mark   = immark->data; 
   pred   = impred->data;

   stk = new_stk(n);
   for (i=1; i < r-1; i++)
      for (j=1; j < c-1; j++) {
         cp = index1(i,j,idxlut);
         if (border[cp] == FG && mark[cp] != FINISHED) {
            push(cp,stk); 
            mark[cp] = INSERTED;
            pred[cp] = cp;
            while ( stk_isnt_empty(stk) ) {   // labels a contour
               p = pop(stk);
               py = p / c;
               px = p % c; 
               mark[p] = FINISHED;
               found_n4=false;
               for (k=0; k<4; k++) {
                  qy = py + n4[k][0];
                  qx = px + n4[k][1];
                  q  = index1(qy,qx,idxlut);
                  if (q == cp && pred[p] != cp) { 
                     evacuate_stk(stk);
                     break;
                  }
                  if (border[q] == FG && mark[q] != FINISHED) {
                     pred[q] = p;
                     found_n4 = true;
                     if (mark[q] == NOT_INSERTED) {
                        push(q,stk);
                        mark[q] = INSERTED;
                     }
                  }
               }
               if (!found_n4) {  // try a diagonal neighbour
                  for (k=0; k<4; k++) {
                     qy = py + diag_neighbours[k][0];
                     qx = px + diag_neighbours[k][1];
                     q  = index1(qy,qx,idxlut);
                     if (q == cp && pred[p] != cp) { 
                        evacuate_stk(stk);
                        break;
                     }
                     if (border[q] == FG && mark[q] != FINISHED) {
                        pred[q] = p;
                        if (mark[q] == NOT_INSERTED) {
                           push(q,stk);
                           mark[q] = INSERTED;
                        }
                     }
                  }
               }

            }
            t = p;
            l = 1;
            while (pred[p] != p) {
               label[p] = l++; 
               p = pred[p];
            }
            if (t != p)
               label[p] = l;
         }
   }


   tmp = imlabel;
   imlabel = imtrim_puint32(imlabel,1,1);

   imfree_puint32(&tmp);
   imfree_puint32(&impred);
   imfree(&immark);
   imfree(&imbin);
   free_stk(stk);

   return imlabel;
}
*/

/*
   Same as label_contour_pixel, but assigns different labels to different contours only.
*/
AnimalExport ImgPUInt32 *
label_contour(Img *img)
{
   int   (*path)[2],
         next[8] = {1,2,3,4,5,6,7,0}, invert[8]  = {4,5,6,7,0,1,2,3},
         neighbours[8][2] = {},
         cane,  cini, 
         k, a, b, t;
   unsigned p,x,y,i,j,r,c;
   bool finished;
   Img *tmp, *mark; 
   ImgPUInt32 *label, *tmp_u;
   int *idxlut;


   path  = n8;
   cane  = 2;
   img = impad(img,1,1,BG); // Assume 0 out of the image 
   r   = (unsigned)img->rows; 
   c   = (unsigned)img->cols;
   tmp = img;
   img = bwborder_np(img);
   imfree(&tmp);
   label  = new_img_puint32(r,c);
   mark   = new_img(r,c);
   idxlut = img->lut;

   t=1;
	for (y=1; y<r-1; y++)   for (x=1; x<c-1; x++) {
      p = index1(y,x,idxlut);
      if (img->data[p] == FG && label->data[p] == 0) {
         t++;
         i = y;
         j = x;
		   // get absolute coordinate of the neighbours
		   for (k=0; k<8; k++) {
		      neighbours[k][0] = path[k][0]+ i;
		      neighbours[k][1] = path[k][1]+ j;
		   }
		
		   finished=false;
         do {
	         RC(label,i,j) = t;
		      // Twist the "blindman's cane" to locate the next contour point
		      cini=cane;
		      do {
		         cane=next[cane];
		         a = neighbours[cane][0];
		         b = neighbours[cane][1];
		      } while (RC(img,a,b) == 0  &&  cane != cini);
		
		      if (cane == cini)
		         finished = true;
		      else {   // Move to a new contour point
		         i += path[cane][0];
		         j += path[cane][1];
		         for (k=0; k<8; k++) {  
		            neighbours[k][0] += path[cane][0];
		            neighbours[k][1] += path[cane][1];
		         }
		         cane = invert[cane];
		         finished =  j==x && i==y;
		      }
		   } while(!finished);
      }
	}

   tmp_u = label;
   label = imtrim_puint32(label,1,1);

   imfree_puint32(&tmp_u);
   imfree(&img);
   return label;
}
/*
AnimalExport ImgPUInt32 *
label_contour(Img *imbin)
{
   unsigned r,c,n,i,j,k,l, t,cp,p,px,py,q,qx,qy; 
   int *idxlut;
   Img *imborder, *immark;
   pixval *border, *mark;
   ImgPUInt32 *impred, *imlabel, *tmp;
   puint32 *pred, *label;
   int_stk *stk;

   n = imbin->rows * imbin->cols;
   imbin = impad(imbin,1,1,BG);
   imborder = bwborder_np(imbin);

   r = imbin->rows;  c = imbin->cols; 
   immark  = new_img(r,c);
   imlabel = new_img_puint32(r,c);
   impred  = new_img_puint32(r,c);

   // variables to avoid indirections:
   idxlut = imbin->lut;
   border = imborder->data;
   label  = imlabel->data;
   mark   = immark->data; 
   pred   = impred->data;

   l = 1;
   stk = new_stk(n);
   for (i=1; i < r-1; i++)
      for (j=1; j < c-1; j++) {
         cp = index1(i,j,idxlut);
         if (border[cp] == FG && mark[cp] != FINISHED) {
            push(cp,stk); 
            mark[cp] = INSERTED;
            pred[cp] = cp;
            do { // labels a contour
               p = pop(stk);
               py = p / c;
               px = p % c; 
               mark[p] = FINISHED;
               for (k=0; k<8; k++) {
                  qy = py + n8[k][0];
                  qx = px + n8[k][1];
                  q  = index1(qy,qx,idxlut);
                  if (q == cp && pred[p] != cp) { 
                     evacuate_stk(stk);
                     break;
                  }
                  if (border[q] == FG && mark[q] != FINISHED) {
                     pred[q] = p;
                     if (mark[q] == NOT_INSERTED) {
                        push(q,stk);
                        mark[q] = INSERTED;
                     }
                  }
               }
            } while ( stk_isnt_empty(stk) ); 
            t = p;
            while (pred[p] != p) {
               label[p] = l; 
               p = pred[p];
            }
            if (t != p)
               label[p] = l++;
         }
   }


   tmp = imlabel;
   imlabel = imtrim_puint32(imlabel,1,1);

   imfree_puint32(&tmp);
   imfree_puint32(&impred);
   imfree(&immark);
   imfree(&imbin);
   free_stk(stk);

   return imlabel;
}
*/

AnimalExport Img *
bwborder(Img *img)
{
   Img *border, *tmp;

   img = impad(img,1,1,BG);
   border = bwborder_np(img);
   tmp = border;
   border = imtrim(border,1,1);
   imfree(&tmp);
   imfree(&img);

   return border;
}

AnimalExport Img *
bwborder_np(Img *img)
{
   Img *border;
   int i,j,k,r,c,ni,nj,p;
   r = img->rows; c = img->cols;

   border = new_img(r,c);
   border->isbinary = true;
   for (i=1; i < r-1; i++)
      for (j=1; j < c-1; j++) {
         p = index1(i,j,img->lut);
         if (img->data[p] == FG) {
            for (k=0; k<4; k++) {
               ni = i + n4[k][0];
               nj = j + n4[k][1];
               if ( RC(img,ni,nj) == BG ) {
                  border->data[p] = FG;
                  break;
               }
            }
         }
      }

   return border;
}



/*-----------------------------------------------------------------
 * Fills binary contours
 *    - paintval is the value with which the region will be filled.
 *    - maskval is the value of the adjacent pixels of the seed that
 *      will be painted.
 *    - OBS: image is not padded; in place function.
 * TODO
 *    - use a simpler int stack, pre-allocated
 *-----------------------------------------------------------------*/
AnimalExport bool
imfill(Img *img, point *seed, pixval paintval, pixval maskval, nhood *nh)
{
   unsigned i;
   bool nh_alloc = false;
   point *cp, np; // current pixel
   pstk_ptr current; // stack of current pixels
   char *fname = "imfill";

   if (nh == DEFAULT_NHOOD) {
      nh = get_4_nhood();   // default useful for filling 8-conn. contours
      if (!nh) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return false;
      }
      nh_alloc=true;
   }

   current = new_pstk();
   ptpush(&current, seed);
   do {
      cp = ptpop(&current);
         if (!cp) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return false;
         }

      RC(img,cp->y,cp->x) = paintval;    
      for (i=0; i < nh->n; i++) { 
         np.x = cp->x + nh->dx[i]; 
         np.y = cp->y + nh->dy[i];
         if(RC(img,np.y,np.x) == maskval)
            ptpush(&current, &np);
      }
      free(cp);
   } while (pstk_isnt_empty(current));

   free_pstk(&current);
   if (nh_alloc)
      free_nhood(&nh);

   return true;
}
/* Very same routine for 32bit Unsigned Integer matrices.
 * Nothing changes in the body, so ONLY EDIT THE ABOVE CODE */
AnimalExport bool
imfill_puint32(ImgPUInt32 *img, point *seed, puint32 paintval, puint32 maskval, nhood *nh)
{
   unsigned i;
   bool nh_alloc = false;
   point *cp, np; // current pixel
   pstk_ptr current; // stack of current pixels
   char *fname = "imfill";

   if (nh == DEFAULT_NHOOD) {
      nh = get_4_nhood();   // default useful for filling 8-conn. contours
      if (!nh) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return false;
      }
      nh_alloc=true;
   }

   current = new_pstk();
   ptpush(&current, seed);
   do {
      cp = ptpop(&current);
         if (!cp) {
            animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
            return false;
         }

      RC(img,cp->y,cp->x) = paintval;    
      for (i=0; i < nh->n; i++) { 
         np.x = cp->x + nh->dx[i]; 
         np.y = cp->y + nh->dy[i];
         if(RC(img,np.y,np.x) == maskval)
            ptpush(&current, &np);
      }
      free(cp);
   } while (pstk_isnt_empty(current));

   free_pstk(&current);
   if (nh_alloc)
      free_nhood(&nh);

   return true;
}


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Label connected components in a binary image
 * 
 * INPUT
 *    - img: binary image
 *    - nh:  connectivity
 *
 * OUTPUT
 *    - img: 0 for background, 1 for the first region, 2 for the second...
 *    - return code >= 0 ---> number of connected foreground regions; success
 *    - return code < 0  ---> failure
 *
 * REMARKS
 *    - there are many faster algorithms, but this one does the job
 *----------------------------------------------------------------------------*/
AnimalExport int
label_components_np(ImgPUInt32 *img, nhood *nh)
{
   int i,j;
   bool stat;
   puint32 l=2;
   point seed;

   for (i=0; i<img->rows; i++)
      for (j=0; j<img->cols; j++)
         if (RC(img,i,j) == 1) {
            seed.y = i; seed.x = j;
            stat = imfill_puint32(img, &seed, l++, FG, nh);
               if (!stat) {
                  animal_err_register ("label_components_ip_np", ANIMAL_ERROR_FAILURE,""); 
                  return -1;
               }
         }

   for (i=0; i<img->rows*img->cols; i++)
      if(DATA(img)[i]) 
         DATA(img)[i]--;

   ISBINARY(img) = false;

   return l-2;
}

/*-----------------------------------------------------------------
 * Fills a binary closed contour which is bounded by functions,
 * above and bellow. 
 *    - Implemented as a finite deterministic automaton
 *-----------------------------------------------------------------*/
#define DONE { state = A; ++j; i=0; } 
int imfill_func(Img *img) {
   int i,j; enum {A, OUT, B, IN} state=A;

   for (j=0; j<img->cols; j++)
      for (i=0; i<img->rows; i++)
         switch (state) {
            case A:
               if (RC(img,i,j) == 0)
                  state = OUT;
               else
                  DONE
               break;
            case OUT:
               if (RC(img,i,j) == 1)
                   state = B;
               break;
            case B:
               if (RC(img,i,j) == 0)
                  state = IN;
               else 
                  DONE
               break;
            case IN:
               if (RC(img,i,j) == 0) 
                  RC(img,i,j) = 1;
               else 
                  DONE
               break;
         }
   return 1;
}

/*-----------------------------------------------------------------
 * Finds an interior point of a binary contour
 *    - for use with imfill
 *    - return point as (x,y) = (col,row)
 *-----------------------------------------------------------------*/
AnimalExport point *
find_interior_point(Img *img)
{
   int i,j;
   point *p;
   pixval prev;
   bool inside;

   for (i=0; i<img->rows; i++) {
      prev = 0;
      inside = false;
      for (j=0; j<img->cols; j++)
         if (RC(img,i,j) == 1) {
            if (inside) {
               p = (point *) malloc(sizeof(point));
               if (p == NULL) return NULL;
               p->x = j-1; p->y = i;
               return p;
            }
            prev = 1;
         } else if (prev == 1) { 
            prev = 0;
            inside = true;
         }
   }
   return NULL;
}

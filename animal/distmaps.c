/*
 * =============================================================================
 *
 * D I S T A N C E   T R A N S F O R M   R O U T I N E S
 *
 * $Revision: 1.5 $ $Date: 2009-04-18 22:53:46 $
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
#include "ift.h"
#include "ift_pqueue.h"
#include "analysis.h"
#include "segment.h"
#include "queue.h"


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description: Distance transforms through various algorithms 
 *
 * INPUT
 *    - binary image (with "1" where the distances shall be
 *    outputted)
 *    - the type of the algorithm (specifyed as a dt_algorithm enumerated 
 *    type, as defined in the header file).
 *    - for an image without "0" (black) pixels, the output is
 *    UNDEFINED. It is the responsability of the user of this routine
 *    to assure that the input will not be a constant image.
 *    - max_dist: maximum (squared) distance to be computed. If (puint32)-1, a
 *    representation of infinity, then this is the maximum possible distance
 *
 * OUTPUT 
 *    - the distance transform in a separate image
 *    - UNDEFINED if the input is constant with all pixels equal to 1.
 *    This means the output image will contain arbitrary values. 
 *
 * REMARKS 
 *    - You may use all the algorithms directly, if you wish, as most of
 *    them are in-place algorithms. This is just a convenience function.
 *
 *    - Distances are calculated in the pixels marked as "1" (foreground). 
 *    To get an external DT, simply invert the input image. To get a DT
 *    in both sides, there are two slightly different options: 
 *
 *    1) Detect the binary border of the object (you must decide if
 *    you want the internal or external border), with 0 at the border
 *    pixels, and 1 at the rest of the image.  (if you use bwborder
 *    you will have to invert the image before the distance transform)
 *
 *    2) Calculate the EDT of the interior, then of the exterior, and
 *    finally combine the result. In this case, there will be no
 *    0-distance value in the resulting EDT.
 *
 * 
 *----------------------------------------------------------------------------*/

bool
distance_transform_ip_max_dist(
    ImgPUInt32 *cost, 
    dt_algorithm alg, 
    puint32 max_dist,
    bool compute_label,
    ImgPUInt32 *imlabel)
{
   char *fname="distance_transform_ip";
   bool stat=true;
   bool compute_all_distances = (max_dist == (puint32) -1);

   if (!compute_all_distances && alg != DT_CUISENAIRE_PMN_1999) {
     animal_err_register (fname, ANIMAL_ERROR_NOT_IMPLEMENTED, "algorithm with maximum distance"); 
     return false;
   }

   if (compute_label && (alg != DT_MAURER2003 || !compute_all_distances)) {
     animal_err_register (fname, ANIMAL_ERROR_NOT_IMPLEMENTED, "algorithm with label computation"); 
     return false;
   }

   assert(cost->isbinary);
   if (alg == DT_LOTUFO_ZAMPIROLLI) {
     stat = edt_lz(cost);
   } else if (alg == DT_MAURER2003) {
      if (compute_label)
        stat = edt_maurer2003_label(cost, imlabel);
      else
        stat = edt_maurer2003(cost);
   } else if (alg == DT_MEIJSTER_2000) {
      stat = edt_meijster2000(cost);
   } else if (alg == DT_CUISENAIRE_PMN_1999) {
/*    stat = edt_cuisenaire_pmn_orig(cost);*/
      stat = edt_cuisenaire_pmn_max_dist(cost, max_dist);
   } else if (alg == DT_CUISENAIRE_PMON_1999) {
      stat = edt_cuisenaire_pmon(cost);
   } else if (alg == DT_CUISENAIRE_PSN4_1999) {
      stat = edt_cuisenaire_psn4(cost);
   } else if (alg == DT_CUISENAIRE_PSN8_1999) {
      stat = edt_cuisenaire_psn8(cost);
   } else if (alg == DT_SAITO_1994) {
      stat = edt_saito(cost);
   } else if (alg == DT_EGGERS_1998) {
      stat = edt_eggers(cost);
   } else if (alg == DT_EXACT_DILATIONS) {
      stat = edt_exact_dilations(cost);
   } else if (alg == DT_BRUTE_FORCE) {
      stat = edt_brute_force(cost);
   } else if (alg == DT_BRUTE_FORCE_WITH_LIST) {
      stat = edt_brute_force_with_list(cost);
   } else if (alg == DT_IFT_8 || alg == DT_IFT) { /* IFT */
      nhood *nh;
      nh = get_8_nhood();
      stat = edt_ift(cost, nh);
      free_nhood(&nh);
   } else if (alg == DT_IFT_4) { /* IFT */
      nhood *nh;
      nh = get_4_nhood();
      stat = edt_ift(cost, nh);
      free_nhood(&nh);
   } else {
      animal_err_register (fname, ANIMAL_ERROR_ILLEGAL_ARGUMENT,"argument must be a valid dt_algorithm enum constant"); 
      return false;
   }

   return stat;
}

/*----FUNCTION----------------------------------------------------------------
 *
 * Description: Shorthand for distance_transform_ip_max_dist to compute all
 * possible distances
 *
 *----------------------------------------------------------------------------*/
bool
distance_transform_ip(ImgPUInt32 *cost, dt_algorithm alg)
{
  return distance_transform_ip_max_dist(cost, alg, (puint32) -1, false, NULL);
}

AnimalExport ImgPUInt32 *
distance_transform_max_dist(
    Img *bin, 
    dt_algorithm alg, 
    puint32 max_dist, 
    bool compute_label,
    ImgPUInt32 **imlabel)
{
   char *fname="distance_transform";
   ImgPUInt32 *cost;
   int r=bin->rows,c=bin->cols;
   puint32 i;
   bool stat;

   assert(bin->isbinary);

   cost = new_img_puint32(r,c);
   if (!cost) {
      animal_err_register(fname,ANIMAL_ERROR_FAILURE,"cost alloc");
      return NULL;
   }
   cost->isbinary=true;
   for (i=0; i<(unsigned)r*c; i++)
      DATA(cost)[i] = DATA(bin)[i];

   if (compute_label) {
     *imlabel = new_img_puint32(r,c);
     if (!*imlabel) {
        animal_err_register(fname,ANIMAL_ERROR_FAILURE,"imlabel alloc");
        return NULL;
     }
   }

   stat = distance_transform_ip_max_dist(cost, alg, max_dist, compute_label, *imlabel);
   CHECK_RET_STATUS(NULL);

   return cost; 
}

AnimalExport ImgPUInt32 *
distance_transform(Img *bin, dt_algorithm alg)
{
  ImgPUInt32 *imlabel=NULL;
  return distance_transform_label(bin, alg, false, &imlabel);
}

AnimalExport ImgPUInt32 *
distance_transform_label(Img *bin, dt_algorithm alg, bool compute_label, ImgPUInt32 **imlabel)
{
  return distance_transform_max_dist(bin, alg, (puint32) -1, compute_label, imlabel);
}

/* 
 * ========================================
 *  ----------------
 *    Maurer's EDT
 *  ----------------
 * ========================================
 */



/*----FUNCTION-----------------------------------------------------------------
 * 
 *  Description:  Calvin Maurer's EDT (Euclidean DistanceTransform): 
 *
 * REFERENCE
 *  Paper: Calvin Maurer et. al. PAMI feb. 2003 
 *
 *  Implementation "inspired" in the LTI lib:
 *    http://ltilib.sourceforge.net
 *  
 *  Distances are calculated only where pixels are equal to 1
 *
 * - PARAMETER ----------------------------------------------------------------
 *      Mode   Type             Name            Description
 * ----------------------------------------------------------------------------
 *    in-out:  ImgPUInt32         im            binary image on input;
 *                                              grayscale on output, with
 *                                              squared distance transform.
 *----------------------------------------------------------------------------*/

inline static bool edt_maurer_2D_from_1D(ImgPUInt32 *im);
inline static bool edt_maurer_2D_from_1D_label(ImgPUInt32 *im, ImgPUInt32 *imlabel);

/* global variable storing the infinity value for each image */
static puint32 infty_;

/* flag indicating no associated label used internally by some algorithms */
static const puint32 no_label_const = PUINT32_MAX;


AnimalExport bool
edt_maurer2003(ImgPUInt32 *im)
{
   char *fname="edt_maurer2003";
   int i,r,c;
   bool stat;

   assert(im->isbinary);

   r = im->rows; c = im->cols;
   infty_ = PUINT32_MAX - r*r - c*c;

   for (i=0; i < r*c; i++)
      if (DATA(im)[i] == FG)
         DATA(im)[i] = infty_;
   
   // Vertical columnwise EDT
   stat = edt_1d_vertical(im);
   // Lotufo's 1D EDT is equivalent to Maurer's D1.
   // There is a remark in section 5 of Maurer's paper that says
   // D1 can be calculated using a routine like this.
   CHECK_RET_STATUS(false);

   stat = edt_maurer_2D_from_1D(im);      
   CHECK_RET_STATUS(false);

   im->isbinary = false;
   return true;
}

//: Same as edt_maurer2003, but also returns a label array indicating the closest 0-pixel.
//
// \param[out] imlabel  An array indicating the closet feature pixel. imlabel[i] == row_major linear
// index of the closest feature voxel. Assuming the image \p im is also stored in row_major order,
// im[i] will contain the corresponding distance.
//
AnimalExport bool
edt_maurer2003_label(ImgPUInt32 *im, ImgPUInt32 *imlabel)
{
   char *fname="edt_maurer2003_label";
   int i,r,c;
   bool stat;

   assert(im->isbinary);

   r = im->rows; c = im->cols;
   infty_ = PUINT32_MAX - r*r - c*c;

   for (i=0; i < r*c; i++)
      if (DATA(im)[i] == FG) {
        DATA(im)[i] = infty_;
        DATA(imlabel)[i] = no_label_const;
      } else
        DATA(imlabel)[i] = i / c;
   
   // Vertical columnwise EDT
   stat = edt_1d_vertical_label(im, imlabel);
   // Lotufo's 1D EDT is equivalent to Maurer's D1.
   // There is a remark in section 5 of Maurer's paper that says
   // D1 can be calculated using a routine like this.
   CHECK_RET_STATUS(false);

   stat = edt_maurer_2D_from_1D_label(im, imlabel);
   CHECK_RET_STATUS(false);

   im->isbinary = false;
   return true;
}

inline static bool maurer_voronoi_edt_2D(ImgPUInt32 *im, puint32 *im_row, unsigned *g, unsigned *h);
inline static void maurer_voronoi_edt_2D_label(ImgPUInt32 *im, puint32 *im_row, puint32 *imlabel_row, unsigned *g, unsigned *h, unsigned *w);

inline bool
edt_maurer_2D_from_1D(ImgPUInt32 *im)
{
   bool stat;
   unsigned i1, *g, *h; // same naming as in the paper
   char *fname="edt_maurer_2D_from_1D";

   unsigned ncols = im->cols;
   unsigned nrows = im->rows;

   // Call voronoi_edt_2D for every row.
   // OBS: g and h are internal to maurer_voronoi_edt_2d and are
   // pre-allocated here for efficiency.
   g = animal_malloc_array(unsigned, ncols);
       if (!g) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }                                                        
   h = animal_malloc_array(unsigned, ncols);
       if (!h) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }                                                        


   puint32 *im_row;
   im_row = DATA(im);

   for (i1=0; i1 < nrows; ++i1, im_row += ncols) {
      stat = maurer_voronoi_edt_2D(im, im_row,  /* internal: */ g, h);
      CHECK_RET_STATUS(false);
   }

   free(g); free(h);

   return true;
}

inline bool
edt_maurer_2D_from_1D_label(ImgPUInt32 *im, ImgPUInt32 *imlabel)
{
   bool stat;
   unsigned i1, *g, *h, *w; // same naming as in the paper
   char *fname="edt_maurer_2D_from_1D";

   unsigned ncols = im->cols;
   unsigned nrows = im->rows;

   // Call voronoi_edt_2D for every row.
   // OBS: g and h are internal to maurer_voronoi_edt_2d and are
   // pre-allocated here for efficiency.
   g = animal_malloc_array(unsigned, ncols);
       if (!g) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }                                                        
   h = animal_malloc_array(unsigned, ncols);
       if (!h) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }                                                        
   w = animal_malloc_array(unsigned, ncols);
       if (!h) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }                                                        

   puint32 *im_row, *imlabel_row;
   im_row = DATA(im);
   imlabel_row = DATA(imlabel);

   for (i1=0; i1 < nrows; ++i1, im_row += ncols, imlabel_row += ncols) {
      maurer_voronoi_edt_2D_label(im, im_row, imlabel_row, /* internal: */ g, h, w);
   }

   free(g); free(h); free(w);

   return true;
}

inline bool remove_edt(int du, int dv, int dw, 
                      int u,  int v,  int w);

inline bool
maurer_voronoi_edt_2D(ImgPUInt32 *im, puint32 *im_row, unsigned *g, unsigned *h)
{
   int l, ns;
   unsigned i, di, dmin, dnext, c;
   puint32 fi;

   c=im->cols;

   // Remove Voronoi sites not nearest to 'line' im_row
   l = -1;
   for (i=0; i < c; ++i){
      if ((fi = im_row[i]) != infty_) {
         while ( l >= 1 && remove_edt(g[l-1], g[l], fi, h[l-1], h[l], i) )
            --l;
         ++l; g[l] = fi; h[l] = i;
      }
   }

   // Assertions at this point:
   //    h[k] == row containing a site k
   //    l == index of last site

   // The following are lines 15-25 of the article
   if ((ns=l) == -1) return true;

   l = 0;
   for (i=0; i < c; ++i) {  // Query Partial Voronoi Diagram
      di = h[l] - i;        // Its ok for di to be unsigned -- modular arithmetic
      dmin = g[l] + di*di;

      for ( ; l < ns; ++l) {
         di = h[l+1] - i;

         if (dmin <= (dnext = g[l+1] + di*di) ) break;

         dmin = dnext;
      }

      im_row[i] = dmin;
   }

   return true;
}

/* Same as maurer_voronoi_edt_2D but with propagation of the label of the
 * nearest 0-pixel.  */
inline void
maurer_voronoi_edt_2D_label(
    ImgPUInt32 *im, 
    puint32 *im_row, 
    puint32 *imlabel_row, 
    unsigned *g, unsigned *h, unsigned *w)
{
   int l, ns;
   unsigned i, di, dmin, dnext, r, c;
   puint32 fi;

   c = im->cols;
   r = im->rows;

   // Remove Voronoi sites not nearest to 'line' im_row
   l = -1;
   for (i=0; i < c; ++i){
      if ((fi = im_row[i]) != infty_) {
         while ( l >= 1 && remove_edt(g[l-1], g[l], fi, h[l-1], h[l], i) )
            --l;
         ++l; g[l] = fi; h[l] = i; w[l] = imlabel_row[i];
      }
   }

   // Assertions at this point:
   //    h[k] == column containing a site k
   //    l == index of last site

   // The following are lines 15-25 of the paper
   if ((ns=l) == -1) return;

   l = 0;
   for (i=0; i < c; ++i) {  // Query Partial Voronoi Diagram
      di = h[l] - i;        // Its ok for di to be unsigned -- modular arithmetic
      dmin = g[l] + di*di;

      for ( ; l < ns; ++l) {
         di = h[l+1] - i;

         if (dmin <= (dnext = g[l+1] + di*di) ) break;

         dmin = dnext;
      }

      im_row[i] = dmin;
      imlabel_row[i] =  w[l]*c + h[l];
   }

   return;
}


inline bool 
remove_edt(int du, int dv, int dw, 
           int u,  int v,  int w)
{
    // 11 integer expressions
    int a = v - u,
        b = w - v,
        c = w - u;

    return  c*dv > b*du + a*dw + a*b*c;
}


/* 
 * ========================================
 *  ----------------------------------
 *    Exact EDT with Exact Dilations
 *  ----------------------------------
 * ========================================
 */

AnimalExport bool
edt_exact_dilations(ImgPUInt32 *image)
{
   char *fname="edt_exact_dilations";
   SEDR *sedr;
   int maxrad, r, c, prev_col, n, rw, cl, i, j, maxi, maxj,
       x, y, *pt, *pi, *pf, *lut, d, cp, npix, count,
       *p;  /* @@@ perhaps a register variable is faster */
   unsigned maxdist, k, pos;
   puint32 *img_data, *imptr;
   ImgPUInt32 *img;
   list_ptr ptr;
   list *boundary;
   bool stat=false;


   assert(image->isbinary);
   r = image->rows; c = image->cols;

   /* estimate the maximum distance to grow */
   maxrad = (int) (ceil((float)MIN(r,c)/2)); 
   for (npix=0,i=0; i<r*c; i++)
      npix += (image->data[i] == FG);
   maxrad = MIN(maxrad,(int) ceil(sqrt((double)npix) / 2));

   sedr = grow_sedr(maxrad);
   if (!sedr) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return false;                                    
   }

   prev_col = c;
   img  = impad_puint32(image, maxrad, maxrad, 0); 
   img_data = img->data;
   lut = img->lut;     /* table for (r,c) indexing */
   r = img->rows; c = img->cols;
   n=r*c;

   boundary = new_list();
   maxdist = (unsigned) maxrad*maxrad + 1;
   /*
      We must mark as INVALID_DIST only those pixels that _will_ be 
      processed by the propagation. 
   */

   /* Insert in list every foreground pixel at distance 1 of the
    * background, which are foreground with at least one background
    * pixel. */
   maxi  = r-maxrad-1;
   maxj  = c-maxrad-1;
   imptr = img_data + index1(maxrad,maxrad,lut);
   for (i=maxrad; i <= maxi; ++i)
      for (j=maxrad; j <= maxj; ++j) {
         imptr = img_data + index1(i,j,lut);
         if (*imptr == 0)  {
            if((j < maxj   && imptr[1] > 0) || (j > maxrad && imptr[-1] > 0) ||
               (i < maxi   && imptr[c] > 0) || (i > maxrad && imptr[-c] > 0))
            stat = list_append(&boundary, imptr-img_data);
            CHECK_RET_STATUS(false);;
         } else
            *imptr = maxdist;
      }

   if (isnt_empty_list(boundary)) {
      count = 0;
      /* -- distances >= 1 -- */             
      pt = sedr->pt;
      p = pt+2;
      for (i=1; i < (int)sedr->length && maxdist >= sedr->sqrd_dist[i]; i++) {
         d = (int)sedr->sqrd_dist[i];
         k=1;
         ptr = get_list_head(boundary);
         pi  = p;
         pf  = pt + sedr->npts[i];
         do {  /* loop the contour */
            cp = get_list_point(ptr);
            x = cp % c;
            y = cp / c;
            p = pi;
            do { /* loop displacements with distance d */
               rw = y + *p;
               cl = x + *(p+1);
               p+=2;
               pos = cl + lut[rw];
               if (img_data[pos] == maxdist) {
                  img_data[pos] = d;
                  ++count;
               }

               /* 
                   Four-fold unroll: # of pts at any distance is a multiple of 4
               */
               rw = y + *p;
               cl = x + *(p+1);
               p+=2;
               pos = cl + lut[rw];
               if (img_data[pos] == maxdist) {
                  img_data[pos] = d;
                  ++count;
               }

               rw = y + *p;
               cl = x + *(p+1);
               p+=2;
               pos = cl + lut[rw];
               if (img_data[pos] == maxdist) {
                  img_data[pos] = d;
                  ++count;
               }

               rw = y + *p;
               cl = x + *(p+1);
               p+=2;
               pos = cl + lut[rw];
               if (img_data[pos] == maxdist) {
                  img_data[pos] = d;
                  ++count;
               }
               
               if (count >= npix) /* All '1'-pixels have been visited */
                  goto STOP_PROPAGATION;  /* It's faster like this */
            } while (p < pf);
            k++;
            ptr = next_list_node(ptr);
        } while (not_nil(ptr));
      }
   }

STOP_PROPAGATION:

   for (i=maxrad; i<r-maxrad; i++)
      for (j=maxrad; j<c-maxrad; j++)
         RC(image, i-maxrad, j-maxrad) = RC(img, i, j);

   /* Liberate memory */   
   imfree_puint32(&img);
   free_list(&boundary);

   image->isbinary=false;
   return true;
}




/* 
 * =====================================================
 *  ---------------------------------------------------
 *    Inexact fast EDT with Image Foresting Transform
 *  ---------------------------------------------------
 * =====================================================
 */

AnimalExport bool
edt_ift(ImgPUInt32 *image, nhood *adj)
{
	char *fname="edt_ift";
   puint32 *img, *imptr, *Dx, *Dy;
   int r,c,n, i, x,maxx, y,maxy, p,px,py, q,qx,qy, *idxlut, 
       left,right,up,down, max_vert, max_hor, nbuck, nh_n,
       *nh_dx, *nh_dy;
   unsigned dx, dy, tmp;
   PQ *pq;
	bool stat;


   assert(image->isbinary);
   image->isbinary=false;

   /* shortcut variables for avoiding indirections */
   img    = DATA(image);
   idxlut = image->lut;

   r  = image->rows;
   c  = image->cols;
   nh_n  = adj->n;
   nh_dx = adj->dx;
   nh_dy = adj->dy;


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
      animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"Variable Dx");  
      return false;                                          
   }

   framesize(adj,&left,&right,&up,&down);
   max_vert = MAX(up,down);
   max_hor  = MAX(left,right);
   /* Number of buckets is maximum arc cost + 1 */
   nbuck = 2*((r-1)*max_vert + (c-1)*max_hor) \
           - max_vert*max_vert - max_hor*max_hor + 1;
//   nbuck = max_vert*max_vert + max_hor*max_hor + 2*((r-1)*max_vert + (c-1)*max_hor);
   pq = new_pqueue(nbuck,n,img);
	if (!pq) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
		return false;
	}

   for (i=0; i < n; ++i)
      if(img[i]) img[i] = COST_MAX;

   /* Insert in list every foreground pixel at distance 1 of the
    * background, which are foreground with at least one background
    * pixel. */
   maxy  = r-1;
   maxx  = c-1;
   imptr = img;
   for (y=0; y <= maxy; ++y)
      for (x=0; x <= maxx; ++x, ++imptr)
         if (*imptr == 0) {
            if (x < maxx && imptr[1]  == COST_MAX) {
               imptr[1] = 1;
               p = imptr+1-img;
				   stat = insert_pqueue(&pq,p);
               CHECK_RET_STATUS(false);;
               Dx[p]=1; Dy[p]=0;
               pq->l.elem[p-1].tag = REMOVED;
            }
            if (x > 0    && imptr[-1] == COST_MAX) {
               imptr[-1] = 1;
               p = imptr-1-img;
				   stat = insert_pqueue(&pq,p);
               CHECK_RET_STATUS(false);;
               Dx[p]=1; Dy[p]=0;
               pq->l.elem[p+1].tag = REMOVED;
            }
            if (y < maxy && imptr[c]  == COST_MAX) {
               imptr[c] = 1;
               p = imptr+c-img;
				   stat = insert_pqueue(&pq,p);
               CHECK_RET_STATUS(false);;
               Dx[p]=0; Dy[p]=1;
               pq->l.elem[p-c].tag = REMOVED;
            }
            if (y > 0    && imptr[-c] == COST_MAX) {
               imptr[-c] = 1;
               p = imptr-c-img;
				   stat = insert_pqueue(&pq,p);
               CHECK_RET_STATUS(false);;
               Dx[p]=0; Dy[p]=1;
               pq->l.elem[p+c].tag = REMOVED;
            }
         }

   while (!is_empty_pqueue(pq)) {
      p = remove_pqueue(pq);
		if (p == NIL) {
			animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
			return false;
		}
      px = p % c;
      py = p / c;
      for (i=0; i < nh_n; i++) {
         qx = px + nh_dx[i];
         qy = py + nh_dy[i];
         if (valid_pixel(r,c,qx,qy)) {
            q  = index1(qy,qx,idxlut);
            if (pq->l.elem[q].tag != REMOVED) {
               dx  = Dx[p]  + abs(px - qx);
               dy  = Dy[p]  + abs(py - qy);
               tmp = dx*dx + dy*dy;
               if (tmp < img[q]) {
                  if (pq->l.elem[q].tag == NOT_INSERTED) {
                     img[q] = tmp;
                     stat = insert_pqueue(&pq,q);
                     CHECK_RET_STATUS(false);;
						} else {
                     stat = update_pqueue(pq,q,tmp);
                     CHECK_RET_STATUS(false);;
                     img[q] = tmp;
						}
                  Dx[q]  = dx;
                  Dy[q]  = dy;
               }
            }
         }
      }
   }

   // Free memory
   free(Dx);
   free(Dy);
   free_pqueue(&pq);
   return true;
}

/*
 * This alg. is O(n^4) for an nxn image, independently of the content
 */

AnimalExport bool
edt_brute_force (ImgPUInt32 *im)
{
   unsigned i, xi, yi,
            j, xj, yj,
            dx,dy, c,
            n, dst;
   puint32 *I=DATA(im);

   c = im->cols;
   n = im->rows*c;
   for (i=0; i<n; ++i)
      if (I[i] == 1) {
         I[i] = PUINT32_MAX;
         xi = i % c;   yi = i / c;
         for (j=0; j<n; ++j)
            if (I[j] == 0) {
               xj  = j % c; yj = j / c;
               dx  = xi-xj; dy = yi-yj;  // ok, modular arithmetic
               dst = dx*dx + dy*dy;
               if (I[i] > dst)
                  I[i] = dst;
            }
      }
   im->isbinary = false;
   return true;
}

/*
 * This one uses about 2x more memory but is faster in most cases
 * It is O(n^4) if the number of white pixels is about the same as the
 * number of black pixels. In general, the complexity is between 
 * O(n^2) and O(n^4) depending on the content.
 */
AnimalExport bool
edt_brute_force_with_list (ImgPUInt32 *im)
{
   unsigned i, xi, yi,
            j, xj, yj,
            dx,dy, c,
            n, dst;
   puint32 *I=DATA(im), *list, n_ones, ptr_zeros;
   char *fname="edt_brute_force_with_list";

   c = im->cols;
   n = im->rows*c;
   ANIMAL_MALLOC_ARRAY(list, puint32, n, fname, false);

   n_ones = 0;
   ptr_zeros = n-1;
   for (i=0; i<n; ++i)
      if (I[i] == 1) {
         list[n_ones++] = i;
         I[i] = PUINT32_MAX;
      } else
         list[ptr_zeros--] = i;

   for (i=0; i<n_ones; ++i) {
         xi = list[i] % c;   yi = list[i] / c;
         for (j=n_ones; j<n; ++j) {
            xj  = list[j] % c; yj = list[j] / c;
            dx  = xi-xj; dy = yi-yj;  // ok, modular arithmetic
            dst = dx*dx + dy*dy;
            if (I[list[i]] > dst)
               I[list[i]] = dst;
         }
   }

   free(list);
   im->isbinary = false;
   return true;
}

/* 
 * =======================
 *  ---------------------
 *    Saito's exact EDT 
 *  ---------------------
 * =======================
 */

/*
 * Final implementation by R. Fabbri, 
 * based on two independent implementations by O. Cuisenaire
 * and J. C. Torelli.
 *
 * PAPER
 *    T. Saito and J.I. Toriwaki, "New algorithms for Euclidean distance 
 *    transformations of an n-dimensional digitised picture with applications",
 *    Pattern Recognition, 27(11), pp. 1551-1565, 1994
 *
 * A nice overview of Saito's method may be found at:
 *    Chapter 2 of "Distance transformations: fast algorithms and applications 
 *    to medical image processing", Olivier Cuisenaire's Ph.D. Thesis, October 
 *    1999, Université catholique de Louvain, Belgium.
 * 
 *  
 */
AnimalExport bool
edt_saito(ImgPUInt32 *im)
{
   char *fname="edt_saito";
   unsigned i,j,r,c,n,l,a,b,m, buffer, nsqr, diag1,
            *ptstart, *ptend, *sq, *df2, *buff, *pt, *npt;
   puint32 *map;



   assert(im->isbinary);
   map = DATA(im);
   r = im->rows; c = im->cols;
   n = r*c; 
   diag1 = ceilf(hypotf(r-1,c-1))-1;   // this is >= the diagonal minus 1

   /* Cuisenaire's idea: a LUT with precomputed i*i */
   nsqr = 2*(diag1 + 1);   /* was: 2*r + 2 in Cuisenaire's code */
   ANIMAL_MALLOC_ARRAY(sq, unsigned, nsqr, fname, false);
   for (i=0; i<nsqr; ++i)
      sq[i] = i*i;

   /* buff stores the current column in step 2 */
   ANIMAL_MALLOC_ARRAY(buff, unsigned, r, fname, false);

   /*-- Step 1 --*/
   for (j=0; j<r; ++j) {
      ptstart = map + c*j;
      ptend = ptstart + c;

      /* forward scan */
      df2 = sq + diag1;  /* the paper: df2 = sq + r, not large enough */
      for (pt=ptstart; pt < ptend; ++pt)
         if (*pt)
            *pt = *(++df2);
         else
            df2 = sq;

      /* backward scan */
      df2 = sq + diag1;  /* the paper: df2 = sq + r, not large enough */
      for (--pt; pt != (unsigned *)(ptstart-1); --pt)
         if (*pt) {
            if (*pt > *(++df2))
               *pt = *df2;
         } else
            df2 = sq;
   }


   /*-- Step 2 --*/

   for (i=0; i<c; ++i) {
      pt = map + i;

      for (j=0; j<r; ++j, pt+=c)
         buff[j]=*pt;

      pt = map + i + c;
      a  = 0;  
      buffer = buff[0];
      for (j=1;  j < r;  ++j, pt+=c) {
         if (a != 0)
            --a;
         if (buff[j] > buffer+1) {
            b = (buff[j] - buffer-1) / 2;
            if (j+b+1 > r)
               b = r-1 -j;

            npt = pt + a*c;
            for (l=a; l<=b; ++l) {
               m = buffer + sq[l+1];
               if (buff[j+l] <= m)
                  break;   /* go to next column j */
               if (m < *npt)
                  *npt = m;
               npt += c;
            }
            a = b;
         } else
            a = 0;
         buffer = buff[j];
      }


      a = 0;
      pt -= 2*c;
      buffer = buff[r-1];

      for (j=r-2;  j != (unsigned)-1;  --j, pt-=c) {
         if (a != 0)
            --a;
         if (buff[j] > buffer+1) {
            b = (buff[j] - buffer-1) / 2;
            if (j < b)
               b = j;

            npt = pt - a*c;
            for (l=a; l<=b; ++l) {
               m = buffer + sq[l+1];
               if (buff[j-l] <= m)
                  break;   /* go to next column j */
               if (m < *npt)
                  *npt = m;
               npt -= c;
            }
            a = b;
         } else
            a = 0;
         buffer = buff[j];
      }

   }

   free(sq);
   free(buff);
   im->isbinary = false;
   return true;
}


bool edt_meijster_2D_from_1D(ImgPUInt32 *im);

/*
 * PAPER
 *    A. Meijster, J.B.T.M. Roerdink, and W.H. Hesselink "A General Algorithm
 *    for Computing Distance Transforms in Linear Time",
 *    proc. of 5th Int. Conf. Mathematical Morphology and its Applications to
 *    Image and Signal Processing, 2000
 */
AnimalExport bool
edt_meijster2000(ImgPUInt32 *im)
{
   char *fname="edt_meijster2000";
   int i,r,c;
   bool stat;
   puint32 infty;

   assert(im->isbinary);

   r = im->rows; c = im->cols;
   infty = PUINT32_MAX - r*r - c*c;
   for (i=0; i < r*c; i++)
      if (DATA(im)[i] == FG)
         DATA(im)[i] = infty;
   
   // Vertical columnwise EDT
   stat = edt_1d_vertical(im);
   // Lotufo's 1D EDT is equivalent to Meijster's scans 1 and 2.
   // What really matters is the 2nd stage.
   CHECK_RET_STATUS(false);

   stat = edt_meijster_2D_from_1D(im);      
   CHECK_RET_STATUS(false);

   im->isbinary = false;
   return true;
}

#define meijster_f(x,i, im_value) ( ( (x) - (i))*((x) - (i) ) + (im_value) )

bool
edt_meijster_2D_from_1D(ImgPUInt32 *im)
{
   char *fname="edt_meijster_2D_from_1D";


   unsigned rows=im->rows, r,
            cols=im->cols, u,
            q, w,
            *s, *t;
   puint32 *img_row, 
   *row_copy;

   s = animal_malloc_array(unsigned, cols);
       if (!s) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }

   t = animal_malloc_array(unsigned, cols);
       if (!t) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }

   row_copy = animal_malloc_array(puint32, cols);
       if (!row_copy) {
          animal_err_flush_trace();                                
          animal_err_register(fname, ANIMAL_ERROR_MALLOC_FAILED,"");  
          return false;                                          
       }

   img_row = DATA(im);
   for (r=0; r<rows; r++,img_row += cols) {
     q = s[0] = t[0] = 0;
     for (u = 1; u < cols; ++u) {
       unsigned im_r_u = img_row[u];
       while (q != (unsigned)-1 
           && meijster_f(t[q],s[q],img_row[s[q]]) >
              meijster_f(t[q],u,im_r_u) 
           )
         --q;

       if (q == (unsigned) -1) {
         q = 0; s[0] = u;
       } else {
         //w = 1 + Sep(s[q],u)
         w = 1 + (unsigned) ( (u*u - (s[q]*s[q]) + img_row[u] - img_row[s[q]]) / (2*(u - s[q])) );
         if (w < cols) {
           ++q;
           s[q] = u;
           t[q] = w;
         }
       }
     }

     memcpy(row_copy, img_row, cols*sizeof(puint32));

     for (u = cols-1; u != (unsigned)-1; --u) {
       img_row[u] = meijster_f(u,s[q], row_copy[s[q]]);
       if (u == t[q])
         --q;
     }
   }

   free(t);
   free(s);
   return true;
}

/*
 * =============================================================================
 *
 * L O T U F O   -   Z A M P I R O L L I' S    E X A C T    E D T 
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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
#include "queue.h"

static bool 
   edt_lz_step2_horizontal_old(ImgPUInt32 *im),
   edt_lz_step2_horizontal(ImgPUInt32 *im);


/*----FUNCTION-------------------------------------------------------------------------
 * 
 * Lotufo-Zampirolli EDT
 *
 * REFERENCE
 *  R. Lotufo and F. Zampirolli, Fast multidimensional parallel euclidean distance 
 *  transform based on mathematical morphology, in T. Wu and D. Borges, editors,
 *  Proccedings of SIBGRAPI 2001, XIV Brazilian Symposium on Computer Graphics 
 *  and Image Processing, pages 100-105. IEEE Computer Society, 2001.
 *
 * - PARAMETER -------------------------------------------------------------------------
 *      Mode   Type             Name            Description
 * -------------------------------------------------------------------------------------
 *    in-out:  ImgPUInt32         im            binary image on input;
 *                                              grayscale on output, 
 *                                              with squared distance transform.
 *-------------------------------------------------------------------------------------*/
AnimalExport bool
edt_lz(ImgPUInt32 *im)
{
   puint32 infty, *im_data;
   char *fname="edt_lz";
   unsigned i, r, c;
   bool stat;

   assert(im->isbinary);

   r = im->rows; c = im->cols;
   infty = PUINT32_MAX - r*r - c*c;

   im_data = DATA(im);

   /* TODO This O(n^2) step is avoided by saito and could be here */
   for (i=0; i < r*c; ++i)
      if (im_data[i]) im_data[i] = infty;

   /* Part 1: vertical 1D EDT */
   stat = edt_lz_step1_vertical(im);
   CHECK_RET_STATUS(false);

   /* Part 2: horizontal 2D EDT from 1D */
   stat = edt_lz_step2_horizontal(im);
   CHECK_RET_STATUS(false);

   im->isbinary = false;
   return true;
}

/*----FUNCTION-------------------------------------------------------------------------
 * 
 * Lotufo-Zampirolli EDT, first implementation of the second part, 
 * which is less efficient but more readable.
 *
 * REFERENCE
 *  R. Lotufo and F. Zampirolli, Fast multidimensional parallel euclidean distance 
 *  transform based on mathematical morphology, in T. Wu and D. Borges, editors,
 *  Proccedings of SIBGRAPI 2001, XIV Brazilian Symposium on Computer Graphics 
 *  and Image Processing, pages 100-105. IEEE Computer Society, 2001.
 *
 * - PARAMETER -------------------------------------------------------------------------
 *      Mode   Type             Name            Description
 * -------------------------------------------------------------------------------------
 *    in-out:  ImgPUInt32         im            binary image on input;
 *                                              grayscale on output, 
 *                                              with squared distance transform.
 *-------------------------------------------------------------------------------------*/
AnimalExport bool
edt_lz_old(ImgPUInt32 *im)
{
   int i, r,c;
   puint32 infty;
   char *fname="edt_lz_old";
   bool stat;

   assert(im->isbinary);

   r = im->rows;
   c = im->cols;
   infty = PUINT32_MAX - r*r - c*c;

   for (i=0; i < r*c; i++)
      if (DATA(im)[i] == FG)
         DATA(im)[i] = infty;

   stat = edt_lz_step1_vertical(im);
   CHECK_RET_STATUS(false);

   stat = edt_lz_step2_horizontal_old(im);
   CHECK_RET_STATUS(false);

   im->isbinary = false;
   return true;
}


/*----FUNCTION-------------------------------------------------------------------------
 * 
 *  Description:  1st step vertical erosions for Lotufo-Zampirolli EDT
 * 
 * - PARAMETER -------------------------------------------------------------------------
 *      Mode   Type             Name            Description
 * -------------------------------------------------------------------------------------
 *    in-out:  ImgPUInt32        im         binary input image; grayscale output.
 *                                          Foreground pixels must have high 
 *                                          enough values ('infinity').
 *-------------------------------------------------------------------------------------*/
bool
edt_lz_step1_vertical(ImgPUInt32 *im)
{
   int   rows=im->rows, r,
         cols=im->cols, c;
   puint32 b;

   for (c=0; c < cols; c++) {
      b=1;
      for (r=1; r<rows; r++)
         if (RC(im,r,c) > RC(im,r-1,c) + b) {
            RC(im,r,c) = RC(im,r-1,c) + b;
            b += 2;
         } else
            b = 1;
      b=1;
      for (r=rows-2; r >= 0; r--) {
         if (RC(im,r,c) > RC(im,r+1,c) + b) {
            RC(im,r,c) = RC(im,r+1,c) + b;
            b += 2;
         } else
            b = 1;
      }
   }

   /* NOTE: Lotufo's implementation (obtained by requesting him) of this first 
    * part  is much less readable. Although pointers could be used more 
    * efficiently, this first part is much faster than the 2nd part and is not 
    * worth optimizing.  So I kept it readable, close to the paper's pseudocode.  
    */

   return true;
}

/* Same as edt_lz_step1_vertical, but also returning the label of the closest
 * feature voxel for each pixel. */
bool
edt_lz_step1_vertical_label(ImgPUInt32 *im, ImgPUInt32 *imlabel)
{
   int   rows=im->rows, r,
         cols=im->cols, c;
   puint32 b;

   for (c=0; c < cols; c++) {
      b=1;
      for (r=1; r<rows; r++)
         if (RC(im,r,c) > RC(im,r-1,c) + b) {
            RC(im,r,c) = RC(im,r-1,c) + b;
            RC(imlabel,r,c) = RC(imlabel,r-1,c);
            b += 2;
         } else
            b = 1;
      b=1;
      for (r=rows-2; r >= 0; r--) {
         if (RC(im,r,c) > RC(im,r+1,c) + b) {
            RC(im,r,c) = RC(im,r+1,c) + b;
            RC(imlabel,r,c) = RC(imlabel,r+1,c);
            b += 2;
         } else
            b = 1;
      }
   }

   /* NOTE: Lotufo's implementation (obtained by requesting him) of this first 
    * part  is much less readable. Although pointers could be used more 
    * efficiently, this first part is much faster than the 2nd part and is not 
    * worth optimizing.  So I kept it readable, close to the paper's pseudocode.  
    */

   return true;
}


/*----FUNCTION-------------------------------------------------------------------------
 * 
 *  Description:  2nd step horizontal erosions for L-Z EDT, OLD 
 *  implementation (more readable but less efficient).
 * 
 *-------------------------------------------------------------------------------------*/
AnimalExport bool
edt_lz_step2_horizontal_old(ImgPUInt32 *im)
{
   SIQueue *Eq, *Eq2, *Wq, *Wq2, *tmp_q;
   int nc, r, c;
   puint32 b;
   bool stat;

   nc = im->cols;
   Eq = siq_new(nc);   
      if(!Eq) { /* report error using Gandalf */;
         animal_err_register ("edt_lz_horizontal", ANIMAL_ERROR_FAILURE,"Demanding new Eq"); 
         return false;
      }
   Eq2 = siq_new(nc);
      if(!Eq2) { 
         animal_err_register ("edt_lz_horizontal", ANIMAL_ERROR_FAILURE,"Demanding new Eq2"); 
         return false;
      }
   Wq = siq_new(nc);
      if(!Wq) { 
         animal_err_register ("edt_lz_horizontal", ANIMAL_ERROR_FAILURE,"Demanding new Wq"); 
         return false;
      }
   Wq2 = siq_new(nc);
      if(!Wq2) { 
         animal_err_register ("edt_lz_horizontal", ANIMAL_ERROR_FAILURE,"Demanding new Wq2"); 
         return false;
      }

   for (r=0; r<im->rows; r++) {
      for (c=1; c < nc; c++) {
         stat = siq_insert(Wq, c);
         stat = siq_insert(Eq, nc-1-c);
      }
      b = 1;
      while (!siq_is_empty(Wq) || !siq_is_empty(Eq)) {
         while (!siq_is_empty(Eq)) {
            stat = siq_remove(Eq, &c);
            if (RC(im,r,c+1) > RC(im,r,c) + b) {
               RC(im,r,c+1) = RC(im,r,c) + b;
               if (c+1 < nc-1) {
                  stat = siq_insert(Eq2, c+1);
               }
            }
         }

         while (!siq_is_empty(Wq)) {
            stat = siq_remove(Wq, &c);
            if (RC(im,r,c-1) > RC(im,r,c) + b) {
               RC(im,r,c-1) = RC(im,r,c) + b;
               if (c-1 > 0) {
                  stat = siq_insert(Wq2, c-1);
               }
            }
         }

         b += 2;
         tmp_q = Wq;
         Wq    = Wq2;
         Wq2   = tmp_q;
         siq_reset(Wq2);

         tmp_q = Eq;
         Eq    = Eq2;
         Eq2   = tmp_q;
         siq_reset(Eq2);
      }
   }

   siq_free(&Eq);
   siq_free(&Wq);
   siq_free(&Wq2);
   siq_free(&Eq2);
   
   return true;
}


/*----FUNCTION-------------------------------------------------------------------------
 * 
 *  Description:  2nd step horizontal erosions for L-Z EDT, fast but 
 *  implementation but considerably more unreadable than 
 *  edt_lz_step2_horizontal_old
 *
 *-------------------------------------------------------------------------------------*/
AnimalExport bool
edt_lz_step2_horizontal(ImgPUInt32 *im)
{
   char *fname="edt_lz_step2_horizontal";
   puint32 b, *Eq, *Eq2, *Wq, *Wq2, *tmp_q, *im_r, *imptr;
   unsigned cols, rows, r, c, 
      Wq_ini, Wq_end,  Eq_ini,  Eq_end,
      Wq2_end, Eq2_end;

   cols = im->cols; rows = im->rows;

   ANIMAL_MALLOC_ARRAY(Eq, puint32, cols, fname, false);
   ANIMAL_MALLOC_ARRAY(Eq2, puint32, cols, fname, false);
   ANIMAL_MALLOC_ARRAY(Wq, puint32, cols, fname, false);
   ANIMAL_MALLOC_ARRAY(Wq2, puint32, cols, fname, false);

   im_r = DATA(im);

   for (r=0; r<rows; r++) {
      for (c=0; c < cols-1; ++c) {
         Wq[c] = c+1;
         Eq[c] = cols-2-c;
      }
      Wq_end = Eq_end = cols - 1;
      Wq_ini = Eq_ini = 0;


      b = 1;
      /* while a queue is not empty */
      while (Wq_end > Wq_ini || Eq_end > Eq_ini) { 
         Eq2_end = Wq2_end = 0;
         while (Eq_end > Eq_ini) {
            c = Eq[Eq_ini++];  /* remove from queue */
            imptr = im_r+c;
            if (imptr[1] > *imptr + b) {
               imptr[1] = *imptr + b;
               if (c+1 < cols-1)
                  Eq2[Eq2_end++] = c+1;  /* insert into queue */
            }
         }

         while (Wq_end > Wq_ini) {
            c = Wq[Wq_ini++];  /* remove from queue */
            imptr = im_r + c - 1;
            if (*imptr > imptr[1] + b) {
               *imptr = imptr[1] + b;
               if (c-1 > 0)
                  Wq2[Wq2_end++] = c-1;  /* insert into queue */
            }
         }

         tmp_q  = Wq; Wq = Wq2; Wq2 = tmp_q;
         tmp_q  = Eq; Eq = Eq2; Eq2 = tmp_q;
         Wq_end = Wq2_end; Eq_end = Eq2_end;
         Wq_ini = Eq_ini = 0;
         b += 2;
      }
      im_r += cols;
   }

   free(Eq);  free(Wq);
   free(Wq2); free(Eq2);
   
   return true;
}

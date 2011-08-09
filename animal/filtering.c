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
  

#include "filtering.h"
#include "util.h"
#include "array.h"
#include "adjacency.h"

/*
   subtracao de media local 2D (vizinhanca circular)
   outside param: indicates value to assume outside the image
   when it will be padded.
*/
AnimalExport Img *
smedia2d(Img *img, int radius, pixval outside)
{
   int i,j, cod;
   unsigned k;
   double sum;
   Img *out;
   DblM *aux;
   nhood *nh;

   /* pad image */
   img = impad(img,radius,radius,outside); 
   aux = new_dbl_m(img->rows,img->cols);

   nh = circular_nhood((double)radius);

   /* @@@@@@@
      This is better done in scilab using fft convolution by a disk
      image. This technique might be used with exact dilation
      as well as with morphological dilation.
   */
   sum=0;
   for (i=radius; i<img->rows-radius-1; i++)
      for (j=radius; j<img->cols-radius-1; j++) {
         for (k=0; k < nh->n; k++)
            sum += (double)RC(img,i+nh->dy[k],j+nh->dx[k])/(double)nh->n;
         RC(aux,i,j) = (double)RC(img,i,j)-(double)sum;
         sum=0;
      }

   /* normalize */                /* @@@ if speed is needed, maybe normalize
                                         within the above loop */
   cod = normal_dbl_m(aux,0,255);  
   if (!cod) return NULL;   

   out = new_img(img->rows-2*radius, img->cols-2*radius);
   for (i=0; i<out->rows; i++)
      for (j=0; j<out->cols; j++)
         RC(out,i,j) =(pixval) RC(aux,i+radius,j+radius);

   free_dbl_m(&aux);
   imfree(&img);
   return out;
}

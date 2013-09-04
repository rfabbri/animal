/* 
 * -------------------------------------------------------------------------
 *
 * M A T H E M A T I C A L   M O R P H O L O G Y
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
  

#include <stdlib.h>
#include "morphology.h"
#include "util.h"
#include "plist.h"
#include "analysis.h"
#include "adjacency.h"

/* se:      Arbitrary structuring element represented 
            as a binary array.
            Defaults to: [0 1 0
                          1 1 1 
                          0 1 0] 
   center:  origin of structuring element.
            Shold be within image dimensions.
            Defaults to the center of the SE array. 

   - Assumes 0 out of image boundaries, so a frame around the image is
   always 0.
 */

AnimalExport Img*
imerode(Img *img, struct_elem *se)
{
   Img *imO;
   bool must_free_se=false;


   if (se == NULL) {
      se = get_crux_se();
      must_free_se = true;
   }

   imO = new_img(img->rows,img->cols);
   imclone_attrib(imO,img);
   
   imerode_np(imO, img, se);

   if (must_free_se)
      free_se(&se);
   return imO;
}

/* 
   - suffix "np" stands for "no padding"
   - assumes image is properly padded!
   - does not trimm result
   - assume ero and se already allocated
   - useful inside loops

*/
AnimalExport int
imerode_np(Img *ero, Img *img, struct_elem *se)
{
   int i,j,k;
   plist_ptr p;
   Img *sedata;


   sedata = se->data;
   if (ISBINARY(img)) {
      for(i=se->center[0]; i < img->rows -(sedata->rows - se->center[0]); i++) 
      for(j=se->center[1]; j < img->cols -(sedata->cols - se->center[1]); j++)
         if (RC(img,i,j) == FG) {
            RC(ero,i,j) = FG;
            for (k=0,p=se->list;  k<se->npts;  k++,p=p->next)
               if (RC(img,i+p->y,j+p->x) == BG) {
                  RC(ero,i,j) = BG;
                  break;
               }
         }
   } else { // grayscale erosion (same thing, but slower)
      for(i=se->center[0]; i < img->rows -(sedata->rows - se->center[0]); i++) 
      for(j=se->center[1]; j < img->cols -(sedata->cols - se->center[1]); j++) {
         RC(ero,i,j) = RC(img,i,j);
         for (k=0,p=se->list;  k<se->npts;  k++,p=p->next)
            if (RC(ero,i,j) > RC(img,i+p->y,j+p->x))
               RC(ero,i,j) = RC(img,i+p->y,j+p->x);
      }
   }

   return 1;
}

AnimalExport Img *
imdilate(Img *img, struct_elem *se)
{
   Img *imO, *aux;
   Img *sedata;
   bool must_free_se=false;

   if (se == NULL) {
      se = get_crux_se();
      must_free_se = true;
   }

   sedata = se->data;
   img = impad2(img, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0],0); 
   imO = new_img(img->rows,img->cols);
   imclone_attrib(imO,img);

   imdilate_np(imO,img,se);

   aux = imO;
   imO = imtrim2(imO, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0]); 
   imfree(&aux);
   imfree(&img);
   if (must_free_se)
      free_se(&se);
   return imO;
}

AnimalExport int
imdilate_np(Img *dil, Img *img, struct_elem *se)
{
   int i,j,k;
   plist_ptr p;
   Img *sedata;

   sedata = se->data;
   for(i=se->center[0]; i < img->rows-(sedata->rows-se->center[0]); i++) 
      for(j=se->center[1]; j < img->cols-(sedata->cols-se->center[1]); j++)
         if (RC(img,i,j) == FG)
            for (k=0,p=se->list;  k<se->npts;  k++,p=p->next) //@@@weird
               RC(dil,i+p->y,j+p->x) = FG;
   return 1;
}



/*
	open + close
*/
AnimalExport Img *
imregularize(Img *img, struct_elem *se)
{
   Img *imO, *aux;
   Img *sedata;
	int i;

   if (se == NULL)
      se = get_crux_se();

   sedata = se->data; //@@@ maybe turn these pad/trim for SE's into a function
   img = impad2(img, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0],0); 
   imO = new_img(img->rows,img->cols);
   imclone_attrib(imO,img);

   imerode_np(imO,img,se);

	for (i=0; i<img->rows*img->cols; i++)
		img->data[i] = 0;
   imdilate_np(img,imO,se);

	for (i=0; i<img->rows*img->cols; i++)
		imO->data[i] = 0;
   imdilate_np(imO,img,se);

	for (i=0; i<img->rows*img->cols; i++)
		img->data[i] = 0;
   imerode_np(img,imO,se);

   aux = imO;
   imO = imtrim2(img, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0]); 
   imfree(&aux);
   imfree(&img);
   if (se != NULL)
      free_se(&se);
   return imO;
}


AnimalExport Img *
imopen(Img *img, struct_elem *se)
{
   Img *imO, *aux;
   Img *sedata;

   if (se == NULL)
      se = get_crux_se();

   sedata = se->data; //@@@ maybe turn these pad/trim for SE's into a function
   img = impad2(img, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0],0); 
   imO = new_img(img->rows,img->cols);
   imclone_attrib(imO,img);

   imerode_np(imO,img,se);
      imfree(&img);
      img = imclone(imO);
   imdilate_np(imO,img,se);

   aux = imO;
   imO = imtrim2(imO, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0]); 
   imfree(&aux);
   imfree(&img);
   if (se != NULL)
      free_se(&se);
   return imO;
}

AnimalExport Img *
imclose(Img *img, struct_elem *se)
{
   Img *imO, *aux;
   Img *sedata;

   if (se == NULL)
      se = get_crux_se();

   sedata = se->data; //@@@ maybe turn these pad/trim for SE's into a function
   img = impad2(img, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0],0); 
   imO = new_img(img->rows,img->cols);
   imclone_attrib(imO,img);

   imdilate_np(imO,img,se);
      imfree(&img);
      img = imclone(imO);
   imerode_np(imO,img,se);

   aux = imO;
   imO = imtrim2(imO, se->center[1], sedata->cols - se->center[1],\
                     se->center[0], sedata->rows - se->center[0]); 
   imfree(&aux);
   imfree(&img);
   if (se != NULL)
      free_se(&se);
   return imO;
}


/*
   new_se
   allocates a new structuring element, and already compute its
   coordinate list.
*/

AnimalExport struct_elem*
new_se(Img *data, int crow, int ccol)
{
   int i;
   plist_ptr p;
   struct_elem *se;
   Img *sedata;
   int is_foreground(pixval);

   se = (struct_elem *) malloc(sizeof(struct_elem));
      if (!se) return NULL;
   se->data = imclone(data);
      if (!se->data) return NULL;
   sedata = imclone(data); /* auxiliar */
      if (!se->data) return NULL;
   se->center[0] = crow; se->center[1] = ccol;

   // the origin of the SE does not matter 
//   RC(sedata,se->center[0],se->center[1]) = 0;

   // determine the SE coordinates.
   se->npts=imfind(sedata,is_foreground,&se->list); 
      if (!se->list) return NULL;

   p=se->list;
   for (i=0; i<se->npts; i++) {
      p->x -= se->center[1];
      p->y -= se->center[0];
      p = p->next;
   }

   imfree(&sedata);
   return se;
}
/* 
   auxiliar to new_se
 */
//@@@ static
AnimalExport int
is_foreground(pixval p)
{
   return p==FG;
}

AnimalExport void
free_se(struct_elem **strel)
{
   struct_elem *se;
   se = *strel;
   if (se != NULL) {
      if (se->data != NULL)
         imfree(&se->data);
      if (se->list != NULL)
         free_plist(&se->list); 
      free(se);
      *strel = NULL;
   }
}


AnimalExport struct_elem*
get_crux_se()
{
   struct_elem *crux;
   Img *img;

   img=new_img(3,3);
                   RC(img,0,1)=1;
   RC(img,1,0)  =  RC(img,1,1)  =  RC(img,1,2)=1;
                   RC(img,2,1)=1;
   crux = new_se(img,1,1);
   imfree(&img);
   return crux;
}

/*
   Exact Dilations
*/

AnimalExport Img *
edilate(Img *img, double radius)
{
   Img *tmp, *result;
   int irad;

   /*
      pad img
   */
   irad = (int) ceil(radius);
   result = new_img(img->rows+2*irad,img->cols+2*irad);

   edilate_np(result,img,radius);

   tmp = result;
   result = imtrim(result,irad,irad);
   imfree(&tmp);
   return result;
}

AnimalExport bool
edilate_np(Img *dil, Img *img, double radius)
{
   int i, j, k, irad, *pt;
   SEDR *sedr;
   unsigned p0;
   unsigned p; //register 

   sedr = grow_sedr(radius);
      if (!sedr) return 0;
   pt = sedr->pt;
   irad=(int) ceil(radius); 

   p0 = 0;
   for (k=0; non_negative_dbl(radius - sedr->distance[k]) && 
             k < (int)sedr->length; k++) {
      for (i=irad; i<dil->rows-irad; i++) 
         for (j=irad; j<dil->cols-irad; j++)         
            if (RC(img,i-irad,j-irad) == FG) {
               p = p0;
               do {  // @@@ unroll
                  RC(dil, i + pt[p], j + pt[p+1]) = FG;
                  p+=2;
               } while (p < sedr->npts[k]);
            }
      p0 = p;
   }

  return true;
}


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Computes regional minima of an image
 *
 * INPUT
 *    - im: grayscale image
 *    - nh: connectivity / structuring element
 *    - option: indicates the pixel value of the regional minima in the output 
 *    image. May be
 *       + REGMIN_BINARY: minima will have value 1; other pixels are zero
 *       + REGMIN_VALUE: minima will have the same value as in the input image
 *       + REGMIN_SEQUENTIAL: minima will have a sequential value from 1 to N
 *
 * OUTPUT 
 *    - an image with the regional minima. Their values depends on the
 *    option parameter (see above)
 *
 * ALGORITHM
 *    - Image Foresting Transform (see ift.h)
 *
 * TODO
 *    - It currently returns only one random pixel in each regional
 *    minimum. It would be nice to allow the whole regional minimum to
 *    be returned.
 * 
 *----------------------------------------------------------------------------*/

AnimalExport ImgPUInt32 *
regional_minima(Img *im, nhood *nh, regmin_opt option)
{
   int r, c;
   puint32 i, k;
   char *fname="regional_minima";
   ImgPUInt32 *regmin;
   ImgPUInt32 *lambda;
   ann_img *aimg;

   r = im->rows; c = im->cols;
   lambda = new_img_puint32(r,c);
      if (!lambda) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }

   for (i=0; i< (puint32) r*c; i++)
      DATA(lambda)[i] = i;

   aimg = ift_lifo(im, lambda, NULL, nh, pcost_ini);
      if (!aimg) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }

   imfree_puint32(&lambda);

   regmin = new_img_puint32(r,c);
      if (!regmin) {
         animal_err_register (fname, ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }

   switch(option) {
      case REGMIN_BINARY:
         for (i=0; i< (puint32)r*c; i++)
            if (DATA(aimg->pred)[i] == i)
               DATA(regmin)[i] = FG;
         break;
      case REGMIN_VALUE:
         for (i=0; i< (puint32)r*c; i++)
            if (DATA(aimg->pred)[i] == i)
               DATA(regmin)[i] = (puint32)DATA(im)[i];
         break;
      case REGMIN_SEQUENTIAL:
         k = 1;
         for (i=0; i< (puint32)r*c; i++)
            if (DATA(aimg->pred)[i] == i)
               DATA(regmin)[i] = k++;
         break;
      default:
         ANIMAL_ERR_FIRST(fname, ANIMAL_ERROR_FAILURE, "invalid option", NULL);
         break;
   }
   
   free_ann_img(&aimg);

   return regmin;
}

/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Watershed transform for image segmentation
 *    - also known as "Sup Reconstruction".
 *    - it is better to calculate gradient before using it without markers.
 *    - its core is easily extended for chromatic images
 * 
 * INPUT
 *    - im: grayscale image
 *    - nh: connectivity / structuring element
 *    - markers: image with the markers (seeds). Each mark must have a
 *    unique label from 1 to N, where N is the number of marks.
 *       + if minima == NULL, then the regional minima of "im" will be
 *       taken as the markers.
 *
 * OUTPUT 
 *    - an image with the watershed regions, each with a unique number.
 *    If minima != NULL, the regions will have the same label as the
 *    corresponding supplied markers.
 *
 * ALGORITHM
 *    - Image Foresting Transform (see ift.h)
 * 
 *----------------------------------------------------------------------------*/
AnimalExport ImgPUInt32 *
watershed(Img *im, ImgPUInt32 *markers, nhood *nh)
{
   ann_img *aimg;
   ImgPUInt32 *regions;

   if (markers == NULL) {
      markers = regional_minima(im, nh, REGMIN_SEQUENTIAL);
         if (!markers) {
            animal_err_register ("watershed", ANIMAL_ERROR_FAILURE,""); 
            return NULL;
         }
   }

   /* wshed proper */

   aimg = ift_fifo(im, markers, im, nh, pcost_suprec);
      if (!aimg) {
         animal_err_register ("watershed", ANIMAL_ERROR_FAILURE,""); 
         return NULL;
      }

   regions = aimg->label;

   imfree_puint32(&aimg->cost);
   imfree_puint32(&aimg->pred);
   free(aimg);
   return regions;
}

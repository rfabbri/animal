/*
 * =============================================================================
 *
 * A N N O T A T E D   I M A G E   S T R U C T U R E
 *
 * This is used by propagation algorithms such as IFT and Exact
 * Dilations, as well as some distance transforms with label propagation.
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
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

#include "annimg.h"

AnimalExport ann_img *
new_ann_img(Img *img)
{
   ann_img *aimg;
   int r,c;

   ANIMAL_MALLOC_OBJECT(aimg, ann_img, "new_ann_img",NULL);

   aimg->img = img;
   r = img->rows; c = img->cols;
   aimg->cost  = new_img_puint32(r,c);
   aimg->label = new_img_puint32(r,c);
   aimg->pred  = new_img_puint32(r,c);

   return(aimg);
}

AnimalExport void
free_ann_img(ann_img **aimg)
{
   ann_img *a = *aimg;

   assert(a);
   imfree_puint32(&a->cost);
   imfree_puint32(&a->label);
   imfree_puint32(&a->pred);
   free(a);
   *aimg = NULL;
}

AnimalExport ImgPUInt32 *
root_map(ImgPUInt32 *pred)
{
   ImgPUInt32 *root=NULL;
   puint32 p, n;

   root = imclone_puint32(pred);

   n = (unsigned) pred->rows * pred->cols;
   for (p=0; p < n; p++)
      DATA(root)[p] = get_root(pred, p);

   return root;
}

AnimalExport int
get_root(ImgPUInt32 *pred, puint32 p)
{
   if (DATA(pred)[p] == p)
      return p;
   return(get_root(pred,DATA(pred)[p]));
}

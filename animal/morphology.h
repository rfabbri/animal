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
  

#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H 1

#include "common.h"
#include "img.h"
#include "adjacency.h"

BEGIN_C_DECLS

typedef enum {
   REGMIN_BINARY, /* minima will have value 1, zero otherwise */
   REGMIN_VALUE,  /* minima will have the same value as in the input image */
   REGMIN_SEQUENTIAL  /* minima will have a sequential value from 1 to N */
} regmin_opt;

typedef struct _structuring_element  {
   Img *data;
   int center[2];
   plist_ptr list;  // @@@ use a vector!!!!!!
   int npts;
} struct_elem;


AnimalExport Img
  *edilate(Img *img, double radius),        /* exact dilations */
  *imdilate(Img *img, struct_elem *se),
  *imerode(Img *img, struct_elem *se),
  *imopen(Img *img, struct_elem *se),
  *imclose(Img *img, struct_elem *se),
  *imregularize(Img *img, struct_elem *se); /* open + close */

AnimalExport ImgPUInt32
  *watershed(Img *im, ImgPUInt32 *markers, nhood *nh),
  *regional_minima(Img *im, nhood *nh, regmin_opt option);

AnimalExport bool
   edilate_np(Img *dil, Img *img, double radius);

AnimalExport int
   imdilate_np(Img *dil, Img *img, struct_elem *se),
   imerode_np(Img *ero, Img *img, struct_elem *se);

AnimalExport void 
   free_se(struct_elem **strel);

AnimalExport struct_elem
   *new_se(Img *data, int crow, int ccol),
   *get_crux_se();

END_C_DECLS
#endif /* !MORPHOLOGY_H */

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
  

#ifndef SEGMENT_H
#define SEGMENT_H 1
#include "common.h"
#include "img.h"
#include "plist.h"
#include "adjacency.h"

BEGIN_C_DECLS


/* --- API --- */
AnimalExport int 
   label_components_np(ImgPUInt32 *img, nhood *nh),
   imfill_func(Img *img);

AnimalExport bool
   imfill(Img *img, point *seed, pixval paintval, pixval maskval, nhood *);

AnimalExport Img
  *bwborder(Img *im),
  *bwborder_np(Img *im);

AnimalExport ImgPUInt32
  *label_contour_pixel(Img *bin, list **bel_seq),
  *label_contour(Img *bin);

AnimalExport point
  *find_interior_point(Img *img); // for closed curves


END_C_DECLS

#endif /* !SEGMENT_H */

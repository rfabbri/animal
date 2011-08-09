/*
 * =============================================================================
 *
 * S H A P E   A N A L Y S Y S   R O U T I N E S  --  HEADER
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
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
 * =============================================================================
 */ 
#ifndef ANALYSIS_H
#define ANALYSIS_H 1

#include "common.h"
#include "img.h"
#include "adjacency.h"
#include "plist.h"
#include "sedr.h"
#include "ift.h"

BEGIN_C_DECLS


typedef enum {
   SKL_COSTA_ESTROZI,     // exact euclidean; slow for thick imgs
   SKL_IFT                // Image foresting transform; euclidean metric; almost exact
} skl_algorithm;

/* structure used by skeletonization routines*/
typedef struct annskl { 
   ImgPUInt32 *skl;
   ImgPUInt32 *lbl;
   ImgPUInt32 *dt;       // squared euclidean distance transform 
   int maxradius;        // maximum computed distance 
} annotated_skl;

/* --- API --- */
AnimalExport int 
   msskl_filter_np(Img *shp, Img *sklimg, annotated_skl *skl, int thresh),
   percol(Img *img, int opt),
   follow(Img *img, int **x, int **y, int nhood),
   thinzs_np(Img *im);

AnimalExport unsigned long 
   bwarea(Img *img);

AnimalExport ImgPUInt32
   *msskl_difference(ann_img *aimg),
   *perimeter(Img *bin);

AnimalExport annotated_skl
   *new_ann_skl(int nrow, int ncol, int maxrad),
   *msskl(Img *bin, char side, skl_algorithm alg);

AnimalExport Img
   *msskl_filter(Img *img, annotated_skl *skl, int thresh);

AnimalExport void 
   de_annotate(ann_img **aimg),
   free_ann_skl(annotated_skl **s);

AnimalExport ann_img
   *exact_propagation(Img *image, ImgPUInt32 *label, list *seed, char side);

END_C_DECLS

#endif /* !ANALYSIS_H */

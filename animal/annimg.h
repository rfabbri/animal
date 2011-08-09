/*
 * =============================================================================
 *
 * A N N O T A T E D   I M A G E   S T R U C T U R E  -- HEADER
 *
 * This is used by propagation algorithms such as IFT and Exact
 * Dilations, as well as some distance transforms with label propagation.
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
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
 * =============================================================================
 */ 
#ifndef ANNIMG_H
#define ANNIMG_H 1

#include "common.h"
#include "img.h"

BEGIN_C_DECLS

typedef struct annimg {
   Img *img;
   ImgPUInt32 *cost;
   ImgPUInt32 *label;
   ImgPUInt32 *pred;
} ann_img;


AnimalExport ann_img *
	new_ann_img(Img *img);

AnimalExport void
	free_ann_img(ann_img **aimg);

AnimalExport ImgPUInt32 
   *root_map(ImgPUInt32 *pred);
	
AnimalExport int 
   get_root(ImgPUInt32 *pred, puint32 p);

END_C_DECLS

#endif /* !ANNIMG_H */

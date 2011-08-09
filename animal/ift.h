/*
 * ============================================================================
 *  
 * I M A G E   F O R E S T I N G  T R A N S F O R M  (IFT)  -- HEADER
 *
 * $Revision: 1.2 $  $Date: 2005-07-23 14:01:31 $
 *
 * Thanks to Dr. Alexandre Falcao for his code, which aided the present one.
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
 * 
 * ============================================================================
 */

#ifndef IFT_GENERIC_H
#define IFT_GENERIC_H 1

#include "common.h"
#include "img.h"
#include "adjacency.h"
#include "annimg.h"

BEGIN_C_DECLS


typedef puint32 (*path_cost) (ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q);


/* For approximate EDT and related operators */
AnimalExport puint32 
   pcost_ini(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q),
   pcost_suprec(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q),
   pcost_edt(ann_img *aimg, ImgPUInt32 *lambda, Img *handicap, int p, int q);

AnimalExport ann_img 
   *ift_lifo(Img *image, ImgPUInt32 *lambda, Img *handicap, nhood *adj, path_cost pcost),
   *ift_fifo(Img *image, ImgPUInt32 *lambda, Img *handicap, nhood *adj, path_cost pcost),
   *ift_euclidean_propagation(Img *image, ImgPUInt32 *lambda, nhood *adj, char side);

END_C_DECLS

#endif /* !IFT_GENERIC_H */

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
  

#ifndef ANIMAL_IMG_H
#define ANIMAL_IMG_H 1

#include "common.h"
#include <limits.h>
#include "util.h"
#include "plist.h"

#define WHITE 1
#define BLACK 0
#define FG 1
#define BG 0
#define INTERIOR 0
#define BOTH     1
#define EXTERIOR 2


BEGIN_C_DECLS

/*
 *  GENERIC type and function declarations 
 */

/* Unsigned Char Image */
#define DEFAULT_ 1
#define IMGTYPE_ Img
#define PIXTYPE_ puint8
#define PIXTYPE_MAX_ PIXVAL_MAX
#include "img_g.h"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_
#undef DEFAULT_

/* Signed Integer Image */
#define IMGTYPE_ ImgPInt32
#define PIXTYPE_ pint32
#define PIXTYPE_MAX_ INT_MAX
#include "img_g.h"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_

/* Unsigned Integer Image */
#define IMGTYPE_ ImgPUInt32
#define PIXTYPE_ puint32
#define PIXTYPE_MAX_ PUINT32_MAX
#include "img_g.h"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_


/* --- API --- */
#define DATA(I)  (I->data)  /* Returns pointer to a pixval array */
#define LUT(I)   (I->lut)

/* (Row,Col) indexing */

#define RC(Im,i,j)   ( Im->data[(j) + Im->lut[i]] ) 
#define RC2(Im,i,j)  ( Im->data[(j) + (i)*Im->cols] )
#define index1(i,j,lut) ( (j) + (lut)[i] )
#define index1_mult(i,j,cols) ( (j) + (i)*cols )
#define valid_pixel(r,c,px,py) (px >= 0 && px < c && \
                                py >= 0 && py < r)
#define ISBINARY(Im) (Im->isbinary)


/* Routines for Img */
AnimalExport Img
   *new_img_from_ptr(int nrows, int ncols, pixval *data),
   *imread(char *filename),
   *imsum(Img *a, Img *b),
   *mply_imgs(Img *a, Img *b), 
   *max_imgs(Img *a, Img *b),
   *min_imgs(Img *a, Img *b),
   *img_and(Img *a, Img *b),
   *img_puint32_to_default(ImgPUInt32 *img),
   *impad2(Img *img, int left, int right, int up, int down, pixval value),
   *imtrim2(Img *img, int left, int right, int up, int down);

AnimalExport ImgPUInt32
   *img_default_to_puint32(Img *img);

AnimalExport void 
   imaddval(Img *a, int val),
   mply_img(Img *a, int val),
   imthresh(Img *img, float level),
   im2binary_ip(Img *img),
   imdrawline(Img *img, unsigned x1, unsigned y1, unsigned x2, unsigned y2),
   destroy_cybervis();

AnimalExport int 
   imfind(Img *img, int (*match)(const pixval val), plist_ptr *xylist),
   max_imgs_ip(Img *a, Img *b),
   min_imgs_ip(Img *a, Img *b),
   img_and_ip(Img *a, Img *b),
   imsum_ip(Img *a, Img *b);

AnimalExport double
   *xproj(Img *img),
   *yproj(Img *img);



END_C_DECLS

#endif /* !ANIMAL_IMG_H */

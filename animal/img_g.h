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
  

/*
   Generic types and prototype declarations to be used by img.h
*/

#define MkConcat_(a,b) a ## b

#ifdef DEFAULT_
#  define SUFFIX_  
#  define Cat_(a,b) a
#else
#  define Cat_(a,b) MkConcat_(a,b)
#  define SUFFIX_ Cat_(_,PIXTYPE_)
#endif /* !IMG_G_H */


/* Structure for a generic grayscale image.  */
typedef struct {
   int rows;
   int cols;                
   int *lut; /* Lookup table with row positions used for (row,col) indexing */
   PIXTYPE_ *data; /* row-wise storage */
   bool isbinary;
} IMGTYPE_;

AnimalExport void 
   Cat_(imfree,SUFFIX_) (IMGTYPE_ **img),
   Cat_(imthresh_i,SUFFIX_) (IMGTYPE_ *img, int level),
   Cat_(imprint,SUFFIX_) (IMGTYPE_ *Img),
   Cat_(imset,SUFFIX_)(IMGTYPE_ *img, PIXTYPE_ value);

AnimalExport bool
   Cat_(imwrite,SUFFIX_)(IMGTYPE_ *img, char *fname, bool normalize),
   Cat_(imwrite_ascii,SUFFIX_)(IMGTYPE_ *img, char *fname),
   Cat_(mply_imgs_ip,SUFFIX_) (IMGTYPE_ *a, IMGTYPE_ *b),
   Cat_(imclone_attrib,SUFFIX_) (IMGTYPE_ *dest, IMGTYPE_ *src),
   Cat_(imnormal,SUFFIX_) (IMGTYPE_ *img, PIXTYPE_ lower, PIXTYPE_ upper),
   Cat_(imshow,SUFFIX_)(IMGTYPE_ *img, long double min, long double max);

AnimalExport IMGTYPE_
  *Cat_(imread_ascii,SUFFIX_)(char *fname),
  *Cat_(imread_ascii_zip,SUFFIX_)(char *name, char *unzipper),
/* new_img allocs a new image and clears its contents */
  *Cat_(new_img,SUFFIX_) (int nrows, int ncols), 
  *Cat_(imclone,SUFFIX_) (IMGTYPE_ *img),
  *Cat_(impad,SUFFIX_)(IMGTYPE_ *img, int szx, int szy, PIXTYPE_ value),
  *Cat_(imtrim,SUFFIX_) (IMGTYPE_ *img, int szx, int szy);

AnimalExport PIXTYPE_ 
   Cat_(max_img,SUFFIX_) (IMGTYPE_ *img),
   Cat_(min_img,SUFFIX_)(IMGTYPE_ *img);

AnimalExport curve
  *Cat_(histogram,SUFFIX_)(IMGTYPE_ *img);

AnimalExport int
Cat_(imwrite_ascii_zip,SUFFIX_)(IMGTYPE_ *img, char *filename, char *zipper);

#undef SUFFIX_
#undef Cat_
#undef MkConcat_

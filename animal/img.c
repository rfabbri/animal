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
  

#include "img.h"
#include "sedr.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>        /* -- ImageMagick headers -- */
#include <time.h>
#include <sys/types.h> 
#include <magick/api.h>    /*---------------------------*/

/*
 *  Globals
 */

/* Routines for Unsigned Char Image */
#define DEFAULT_ 1
#define IMGTYPE_ Img
#define PIXTYPE_ puint8
#define PIXTYPE_MAX_ PIXVAL_MAX
#include "img_g.c_incl"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_
#undef DEFAULT_

/* Routines for Signed Integer Image */
#define IMGTYPE_ ImgPInt32
#define PIXTYPE_ pint32
#define PIXTYPE_MAX_ INT_MAX
#include "img_g.c_incl"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_

/* Routines for Unsigned Integer Image */
#define IMGTYPE_ ImgPUInt32
#define PIXTYPE_ puint32
#define PIXTYPE_MAX_ PUINT32_MAX
#include "img_g.c_incl"
#undef IMGTYPE_
#undef PIXTYPE_
#undef PIXTYPE_MAX_


AnimalExport Img *
new_img_from_ptr(int nrows, int ncols, pixval *data)
{
   Img *I=NULL;
   int i;

   /* --- Allocations --- */
   I = (Img *) malloc(sizeof(Img));
   if (I == NULL) return NULL;

   if (I->data == NULL) return NULL;
   I->data = data;

   I->lut = (int *) calloc(nrows, sizeof(int));
   if (I->lut == NULL) return NULL;

   /* --- Settings --- */
   for (i=0; i<nrows; i++)
      I->lut[i] = i*ncols;
   I->rows = nrows;
   I->cols = ncols;
   I->isbinary = false;

   return I;
}

AnimalExport Img *
impad2(Img *img, int left, int right, int up, int down, pixval value)
{
   Img *pimg; /* padded image */
   int i, j;
   
   pimg = new_img(img->rows+up+down, img->cols+left+right);
   if (pimg == NULL) return NULL;
   imclone_attrib(pimg, img);

   if (value != 0) 
      imset(pimg,value); 


   /* USAR MEMCPY */
   for (i=0; i<img->rows; i++) 
      for (j=0; j<img->cols; j++)
         RC(pimg,i+up,j+left) = RC(img,i,j);

   return pimg;
}


AnimalExport Img *
imtrim2(Img *img, int left, int right, int up, int down)
{
   Img *timg;
   int i,j;

   timg = new_img(img->rows-up-down, img->cols-left-right);
   if (timg == NULL) return NULL;
   imclone_attrib(timg, img);

   for (i=0; i<timg->rows; i++) 
      for (j=0; j<timg->cols; j++)
         RC(timg,i,j) = RC(img,i+up,j+left);

   return timg;
}


static int RGBMagick2GrayIMG(PixelPacket *pix, Img *img);
static int PSEUDOMagick2GrayIMG(Image *image, Img *img);


/*----------------------------------------------------------
 * imread
 *
 * Read ANY image into a graylevel direct-class image
 *----------------------------------------------------------*/

AnimalExport Img *
imread(char *filename)
{    
   Img *img;

   unsigned int i,j;
   unsigned short divider;
   unsigned long imgsize;


   /* ImageMagick variables */     
   ExceptionInfo  exception;
   Image          *image;
   ImageInfo      *image_info;
   PixelPacket    *pix=NULL;
   ImageType      itype;

   /* Initialize the image info structure and read an image.  */
   InitializeMagick(NULL);
   GetExceptionInfo(&exception);
   image_info=CloneImageInfo((ImageInfo *) NULL);
   strcpy(image_info->filename, filename);
    
   image=ReadImage(image_info,&exception);
   if (image == (Image *) NULL) { 
      if(exception.reason != NULL)
         animal_error(exception.reason, "function imread");
      return NULL;
   } 

   imgsize = (image->rows) * (image->columns);
   if (image->rows > INT_MAX || image->columns > INT_MAX)
      return NULL;
   img = new_img(image->rows, image->columns);

   switch(image->storage_class) {
      case DirectClass:
            itype = GetImageType(image, &exception);

            pix=GetImagePixels(image, 0, 0, image->columns, image->rows);
            if(pix == (PixelPacket *) NULL) {
               GetExceptionInfo(&exception);  
               GetImageException(image,&exception); 
               if(exception.reason != NULL)
                  animal_error(exception.reason, "function imread");
               DestroyExceptionInfo(&exception);
               DestroyMagick();
               return NULL;
            }

            if(itype == BilevelType ) {
               printf("Bilevel Image\n");
               divider=(QuantumDepth==16)?65535:255;
               for (i=0; i < image->rows; i++) 
                  for (j=0; j < image->columns; j++) 
                     RC(img,i,j) = RCbyR(pix,i,j,img->cols).red/divider;
               img->isbinary = true;
            } else {
               printf("Truecolor Image\n");
               RGBMagick2GrayIMG(pix,img); 
            }
            break;
      default:       
            printf("Indexed Image\n");  
            PSEUDOMagick2GrayIMG(image,img);
            break;
   }
   
   /* Terminate Imagemagick */
   DestroyImageInfo(image_info);
   DestroyImage(image);
   DestroyMagick();
   return img;
}


/* Private utility for imread */
int
RGBMagick2GrayIMG(PixelPacket *pix, Img *img) 
{
   int i,j;
   unsigned short r,g,b, divider; 
   float y;

   divider = (QuantumDepth==16)? 257:1;

   for (i=0; i < img->rows; i++) {
      for (j=0; j < img->cols; j++) {
         r =  (float) RCbyR(pix,i,j,img->cols).red;
         g =  (float) RCbyR(pix,i,j,img->cols).green;
         b =  (float) RCbyR(pix,i,j,img->cols).blue;
         y = 0.299*r + 0.587*g + 0.114*b;
         RC(img,i,j) = (pixval) (y / divider);
      }
   }
   return 1;
}

/* Private utility for imread */
int
PSEUDOMagick2GrayIMG(Image *image, Img *img)
{
   int i,j;
   float y;
   unsigned short r, g, b, divider;
   IndexPacket    *index; 
   ExceptionInfo  exception;
   GetExceptionInfo(&exception);

   if((index = GetIndexes(image)) == (IndexPacket *) NULL) {
      GetExceptionInfo(&exception);  
      GetImageException(image,&exception); 
      if( (exception.reason != NULL) && (exception.description != NULL))
         animal_error(exception.reason,"function PSEUDOMagick2GrayIMG");
      return 0;
   }

   if (image->colors == 2 || IsMonochromeImage(image,&exception)) {
      img->isbinary = true;
      divider = MaxRGB;
   } else
      divider = (QuantumDepth==16)? 257:1;

   for (i=0; i< img->rows; i++)
      for (j=0; j< img->cols; j++) {
         r = image->colormap[ RCbyR(index,i,j,img->cols) ].red;
         g = image->colormap[ RCbyR(index,i,j,img->cols) ].green;
         b = image->colormap[ RCbyR(index,i,j,img->cols) ].blue;
         y = 0.299*r + 0.587*g + 0.114*b;
         RC(img,i,j) = (pixval) (y / divider);
      }

   return 1;
}

/*
   simplest possible binarization
*/
AnimalExport void
im2binary_ip(Img *img)
{
   int i;
   pixval max;

   max = max_img(img);
   for (i=0; i<img->rows*img->cols; i++)
      img->data[i] = img->data[i]/max;
   img->isbinary=true;
}


AnimalExport void
imthresh(Img *img, float level)
{
   int i;
   for (i=0; i<img->rows*img->cols; i++)
      img->data[i] = (img->data[i]>=(pixval)level*255);
   img->isbinary = true;
}

//inline@@@
AnimalExport Img *
img_and(Img *a, Img *b) 
{
   Img *nd;

   nd=imclone(a);
   if (nd==NULL) return NULL;
   img_and_ip(nd,b);
   nd->isbinary = true;
   return nd;
}

//inline@@@
AnimalExport int
img_and_ip(Img *a, Img *b)
{
   int i;

   for (i=0; i < a->rows*a->cols; i++)
      a->data[i] = a->data[i] && b->data[i]; 

   return 1;
}

AnimalExport Img *
mply_imgs(Img *a, Img *b)
{
   Img *mp;
   
   mp=imclone(a);
   if (mp==NULL) return NULL;
   mp->isbinary = false;
   mply_imgs_ip(mp,b);
   return mp;
}

//inline@@@
AnimalExport Img *
imsum(Img *a, Img *b)
{
   Img *im;
   im = imclone(a);
   if (im==NULL) return NULL;
   imsum_ip(im,b);
   im->isbinary = false;
   return im;
}

/*
   result placed in a
*/
AnimalExport int
imsum_ip(Img *a, Img *b)
{
   int i;
   unsigned long s;

   for (i=0; i < a->rows*a->cols; i++) {
      s = a->data[i] + b->data[i]; 
      a->data[i] = (pixval)(s > PIXVAL_MAX)? PIXVAL_MAX :s;
   }

   return 1;
}

AnimalExport Img *
max_imgs(Img *a, Img *b)
{
   Img *s;

   s=imclone(a);
   if (s==NULL) return NULL;
   s->isbinary=false;
   max_imgs_ip(s,b);
   return s;
}

AnimalExport int
max_imgs_ip(Img *a, Img *b)
{
   int i;

   for (i=0; i<a->rows*a->cols; i++)
      a->data[i] = MAX(a->data[i],b->data[i]);

   return 1;
}

AnimalExport Img *
min_imgs(Img *a, Img *b)
{
   Img *s;

   s=imclone(a);
   if (s==NULL) return NULL;
   s->isbinary=false;
   min_imgs_ip(s,b);
   return s;
}

AnimalExport int
min_imgs_ip(Img *a, Img *b)
{
   int i;
   
   for (i=0; i<a->rows*a->cols; i++)
      a->data[i] = MIN(a->data[i],b->data[i]);

   return 1;
}

/*
   its your problem not to add negative or too large values
*/
AnimalExport void
imaddval(Img *a, int val)
{
   int i;
   for (i=0; i < a->rows*a->cols; i++)
      a->data[i] += val;
}

AnimalExport void
mply_img(Img *a, int val)
{
   int i;
   for (i=0; i < a->rows*a->cols; i++)
      a->data[i] *= val;
}


/*
   Returns a list of coordinates "xylist" of "img" pixels for
   which the function "match" returns a positive value (>=1).
   Match is user-defined.
*/
AnimalExport int
imfind(Img *img, int (*match)(const pixval val), plist_ptr *xylist)
{
   int i,j, npts=0;

   *xylist = NewPList();
   for (i=0; i<img->rows; i++)
      for (j=0; j<img->cols; j++)
         if (match(RC(img,i,j))) {
            PrependPList(xylist,j,i);
            npts++;
         }
   return npts;
}

/*
   x = col
   y = nrows - row - 1
*/
AnimalExport void
imdrawline(Img *img, unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
#define ani_swap(a,b) { tmp = a; a = b; b = tmp; }

   unsigned x,y, tmp;
   double a;  

   if ( (x2-x1) == 0) { // vertical line
      if ( (y2-y1) == 0) {
         RC(img,img->rows-y1-1,x1)=1;
         return;
      }
      if (y1 > y2)
         ani_swap(y1,y2);
      for (y=y1; y<=y2; y++)
         RC(img,img->rows-y-1,x1)=1;
      return;
   }

   a = ((double)y2-y1)/((double)x2-x1);
   if (-1 <= a && a <= 1) {
      if (x2 < x1) {
         ani_swap(x2,x1);
         ani_swap(y2,y1);
      }
      for (x=x1; x<=x2; x++) {
         y = PROUND(unsigned,y1 + a*(x-x1));  
         RC(img,img->rows-y-1,x)=1;
      }
   } else if (a > 1) {
      if (y2 < y1) {
         ani_swap(x2,x1);
         ani_swap(y2,y1);
      }
      a = 1.0/a;
      for (y=y1; y<=y2; y++) {
         x = PROUND(unsigned,x1 + a*(y-y1));  
         RC(img,img->rows-y-1,x)=1;
      }
   } else { /* a < -1 */
      if (y2 > y1) {
         ani_swap(x2,x1);
         ani_swap(y2,y1);
      }
      a = 1.0/a;
      for (y=y2; y<=y1; y++) {
         x = PROUND(unsigned,x1 - a*(y1-y));  
         RC(img,img->rows-y-1,x)=1;
      }
   }
}

AnimalExport double *
yproj(Img *img)
{
   int i,j;
   double *v;

   v = (double *)calloc(img->cols,sizeof(double));
   if (v==NULL) return NULL;

   for (j=0; j<img->cols; j++)
      for (i=0; i<img->rows; i++)
         v[j] += RC(img,i,j);
   return v;
}

AnimalExport double *
xproj(Img *img)
{
   int i,j;
   double *v;
   v = (double *)calloc(img->rows,sizeof(double));
   if (v==NULL) return NULL;

   for (i=0; i<img->rows; i++)
      for (j=0; j<img->cols; j++)
         v[i] += RC(img,i,j);
   return v;
}


/*
 *  Conversion between image types
 */

AnimalExport ImgPUInt32 *
img_default_to_puint32(Img *img)
{
   ImgPUInt32 *ui;
   int i;

   ui = new_img_puint32(img->rows,img->cols);
   for (i=0; i<ui->rows*ui->cols; i++)
      ui->data[i] = (puint32) img->data[i];

   return ui;
}

/*
    If the default image has a smaller range than the ImgPUInt32,
    behaviour for too large values is undefined.
    Try using the "imnormal" routine before this one.
 */
AnimalExport Img *
img_puint32_to_default(ImgPUInt32 *img)
{
   Img *di;
   int i;

   di = new_img(img->rows,img->cols);
   for (i=0; i<di->rows*di->cols; i++)
      di->data[i] = (pixval) img->data[i];

   return di;
}

/*
   Frees all global data structures allocated so far.
   The user may continue using the API.
*/
AnimalExport void
destroy_cybervis()
{
   if (global_sedr)
      free_sedr(&global_sedr);
}

#include "common.h"
#include<animal/animal.h>


main (int argc, char *argv[])
{
   char filename[100];
   ImgPUInt32 *im,*im2;
   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);
   int stat;
   unsigned i;

   if (--argc >=1)
      strncpy(filename, argv[1],99);
   else {
      fprintf(stderr,"usage: txtsee <filename>\n");
      return 1;
   }

   /*
   im = new_img_puint32(500,500);
   for (i=0; i<im->rows*im->cols; ++i) // fill with random content 
      DATA(im)[i] = i % 100000 + 2000;


   // Write in tst.dat.gz 
   stat = imwrite_ascii_zip_puint32(im,"tst.dat","gzip -f --fast");
   if (stat != 0) {
      fprintf(stderr,"error writing file\n");
      return 1;
   }
   imfree_puint32(&im);
   */

   /* Decompress and read the image */
   im2 = imread_ascii_zip_puint32(filename,NULL);
   if (!im2) {
      fprintf(stderr,"error reading file\n");
      return 1;
   }

   imshow_puint32(im2,0,-1);

   imfree_puint32(&im2);
   animal_heap_report(NULL);

	return 0;
}

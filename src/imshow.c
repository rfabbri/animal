#include<animal/animal.h>
#include <string.h>

/* 
 * This program just tests the  imshow routine for various cases
 *
 */


main (int argc, char *argv[])
{
   char filename[100] = "../images/integral.png";
   Img *im;
   ImgPUInt32 *tmp;
   bool stat;
   unsigned i;

   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);

   if (--argc >=1)
      strncpy(filename, argv[1],99);

   im=imread(filename);
   printf("Showing image the way it was read (no scaling)\n");
   printf("isbinary: %s\n",(im->isbinary)?"yes":"no");
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing binary image with binary==true\n");
   im2binary_ip(im);
   stat = imshow(im,0,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing binary image with binary==false (treated as grayscale)\n");
   im->isbinary = false;
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing binary image in PUINT32 format\n");
   tmp = img_default_to_puint32(im);
   stat = imshow_puint32(tmp,0,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing binary image in PUINT32 format, isbinary==false\n with scaling\n");
   tmp->isbinary=false;
   stat = imshow_puint32(tmp,0,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing binary image in PUINT32 format, isbinary==false; \n no scaling\n");
   stat = imshow_puint32(tmp,-1,-1);
   if (!stat) animal_err_default_reporter();

   for (i=0; i<tmp->rows*tmp->cols; ++i)
      if (DATA(tmp)[i])
         DATA(tmp)[i] = PUINT32_MAX/2;
   printf("Showing dark image in PUINT32 format, \n no scaling\n");
   stat = imshow_puint32(tmp,-1,-1);
   if (!stat) animal_err_default_reporter();

   imfree(&im);
   imfree_puint32(&tmp);

   im = new_img(300,300);
   printf("Showing black constant image, binary == false\n no scaling\n");
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing black constant image, binary == true\n no scaling\n");
   im->isbinary=true;
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   im->isbinary=false;
   printf("Showing white constant image, binary == false\n no scaling\n");
   imset(im,255);
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   printf("Showing white constant image, binary == false\n with scaling\n");
   stat = imshow(im,0,-1);
   if (!stat) animal_err_default_reporter();

   imset(im,150);
   stat = imshow(im,-1,-1);
   if (!stat) animal_err_default_reporter();

   imfree(&im);

   // showing constant white image
   // showing constant black image
   // showing constant white PUINT32 image
   // showing constant black PUINT32 image

   animal_heap_report(NULL);
	return 0;
}

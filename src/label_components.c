#include<animal/animal.h>
#include <string.h>


main (int argc, char *argv[])
{
   char *filename = "../images/integral.png";
   Img *im;
   ImgPUInt32 *lbl;
   int stat;
   nhood *nh;

   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);


   if (--argc >=1)
      strncpy(filename, argv[1],99);

   im=imread(filename);
   imshow(im,0,-1);

   /* ------- */

   lbl = img_default_to_puint32(im);
   nh = get_8_nhood();
   stat=label_components_np(lbl,nh);
      if(stat < 0)
         animal_err_default_reporter();

   printf("Number of regions: %d\n",stat);

   imshow_puint32(lbl,0,-1);

   free_nhood(&nh);
   imfree(&im);
   imfree_puint32(&lbl);
   animal_heap_report(NULL);
	return 0;
}

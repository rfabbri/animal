#include<animal/animal.h>
#include <string.h>


main (int argc, char *argv[])
{
   char *filename = "../images/edges.png";
   nhood *nh;
   Img *im;
   ImgPUInt32 *regions;

   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);

   if (--argc >=1)
      strncpy(filename, argv[1],99);

   im=imread(filename);
//   imshow(im,0,-1);

   nh = get_8_nhood();
   regions = watershed(im,NULL,nh);

 //  imshow_puint32(regions,0,-1);

   imfree(&im);
   free_nhood(&nh);
   imfree_puint32(&regions);
   animal_heap_report(NULL);
	return 0;
}

#include <string.h>
#include <animal/animal.h>


main(int argc, char *argv[])
{
   Img *img, *tmp, *overpose;
   char filename[100] = "../images/bicycle.png";
   annotated_skl *skl;
   int thresh=25, i;

   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);

   if (--argc >=1)
      strncpy(filename, argv[1],99);
   if (argc == 2)
      sscanf(argv[2], "%d",&thresh);


   img=imread(filename);
   im2binary_ip(img);

	// regularize the image
   tmp = img;
   img = imregularize(img, NULL);
   imfree(&tmp);

   skl = msskl(img,INTERIOR,SKL_IFT);
   if (!skl)
      animal_err_default_reporter();

   overpose = new_img(img->rows,img->cols);
   for (i=0; i<img->rows*img->cols; i++)
      overpose->data[i] =(skl->skl->data[i] >= (puint32) thresh) + img->data[i];

   if (!imshow(overpose,0,-1))
      printf("err\n");

   tmp=msskl_filter(NULL, skl, thresh);
   for (i=0; i<img->rows*img->cols; i++)
      overpose->data[i] = (skl->skl->data[i] >= (puint32) thresh) + tmp->data[i];

   imshow(overpose,0,-1);
   imwrite(overpose,"teste.png",true);

   free_ann_skl(&skl);
   imfree(&img);
   imfree(&tmp);
   imfree(&overpose);

   animal_heap_report(NULL);
   return 0;
}

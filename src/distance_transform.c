#include <animal/animal.h>
#include <string.h>

/*
 *   This file demonstrates the distance transform methods implemented
 *   in Animal
 */

int
main (int argc, char *argv[])
{
   char filename[100] = "../images/integral.png";
   Img *im;
   ImgPUInt32 *img, *label=NULL;

   animal_err_set_trace(ANIMAL_ERR_TRACE_ON);

   if (--argc >=1)
      strncpy(filename, argv[1],99);



/*   im=imread(filename);*/
/*   im2binary_ip(im);*/
   im = new_img(7,3);
   im->isbinary=true;
   imset(im,1);
   RC(im,0,0)=0;
   RC(im,0,2)=0;
   RC(im,2,2)=0;
   printf("%dx%d image\n",im->cols,im->rows);
   //imshow(im,0,-1);

   /* Uncomment the following to experiment the methods */
/*   img = distance_transform(im, DT_LOTUFO_ZAMPIROLLI);*/
/*   img = distance_transform(im, DT_MAURER2003);*/
   img = distance_transform_label(im, DT_MAURER2003, true, &label);
/*   img = distance_transform(im, DT_MEIJSTER_2000);*/
   printf("Maurer:\n");
//   img = distance_transform(im, DT_CUISENAIRE_PMON_1999);
/*   img = distance_transform(im, DT_CUISENAIRE_PMN_1999);*/
//   img = distance_transform(im, DT_CUISENAIRE_PSN8_1999);
//   img = distance_transform(im, DT_IFT_8);
//   img = distance_transform(im, DT_EXACT_DILATIONS);
//   img = distance_transform(im, DT_BRUTE_FORCE);
   if (!img)
      animal_err_default_reporter();
   if (!label)
      animal_err_default_reporter();


//   for (i=0; i<im->rows*im->cols; i++) {
//      if (DATA(img)[i] == 0)
//         DATA(img)[i] += 2000;
//      DATA(img)[i] = log(1+DATA(img)[i]);
//      //DATA(img)[i] = sqrt(DATA(img)[i]);
//   }
/*   imshow_puint32(img,0,-1);*/
   imprint_puint32(img);

   if(label) {
     printf("Label:\n");
     imprint_puint32(label);
     imfree_puint32(&label);
   }

   imfree(&im);
   imfree_puint32(&img);
   animal_heap_report(NULL);
	return 0;
}

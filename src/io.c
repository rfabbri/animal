#include<stdio.h>
#include<string.h>
#include<animal/animal.h>

main(int argc, char *argv[])
{
   Img *img;
   char filename[100] = "../images/star.bmp";
   if (--argc ==1)
      strncpy(filename, argv[1],99);


   img=imread(filename);
   if (img == NULL)
      animal_error("unknown reason","main, while reading image");

   printf("size:\t%d rows X %d columns\n\r", img->rows, img->cols);

   if (!imshow(img,-1,-1))
      animal_error("unknown reason","main, while displaying image");
   imfree(&img);

   return 0;
}

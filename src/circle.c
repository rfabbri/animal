#include<stdio.h>
#include<animal/animal.h>

main(int argc, char *argv[])
{
   Img *img;
   nhood *nh;
   int i,radius=10;

   if (--argc == 1)
      sscanf(argv[1],"%d",&radius);

   img=new_img(2*radius+1,2*radius+1);

   nh = circular_nhood(radius);
   for (i=0; i<nh->n; i++)
      RC(img,radius+nh->dy[i],radius+nh->dx[i]) = 1;

      imshow(img,0,-1); 

   free_nhood(&nh);
   imfree(&img);
   return 0;
}

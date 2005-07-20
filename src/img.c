#include <animal/animal.h>

main()
{
   Img *I;
   pixval *p;

   I = new_img(3,5);
      if (!I) animal_error(MEMERROR1,"main");

   p = DATA(I);
   p[3*4]=69;
   imprint(I);
   imfree(&I);
   return 0;
}

#include<animal/animal.h>

main()
{
   Img *I;
   plist_ptr xylist;
   int match(pixval p);

   I = new_img(3,5);
   I->data[0]=1; RC(I,1,3)=1; RC(I,0,2)=1;
   imfind(I, match, &xylist);

   imprint(I);
   print_plist(xylist);

   free_plist(&xylist);
   imfree(&I);
   return 0;
}

int match(pixval p)
{
   return p == 1;
}

#include <stdio.h>
#include <animal/animal.h>

main(int argc, char *argv[])
{
   SEDR *s;
   double radius=0;

   if (--argc == 1)
      sscanf(argv[1], "%lf", &radius);
   
   s = grow_sedr(radius);
      if (!s) animal_error(MEMERROR1,"grow_sedr");
   print_sedr(s, MIN(s->length,15));
   printf("****************************\n");
   s = grow_sedr(3);
   print_sedr(s, MIN(s->length,15));
   free_sedr(&s);

   return 0;
}

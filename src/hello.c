#include <animal/animal.h>
#include <stdio.h>
/*
   Very simple example just for understanding how
   to use the animal_hello routine from libanimal to sum two 
   variables.
*/

main()
{
   double a=97.21, b=-28.21;

   printf("The sum of %f and %f equals %f.\n",a,b, animal_hello(a,b));

   return 0;
}

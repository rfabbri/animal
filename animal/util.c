/*
 * -------------------------------------------------------------------------
 * ANIMAL - ANIMAL IMage Processing LibrarY
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------------
 */ 
  

#include "util.h"
#include <stdlib.h>

/* print error message and halt */
AnimalExport void 
animal_error(char *what,char *where)
{ 
  fprintf(stderr,"ERROR at %s: %s\n",where, what);
  exit(1);
}

/* swap a and b */
AnimalExport void 
intswap(int *a, int *b)
{ 
  int c;    
  c  = *a;
  *a = *b;
  *b = c;
}

AnimalExport double **
alloc_double_m(int row, int col)
{
   char *fname ="alloc_double_m";
   int i;
   double **M;

   ANIMAL_MALLOC_ARRAY(M, double *, row, fname,NULL);

	for (i=0; i<row; i++)
		ANIMAL_MALLOC_ARRAY(M[i], double, col, fname,NULL);

   return M;
}

AnimalExport void 
free_double_m(double ***M, int row)
{
   int i;

	for (i=0; i<row; i++) 
		free((*M)[i]);
	free(*M);
}

/*
   works on row-wise stored matrices
*/
AnimalExport void 
mply2dfloat(float *M1, int r1, float *M2, int c2, int col, float *M1M2 )
{
   int i,j,k; float soma;

	for(i=0; i< r1; ++i)
		for(j=0; j< c2; ++j) {
         for (soma=0,k=0; k<col; ++k)
            soma += RCbyR(M1,i,k,col) * RCbyR(M2,k,j,c2);
         RCbyR(M1M2,i,j,c2) = soma;
      }
}

void show_int_v(int *v, int dim)
{
   int i;
	for (i=0; i<dim; ++i)
		printf("%6i ", v[i]);
	putchar('\n');
}

/*
   Curve methods
*/
AnimalExport curve *
new_curve(unsigned n)
{
   curve *c;
 
   c = (curve *)malloc(sizeof(curve));
      if (!c) return NULL;
   c->n = n;
   c->X = (double *)calloc(n,sizeof(double));
   c->Y = (double *)calloc(n,sizeof(double));
      if (!c->X || !c->Y) return NULL;
   return c;
}

AnimalExport void
free_curve(curve **pc)
{
   curve *c=*pc; 
   
   if (c) {
      free(c->X);
      free(c->Y);
      free(c);
      *pc = NULL;
   }
}

AnimalExport void
print_curve(curve *c)
{
   unsigned i;
   for(i=0; i < c->n; i++) 
      printf("%5u:%f\t%f\n",i,c->X[i],c->Y[i]);
}

AnimalExport void 
rgb2hsv(float r, float g, float b, float *h, float *s, float *v)
{
   float max, min, delta;

   if (r > g) {
      max = r;
      min = g;
   } else  {
      max = g;
      min = r;
   }

   if (max < b) 
      max = b;
   else if (min > b)
      min = b;
    
   delta = max - min;

   *v = max;
   if (max != 0.0)
     *s = delta / max;
   else
     *s = 0.0;

   if (*s == 0.0) *h = -1;
   else {
     if (r == max)
       *h = (g - b) / delta;
     else if (g == max)
       *h = 2 + (b - r) / delta;
     else if (b == max)
       *h = 4 + (r - g) / delta;
     *h *= 60.0;
     if (*h < 0) *h += 360.0;
     *h /= 360.0;
   }
}

AnimalExport void 
hsv2rgb(float h, float s, float v, float *r, float *g, float *b)
{
   int i;
   float f,p,q,t;

   if (s == 0.0) {
      *r = *g = *b = v;
      return;
   }
   
   if (h == 1.0)
      h = 0.0;

   h = h*6;
   i = (int) h;
   f = h - i;
   p = v * (1- s);
   q = v * (1- s*f);
   t = v * (1- s*(1-f));

   switch (i) {
      case 0:
         *r = v; *g = t; *b = p;
         break;
      case 1:
         *r = q; *g = v; *b = p;
         break;
      case 2:
         *r = p; *g = v; *b = t;
         break;
      case 3:
         *r = p; *g = q; *b = v;
         break;
      case 4:
         *r = t; *g = p; *b = v;
         break;
      case 5:
         *r = v; *g = p; *b = q;
         break;
   }
}

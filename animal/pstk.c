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
  

#include "pstk.h"
#include <stdlib.h>

AnimalExport int 
ptpush(pstk_ptr *pstk, point *pt)
{
   return PrependPList(pstk, pt->x, pt->y);
}

AnimalExport point *
ptpop(pstk_ptr *pstk)
{
   pstk_ptr aux, stk;
   point *p;

   stk = *pstk;
   assert(pstk_isnt_empty(stk));

   ANIMAL_MALLOC_OBJECT(p,point,"ptpop",NULL);

   p->x = stk->x; p->y = stk->y;
   
   /* remove top */
   aux=stk;
   stk=stk->next;
   free(aux);
   *pstk=stk;
   return p;
}

AnimalExport void 
free_pstk(pstk_ptr *p)
{
   free_plist(p);
}


// SIMPLE INTEGER STACK METHODS

AnimalExport int_stk *
new_stk(int n)
{
   int_stk *stk;
   char *func="new_stk";

   assert(n);
   ANIMAL_MALLOC_OBJECT(stk,int_stk,func,NULL);
   ANIMAL_MALLOC_ARRAY(stk->data, int, n, func,NULL);

   stk->n   =  n;
   stk->top = -1; 
   return stk;
}

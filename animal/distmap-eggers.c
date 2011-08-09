/*
 * =============================================================================
 *
 * E G G E R S'   E U C L I D E A N   D I S T A N C E    M A P P I N G
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:47 $
 *
 * ANIMAL - ANIMAL IMage Processing LibrarY
 * Copyright (C) 2002,2003-2011  Ricardo Fabbri <rfabbri@users.sourceforge.net>
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
 * =============================================================================
 */ 

#include "distmaps.h"

/* TODO
 * - use inline specifier */

#define LIST_SIZE 1000  /* Lists are static and reallocated if necessary */

/* List nodes*/
typedef struct egg_node {
  unsigned x, y;
  unsigned k;         /* direction index */
  puint32 d; 
} egg_node;

typedef struct egg_NODE{
  unsigned x, y;
  unsigned k;         /* direction index */
} egg_NODE;

/* Static lists */
typedef struct egg_list {
  egg_node *pixels; 
  unsigned size;
  unsigned start, end;
} egg_list;

typedef struct egg_LIST {
  egg_NODE *pixels;
  unsigned size;
  unsigned start, end;
} egg_LIST;

/*------------------------------------------------------
 *    Prototypes
 */

static egg_list *
   create_egg_list(unsigned size);

static egg_LIST *
   create_egg_LIST(unsigned size);

static bool
   update_egg_list(unsigned size, egg_list *lst),
   update_egg_LIST(unsigned size, egg_LIST *lst),
   add_to_egg_LIST (egg_LIST *lst, unsigned x, unsigned y, unsigned k),
   add_to_egg_list (egg_list *lst, unsigned x, unsigned y, unsigned k, unsigned d),
   pop_egg_list(egg_list *lst, egg_node *z),
   pop_egg_LIST(egg_LIST *lst, egg_NODE *z);

static void
   delete_egg_list(egg_list *lst),
   delete_egg_LIST(egg_LIST *lst),
   empty_egg_list(egg_list *lst),
   empty_egg_LIST(egg_LIST *lst);


/*------------------------------------------------------
 *    Main function
 */

#define inrange(x,y) (x >= 0 && x < sx && y >= 0 && y < sy)

AnimalExport bool
edt_eggers(ImgPUInt32 *im)
{
   char *fname="edt_eggers";
   short sx, sy;
   unsigned k, x, y, iter, DIRECT, INDIRECT, *pt, MAXD, adddir, addind;
   puint32 d, *map;
   bool non_empty, stat;
   int dx[8] = {1,1,0,-1,-1,-1,0,1},
       dy[8] = {0,-1,-1,-1,0,1,1,1};

         


   egg_LIST *LIST1, *LIST2, *TLIST;   
   egg_list *list1, *list2, *tlist;
   egg_node z; egg_NODE Z;


   /* Memory Allocation */

   LIST1 = create_egg_LIST(LIST_SIZE); 
   if (!LIST1) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return false;
   }
   LIST2 = create_egg_LIST(LIST_SIZE);
   if (!LIST2) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return false;
   }
   list1 = create_egg_list(LIST_SIZE); 
   if (!list1) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return false;
   }
   list2 = create_egg_list(LIST_SIZE);
   if (!list2) {
      animal_err_register (fname, ANIMAL_ERROR_FAILURE,"");
      return false;
   }

   /* Initialization */

   sx = im->cols; sy = im->rows;
   iter = 0;
   INDIRECT = 0;
   MAXD = sx*sx + sy*sy;

   pt = map = DATA(im);
   for (y=0; y<sy; ++y)
      for (x=0; x<sx; ++x, ++pt) {
         if (*pt == 0) {
            if (x < sx-1 && *(pt+1) != 0)
               add_to_egg_LIST(LIST1,x,y,1);
            if (y != 0 && *(pt-sx) != 0)
               add_to_egg_LIST(LIST1,x,y,3);
            if (x != 0 && *(pt-1) != 0)
               add_to_egg_LIST(LIST1,x,y,5);
            if (y < sy-1 && *(pt+sx) != 0)
               add_to_egg_LIST(LIST1,x,y,7);
         } else
            *pt = MAXD;
      }

   /* Iterations */

   do {
      non_empty = false;

      ++iter;
      adddir=2*iter -1;  addind=2*adddir;
      DIRECT   = INDIRECT + adddir;
      INDIRECT = INDIRECT + addind;

      while (pop_egg_list(list1, &z)) {
         non_empty = true;
         /* propagate(z,k,d+adddir) */
         k = z.k;
         x = z.x + dx[k];
         y = z.y + dy[k];
         d = z.d + adddir;
         if (inrange(x,y) && *(pt = map+x+sx*y) > d) {
            *pt = d;
            stat = add_to_egg_list(list2,x,y,k,d);
            CHECK_RET_STATUS(false);
         }
         /* end of propagate(z,k,d+adddir) */
      }

      while (pop_egg_LIST(LIST1, &Z)) {
         non_empty = true;
         /* propagate(z,k+1,DIRECT) */
         k = (Z.k + 1)%8;
         x = Z.x + dx[k];
         y = Z.y + dy[k];
         if (inrange(x,y) && *(pt = map+x+sx*y) > DIRECT) {
            *pt  = DIRECT;
            stat = add_to_egg_list(list2,x,y,k,DIRECT);
            CHECK_RET_STATUS(false);
         }
         /* propagate(z,k-1,DIRECT) */
         k = (Z.k - 1)%8;
         x = Z.x + dx[k];
         y = Z.y + dy[k];
         if (inrange(x,y) && *(pt = map+x+sx*y) > DIRECT) {
            *pt  = DIRECT;
            stat = add_to_egg_list(list2,x,y,k,DIRECT);
            CHECK_RET_STATUS(false);
         }

         /* PROPAGATE(z,k,INDIRECT) */
         k = Z.k;
         x = Z.x + dx[k];
         y = Z.y + dy[k];
         if (inrange(x,y) && *(pt = map+x+sx*y) > INDIRECT) {
            *pt  = INDIRECT;
            stat = add_to_egg_LIST(LIST2,x,y,k);
            CHECK_RET_STATUS(false);
         }

      }

      TLIST=LIST1; LIST1=LIST2; LIST2=TLIST;
      empty_egg_LIST(LIST2);

      tlist=list1; list1=list2; list2=tlist;
      empty_egg_list(list2);
   } while(non_empty);

   /* De-allocation */
   delete_egg_list(list2); delete_egg_list(list1);
   delete_egg_LIST(LIST2); delete_egg_LIST(LIST1);

   im->isbinary = false;
   return true;
}

/*------------------------------------------------------
 *    List functions
 */
egg_list *
create_egg_list(unsigned size) 
{
   char *fname="create_egg_list";
   egg_list *lst;

   ANIMAL_MALLOC_OBJECT(lst, egg_list, fname, NULL);
   ANIMAL_MALLOC_ARRAY(lst->pixels, egg_node, size, fname, NULL);
   lst->start = lst->end = 0;
   lst->size  = size;
   return lst;
}

egg_LIST *
create_egg_LIST(unsigned size)
{
   char *fname="create_egg_LIST";
   egg_LIST *lst;

   ANIMAL_MALLOC_OBJECT(lst, egg_LIST, fname, NULL);
   ANIMAL_MALLOC_ARRAY(lst->pixels, egg_NODE, size, fname, NULL);
   lst->start = lst->end = 0;
   lst->size  = size;
   return lst;
}

bool
update_egg_list(unsigned size, egg_list *lst)
{
   char *fname="update_list";
   ANIMAL_REALLOC_ARRAY(lst->pixels, egg_node, size, fname, false);
   lst->size = size;
   return true;
}

bool
update_egg_LIST(unsigned size, egg_LIST *lst)
{
   char *fname="update_LIST";
   ANIMAL_REALLOC_ARRAY(lst->pixels, egg_NODE, size, fname, false);
   lst->size = size;
   return true;
}

void
delete_egg_list(egg_list *lst)
{
   free(lst->pixels);
   free(lst);
}

void
delete_egg_LIST(egg_LIST *lst)
{
   free(lst->pixels);
   free(lst);
}

void
empty_egg_list(egg_list *lst)
{
   lst->start = lst->end = 0;
}

void
empty_egg_LIST(egg_LIST *lst)
{
   lst->start = lst->end = 0;
}

bool
add_to_egg_LIST (egg_LIST *lst, unsigned x, unsigned y, unsigned k)
{
   char *fname="add_to_egg_LIST";
   egg_NODE *z;
   bool stat;
   if (lst->end + 2 >= lst->size) {
      stat = update_egg_LIST(lst->size*2, lst);
      CHECK_RET_STATUS(false);
   }

   z = lst->pixels + lst->end;
   z->x=x; z->y=y; z->k=k;
   lst->end++;

   return true;
}

bool
add_to_egg_list (egg_list *lst, unsigned x, unsigned y, unsigned k, unsigned d)
{
   char *fname="add_to_egg_list";
   egg_node *z;
   bool stat;
   if (lst->end + 2 >= lst->size) {
      stat = update_egg_list(lst->size*2, lst);
      CHECK_RET_STATUS(false);
   }

   z = lst->pixels + lst->end;
   z->x=x; z->y=y; z->k=k; z->d=d;
   lst->end++;

   return true;
}

bool
pop_egg_list(egg_list *lst, egg_node *z)
{
   if (lst->end == lst->start)
      return false;
   *z = lst->pixels[lst->start++];
   return true;
}

bool
pop_egg_LIST(egg_LIST *lst, egg_NODE *z)
{
   if (lst->end == lst->start)
      return false;
   *z = lst->pixels[lst->start++];
   return true;
}

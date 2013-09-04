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
  

#include "plist.h"
#include "util.h"
#include <stdlib.h>


#define Allocpoint() (plist_ptr) malloc(sizeof(pt))

AnimalExport void 
free_plist(plist_ptr *p)
{
   plist_ptr aux;

   aux = *p;
   while (aux != NULL) {
      aux = (*p)->next;
      free(*p);
      *p = aux;
   }
}


/* 
 * Returns 0 if allocation fails; 1 for success;
 */
AnimalExport int
PrependPList(plist_ptr *head, int x, int y)
{
   plist_ptr pnew;

   pnew = Allocpoint();
   if (pnew == NULL)
      return 0;
   pnew->x = x;
   pnew->y = y;

   pnew->next = *head;
   *head = pnew;
   return 1;
}

/*
   Returns NULL if allocation fails. 
   Otherwise, returns a pointer to the newly installed element.
*/
AnimalExport plist_ptr 
append_plist(plist_ptr *lst, int x, int y)
{
   plist_ptr p, paux, prev=NULL;

   p=*lst;
   
   while (p != NULL) {
      prev = p;
      p = p->next;
   }

   paux = Allocpoint();
   if (!paux)
      return NULL;
   
   paux->x = x;
   paux->y = y;

   if (prev != NULL)
      prev->next = paux;
   else  /* list was intitially empty */
      *lst = paux;
   

   paux->next = NULL;

   return paux;
}

AnimalExport void
print_plist(plist_ptr p)
{
   printf("\n--------------------\n");
   while (p != NULL) {
      printf("%d %d\n", p->x, p->y);
      p = p->next;
   }
   printf("---------END--------\n");
}

AnimalExport void
print_list(list *p)
{
   printf("\n-------LIST---------\n");
   while (isnt_empty_list(p)) {
      printf("%d\n", p->val);
      p = p->next;
   }
   printf("--------END---------\n");
}

//-----------------------------------------------------------------------------
AnimalExport void 
free_list(list **l)
{
   list *aux;
   while (isnt_empty_list(*l)) {
      aux = *l;
      *l = next_list_node(*l);
      free(aux);
   }
   *l = NULL;
}

AnimalExport int
list_append(list **lst, int val)
{
   char *fname="list_append";
   list *pnode, *last=*lst;

   ANIMAL_MALLOC_OBJECT(pnode, list, fname,false);
   pnode->val = val;

   if (last) {
      while (last->next)
         last = last->next; 
      pnode->next = last->next;
      last->next  = pnode;
   } else {
      pnode->next = NULL;
      *lst = pnode;
   }
   return true;
}

AnimalExport int
list_prepend(list **lst, int val)
{
   list *node;

   node = (list *) malloc(sizeof(list));
   if (!node) return false;
   node->val = val;
   node->next = *lst;
   *lst = node;
   return true;
}

AnimalExport int 
list_remove_head(list **lst, int *val)
{
   int elem;
   list *aux;

   if (is_empty_list(*lst))
      return false;

   aux  = *lst; 
   *lst = next_list_node(aux);
   elem = get_list_point(aux);
   free(aux);
   *val = elem;
   return true;
}

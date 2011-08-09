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
  

#ifndef PLIST_H
#define PLIST_H 1


#include "common.h"

BEGIN_C_DECLS

/*
 *  List of (row,col) point coordinates.
 */

/* --- Types --- */
typedef struct pt *plist_ptr;
typedef struct pt {
   int x;
   int y;
   struct pt *next;
} pt;

typedef struct list *list_ptr;
typedef struct list { // list of single integer indexes
   int val;
   struct list *next;
} list;


/* --- API --- */
#define NewPList()    NULL
#define is_empty(p)   ( (p)==NULL )  /* "p" is plist_ptr */
#define isnt_empty(p)   ( (p)!=NULL )  /* "p" is plist_ptr */
#define Nextpoint(p) ( (p)->next )

AnimalExport plist_ptr 
   append_plist(plist_ptr *p, int x, int y);

AnimalExport int 
   PrependPList(plist_ptr *head, int x, int y);

AnimalExport void
   free_plist(plist_ptr *p),
   print_plist(plist_ptr p);


/*
   @@@ the interface to list is more abstract than the plist interface
*/
#define new_list()    NULL
#define is_empty_list(l)   ( (l)==NULL )  
#define isnt_empty_list(l) ( (l)!=NULL ) 
#define next_list_node(listptr)  ( (listptr)->next )
#define get_list_point(listptr) ( (listptr)->val )
#define not_nil(listptr) ((listptr)!=NULL)
#define get_list_head(l)   (l)  // returns  a listptr

AnimalExport int 
   list_remove_head(list **l, int *val),
   list_append(list **l, int val),
   list_prepend(list **l, int val);

AnimalExport void 
   print_list(list *l),
   free_list(list **l);



END_C_DECLS

#endif /* !PLIST_H */

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
  

#include<stdio.h>
#include"set.h"
#include"util.h"

AnimalExport inline int 
set_insert(set **s, int val)
{
   return list_prepend(s,val);
}

AnimalExport inline void 
free_set(set **s)
{
   free_list(s);
}

AnimalExport inline int
set_remove(set **s, int *val)
{
   return list_remove_head(s,val);
}

AnimalExport inline int
get_a_set_element(set *s, set_iterator *it, int *val)
{
   if (*it == NULL) 
      return false;

   *val = (*it)->val;
   *it = (*it)->next;
   return true;
}

AnimalExport inline int
get_number_of_set_elements(set *s)
{
   int n=0;

   while(s)
      s=s->next;

   return n;
}

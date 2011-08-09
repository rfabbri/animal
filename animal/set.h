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
  

#ifndef SET_H_
#define SET_H_ 1

#include "common.h"
#include "plist.h"

BEGIN_C_DECLS
/*
   Integer set
*/

typedef list set;
typedef set *set_iterator;

#define new_set() new_list()
#define is_empty_set(l)   is_empty_list(l)
#define isnt_empty_set(l) isnt_empty_list(l)
#define initial_set_position(s) s;

AnimalExport int 
   set_remove(set **s, int *val),
   get_a_set_element(set *s, set_iterator *it, int *val),
   set_insert(set **s, int val);
   
AnimalExport inline int
   get_number_of_set_elements(set *s);

AnimalExport void 
   free_set(set **s);


END_C_DECLS

#endif /* !SET_H */

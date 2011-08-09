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
  

#ifndef PSTK_H
#define PSTK_H 1
#include "common.h"
#include "plist.h"
#include "util.h"

BEGIN_C_DECLS
/*
 *  Stack of (row,col) point coordinates. More suited for internal
 *  usage of the library. 
 */

typedef plist_ptr pstk_ptr;

#define new_pstk()    NULL
#define pstk_is_empty(p)   ( (p)==NULL ) 
#define pstk_isnt_empty(p)   ( (p)!=NULL )  

AnimalExport int 
   ptpush(pstk_ptr *stk, point *pt);

AnimalExport point
   *ptpop(pstk_ptr *stk);

AnimalExport void 
   free_pstk(pstk_ptr *p);


/*
 *   A SIMPLE INTEGER STACK
 *       - used for storing points as single indexes
 *       - number of elements is known in advance
 *       - interface is good enough for use outside Animal
 */

typedef struct stk {
   int *data;
   int top;
   int n;
} int_stk;

// Methods for a simple integer stack
#define free_stk(stk) free(stk);
#define evacuate_stk(stk) stk->top = -1
#define push(p,stk) { stk->top++; stk->data[stk->top] = (p); }
#define pop(stk) stk->data[stk->top--]
#define stk_isnt_empty(stk) (stk->top != -1)

AnimalExport int_stk
   *new_stk(int n);

END_C_DECLS

#endif /* !PSTK_H */

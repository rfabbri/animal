/*
 * =============================================================================
 * 
 * I F T   P R I O R I T Y   Q U E U E
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
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
  

#ifndef IFT_PQUEUE_H
#define IFT_PQUEUE_H
#include "common.h"
#include "util.h"

BEGIN_C_DECLS
/* 
   The IFT priority queue PQ consists of two data structures:
   a circular queue C and a table L that encodes all possible
   doubly-linked lists. Each node of the circular queue C points
   to the doubly-liked-list in L that have nodes with the same cost.

   PQ requires that the maximum possible increment along the paths be a
   non-negative integer less than the number of buckets in C. An extra
   bucket is created to store infinity costs for the LIFO policy. The
   queue size increases dynamically whenever (maxcost-mincost) >
   (nbuckets-1).

   PQ->c.first[i] gives the first element that is in bucket i.
   PQ->c.last[i]  gives the last  element that is in bucket i.
   PQ->c.nbuckets gives the number of buckets in C.  
	PQ->c.mincost  gives the minimum cost of a node in queue.
	PQ->c.maxcost  gives the maximum cost of a node in queue.
	PQ->c.tiebreak gives the FIFO or LIFO tie breaking policies

   All possible doubly-linked lists are represented in L. Each
   bucket contains a doubly-linked list that is treated as a FIFO.

   PQ->l.elem[i].next -- the next element to i; 
   PQ->l.elem[i].prev -- the previous element to i; 
   PQ->l.elem[i].tag  -- the status of an element with respect to PQ: 
      INSERTED, NOT_INSERTED, or REMOVED

	PQ->l.nelems	    -- the total number of elements that
	  	can be inserted in PQ. It is usually the number of pixels
	  	in a given image or the number of nodes in a graph.
	PQ->l.cost[i]		 -- the cost of element i in the graph


   Insertions and deletions are done in O(1).  
	Removal may take O(k+1), where k+1 is the number of buckets.


   (Thanks to Alexandre Falcao for providing sample code)
*/


/* Tags for PQ nodes */
#define NIL            -1  /* bucket is empty in the PQ         */
#define NOT_INSERTED    0  /* node was never inserted in the PQ */
#define INSERTED        1  /* node is in the PQ                 */
#define REMOVED         2  /* node has been removed from the PQ */

#define IFT_FIFOBREAK   0
#define IFT_LIFOBREAK   1
#define QSIZE		      1024
#define COST_MAX        PUINT32_MAX
#define COST_MIN        0
#define set_pqueue_tiebreak(a,b) (a)->c.tiebreak=b

typedef struct pqnode {
   int next;
   int prev;
   char tag; /* status of the node inside the PQ */
} pqnode;

typedef struct doubly_linked_lists {
    pqnode *elem; /* array of doubly-linked lists nodes */
    puint32 nelems; /* total number of elements */
	 puint32 *cost;  /* cost of the nodes */
} doubly_linked_lists; 

typedef struct circular_queue {
   int *first;  /* queue of the first elements of each doubly linked list */
   int *last;   /* queue of the last  elements of each doubly linked list */
   puint32 nbuckets; 
   puint32 mincost; /* minimum cost of a node in queue */
   puint32 maxcost; /* maximum cost of a node in queue */
	char tiebreak; /* LIFO or FIFO (default) */
} circular_queue;

typedef struct pq { /* Priority queue for IFT (designed by A. Falcao) */
   circular_queue c;  
   doubly_linked_lists l;
} PQ;

/* --- API --- */
PQ
   *new_pqueue(puint32 nbuckets, puint32 nelems, puint32 *cost),
	*enlarge_pqueue(PQ **q, puint32 nbucks);

void 
   free_pqueue(PQ **q),
   reset_pqueue(PQ *q),
	remove_pqueue_elem(PQ *q, int elem);

bool 
   update_pqueue(PQ *q, int elem, puint32 newcost),
   insert_pqueue(PQ **q, int elem),
   is_empty_pqueue(PQ *q);

int 
   remove_pqueue(PQ *q);


END_C_DECLS
#endif /* !IFT_PQUEUE_H */

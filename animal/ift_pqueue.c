/* 
 * =============================================================================
 *
 * I F T   P R I O R I T Y   Q U E U E
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
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

#include "ift_pqueue.h"

PQ *
new_pqueue(puint32 nbuckets, puint32 nelems, puint32 *cost)
{
	PQ *q=NULL;
	char *func = "new_pqueue"; 

	ANIMAL_MALLOC_OBJECT(q, PQ, func,NULL);

	ANIMAL_MALLOC_ARRAY(q->c.first, int, nbuckets+1, func,NULL);
	ANIMAL_MALLOC_ARRAY(q->c.last,  int, nbuckets+1, func,NULL);
	q->c.nbuckets = nbuckets;
	ANIMAL_MALLOC_ARRAY(q->l.elem,  pqnode, nelems, func,NULL);
	q->l.nelems = nelems;
	q->l.cost 	= cost;
	reset_pqueue(q);

	return q;
}

void 
reset_pqueue (PQ *q)
{
  puint32 i;

  q->c.mincost = COST_MAX;
  q->c.maxcost = COST_MIN;
  set_pqueue_tiebreak(q,IFT_FIFOBREAK);

  for (i=0; i < q->c.nbuckets+1; i++)
    q->c.first[i] = q->c.last[i] = NIL;
	
  for (i=0; i < q->l.nelems; i++) {
    q->l.elem[i].next =  q->l.elem[i].prev = NIL;
    q->l.elem[i].tag = NOT_INSERTED;
  }
}

void 
free_pqueue (PQ **q)
{
   PQ *pq;
   pq = *q;

   assert(pq);
	assert(pq->c.first);
   free(pq->c.first);

	assert(pq->c.last);
   free(pq->c.last);

	assert(pq->l.elem);
   free(pq->l.elem);

   free(pq);
   *q = NULL;
}


bool
is_empty_pqueue(PQ *q)
{
   puint32 current, last, nb;

   nb = q->c.nbuckets;
	current = q->c.mincost % nb;

	if (q->c.first[current] != NIL)
		return false;

	last = current;
	current = (current + 1) % nb;

	while ( q->c.first[current] == NIL && current != last)
		current = (current + 1) % nb;

	return  q->c.first[current] == NIL;
}

PQ *
enlarge_pqueue(PQ **pq, puint32 nbucks)
{
	PQ *q1, *q=*pq;
	puint32 i; 
   int first_i;
   puint32 bucket;

	q1=new_pqueue(nbucks, q->l.nelems, q->l.cost);
	if (!q1) {
      animal_err_register ("enlarge_pqueue", ANIMAL_ERROR_FAILURE,""); 
		return NULL;
	}

	q1->c.mincost  = q->c.mincost;
	q1->c.maxcost  = q->c.maxcost;
	q1->c.tiebreak = q->c.tiebreak;
	for (i=0; i < q->c.nbuckets+1; i++)
		if (q->c.first[i] != NIL) {
			first_i = q->c.first[i];
			bucket  = q->l.cost[first_i] % q->c.nbuckets;
			q1->c.first[bucket] = first_i;
			q1->c.last[bucket]  = q->c.last[i];
		}

	for (i=0; i < q->l.nelems; i++)
		q1->l.elem[i] = q->l.elem[i];

	free_pqueue(pq);
	return q1;
}

/*
   Inserts an element into the priority queue.
      - Elements are inserted in the end of the bucket
*/
bool
insert_pqueue(PQ **pq, int elem)
{
	PQ *q=*pq;
   //puint32 bucket;
   puint32 mincost, maxcost, bucket, nb;

	nb = q->c.nbuckets;

	mincost = q->c.mincost;  maxcost = q->c.maxcost;

	if (q->l.cost[elem] == COST_MAX)
		bucket = nb;
	else {
		if (q->l.cost[elem] < mincost)
			mincost = q->l.cost[elem];
		if (q->l.cost[elem] > maxcost)
			maxcost = q->l.cost[elem];
		if ( (maxcost-mincost) > nb-1) {
#ifndef NDEBUG
			fprintf(stderr,"Enlarging queue.\n");
			fprintf(stderr,"maxcost - mincost +1 = %d\n",maxcost-mincost+1);
#endif
			*pq = q = enlarge_pqueue(pq, 2*(maxcost-mincost) + 1);
		}
		bucket = q->l.cost[elem] % q->c.nbuckets;
		q->c.mincost = mincost;
		q->c.maxcost = maxcost;
	}
	bucket = q->l.cost[elem] % q->c.nbuckets;

	if (q->c.first[bucket] 	== NIL) {
		q->c.first[bucket] 	= elem;
		q->l.elem[elem].prev = NIL;
	} else {
		q->l.elem[q->c.last[bucket]].next = elem;
		q->l.elem[elem].prev = q->c.last[bucket];
	}

	q->c.last[bucket] 	= elem;
	q->l.elem[elem].next = NIL;
	q->l.elem[elem].tag  = INSERTED;
	return true;
}

/*
	  Remove elements from queue.
*/


/*----FUNCTION----------------------------------------------------------------
 * 
 * Description:  Remove the element with least cost from the priority queue
 *
 *	   FIFO policy:
 *      	- elems removed from the start of the doubly linked list.
 *	   LIFO policy:
 *     	- elems removed from the end.
 * 
 * INPUT
 *    - q: priority queue
 *
 * OUTPUT 
 *    - the element, in case of success. Otherwise, the function
 *    returns NIL.
 * 
 *----------------------------------------------------------------------------*/
int
remove_pqueue (PQ *q)
{
	int elem=NIL, next, prev;	
   puint32 current, last, nb;

	nb = q->c.nbuckets;
	current = q->c.mincost % nb;

	if (q->c.first[current] == NIL) {
		last = current;

		current = (current + 1) % (nb+1);

		while  (q->c.first[current] == NIL  &&  current != last)
			current = (current+1) % (nb+1);

		if (q->c.first[current] != NIL)
			q->c.mincost = q->l.cost[q->c.first[current]];
		else
			ANIMAL_ERR_FIRST("remove_pqueue", ANIMAL_ERROR_FAILURE,"queue is empty",NIL);
	}

	if (q->c.tiebreak == IFT_LIFOBREAK) { /* remove from the end of the list */
		elem = q->c.last[current];
		prev = q->l.elem[elem].prev;
		if (prev == NIL) /* elem is alone in the list */
			q->c.first[current] = q->c.last[current] = NIL;
		else {
			q->c.last[current] = prev;
			q->l.elem[prev].next = NIL;
		}
	} else { /* FIFO: remove from the start of the list */
		elem = q->c.first[current];
		next = q->l.elem[elem].next;
		q->c.first[current] = next;

		if (next == NIL)  /* list will be empty */
			q->c.last[current] = NIL;
		else
			q->l.elem[next].prev = NIL;
	}

	q->l.elem[elem].tag = REMOVED;
   return elem;
}

void
remove_pqueue_elem(PQ *q, int elem)
{
	int prev, next;
   puint32 bucket;

   if (q->l.cost[elem] == COST_MAX)
   	bucket = q->c.nbuckets;
   else
     	bucket = q->l.cost[elem] % q->c.nbuckets;
 
   prev = q->l.elem[elem].prev;
   next = q->l.elem[elem].next;
   
   /* if elem is the first */
   if (q->c.first[bucket] == elem)
     	q->c.first[bucket] = next;
   else   /* elem is in the middle or end */
     	q->l.elem[prev].next = next;

   if (next == NIL) /* elem is the last */
    	q->c.last[bucket] = prev;
   else 
    	q->l.elem[next].prev = prev;
 
   q->l.elem[elem].tag = REMOVED;
}

bool
update_pqueue(PQ *q, int elem, puint32 newcost)
{
	remove_pqueue_elem(q, elem);
	q->l.cost[elem] = newcost;
	return insert_pqueue(&q, elem);
}

/*
 * -------------------------------------------------------------------------
 * Q U E U E   D A T A   S T R U C T U R E S
 *
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:47 $
 *
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
  

#include "queue.h"


/*-------------------------------------------------------------------------
 *   SIQueue - Static Integer Queue
 *
 *   @@@ This is the first file to use Gandalf's error and memory
 *   handling framework. Gradually all files will use this framework.
 *
 *    TODO
 *
 *       - in SIP, define an error handler using sciprint
 *       - convert all files under "src" to use animal_err_set_trace
 *
 *------------------------------------------------------------------------*/

AnimalExport SIQueue *
siq_new(int n) 
{
   SIQueue *q;

   ANIMAL_MALLOC_OBJECT(q, SIQueue, "siq_new",NULL);
   q->ini = q->end = -1;
   assert(n > 0);
   q->size = n;
   ANIMAL_MALLOC_ARRAY(q->data, int, n, "siq_new",NULL);
   q->data_alloc = true;

   return q; 
}

/*----FUNCTION-------------------------------------------------------------------------
 * 
 *  Description:  new queue from an already allocated array
 *  TODO: 
 *   - deal with both static and dynamic data
 * 
 * - PARAMETER -------------------------------------------------------------------------
 *      Mode   Type             Name            Description
 * -------------------------------------------------------------------------------------
 *        in:   int            n                number of elements in queue that are 
 *                                                 already initialized in array
 *        in:   int            n_total          total size of the array
 *-------------------------------------------------------------------------------------*/
AnimalExport SIQueue *
siq_new_from_static_array(int *array, int n_total, int n)
{
   SIQueue *q;

   assert(n_total > 0 && n <= n_total && array);
   ANIMAL_MALLOC_OBJECT(q, SIQueue, "siq_new",NULL);
   q->end  = n-1;
   q->ini  = -1;
   q->size = n_total;
   q->data = array;
   q->data_alloc = false;

   return q;
}

AnimalExport bool
siq_remove(SIQueue *q, int *val)
{
   if (siq_is_empty(q))
      ANIMAL_ERR_FIRST("siq_remove", ANIMAL_ERROR_FAILURE, "queue already empty", 
            false);

   *val = q->data[++q->ini];

   return true;
}

AnimalExport bool
siq_insert(SIQueue *q, const int val)
{
   assert(q);
   if (q->end == q->size) 
      ANIMAL_ERR_FIRST("siq_insert", ANIMAL_ERROR_FAILURE, "static-queue overflow",
            false);

   q->data[++q->end] = val;

   return true;
}

AnimalExport void
siq_free(SIQueue **sq)
{
   SIQueue *q=*sq;

   assert(q);
   if (q->data_alloc) 
      free(q->data); 
   free(q);
   *sq = NULL;
}


AnimalExport void
siq_print(SIQueue *q)
{
   int i;
   
   printf("SIQueue (len = %d, head = %d, tail = %d): ", q->size, q->ini, q->end);
   if (siq_is_empty(q)) {
      printf("EMPTY\n");
   } else {
      for (i=q->ini+1; i <= q->end; i++)
         printf("%d ", q->data[i]);
      putchar('\n'); 
   }
}

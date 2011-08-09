/*
 * -------------------------------------------------------------------------
 *
 * Q U E U E   D A T A   S T R U C T U R E S    --  HEADER
 *
 * $Revision: 1.2 $ $Date: 2005-07-23 14:01:31 $
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
  

#ifndef SIP_QUEUE_H
#define SIP_QUEUE_H 1


#include "common.h"
#include "util.h"

BEGIN_C_DECLS

/*
 *   SIQueue - A simple, fast, "Static Integer Queue", used by
 *   e.g. L-Z distance transform. 
 */

typedef struct SIQueue {
   int *data;
   int ini;
   int end;
   int size;
   bool data_alloc;
} SIQueue;

AnimalExport SIQueue
   *siq_new(int n),
   *siq_new_from_static_array(int *array, int n_total, int n);

AnimalExport bool
   siq_insert(SIQueue *q, int e),
   siq_remove(SIQueue *q, int *e);

AnimalExport void
   siq_free(SIQueue **q),
   siq_print(SIQueue *q);

#define siq_is_empty(Q) (Q->ini == Q->end)
#define siq_head(Q) (Q->data[Q->ini + 1])
#define siq_reset(Q) (Q->ini = Q->end = -1)


END_C_DECLS

#endif /* !SQUEUE_H */

/*
 * -------------------------------------------------------------------------
 * $Revision: 1.1 $ $Date: 2005-07-20 19:15:48 $
 *
 * ANIMAL - AN IMAging Library
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * LICENSE NOTE: THIS FILE CONTAINS CODE DERIVED FROM THE GANDALF LIBRARY
 *
 * Modifications started in 27 may 2004 from Gandalf v. 1.3.2 found at
 * http://gandalf-library.sf.net
 *
 * All original Gandalf code was (c) 2000 Imagineer Software Limited and 
 *                                        Industrial Research Limited
 *
 * The authors of the original, non-modified code were:
 * Phil McLauchlan, Allan Jaenicke, and Phillip Ngan
 *
 * Those authors are NOT responsible for the changes found herein.
 * The Animal authors assume their own animal incompetence for what
 * worsened, and thank the original authors for inspiring the rest.
 * -------------------------------------------------------------------------
 */ 





#include "allocate.h"
#include <stdarg.h>

/**
 * \addtogroup Common
 * \{
 */

/**
 * \defgroup CommonAllocate Common Allocation Routines
 * \{
 */

/**
 * \brief Frees a \c NULL-terminated variable argument list of memory blocks.
 * \param ptr The first memory block to free
 * \param ... List of other blocks to free, terminated by \c NULL
 * \return No value.
 *
 * Invokes \c free() to free each memory block in the list of pointers
 * starting with \a ptr and ending with \c NULL .
 *
 * \sa animal_malloc_object(), animal_malloc_array().
 */
AnimalExport void
 animal_free_va ( void *ptr, ... )
{
   va_list ap;

   va_start ( ap, ptr );
   while ( ptr != NULL )
   {
      /* free next matrix */
      free ( ptr );

      /* get next pointer in list */
      ptr = va_arg ( ap, void * );
   }

   va_end ( ap );
}

/**
 * \}
 */

/**
 * \}
 */

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
  

#include "common.h"

/* The following will be activated in case we use GLUT in the future */
/*#if ANIMAL_HAVE_BAD_GLUT*/
   /* Workaround for NVIDIA GLUT libs.  
    * We define some stubs because these functions are irrelevant */
/*
   void glXGetFBConfigAttribSGIX() {}
   void glXQueryChannelDeltasSGIX() {}
   void glXChannelRectSyncSGIX() {}
   void glXChannelRectSGIX() {}
   void glXQueryChannelRectSGIX() {}
   void glXGetFBConfigFromVisualSGIX() {}
   void glXBindChannelToWindowSGIX() {}
#endif
*/

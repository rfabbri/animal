## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Ricardo Fabbri
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


INCLUDES = -I$(top_builddir) -I$(top_srcdir)

# Headers which are installed
nobase_pkginclude_HEADERS = animal.h common.h hello.h adjacency.h analysis.h \
      array.h animal-config.h filtering.h \
      img_g.h img.h morphology.h plist.h pstk.h sedr.h segment.h set.h\
      util.h queue.h ift.h annimg.h distmaps.h\
		base/allocate.h \
		base/bit_array.h \
		base/err.h \
		base/err_trace.h \
		base/memory_stack.h \
		base/misc_defs.h \
		base/misc_error.h

# Headers which are not installed
noinst_HEADERS = ift_pqueue.h

lib_LTLIBRARIES  = libanimal.la

libanimal_la_SOURCES = adjacency.c analysis.c array.c filtering.c hello.c \
      ift_pqueue.c img.c morphology.c plist.c pstk.c sedr.c segment.c \
      set.c util.c queue.c ift.c annimg.c distmaps.c distmap-cuisenaire.c \
		distmap-cuisenaire-orig.c common.c distmap-eggers.c distmap-lz.c \
		base/allocate.c \
		base/bit_array.c \
		base/err.c \
		base/err_trace.c \
		base/memory_stack.c \
		base/misc_defs.c \
		base/misc_error.c

libanimal_la_LDFLAGS = -no-undefined \
	-version-info 0:0:0 \
   -release $(MAJOR_VERSION).$(MINOR_VERSION).$(MICRO_VERSION)

#libanimal_la_LIBADD = base/libanimal_base.la

AM_CPPFLAGS = @LIBRARY_EXTRA_CPPFLAGS@ 

EXTRA_DIST = img_g.c_incl

DISTCLEANFILES = animal-config.h


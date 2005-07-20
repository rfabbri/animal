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


INCLUDES = -I$(top_builddir) -I$(top_builddir)/animal \
           -I$(top_srcdir) -I$(top_srcdir)/animal

LDADD = $(top_builddir)/animal/libanimal.la

EXTRA_DIST = EXAMPLE.txt

noinst_PROGRAMS  = hello skl img circle plist find \
						 io misc sedr distance_transform watershed label_components\
						 imshow txtsee

hello_SOURCES  = hello.c
skl_SOURCES    = skl.c
img_SOURCES    = img.c
circle_SOURCES = circle.c
find_SOURCES   = find.c
io_SOURCES     = io.c
misc_SOURCES   = misc.c
plist_SOURCES  = plist.c
sedr_SOURCES   = sedr.c
distance_transform_SOURCES	= distance_transform.c
watershed_SOURCES = watershed.c
label_components_SOURCES = label_components.c
imshow_SOURCES = imshow.c
txtsee_SOURCES = txtsee.c

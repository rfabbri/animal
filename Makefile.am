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


# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.8.5 foreign
ACLOCAL_AMFLAGS = -I config

# Additional files to distribute
EXTRA_DIST = autogen.sh README.txt INSTALL.txt COPYING.txt TODO.txt \
   ChangeLog.txt images/bicycle.png images/circle30.png images/star.bmp\
	images/edges.png images/integral.png

SUBDIRS = animal config
DIST_SUBDIRS = animal config src

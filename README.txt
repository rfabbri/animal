AnImaL - AN IMAging Library

DESCRIPTION
===========

AnImaL is an imaging and computer vision library written in C.  Differently
from most other toolkits, it is intended to provide the maximum functionality
by making strong usage of similar libraries. It is intended to be a common
API to the best free imaging and computer vision libraries already available,
such as ImageMagick.  By defining a common API and using external
libraries, AnImaL selects the best implementation available in them to
implement a particular algorithm. This way AnImaL unites the best from the
other libraries it uses.

Of course, AnImaL has its own implementations for many routines. But whenever
possible, it uses a external routines. This code reuse makes development
much faster than monolithic libraries, which have to implement their own
solution for everything, ignoring excellent code already available out there.

AnImaL is fairly portable, like the other libraries it uses.

INSTALLATION
============
Please read the INSTALL.txt file


EXAMPLES
========

See the EXAMPLES.txt in the src directory for instructions
on how to compile programs that use AnImaL.


HISTORICAL NOTES
================

Animal was created for SIP, the Scilab Image Processing toolbox
(http://siptoolbox.sourceforge.net).  It has become independent of
SIP and Scilab, but it has influences from its origins. For example,
FFT and some other numerical operations are not yet implemented
in Animal, as its primary author uses Scilab for this. It is our
purpose to use/merge a library like Gandalf for some linear algebra
and computer vision algorithms (http://gandalf-library.sf.net). We
started doing this in Animal 0.6.0. We have been improving Gandalf
by merging enhanced versions of it into Animal instead of improving
Gandalf directly.

Copyright (c) 2003-2011  Ricardo Fabbri <rfabbri@users.sourceforge.net>

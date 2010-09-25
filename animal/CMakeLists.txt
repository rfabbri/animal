CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

INCLUDE_DIRECTORIES (${ImageMagick_INCLUDE_DIRS})

# collect up the source files
SET (SRC_FILES 
    adjacency.c 
    analysis.c 
    annimg.c 
    array.c
    common.c 
	distmap-cuisenaire.c 
    distmap-eggers.c 
    distmap-lz.c 
    distmaps.c
	filtering.c 
    hello.c 
    ift.c 
    ift_pqueue.c 
    img.c
    #img_g.c
    morphology.c
	plist.c
    pstk.c
    queue.c 
    sedr.c
    segment.c 
    set.c 
    util.c
	base/allocate.c 
    base/bit_array.c 
    base/err.c 
    base/err_trace.c
	base/memory_stack.c 
    base/misc_defs.c
    base/misc_error.c
    )

ADD_LIBRARY(libanimal SHARED ${SRC_FILES})

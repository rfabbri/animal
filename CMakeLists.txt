CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(ANIMAL)
SUBDIRS(animal)


FIND_PACKAGE(ImageMagick REQUIRED)
if(ImageMagick_FOUND)
    set(ImageMagick_INCLUDE_DIRS /usr/include/ImageMagick CACHE PATH "")
else(ImageMagick_FOUND)
    message(FATAL_ERROR
    "Cannot build Animal without ImageMagick. Please set ImageMagick_DIR.")
endif(ImageMagick_FOUND)

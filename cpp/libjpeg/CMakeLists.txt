cmake_minimum_required(VERSION 2.8.4)
project(jpeg)

# Set C flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Set sources
set(SOURCE_FILES
    jpeg-9a/jaricom.c
    jpeg-9a/jcapimin.c
    jpeg-9a/jcapistd.c
    jpeg-9a/jcarith.c
    jpeg-9a/jccoefct.c
    jpeg-9a/jccolor.c
    jpeg-9a/jcdctmgr.c
    jpeg-9a/jchuff.c
    jpeg-9a/jcinit.c
    jpeg-9a/jcmainct.c
    jpeg-9a/jcmarker.c
    jpeg-9a/jcmaster.c
    jpeg-9a/jcomapi.c
    jpeg-9a/jconfig.h
    jpeg-9a/jcparam.c
    jpeg-9a/jcprepct.c
    jpeg-9a/jcsample.c
    jpeg-9a/jctrans.c
    jpeg-9a/jdapimin.c
    jpeg-9a/jdapistd.c
    jpeg-9a/jdarith.c
    jpeg-9a/jdatadst.c
    jpeg-9a/jdatasrc.c
    jpeg-9a/jdcoefct.c
    jpeg-9a/jdcolor.c
    jpeg-9a/jdct.h
    jpeg-9a/jddctmgr.c
    jpeg-9a/jdhuff.c
    jpeg-9a/jdinput.c
    jpeg-9a/jdmainct.c
    jpeg-9a/jdmarker.c
    jpeg-9a/jdmaster.c
    jpeg-9a/jdmerge.c
    jpeg-9a/jdpostct.c
    jpeg-9a/jdsample.c
    jpeg-9a/jdtrans.c
    jpeg-9a/jerror.c
    jpeg-9a/jerror.h
    jpeg-9a/jfdctflt.c
    jpeg-9a/jfdctfst.c
    jpeg-9a/jfdctint.c
    jpeg-9a/jidctflt.c
    jpeg-9a/jidctfst.c
    jpeg-9a/jidctint.c
    jpeg-9a/jinclude.h
    jpeg-9a/jmemmgr.c
    jpeg-9a/jmemnobs.c
    jpeg-9a/jmemsys.h
    jpeg-9a/jmorecfg.h
    jpeg-9a/jpegint.h
    jpeg-9a/jpeglib.h
    jpeg-9a/jquant1.c
    jpeg-9a/jquant2.c
    jpeg-9a/jutils.c
    jpeg-9a/jversion.h)

# Create library from sources
add_library(jpeg ${SOURCE_FILES})

# Make sure the compiler can find include files for LibJpeg library
# when other libraries or executables link to LibJpeg
target_include_directories (jpeg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
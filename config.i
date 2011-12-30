########################################################################
#                                                                      #
# GLE - Graphics Layout Engine <http://www.gle-graphics.org/>          #
#                                                                      #
# Modified BSD License                                                 #
#                                                                      #
# Copyright (C) 2009 GLE.                                              #
#                                                                      #
# Redistribution and use in source and binary forms, with or without   #
# modification, are permitted provided that the following conditions   #
# are met:                                                             #
#                                                                      #
#    1. Redistributions of source code must retain the above copyright #
# notice, this list of conditions and the following disclaimer.        #
#                                                                      #
#    2. Redistributions in binary form must reproduce the above        #
# copyright notice, this list of conditions and the following          #
# disclaimer in the documentation and/or other materials provided with #
# the distribution.                                                    #
#                                                                      #
#    3. The name of the author may not be used to endorse or promote   #
# products derived from this software without specific prior written   #
# permission.                                                          #
#                                                                      #
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR   #
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED       #
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   #
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY       #
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   #
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE    #
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS        #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER #
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      #
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN  #
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        #
#                                                                      #
########################################################################
#
# global makefile include config.i for gle
# this sets options common to all compilers
#
#  this sets the global version number for all programs
#
MODULE_NAME		=gle40
BASE_NAME		=GLE
MAJOR_VERSION_NUMBER 	=4
MINOR_VERSION_NUMBER 	=2
BUILD_NUMBER    	=4
VERSION_NUMBER  	=$(MAJOR_VERSION_NUMBER).$(MINOR_VERSION_NUMBER)
VERSION_INFO		=-DGLEVN="\"$(VERSION_NUMBER).$(BUILD_NUMBER)\""
TAG 			=$(MODULE_NAME)_R_$(MAJOR_VERSION_NUMBER)_$(MINOR_VERSION_NUMBER)_$(BUILD_NUMBER)
GLE_SRC_DIRS            =src/gle src/makefmt src/fbuild src/manip src/gui

#
# -- optional packages (ignored if one uses ./configure)
#

HAVE_LIBTIFF = 0
# libtiff at www.libtiff.org is needed for inclusion of
# tiff files.  if you dont have it comment out the line above
# if you have it you must set the environment variable
# LIBTIFFDIR to point to the location of libtiff

HAVE_LIBPNG = 0
# libpng at www.libpng.org is needed for inclusion of
# PNG files.  if you dont have it comment out the line above
# you will also need zlib
# if you have it you must set the environment variable
# LIBPNGDIR to point to the location of libpng
# ZLIBDIR to point to the location of zlib

# For JPEG and GIF images, no extra libs are required!

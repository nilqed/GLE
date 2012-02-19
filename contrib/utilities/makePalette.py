#!/usr/bin/python

########################################################################
#                                                                      #
# GLE - Graphics Layout Engine <http://www.gle-graphics.org/>          #
#                                                                      #
# Modified BSD License                                                 #
#                                                                      #
# Copyright (C) 2012 Brandon Aubie                                     #
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
# GOODS OR SERVICES# LOSS OF USE, DATA, OR PROFITS# OR BUSINESS        #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER #
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      #
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN  #
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        #
#                                                                      #
########################################################################
# 
# Type ./makePalette.py -h for help, but in general, you just past a list 
# of colours in the form "r,g,b" and it does the rest to make a linear
# combination.  If you put -n myName it will call the subroutine myName
# and if you put -o filename.gle then it will write the results to
# filename.gle instead of printing them to the screen.
# 
# Example:
# ./makePalette.py -n rainbow -o rainbow.gle "0,0,1" "0,1,1" "0,1,0" "1,1,0" "1,0,0" "1,0,0"

import argparse

parser = argparse.ArgumentParser(description='Create GLE Pallete Functions')
parser.add_argument('colors', nargs='*', help='A list of at least 2 strings in the format "r,g,b" where r,g,b are between 0 and 1')
parser.add_argument('-n', action='store', dest='name', default='mypalette', help='Name of your palette subroutine')
parser.add_argument('-o', action='store', dest='filename', default='',help='Filename to output the subroutine too. Leave blank to print to screen')

inputs = parser.parse_args()

if len(inputs.colors) < 2:
	print "Error: Must supply at least 2 colors"
	quit()

if inputs.name == "": inputs.name = "mypalette"

# Parse color strings
colors = [[float(c.split(",")[0]), float(c.split(",")[1]),
float(c.split(",")[2])] for c in inputs.colors if len(c.split(",")) == 3]


r = "sub " + inputs.name + " z\n"
r = r + "  local r = 0\n"
r = r + "  local g = 0\n"
r = r + "  local b = 0\n"

dz = 1.0 / (len(colors) - 1)

r = r + "  if (z <= " + str(dz) + ") then\n"
r = r + "    r = " + str(colors[0][0]) +"*("+str(dz)+"-z)/"+str(dz)+"+"+str(colors[1][0]) +"*(z/"+str(dz)+")\n"
r = r + "    g = " + str(colors[0][1]) +"*("+str(dz)+"-z)/"+str(dz)+"+"+str(colors[1][1]) +"*(z/"+str(dz)+")\n"
r = r + "    b = " + str(colors[0][2]) +"*("+str(dz)+"-z)/"+str(dz)+"+"+str(colors[1][2]) +"*(z/"+str(dz)+")\n"

for color in range(2, len(colors)):
	z = color*dz
	r = r + "  else if (z <= " + str(z)+") then \n"
	r = r + "    r = " + str(colors[color-1][0]) +"*("+str(z)+"-z)/"+str(dz)+"+"+str(colors[color][0]) +"*((z-"+str(z-dz)+")/"+str(dz)+")\n"
	r = r + "    g = " + str(colors[color-1][1]) +"*("+str(z)+"-z)/"+str(dz)+"+"+str(colors[color][1]) +"*((z-"+str(z-dz)+")/"+str(dz)+")\n"
	r = r + "    b = " + str(colors[color-1][2]) +"*("+str(z)+"-z)/"+str(dz)+"+"+str(colors[color][2]) +"*((z-"+str(z-dz)+")/"+str(dz)+")\n"

r = r + "  end if\n"
r = r + "  return rgb(r,g,b)\n"
r = r + "end sub\n"

if inputs.filename == "":
	print r
else:
	f = open(inputs.filename, 'w')
	f.write(r)

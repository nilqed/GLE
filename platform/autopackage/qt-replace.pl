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

# Replace entries in makefile

# ./configure -prefix /localhost/tmp/jans/qt4.3.2 -nomake examples,tools -release -static -no-qt3support -no-openssl -no-cups -no-opengl -no-xinerama -no-xfixes -no-xrandr -no-tablet -no-glib -no-mmx -no-3dnow -no-sse -no-sse2

$file = $ARGV[0];

open(IN, $file) || die "Can't open $file";
@LINES = <IN>;
close(IN);

open(OUT, ">$file") || die "Can't create $file";
foreach $line (@LINES) {
	chomp($line);
	if ($line =~ /^CXX\s+\=/) {
		$line = "CXX = apg++";
	}
	if ($line =~ /^LINK\s+\=/) {
		$line = "LINK = apg++";
	}
	if ($line =~ /^LFLAGS\s+\=\s+(.+)$/) {
		$flags = $1;
		$line = "LFLAGS = $flags -static-libgcc -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic";
	}
	print OUT "$line\n";
}
close(OUT);

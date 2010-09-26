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

$make = $ARGV[0];

open(IN, "configure.ac") || die "Can't open configure.ac";
@LINES = <IN>;
close(IN);

$found = 0;
open(OUT, ">configure.ac") || die "Can't write to configure.ac";
for ($i = 0; $i <= $#LINES; $i++) {
	$line = $LINES[$i];
	chomp($line);
	if ($found == 0) {
		if ($line =~ /^GLE_SNAPSHOT\=/) {
			$found = 1;			
			if ($make == 1) {
				$date = `date +\%m\%d\%y`;
				$date =~ s/[\n\r]+$//;
				if ($ARGV[1] eq "") {
					$snap = "S$date";
				} else {
					$snap = $ARGV[1];
				}
				print OUT "GLE_SNAPSHOT=\"-$snap\"\n";
				print OUT "AC_DEFINE_UNQUOTED(GLE_SNAPSHOT, \"\$GLE_SNAPSHOT\")\n";
				print "Making snapshot: $snap\n";
			} else {
				print OUT "GLE_SNAPSHOT=\"\"\n";
				print "Clearing snapshot\n";
			}
			if ($LINES[$i+1] =~ /AC_DEFINE_UNQUOTED/) {
				$i++;
			}			
		} else {
			print OUT "$line\n";
		}
	} else {
		print OUT "$line\n";
	}
}
close(OUT);

system("autoconf");

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

#!/usr/bin/perl
use strict;
use warnings;
#my $gle = "gle";
my $gle = "../../build/bin/gle";
#if ($ENV{"RUNBUILD"} == 1) {

#} 

my $clean = 0;
if(@ARGV >= 1){
	if(uc($ARGV[0]) eq "CLEAN"){
		$clean = 1;
	}
}

open(IN, "fonts.csv");
while (my $line = <IN>) {
	chop($line);
	if ($line =~ /^([0-9]+)\,([^\,]+)\,/) {
		my $id = $1; 
		my $name = $2;
		if($clean == 0){
			if (!(-f "$id.pdf")) {
				open(OUT, ">$name.gle");
				print OUT "size 7.75 7\n";
				print OUT "include \"font-table.gle\"\n";
				print OUT "\@show_table \"$name\" \"$id\"\n";
				close(OUT);
				system("$gle -d pdf -o $name.pdf font-table.gle $name");
			}
		}else{
			system("erase /QE $name.gle $name.eps $name.pdf");
		}

	}
}
close(IN);

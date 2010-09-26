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

# cat srcfiles.txt | xargs perl platform/script/checklicense.pl

# identify -verbose *.png | grep Comment
# for file in *.png ; do mogrify -comment "License: GPL v.2 or later" "$file" ; done

open(IN, "platform/script/licenses.csv") || die "Can't open 'platform/script/licenses.csv'";
while ($line = <IN>) {
	chomp($line);
	if (!($line =~ /^\s*\!.*$/) && !($line =~ /^\s*$/)) {
		if ($line =~ /^([^\,]+)\,\s*(.+)$/) {
			$file = $1;
			$lic = $2;
			if ($lic =~ /Modified BSD/) {
				$MOD_BSD_LIC{$file} = 1;
				$HAS_LIC{$file} = 1;
			}
			if ($lic =~ /GPL v2 or later/) {
				$GPL_LIC2L{$file} = 1;
				$HAS_LIC{$file} = 1;
			}
		}
	}
}

for ($i = 0; $i <= $#ARGV; $i++) {
	$file = $ARGV[$i];

	$modified_bsd = 0;
	$gpl = 0;
	$gpl2 = 0;
	$gpl21 = 0;
	$gpl_later = 0;
	$zlib = 0;

	if ($file =~ /\.png$/) {
		$comment = `identify -verbose $file`;
		if ($comment =~ /GPL v\.2 or later/) {
			$gpl = 1;
			$gpl2 = 1;
			$gpl_later = 1;
		}
		if ($comment =~ /Modified BSD/) {
			$modified_bsd = 1;
		}
	} else {
		@ALL = ();
		open(IN, $file) || die "Can't open '$file'";
		while ($line = <IN>) {
			if ($line =~ /^[\s\#\%\!\*\;]+ZLIB\/LIBPNG LICENSE/) {
				$zlib = 1;
			}
			if ($line =~ /^[\s\#\%\!\*\;\.\\\"]+Modified BSD License/) {
				$modified_bsd = 1;
			}
			if ($line =~ /^rem[\s\#\%\!\*]+Modified BSD License/) {
				$modified_bsd = 1;
			}
			if ($line =~ /General Public License/) {
				$gpl = 1;
			}
			$line =~ s/[\n\r]+//g;
			push @ALL, $line;
		}	
		close(IN);

		if ($gpl == 1) {
			$lines = join(" ", @ALL);
			$lines =~ s/[\*\#\!\%]/ /g;
			$lines =~ s/\;\;/ /g;
			$lines =~ s/\s\s+/ /g;
		
			# print "$lines\n";
	
			if ($lines =~ /version 2 of the License, or \(at your option\) any later version/) {
				$gpl2 = 1;
				$gpl_later = 1;
			}
			if ($lines =~ /either version 2, or \(at your option\) any later version/) {
				$gpl2 = 1;
				$gpl_later = 1;
			}
			if ($lines =~ /version 2\.1 of the License, or \(at your option\) any later version/) {
				$gpl21 = 1;
				$gpl_later = 1;
			}
		}
	}

	if ($HAS_LIC{$file} != 1) {
		if ($gpl == 1) {
			if (($gpl21 == 1) && ($gpl_later == 1)) {
				$GPL_LIC21L{$file} = 1;
			} elsif (($gpl2 == 1) && ($gpl_later == 1)) {
				$GPL_LIC2L{$file} = 1;
			} elsif ($gpl2 == 1) {
				$GPL_LIC2{$file} = 1;
			} else {
				$GPL_LIC{$file} = 1;
			}
		} elsif ($modified_bsd == 1) {
			$MOD_BSD_LIC{$file} = 1;
		} elsif ($zlib == 1) {
			$ZLIB_LIC{$file} = 1;
		} else {
			$UNKNOWN{$file} = 1;
		}
	}
}

print "*** Modified BSD License ***\n\n";
foreach $f (sort keys(%MOD_BSD_LIC)) {
	print "$f\n";
}

print "\n*** LGPL v2.1 or later ***\n\n";
foreach $f (sort keys %GPL_LIC21L) {
	print "$f\n";
}

print "\n*** GPL v2 or later ***\n\n";
foreach $f (sort keys %GPL_LIC2L) {
	print "$f\n";
}

print "\n*** GPL v2 ***\n\n";
foreach $f (sort keys %GPL_LIC2) {
	print "$f\n";
}

print "\n*** GPL (unknown) ***\n\n";
foreach $f (sort keys %GPL_LIC) {
	print "$f\n";
}

print "\n*** ZLIB/LIBPNG License ***\n\n";
foreach $f (sort keys %ZLIB_LIC) {
	print "$f\n";
}

print "\n*** Unknown ***\n\n";
foreach $f (sort keys %UNKNOWN) {
	print "$f\n";
}

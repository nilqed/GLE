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

$distall = 0;

sub try_sel {
	my ($path, $file) = @_;
	my ($ok, $nok, $pos);
	if ($file eq ".notdist") {
		return;
	}
	$ok = 0;
	foreach $pos (@SEL) {
		if (eval($pos)) {
			$ok = 1;
		}
	}
	$nok = 0;
	foreach $pos (@IGN) {
		if (eval($pos)) {
			$nok = 1;
		}
	}
	$path =~ s/^\.\///;
	if (($ok == 1) || ($nok == 1)) {
		if ($nok != 1) {
			print "$path\n";
		}
	} else {
		print STDERR "What about: '$path'\n";
	}
}

sub recursive_search {
	my ($dir) = @_;
	print STDERR "Recursive Directory: $dir\n";
	my (@FILES, $handle, $file, $path, $ntline, %NOTDIST);
	opendir($handle, $dir);
	@FILES = readdir($handle);
	closedir($handle);
	if (-f "$dir/.notdist") {
		open(NOTDIST, "$dir/.notdist");
		while ($ntline = <NOTDIST>) {
			chomp($ntline);
			if ($ntline =~ /^distall\: (.+)$/) {
				$ntline = $1;
				if ($distall == 0) {
					$NOTDIST{$ntline} = 1;
				}
			} else {
				$NOTDIST{$ntline} = 1;
			}
		}
		close(NOTDIST);
	}
	foreach $file (@FILES) {
		if (!($file =~ /^\.svn$/) && !($file =~ /^CVS$/) && !($file =~ /^\.cvsignore$/) && !($file =~ /^\.+$/) && !($file =~ /^\.gle$/)) {
			$path = $dir . "/" . $file;
			if (-d $path) {
				recursive_search($path);
			} elsif (!defined($NOTDIST{$file})) {
				try_sel($path, $file);
			} else {
				print STDERR "Can't distribute: $path\n";
			}
		}
	}
}

sub dir_search {
	my ($dir) = @_;
	print STDERR "Directory: $dir\n";
	my (@FILES, $handle, $file, $path);
	opendir($handle, $dir);
	@FILES = readdir($handle);
	closedir($handle);
	foreach $file (@FILES) {
		if (!($file =~ /^\.svn$/) && !($file =~ /^CVS$/) && !($file =~ /^\.cvsignore$/) && !($file =~ /^\.+$/)) {
			$path = $dir . "/" . $file;
			if (!(-d $path)) {
				try_sel($path, $file);
			}
		}
	}
}


sub update_regex {
	my ($re) = @_;
	$re =~ s/\./\\./g;
	$re =~ s/\*/\.*/g;
	return "\$file =~ /^$re\$/";
}

if ($ARGV[0] eq "-all") {
	$distall = 1;
	$file = $ARGV[1];
} else {
	$file = $ARGV[0];
}
open(IN, $file) || die "Can't open '$file'";
while ($line = <IN>) {
	chomp($line);
	if (!($line =~ /^\s*$/) && !($line =~ /^\s*!/)) {
		($dir, $select, $ignore) = split(/\s*,\s*/, $line);
		$select =~ s/\s+$//;
		$ignore =~ s/\s+$//;
		@SEL = split(/\s*;\s*/, $select);
		for ($i = 0; $i <= $#SEL; $i++) {
			$SEL[$i] = update_regex($SEL[$i]);
		}
		@IGN = split(/\s*;\s*/, $ignore);
		for ($i = 0; $i <= $#IGN; $i++) {
			$IGN[$i] = update_regex($IGN[$i]);
		}
		if ($dir =~ /^\$(.+)$/) {
			$dir = $1;
			dir_search($dir);
		} else {
			recursive_search($dir);
		}
	}
}
close(IN);

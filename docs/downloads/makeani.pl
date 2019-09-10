#!/usr/bin/perl

$makepdf = 0;
$addslideno = 0;
$fname = "";

# Parse command line arguments
for ($i = 0; $i <= $#ARGV; $i++) {
	$arg = $ARGV[$i];
	if ($arg =~ /^\-/) {
		if ($arg eq "-pdf") {
			$makepdf = 1;
		} elsif ($arg eq "-addslideno") {
			$addslideno = 1;
		}
	} else {
		$fname = $arg;
	}
}

# Count number of slides
$count = 1;
open(FILE, "$fname.gle") || die "Bug";
while ($line=<FILE>) {
	chop($line);
	if ($line =~ /^!\s*ONLYSLIDE\s[0-9]+\-([0-9]+)/) {
		$nb = $1;
		if ($count < $nb) { $count = $nb; }
	} elsif ($line =~ /^!\s*\S\S\S\SSLIDE\s([0-9]+)\s*$/) {
		$nb = $1;
		if ($count < $nb) { $count = $nb; }
	}
}
close(FILE);
print "Nb slides: $count\n";

# Construct each slide in turn
for ($slide = 1; $slide <= $count; $slide++) {
	$enable = 1;
	open(FILE, "$fname.gle") || die "Bug";
	open(OUT, ">$fname-$slide.gle") || die "Bug";
	while ($line=<FILE>) {
		chop($line);
		if ($line =~ /^!\s*ONLYSLIDE\s([0-9]+)\-([0-9]+)/) {
			$nb1 = $1;
			$nb2 = $2;
			if (($slide >= $nb1) && ($slide <= $nb2)) {
				$enable = 1;
			} else {
				$enable = 0;
			}
		} elsif ($line =~ /^!\s*ALLSLIDES\s*$/) {
			$enable = 1;
		} elsif ($line =~ /^!\s*\S\S\S\SSLIDE\s(\S+)\s*$/) {
			$nb = $1;
			if ($line =~ /FROM/) {
				if ($slide >= $nb) {
					$enable = 1;
				} else {
					$enable = 0;
				}
			} else {
				if ($nb == $slide) {
					$enable = 1;
				} else {
					$enable = 0;
				}
			}
		} else {
			if ($enable == 1) {
				print OUT "$line\n";
			}
		}
	}
	if ($addslideno == 1) {
		print OUT "set hei 0.3\n";
		print OUT "set just bl\n";
		print OUT "amove 0.01 0.01\n";
		print OUT "write \"$slide\"\n";
	}
	close(OUT);
	close(FILE);
	print "gle_ps /eps $fname-$slide.gle\n";
	system("gle_ps /eps $fname-$slide.gle");
	if ($makepdf == 1) {
		system("epstopdf $fname-$slide.eps");
	}
}

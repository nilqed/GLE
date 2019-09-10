#!/usr/bin/perl
#
# makes a gle web page for an individual gle file
#

my($file, $doimg, $toroot) = @ARGV;

my $base = $file;
$base =~ s/\.gle//i;
my $icon = $base."_icon.png";
my $big  = $base.".png";
my $eps  = $base.".eps.gz";
my $pdf  = $base.".pdf";
my $page = $base.".html";

print "perl ${toroot}script/code2html/code2html -l gle $file > temp.html\n";
system("perl ${toroot}script/code2html/code2html -l gle $file > temp.html");

#$text = load_file_into_string($file);
open(fhgle,">$page.tmp");
if ($doimg == 1) {
	print fhgle "<h2>GLE Example: <a href=\"$file\">$file</a></h2>\n";
} else {
	print fhgle "<h2>GLE Library: <a href=\"$file\">$file</a></h2>\n";
}
if ($doimg == 1) {
	print fhgle "<img src=\"".$big."\" border=\"0\"><p>[<a href=".$pdf.">PDF file</a>]</p>\n";
        print fhgle "<p>&nbsp;</p><hr>\n";
}
print fhgle "<pre>\n";
open(IN, "temp.html") || die "Can't open temp.html";
$enable = 0;
$empty = 0;
$first = 1;
while ($line = <IN>) {
	$line =~ s/[\r\n]//g;
	if ($line =~ /\<pre\>/) {
		$enable = 1;
	} elsif ($line =~ /\<\/pre\>/) {
		$enable = 0;
	} else {
		if ($enable == 1) {
			$printed = 0;
			if ($line =~ /\&quot\;([^\.]+)\.gle\&quot\;/) {
				$incf = $1;
				if (-f "../../subroutines-preview/$incf.gle") {
					$line =~ s/\&quot\;([^\.]+)\.gle\&quot\;/<a href=\"..\/..\/subroutines-preview\/$1.html\"><font color="#008000">\&quot\;$1.gle\&quot\;<\/font><\/a>/g;				
				} else {
					$line =~ s/\&quot\;([^\.]+)\.gle\&quot\;/<a href=\"..\/..\/subroutines\/$1.html\"><font color="#008000">\&quot\;$1.gle\&quot\;<\/font><\/a>/g;				
				}
			}
			$line =~ s/^([^!]+)\&quot\;([^\.]+)\.dat\&quot\;/$1<a href=\"$2.dat\"><font color="#008000">\&quot\;$2.dat\&quot\;<\/font><\/a>/g;
			$line =~ s/^([^!]+)\&quot\;([^\.]+)\.csv\&quot\;/$1<a href=\"$2.csv\"><font color="#008000">\&quot\;$2.csv\&quot\;<\/font><\/a>/g;			
			$line =~ s/\&quot\;([^\.]+)\.z\&quot\;/<a href=\"$1.z\"><font color="#008000">\&quot\;$1.z\&quot\;<\/font><\/a>/g;			
			$line =~ s/\*\*\s([^\s]+)\s\*\*/<a href=\"$1\" target=_blank>$1<\/a>/g;
#			if (($line =~ /begin.+tex/) || ($line =~ /begin.+table/)) {
#				$textblock = 1;
#			} elsif ($textblock == 1) {
#				if ($line =~ /^\s*\<strong\>end\<\/strong\>/) {
#					$textblock = 0;
#				}
#				if ($textblock == 1) {				
#					$line =~ s/\<[^\>]+\>//g;
#					print fhgle "<font color=\"#008000\">$line</font>\n";
#					$printed = 1;
#				}
#			}
			if ($printed == 0) {
				if ($line =~ /^\s*$/) {
					$empty++;
				} else {			
					# Skip empty lines at start and end of file
					if ($first == 0) {
						for ($i = 0; $i < $empty; $i++) {
							print fhgle "\n";
						}
					}
					$empty = 0; $first = 0;
					print fhgle "$line\n";
				}
			}
		}
	}
}
close(IN);
print fhgle "</pre>\n";
if ($doimg == 1) {
	print fhgle "<hr><p>&nbsp;</p>\n";
	print fhgle "[<a href=\"index.html\">Return to examples page</a>]\n";
} else {
	print fhgle "<p>&nbsp;</p>\n";
	print fhgle "[<a href=\"index.html\">Return to subroutines page</a>]\n";
}
close(fhgle);
system("perl ${toroot}script/combine.pl $toroot -english ${toroot}src/template.html ${toroot}src/menu.html $page.tmp $page");
system("rm $page.tmp");

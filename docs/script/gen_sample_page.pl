#!/usr/bin/perl
#
# generates gle sample page based on gle files in .\samples subdir
#
#useage gen_samples_page.pl
# is will search for all .gle files in .\samples, make icons and text for the main page and 
# big gifs and full source for the sub pages
#

#use LWP::Simple;
#use Text::DelimMatch;
#use Text::ParseWords;
use File::Basename;
use File::stat;
use Time::localtime;

#
# icon size
#
$idx = 175;
$idy = 175;
#
# big size
#
$dx  = 450;
$dy  = 450;
#
# -- number of columns
#
$columns=3;
#
$main_page = "index.tmp.html";
$toroot = $ARGV[1];

open(fh,">".$main_page);
print fh "<h2>GLE Example Repository</h2>\n";
print fh "<p>Click on each figure to see a larger picture and the GLE source code.</p>";
print fh "<p>[<a href=\"../index.html\">Examples</a>] / [$ARGV[0]]</p>\n";
print fh "<table>";

sub get_gle_descript
{
	my ($fname) = @_;
	open(fh1, "<$fname");
	my $get = 1;
	my $ret = "";
	@gg = <fh1>;
	close(fh1);
	foreach $line (@gg){
		# remove spaces and tabs
		chomp($line);
		$line =~ s/^\s+//;
		$line =~ s/^\t+//;
		$line =~ s/\s+$//;
		if($get == 1)
		{
			#
			# read each line into the file unless it doesn't start with !
			#
			if(index($line,"!")==0)
			{
				$ret = $ret.$line."</br>\n";
			}
			else
			{
				$get = 0;
			}
		}
	}
	return $ret;
}

sub gle_sample
{
	#
	# makes table entry and sample page from .gle file
	#
	my ($file) = @_;
	my $ret = "";
	my $base = $file;
	$base =~ s/\.gle//i;
	my $icon = $base."_icon.png";
	my $big  = $base.".png";
	my $eps  = $base.".eps.gz";
	my $pdf  = $base.".pdf";
	my $page = $base.".html";
	my @files = ($icon,$big,$page,$file,$eps);
	#
	# form table entry
	#
	$ret = "<td><div class=\"excell\"><a href=".$page."><img align=top src=\"".$icon."\" border=\"0\"></a></div>[<a href=".$page.">GLE File</a>] [<a href=".$pdf.">PDF file</a></a>]</td>";
	return $ret, @files;
}

opendir(dirh, ".");
@all_files = readdir dirh;
closedir(dirh);

foreach $file (@all_files) {
	if ($file =~ /.+\.gle$/) {
		push @gle_files, $file;
	}
}

@entries = ();

if (-f "order.txt") {
	open(IN, "order.txt") || die "Can't open order.txt";
	while ($line = <IN>) {
		chomp($line);
		$line =~ s/\s+$//;
		$file = $line;
		if ($HAS_FILE{$file} == 1) {
			print ">> Duplicate file (in order.txt):\n";
			print ">> '$file'\n";		
		}
		push @entries, $file;		
		$HAS_FILE{$file} = 1;
	}
	close(IN);
	foreach $file (@gle_files) {
		if ($HAS_FILE{$file} != 1) {
			print ">> Missing file (in order.txt):\n";
			print ">> '$file'\n";
		}
	}
} else {
	@entries = sort @gle_files;
}

$i=0;
foreach $entry (@entries) {
	if($i%$columns == 0 ){ print fh "<tr align=\"center\" valign=\"bottom\">";}
	($entry,@upload) = gle_sample($entry);
	print fh $entry."\n";
	if($i == 2*$columns-1){print fh "</tr>";}
	$i++;
}
print fh "</table>\n";
close(fh);

system("perl $toroot/script/combine.pl $toroot -english $toroot/src/template.html $toroot/src/menu.html index.tmp.html index.html");
system("rm index.tmp.html");

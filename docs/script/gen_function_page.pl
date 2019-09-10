#!/usr/bin/perl
#
# generates gle dowload page based on input from text file
#
#useage gen_dl_page.pl input.txt page.html
#
# format for input.txt
# filename.zip
# description
# filename.zip
# description
#.....etc

# use LWP::Simple;
# use Text::DelimMatch;
# use Text::ParseWords;
use File::Basename;
use File::stat;
use Time::localtime;

$main_page = "index.html";

sub get_gle_descript
{
	my ($fname) = @_;
	open(fh1, "<$fname");
	my $get = 1;
	my $ret = "";
	@gg = <fh1>;
	close(fh1);
	foreach $line (@gg) 
	{
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
sub doc_gle_file
{
	#
	# documents a gle file returns HTML encoded string
	#
	my ($gle_filename) = @_;

	my $html_filename = $gle_filename;
	$html_filename =~ s/\.gle/_doc\.html/;
	my $html_gle_src = $gle_filename;
	$html_gle_src =~ s/\.gle/\.html/;
	my $doc_filename = $gle_filename;
	$doc_filename =~ s/\.gle/\.txt/;

	my @files = ($gle_filename,$html_filename);
	
	my $ret = "<h2>GLE Library: $gle_filename</h2>\n<p><a href=".$gle_filename.">Download \"".$gle_filename."\"</a></p>";
	
	if (-e($doc_filename)){
		$ret .= "<p><a href=".$doc_filename.">Documentation \"".$doc_filename."\"</a></p>";
	}
	$ret .= "<p>List of subroutines:<code><ul>\n";

	open(fhgle,$gle_filename);
	my @text = <fhgle>;
	my $line = "";
	$readdoc=0;
	$doclink = $html_filename;
	foreach $line (@text)
	{
		chomp($line);
		$line =~ s/^\s+//;
		$line =~ s/^\t+//;
		$line =~ s/\s+$//;
		if($readdoc == 1)
		{
			#
			# read each line into the file unless it doesn't start with !
			#
			if(index($line,"!")==0)
			{
				$ret .= $line."<br>\n";
			}
			else
			{
				$ret .= "<br>\n";
				$readdoc = 0;
			}
		}
		if(index($line,"sub")==0)
		{
			$line =~ s/sub //;
			$line = "<b><big>".$line."<\/b><\/big>";
			
			$ret .= "<li>".$line."</li><br>\n";
			$readdoc = 1;
		}
		if ($line =~ /Documentation\s\*\*\s([^\s]+)\s\*\*/) {
			$doclink = $1;
		}
	}

	close(fhgle);
	$ret .= "</ul></code>\n";
	$ret .= "<p>[<a href=".$main_page.">Return to subroutines page</a>]</p>\n";
	#
	# form table entry
	#
	open(fhp,">${html_filename}.tmp");
	print fhp $ret;
	close(fhp);
	system("perl ../script/combine.pl $ARGV[0] -english ../src/template.html ../src/menu.html ${html_filename}.tmp ${html_filename}");
    system("rm ${html_filename}.tmp");
	
	$ret = "<tr>"."<td>"."<a href=\"".$html_gle_src."\">".$gle_filename."</a>"."<br>"."<a href=\"".$doclink."\">documentation</a>"."</td>"."<td>".get_gle_descript($gle_filename)."</td>"."</tr>\n";
	return $ret, @files;

}

open(fh,">$main_page.tmp");
print fh "<h2>GLE Subroutines</h2>\n";

opendir(dirh, ".");
@entries = sort readdir dirh;
closedir(dirh);

$Blank ="</tr><td colspan=\"\2\">&nbsp;</td></tr>\n";

print fh "<table>\n";
print fh $Blank;

foreach $entry (@entries) 
{
	if(index($entry,".gle") != -1 && index($entry,".bak") == -1)
	{
		##print $entry."\n";
		($table, @upload) = doc_gle_file($entry);
		print fh $table;
		print fh $Blank;

	}
}
print fh "</table>\n";
close(fh);
system("perl ../script/combine.pl $ARGV[0] -english ../src/template.html ../src/menu.html $main_page.tmp $main_page");
system("rm $main_page.tmp");

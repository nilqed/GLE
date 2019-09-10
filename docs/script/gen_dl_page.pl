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
use Text::ParseWords;
use File::Basename;
use File::stat;
use Time::localtime;


$username = "o3rts6ib";
$password = "te5m8rou9g";

$DownloadLocation="http://groups.yahoo.com/group/cgle/files/";
$DownloadLocation="ftp://169.226.206.156/ExtraWeb/gle/";
$DownloadLocation="ftp://$username:$password\@169.226.206.156/ExtraWeb/gle/";
$DownloadLocation="";
$SFDLLocation="http://prdownloads.sourceforge.net/glx/";
$SFDLLocationEnding="?download";

sub table_start {
	$ret = "<tr><td BGCOLOR=#FFFFFF><div style=\"margin-left: 8px;\"><FONT SIZE=\"4\"><B>".$_[0]."</B></FONT></div></td><td>&nbsp;</td></tr>\n";
	$ret = $ret . "<tr><td><font size=-2>&nbsp;</font></td><td><font size=-2>&nbsp;</font></td></tr>\n";
	# <tr><td width=\"25%\" bgcolor=\"#d0d0d0\">Filename</td>
	# <td width=\"12.5%\" bgcolor=\"#d0d0d0\" align=\"center\">Date</td>
	# <td width=\"12.5%\" bgcolor=\"#d0d0d0\" align=\"center\">Size</td>
	# <td width=\"50%\" bgcolor=\"#d0d0d0\">Description</td></tr>\n";
	return $ret;
}

sub table_end {
	$ret = "<tr><td>&nbsp;</td><td>&nbsp;</td></tr>\n";
	return $ret;
}

$version = "4.0.10";
$version8 = "4.0.8";
$prev_version = "4.0.7";

open (fh,">$ARGV[1].tmp");
print fh "<h2>GLE Download Page</h2>\n";
print fh "<div style=\"margin-left: 8px;\">Consider subscribing to <a href=\"../main/docs.html\">GLE-announce</a> to be notified about new GLE versions.</div>\n";


open(fhin,$ARGV[0]);
@file=<fhin>;
$fname=1;
$location="";
$csize = "";
$ext = 0;
print fh "<table width=100%>\n";
foreach $line (@file) 
{
	$line =~ s/[\n\r\s]+$//g;
	if($line eq "[EXECUTABLES]") {
		$location = $line;
		print fh table_end();		
		print fh table_start("Binary Installers");
	}elsif($line eq "[DOCUMENTATION]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Documentation");
	}elsif($line eq "[SOURCE CODE]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Source Code");
	}elsif($line eq "[UTILITY]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Utility Packages");
	}elsif($line eq "[OLD VERSIONS]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Older Versions");
	}elsif($line eq "[PREVIEW]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Preview");		
	}elsif($line eq "[SYNTAX]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Syntax Highlighting");
	}elsif($line eq "[SCRIPTS]"){
		$location = $line;
		print fh table_end();
		print fh table_start("Scripts");
	}
	if(index($line,"I=") == 0) {
		$image = $line;
		$image =~ s/^I=//;
	}	
	if(index($line,"U=") == 0) {
		$url = $line;
		$url =~ s/^U=//;
	}
	if(index($line,"S=") == 0) {
		$csize = $line;
		$csize =~ s/^S=//;
	}	
	if(index($line,"LINK=EXT") == 0) {
		$ext = 1;
	}	
	if(index($line,"N=") == 0) {
		$name = $line;
		$name =~ s/^N=//;
		print fh "<tr><td><table><tr><td>&nbsp;&nbsp;";
		if ($image ne "") {
			print fh "<img width=\"32\" height=\"32\" src=\"../images/bitmap/$image\" align=center>&nbsp;&nbsp;";
		} else {
			print fh "<img width=\"32\" height=\"32\" src=\"../images/bitmap/empty-logo.gif\" align=center>&nbsp;&nbsp;";
		}
		$image = "";		
		print fh "</td><td>";
		print fh "<font size=-1><a href=\"$url\"";
		if ($ext == 1) {
			print fh " target=_blank";
		}
		print fh ">$name</a></font>";
		my $localfile = $csize;
		my $haslocal = 0;		
		if ($csize ne "") {
			if (-e "$csize") {
				$haslocal = 1;
			} elsif (-e "exclude/$csize") {
				$localfile = "exclude/$csize";
				$haslocal = 1;
			}		
		}
		if ($haslocal == 1) {
			$date = ctime( stat("$localfile")->mtime );
			$date =~ s/[0-9]+:[0-9]+:[0-9]+//g;
			$date = substr($date,4);
			$size = stat("$localfile")->size;
			$size = scalar($size);
			if($size >=0 && $size < 1024 ){
				$tsize = sprintf "%d B",$size;
			}elsif($size >=1024 && $size < 1024*1024){
				$tsize = sprintf  "%.1lf KB",$size/1024;
			}elsif($size >=1024*1024 && $size < 1024*1024*1024){
				$tsize = sprintf  "%.1lf MB",$size/(1024*1024);
			}elsif($size >=1024*1024*1024 && $size <1024*1024*1024*1024){
				$tsize = sprintf  "%.1lf GB",$size/(1024*1024*1024);
			}
			print fh "<br><font size=-2>($tsize, $date)</font>";
		}
		$csize = "";
		print fh "</td></tr></table></td>";
		$ext = 0;
	}		
	if(index($line,"F=") == 0)
	{
		$line =~ s/__VERSION8__/$version8/g;
		$line =~ s/__VERSION__/$version/g;
		$line =~ s/__PREVVERSION__/$prev_version/g;
		#
		# this is a file
		#
#		print $line."\n";
		$line =~ s/F=//;
		($filename,$location) = split(/:/,$line,2);
	#	print $filename." ".$location."\n";
		my $tsize;
		my $localfile = $filename;
		my $haslocal = 0;
		if (-e "$filename") {
			$haslocal = 1;
		} elsif (-e "exclude/$filename") {
			$localfile = "exclude/$filename";
			$haslocal = 1;
		}
		if ($haslocal == 1) {
			$date = ctime( stat("$localfile")->mtime );
			$date =~ s/[0-9]+:[0-9]+:[0-9]+//g;
			$date = substr($date,4);
			$size = stat("$localfile")->size;
			$size = scalar($size);
			if($size >=0 && $size < 1024 ){
				$tsize = sprintf "%d B",$size;
			}elsif($size >=1024 && $size < 1024*1024){
				$tsize = sprintf  "%.1lf KB",$size/1024;
			}elsif($size >=1024*1024 && $size < 1024*1024*1024){
				$tsize = sprintf  "%.1lf MB",$size/(1024*1024);
			}elsif($size >=1024*1024*1024 && $size <1024*1024*1024*1024){
				$tsize = sprintf  "%.1lf GB",$size/(1024*1024*1024);
			}
		}else{
			print "$filename does not exist\n";
		}
		print fh "<tr><td><table><tr><td>&nbsp;&nbsp;";
		if ($image ne "") {
			print fh "<img width=\"32\" height=\"32\" src=\"../images/bitmap/$image\" align=center>&nbsp;&nbsp;";
		} else {
			print fh "<img width=\"32\" height=\"32\" src=\"../images/bitmap/empty-logo.gif\" align=center>&nbsp;&nbsp;";
		}
		print fh "</td><td>";
		$image = "";
		$nicename = $filename;
		if($location eq "SF"){
			# this belongs to source forge
			print fh "<font size=-1><a href=\"".$SFDLLocation.basename($filename).$SFDLLocationEnding."\">".basename($nicename)."</a></font>&nbsp;";
		}else{
			print fh "<font size=-1><a href=\"".$DownloadLocation.basename($filename)."\">".basename($nicename)."</a></font>&nbsp;";
		}
		print fh "<br><font size=-2>($tsize, $date)</font></td></tr></table></td>";
		$fname=0;
		$ext = 0;
	}
	if(index($line,"D=") == 0)
	{
		$line =~ s/__VERSION8__/$version8/g;
		$line =~ s/__VERSION__/$version/g;
		$line =~ s/__PREVVERSION__/$prev_version/g;
		$line =~ s/D=//;
		#this is a description
		print fh  "<td><font size=-1>".$line."</font></td></tr></font>\n";
		$fname=1;
	}
}
$today=ctime();
print fh "</table>\n";
close(fh);
system("perl ../script/combine.pl ../ -english ../src/template.html ../src/menu.html $ARGV[1].tmp $ARGV[1]");
system("rm $ARGV[1].tmp");


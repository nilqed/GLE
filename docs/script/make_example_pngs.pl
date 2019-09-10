
my($file, $opts) = @ARGV;

my $base = $file;
$base =~ s/\.gle//i;
my $icon = $base."_icon.png";
my $big  = $base.".png";
my $eps  = $base.".eps";
my $pdf  = $base.".pdf";

$dpi = 100;
$cm_per_inch = 2.54;

$wid = 0;
$hei = 0;
open(FILE, $file) || die "Can't open: $file";
while ($line = <FILE>) {
	$line =~ s/[\r\n]//g;
	if ($line =~ /^size\s+([0-9]+)\s+([0-9]+)/) {
		$wid = $1;
		$hei = $2;
	}
	if ($line =~ /^\!\s*Page\ssize\s\(([0-9\.]+)\s+by\s+([0-9\.]+)/) {
		$wid = $1;
		$hei = $2;
	}
	if ($line =~ /^\!\s*OPTS\:\s*(.*)$/) {
		$opts = $1;
	}
}
close(FILE);

$size = $wid;
if ($hei > $size) { $size = $hei; }
if ($size == 0) { $size = 10; }

$pix = $dpi*$size/$cm_per_inch;
if ($pix < 300) {
	$dpi = 300*$cm_per_inch/$size;
}
if ($pix > 550) {
	$dpi = 550*$cm_per_inch/$size;
}
$dpi = int($dpi+0.5);
print "Size: $size, Hei = $hei, Pix = $pix, DPI: $dpi\n";

$gle = "gle";
if ($ENV{"RUNGLE"} ne "") {
	$gle = $ENV{"RUNGLE"};
}

print "$gle -dpi $dpi -d png $file\n";
if ($ENV{"VALGRINDGLE"} ne "") {
	system("valgrind $gle -dpi $dpi -d png $opts $file");
} else {
	system("$gle -dpi $dpi -d png $opts $file");
}
system("convert -resize 150x150 $big $icon");
system("$gle -d pdf $opts $file\n");


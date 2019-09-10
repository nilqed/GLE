
$toroot   = $ARGV[0];
$lang     = $ARGV[1];
$template = $ARGV[2];
$menu     = $ARGV[3];
$input    = $ARGV[4];
$target   = $ARGV[5];
$abs      = $ARGV[6];
$subdir   = $ARGV[7];

open(IN, "${toroot}/src/VERSION.txt") || die "Can't open '${toroot}/src/VERSION.txt'";
$VERSION = <IN>;
chomp($VERSION);
close(IN);

if ($abs eq "-absolute") {
	$MKABS = 1;
} else {
	$MKABS = 0;
}
if ($abs eq "-pdf") {
	$MKPDF = 1;
	$DEFINE{"pdf"} = 1;
	print "Constructing .pdf\n";
} else {
	$MKPDF = 0;
}

$ITEMIZE = 0;

$lang =~ s/\-//g;
$select =~ s/\-//g;

@MonthsEn = (
	"January", "February", "March", "April",
	"May", "June", "July", "August", "September",
	"October", "November", "December"
);

@MontsDutch = (
	"januari", "februari", "maart", "april",
	"mei", "juni", "juli", "augustus", "september",
	"oktober", "november", "december"
);

print "Input: $input [$lang] menu: $select\n";

open(FILE, "$template") || die "Can't open $template";
open(MENU, "$menu")     || die "Can't open $menu";
open(OUT, ">$target")   || die "Can't open $target";

if ($target =~ /\.php/) {
	open(INPUT, "$input") || die "Can't open $input";
	while ($line = <INPUT>) {
		chopcrlf();
		if ($line =~ /\<\!\-\-clus-version\-\-\>/) {
			$eval = "\$line =~ s/\\<\\!\\-\\-clus-version\\-\\-\\>/$VERSION/g;";
			eval($eval);
		}
		if ($line =~ /\<\!\-\-\sscript\-([a-z\-]+)/) {
			$cmnd = $1;
			if ($cmnd eq "insert-template-top") {
				$done = 0;
				while (($done == 0) && ($line = <FILE>)) {
					chopcrlf();
					if ($line =~ /\<\!\-\-\sscript\-([a-z\-]+)/) {
						$cmnd = $1;
						if ($cmnd eq "insert-date") {
							dodate();
						} elsif ($cmnd eq "insert-content") {
							$done = 1;
						}
					} else {
						print OUT "$line\n";
					}
				}
			} elsif ($cmnd eq "insert-template-bottom") {
				while ($line = <FILE>) {
					chopcrlf();
					if ($line =~ /\<\!\-\-\sscript\-([a-z\-]+)/) {
						$cmnd = $1;
						if ($cmnd eq "insert-date") {
							dodate();
						}
					} else {
						print OUT "$line\n";
					}
				}
			}
		} else {
			print OUT "$line\n";
		}
	}
	close(INPUT);
} else {
	$enable = 1; $defenable = 1;
	while ($line = <FILE>) {
		chopcrlf();
		if ($line =~ /\<\!\-\-\sscript\-([a-z\-]+)/) {
			$cmnd = $1;
			if ($cmnd eq "insert-date") {
				dodate();
			} elsif ($cmnd eq "insert-menu") {
				domenu();
			} elsif ($cmnd eq "insert-content") {
				docontent();
			} else {
				processspecial();
			}
		} else {
			if (($enable == 1) && ($defenable == 1)) {
				makeabs();
				print OUT "$line\n";
			}
		}
	}
}

close(OUT);
close(MENU);
close(FILE);

sub chopcrlf {
	$line =~ s/\n//;
	$line =~ s/\r//;
}

sub domenu {
	$found = 0;
	while (($found == 0) && ($line = <MENU>)) {
		if (eval("\$line =~ /\\[$lang\\]/")) {
			$found = 1;
		}
	}
	$index = 1;
	$found = 0;
	while (($found == 0) && ($line = <MENU>)) {
		if ($line =~ /\[[a-z]+\]/) {
			$found = 1;
		} else {
			if ($line =~ /^([^\&]+)\&(.+)$/) {
				$title = $1;
				$link  = $2;
				$title =~ s/^\s+//;
				$title =~ s/\s+$//;
				$link =~ s/^\s+//;
				$link =~ s/\s+$//;
				if ($index == 1) {
					$add = "1";
				} else {
					$add = "";
				}
				if ($index == $select) {
					print OUT "<tr><td class=\"menu-elem$add-sel\">";
				} else {
					print OUT "<tr><td class=\"menu-elem$add\">";
				}
				if ($MKABS == 1) {
					$link =~ s/\.\.\//http:\/\/www.cs.kuleuven.ac.be\/~jan\//ig;
					eval("\$link =~ s/^([^h]|h[^t]|ht[^t]|htt[^p])/http:\\/\\/www.cs.kuleuven.ac.be\\/~jan\\/$subdir\\/\$1/ig");
				}
				print OUT "<a href=\"${link}\">${title}</a></td></tr>\n";
				$index++;
			} else {
				die "Error in menu line $line";
			}
		}
	}
}

sub proccontent {
	my ($infile) = @_;
	my $prevenable = $enable;
	$enable = 1; $defenable = 1;
	while (eval("\$line = <$infile>")) {
		chopcrlf();
		if ($line =~ /\<\!\-\-clus-version\-\-\>/) {
			$eval = "\$line =~ s/\\<\\!\\-\\-clus-version\\-\\-\\>/$VERSION/g;";
			eval($eval);
		}
		if ($line =~ /\<\!\-\-\sscript\-([a-z\-]+)/) {
			$cmnd = $1;
			processspecial();
		} else {
			if (($enable == 1) && ($defenable == 1)) {
				makeabs();
				print OUT "$line\n";
			}
		}
	}
	$enable = $prevenable; $defenable = 1;
}

sub docontent {
	open(INPUT,"$input") || die "Can't open $input";
	proccontent("INPUT");
	close(INPUT);
}

sub dopage {
	if ($line =~ /page\=\"([^\"]+)\"/) {
		$fname = $1;
		open(PAGE, $fname) || die "Can't open page $fname";
		print "Include file: $fname\n";
		proccontent("PAGE");
		close(PAGE);
	} else {
		print "No page found on $line\n";
	}
}

sub makeabs {
	if ($line =~ /\<\!\-\-root-url\-\-\>/) {
		if ($toroot eq ".") {
			$torootesc = "";
		} else {
			$torootesc = $toroot;
		}
		$torootesc =~ s/\//\\\//g;
		$eval = "\$line =~ s/\\<\\!\\-\\-root-url\\-\\-\\>/$torootesc/g;";
		eval($eval);
	}
	if ($MKABS == 1) {
		$line =~ s/(\<img\ssrc\s*\=\s*\")\.\.\//$1http:\/\/www.cs.kuleuven.ac.be\/~jan\//ig;
		eval("\$line =~ s/(\\<img\\ssrc\\s*\\=\\s*\\\")([^h]|h[^t]|ht[^t]|htt[^p])/\$1http:\\/\\/www.cs.kuleuven.ac.be\\/~jan\\/$subdir\\/\$2/ig");

		$line =~ s/(background\s*\=\s*\")\.\.\//$1http:\/\/www.cs.kuleuven.ac.be\/~jan\//ig;
		eval("\$line =~ s/(background\\s*\\=\\s*\")([^h]|h[^t]|ht[^t]|htt[^p])/\$1http:\\/\\/www.cs.kuleuven.ac.be\\/~jan\\/$subdir\\/\$2/ig");

		$line =~ s/(href\s*\=\s*\")\.\.\//$1http:\/\/www.cs.kuleuven.ac.be\/~jan\//ig;
		eval("\$line =~ s/(href\\s*\\=\\s*\")([^h]|h[^t]|ht[^t]|htt[^p])/\$1http:\\/\\/www.cs.kuleuven.ac.be\\/~jan\\/$subdir\\/\$2/ig");
	}
	if ($MKPDF == 1) {
		makepdf();
	}
}

sub makepdf {
	$line =~ s/color\=\"[^\"]*\"//gi;
	$line =~ s/\<h[123]\>([^\<]+)\<\/h[123]\>/\\section{$1}/gi;
	$line =~ s/\<h[4]\>([^\<]+)\<\/h[4]\>/\\subsection{$1}/gi;
	$line =~ s/\<\/*ul\>//gi;
	$line =~ s/\<\/li\>//gi;
	$line =~ s/\<\/*body\>//gi;
	$line =~ s/\<\/html\>/\\end{document}/gi;
	$line =~ s/\<\/font\>/}/gi;
	$line =~ s/\<font\s+size=\"-1\"\>/{\\footnotesize /gi;
	$line =~ s/\<\/em\>/}/gi;
	$line =~ s/\<em\>/{\\em /gi;
	$line =~ s/\<br\>/\\mbox{}\\\\ /gi;
	$line =~ s/\<a\shref=[^\>]*\>//gi;
	$line =~ s/\<hr[^\>]*\>/\\rule{\\textwidth}{0.05cm}/gi;
	$line =~ s/\<\/a\>//gi;
	$line =~ s/\<\/span\>/}/gi;
	$line =~ s/\<span[^\>]*\>/\\url{/gi;
	if ($line =~ /\<li\>/i) {
		if ($ITEMIZE != 1) {
			$ITEMIZE = 1;
			print OUT "\\begin{itemize}\n";
		}
	} elsif ($line =~ /^\s*$/) {
		if ($ITEMIZE == 1) {
			$ITEMIZE = 0;
			print OUT "\\end{itemize}\n";
		}
	}
	$line =~ s/\<li\>/\\item /gi;
}

sub processspecial {
	if ($cmnd =~ /lang\-([a-z]+)/) {
		$sellang = $1;
		if ($sellang eq "any") {
			$enable = 1;
		} else {
			if ($sellang eq $lang) {
				$enable = 1;
			} else {
				$enable = 0;
			}
		}
	} elsif ($cmnd =~ /insert\-language/) {
		if ($MKPDF == 1) {
			print OUT "\\selectlanguage{$lang}\n";
		}
	} elsif (($cmnd eq "insert-page") && ($enable == 1)) {
		dopage();
	} elsif ($line =~ /define\=\"([^\"]+)\"/) {
		$def = $1;
		$DEFINE{$def} = 1;
	} elsif ($line =~ /notdef\=\"([^\"]+)\"/) {
		$def = $1;
		if ($DEFINE{$def} == 1) {
			$defenable = 0;
		} else {
			$defenable = 1;
		}
	} elsif ($line =~ /ifdef\=\"([^\"]+)\"/) {
		$def = $1;
		if ($DEFINE{$def} == 1) {
			$defenable = 1;
		} else {
			$defenable = 0;
		}
	} elsif ($cmnd =~ /endif/) {
		$defenable = 1;
	}
#	print "After $line -> $defenable\n";
}

sub dodate {
	# Get the all the values for current time
	my ($Second, $Minute, $Hour, $Day, $Month, $Year, $WeekDay, $DayOfYear, $IsDST) = localtime(time);
	# Months of the year are not zero-based
	my $RealYear = $Year + 1900;
	if ($lang =~ /dutch/) {
		$RealMonth = $MontsDutch[$Month];
		$FullDate = "Laatste wijziging: $Day $RealMonth $RealYear.";
	} else {
		$RealMonth = $MonthsEn[$Month];
		$FullDate = "Last update: $RealMonth $Day, $RealYear.";
	}
	eval("\$line =~ s/\\<\\!\\-\\-\\sscript[a-z\\-]+\\s\\-\\-\\>/$FullDate/");
	print OUT "$line\n";
}

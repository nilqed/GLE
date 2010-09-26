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

use Fcntl ':mode';

$files = `find .`;
@files = split(/[\n\r]+/, $files);

@OKEXEEXT_A = ("cmd","sh","pl","exe","so","package");
@NOEXEEXT_A = ("gle","cpp","h","i","gif","png","html","hlp","ico","pdf","tex","dat","csv","bak","let","sur","z","txt","bmp","css","big","key","lab","lis","xpm","ttf","sty","stx","win32","nsi");
@NOEXTFILES_A = ("changelog","surf_batch","surf_regis","surf_tek","surf_vt","surf_x");

foreach $ext (@NOEXEEXT_A) {
	$NOEXEEXT{$ext} = 1;
}

foreach $ext (@OKEXEEXT_A) {
	$OKEXEEXT{$ext} = 1;
}

foreach $ext (@NOEXTFILES_A) {
	$NOEXTFILES{$ext} = 1;
}

foreach $f (@files) {
	if (!($f =~ /\.svn/)) {
		($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat($f);
		if (S_ISLNK($mode) ||
		    S_ISBLK($mode) ||
		    S_ISCHR($mode) ||
		    S_ISFIFO($mode) ||
		    S_ISSOCK($mode)) {
			printf "*** FILE: $f has strange mode: %o\n", $mode;
		}
		if (($mode & S_ISUID) != 0) {
			printf "*** FILE: $f has UID bit set: %o\n", $mode;
		}
		if ((($mode & S_ISGID) != 0) ||
		    (($mode & S_ISVTX) != 0)) {
			printf "*** FILE: $f has GID/Sticky bit set: %o\n", $mode;
		}
		if ((($mode & S_IWGRP) != 0) || (($mode & S_IWOTH) != 0)) {
			printf "*** FILE: $f is group/other writable: %o\n", $mode;
		}
		if ((($mode & S_IRUSR) == 0) || (($mode & S_IRUSR) == 0) || (($mode & S_IRUSR) == 0)) {
			printf "*** FILE: $f is not world readable: %o\n", $mode;
		}
		if (($mode & S_IWUSR) == 0) {
			printf "*** FILE: $f is read only: %o\n", $mode;
		}
		if (($mode & S_IXUSR) != 0) {
			if ((($mode & S_IXOTH) == 0) || (($mode & S_IXGRP) == 0)) {
					printf "*** FILE: $f user executable, but not world executable: %o\n", $mode;
			}
			if (S_ISREG($mode)) {
				$ok = 0;
				if ($f =~ /\.([^\.\/]+)$/) {
					$ext = lc($1);
					if ($NOEXEEXT{$ext} == 1) {
						$ok = 1;
						printf "*** FILE: $f should *not* be executable, fixing: %o\n", $mode;
						chmod 0644, $f;
					}
					if ($OKEXEEXT{$ext} == 1) {
						$ok = 1;
					}
				}
				if ($f =~ /\/([^\/]+)$/) {
					$fname = lc($1);
					if ($fname =~ /makefile/) {
						$ok = 1;
						printf "*** FILE: $f should *not* be executable, fixing: %o\n", $mode;
						chmod 0644, $f;
					}
					if ($NOEXTFILES{$fname} == 1) {
						$ok = 1;
						printf "*** FILE: $f should *not* be executable, fixing: %o\n", $mode;
						chmod 0644, $f;
					}
				}
				if ($ok == 0) {
					printf "*** FILE: $f is executable: %o\n", $mode;
				}
			}
		}
		if ((!S_ISREG($mode)) && (!S_ISDIR($mode))) {
			printf "*** FILE: $f is not a regular file nor a directory: %o\n", $mode;
		}
		if (S_ISREG($mode) && S_ISDIR($mode)) {
			printf "*** FILE: $f both a regular file and a directory: %o\n", $mode;
		}
		if (S_ISDIR($mode)) {
			if (($mode & S_IXUSR) == 0) {
				printf "*** FILE: $f is a directory and not executable: %o\n", $mode;
			}
		}
	}
}

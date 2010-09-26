;;; gle-mode.el --- major mode for editing .gle files

 ;; Installation:
 ;; Put this file somewhere in your load-path (e.g. /usr/share/emacs/site-lisp)
 ;; Add the following to your ~/.emacs file:
 ;; (autoload 'gle-mode "gle-mode")
 ;; (add-to-list 'auto-mode-alist '("\\.gle\\'" . gle-mode))
 ;; gle-mode will now be called whenever a .gle file is opened, or by executing
 ;; M-x gle-mode

 ;; Copyright:
 ;; This file is free software; you can redistribute it and/or modify
 ;; it under the terms of the GNU General Public License as published by
 ;; the Free Software Foundation; either version 2, or (at your option)
 ;; any later version.

 ;; This file is distributed in the hope that it will be useful,
 ;; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ;; GNU General Public License for more details.

 ;; You should have received a copy of the GNU General Public License
 ;; along with GNU Emacs; see the file COPYING.  If not, write to
 ;; the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 ;; Boston, MA 02111-1307, USA.

 ;; To done (a.k.a. Version History):
 ;; 0.2.1: Then "adapted" it some more so that it worked!
 ;; 0.2.0: Shamelessly "adapted" some of Kai Nordlund's code to allow Emacs
 ;;        to call GLE inline :-)
 ;; 0.1.5: Regexps cleaned up after suggestions by Stefan Monnier, also
 ;;        Installation instructions trimmed a little.
 ;; 0.1.0: First ever public version!  God it's ugly :-(

 ;; To do:
 ;; Some more regexps to tidy
 ;; Create some automatic indentification

(require 'lmenu)

 ;; Begin gle-mode
(defvar gle-mode-syntax-table
   (let ((st (make-syntax-table)))
     (modify-syntax-entry ?! "<" st)
     (modify-syntax-entry ?\n ">" st)
     st)
   "Syntax table for `gle-mode'.")

(defvar gle-font-lock-keywords
   `((,(concat "\\<"
	       (regexp-opt '("begin" "start" "both" "end" "define" "for" "to" "next" "if" "then" "else" "include" "postscript" "bigfile" "data" "fullsize" "let" "set") t)
	       "\\>") (0 font-lock-keyword-face))
     (,(concat "\\<"
	       (regexp-opt '("sub") t)
	       "\\>") (0 font-lock-function-name-face))
     (,(concat "\\<"
	       (regexp-opt '("aline" "rline" "line" "amove" "rmove" "arc" "arcto" "bezier" "circle" "closepath" "curve" "ellipse" "ellipse_arc" "ellipse_narc" "grestore" "gsave" "join" "rbezier" "reverse" "save" "text" "write" "bar" "size" "title" "xaxis" "yaxis" "x2axis" "y2axis" "hscale" "vscale" "xlabels" "ylabels" "x2labels" "y2labels" "xnames" "ynames" "x2names" "y2names" "xplaces" "yplaces" "x2places" "y2places" "xside" "yside" "x2side" "y2side" "xticks" "yticks" "xsubticks" "ysubticks" "x2subticks" "y2subticks" "xtitle" "ytitle" "x2title" "y2title") t)
	       "\\>") (0 font-lock-constant-face))
     (,(concat "\\<"
	       (regexp-opt '("width" "err" "herr" "errwidth" "herrwidth" "errup" "herrup" "errdown" "herrdown" "hei" "color" "dashlen" "font" "fontlwidth" "just" "add" "nobox" "name" "arrow" "stroke" "fill" "justify" "cap" "lstyle" "lwidth" "from" "dist" "key" "xmin" "ymin" "xmax" "ymax" "nomiss" "smooth" "smoothm" "dsubticks" "dpoints" "nticks" "dticks" "font" "shift" "offset" "position" "marker" "msize" "mscale" "ignore") t)
	       "\\>") (0 font-lock-variable-name-face))
     (,(concat "\\<"
	       (regexp-opt '("box" "clip" "origin" "path" "marker" "rotate" "scale" "table" "text" "translate") t)
	       "\\>") (0 font-lock-string-face))
     (,(concat "\\<"
	       (regexp-opt '("butt" "round" "square" "mitre" "bevel" "left" "center" "right" "[tcblr]{2}" "on" "off" "grid" "log" "nofirst" "nolast") t)
	       "\\>") (0 font-lock-builtin-face))
     (,(concat "\\<"
	       (regexp-opt '("time\\$" "date\\$") t)
	       "\\>") (0 font-lock-string-face))
     ("\\s *@\\w+" (0 font-lock-function-name-face))
     ("\\b\\(left\\$\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(right\\$\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(seg\\$\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(num1?\\$\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(val\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(pos\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(len\\)\\s *(" (1 font-lock-string-face))
     ("\\b\\(abs\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?cosh\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(cos\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?coth?\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?csch?\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?sech?\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?sinh\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(sin\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(atn\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(a?tanh\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(tan\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(exp\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(fix\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(int\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(log\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(log10\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(sgn\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(sqrt?\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(todeg\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(torad\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(not\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(rnd\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\([xy]end\\)\\s *(\\s *)" (1 font-lock-function-name-face))
     ("\\b\\([xy]pos\\)\\s *(\\s *)" (1 font-lock-function-name-face))
     ("\\b\\([xy]g\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(twidth\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(theight\\)\\s *(" (1 font-lock-function-name-face))
     ("\\b\\(tdepth\\)\\s *(" (1 font-lock-function-name-face))
    )
   "Keyword highlighting specification for `gle-mode'.")


(defun gle-create-postscript ()
  "Creates eps file from current gle file" 
  (interactive)
  (save-buffer)
  (let ((file (file-name-nondirectory buffer-file-name)))
  (shell-command (concat "gle -d eps " file )))
)

(defun gle-view-postscript () 
  "Create and view Postscript from current gle file" 
  (interactive)
  (save-buffer)
  (let ((file (file-name-nondirectory buffer-file-name)))
  (shell-command (concat "gle -d eps " file )))
  (shell-command (concat "gv "          (file-name-sans-extension (buffer-file-name)) ".eps" ))
  (delete-file   (concat (file-name-sans-extension (buffer-file-name)) ".eps"))  
)

(defun gle-view-png ()
  (interactive)
  (save-buffer)
  (let ((file (file-name-nondirectory buffer-file-name)))
  (shell-command (concat "gle -d eps " file )))
  (shell-command "convert emacsgle.eps emacsgle.png")
  (shell-command "xv emacsgle.png")
  (shell-command "rm emacsgle.eps emacsgle.png")
) 

(defun gle-write-standard-graph ()
   "Command writes out gle commands for drawing a simple graph"
  (interactive)
  (insert "size 26 19\n")
  (insert "set lwidth 0.06\nset hei 0.5\nset font psh\n")
  (insert "begin graph\n   nobox\n")
  (insert "   \n")
  (insert "   data file.gle d1\n")
  (insert "   \n")
  (insert "   d1 marker dot msize 0.7\n")
  (insert "   d1 line lstyle 1 key \"\"\n")
  (insert "   \n")
  (insert "   key hei 0.6 nobox\n")
  (insert "   \n")
  (insert "   !yaxis min 0 max 10 ! log\n")
  (insert "   !xaxis min 0 max 10 ! log\n")
  (insert "   \n")
  (insert "   xtitle \"\" dist 0.3 hei 0.6\n")
  (insert "   ytitle \"\" dist 0.3 hei 0.6\n")
  (insert "   ! x2title \"\"\n")
  (insert "   \n")
  (insert "end graph\n")

) 

(define-derived-mode gle-mode fundamental-mode "GLE"
   "A major mode for editing .gle files."
   (set (make-local-variable 'comment-start) "! ")
   (set (make-local-variable 'comment-start-skip) "!+\\s-*")
   (set (make-local-variable 'font-lock-defaults)
        '(gle-font-lock-keywords))
   (run-hooks 'gle-mode-hook)
   )

(define-key gle-mode-map [menu-bar gle-mode]
  (cons "GLE" (make-sparse-keymap "GLE")))
(define-key gle-mode-map [menu-bar gle-mode gle-write-standard-graph]
  '("Write graph" . gle-write-standard-graph))
(define-key gle-mode-map [menu-bar gle-mode separator-gle-mode-2]
  '("--------------------"))
(define-key gle-mode-map [menu-bar gle-mode gle-create-postscript]
  '("Create ps file" . gle-create-postscript))
(define-key gle-mode-map [menu-bar gle-mode separator-gle-mode-1]
  '("--------------------"))
(define-key gle-mode-map [menu-bar gle-mode gle-view-png]
  '("View png file" . gle-view-png))
(define-key gle-mode-map [menu-bar gle-mode gle-view-postscript]
  '("View ps file" . gle-view-postscript))

(define-key gle-mode-map "\C-c\C-p" 'gle-create-postscript)
(define-key gle-mode-map "\C-c\C-v" 'gle-view-postscript)
(define-key gle-mode-map "\C-c\C-x" 'gle-view-x)
(define-key gle-mode-map "\C-c\C-b" 'gle-write-basic-commands)

(provide 'gle-mode)
 ;; End gle-mode

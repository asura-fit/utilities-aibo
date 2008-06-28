;; -*- Emacs-Lisp -*-
;; asura-scm.el  -- Scheme editing mode for Asura
;;

(require 'lisp-mode)
(require 'scheme)

(defvar asura-scm-mode-syntax-table nil "")
(if (not asura-scm-mode-syntax-table)
    (let ((i 0))
      (setq asura-scm-mode-syntax-table (make-syntax-table))
      (set-syntax-table asura-scm-mode-syntax-table)

      ;; Default is atom-constituent.
      (while (< i 256)
	(modify-syntax-entry i "_   ")
	(setq i (1+ i)))

      ;; Word components.
      (setq i ?0)
      (while (<= i ?9)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))
      (setq i ?A)
      (while (<= i ?Z)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))
      (setq i ?a)
      (while (<= i ?z)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))

      ;; Whitespace
      (modify-syntax-entry ?\t "    ")
      (modify-syntax-entry ?\n ">   ")
      (modify-syntax-entry ?\f "    ")
      (modify-syntax-entry ?\r "    ")
      (modify-syntax-entry ?  "    ")

      ;; These characters are delimiters but otherwise undefined.
      ;; Brackets and braces balance for editing convenience.
      (modify-syntax-entry ?\[ "(]  ")
      (modify-syntax-entry ?\] ")[  ")
      (modify-syntax-entry ?{ "(}  ")
      (modify-syntax-entry ?} "){  ")
      (modify-syntax-entry ?\| "  23")

      ;; Other atom delimiters
      (modify-syntax-entry ?\( "()  ")
      (modify-syntax-entry ?\) ")(  ")
      (modify-syntax-entry ?\; "<   ")
      (modify-syntax-entry ?\" "\"    ")
      (modify-syntax-entry ?' "  p")
      (modify-syntax-entry ?` "  p")

      ;; Special characters
      (modify-syntax-entry ?, "_ p")
      (modify-syntax-entry ?@ "_ p")
      (modify-syntax-entry ?# "_ p14")
      (modify-syntax-entry ?\\ "\\   ")))

(defvar asura-scm-mode-map nil)
(setq asura-scm-mode-map nil)     ;; for development
(if asura-scm-mode-map
    ()
  (setq asura-scm-mode-map (make-sparse-keymap))

  ;; keymap for evaluation
  (define-key asura-scm-mode-map "\C-cc" 'asura-scm-eval-buffer)
  (define-key asura-scm-mode-map "\C-cr" 'asura-scm-eval-region)
  (define-key asura-scm-mode-map "\C-ce" 'asura-scm-eval-last-sexp)
  (define-key asura-scm-mode-map "\C-cf" 'asura-scm-eval-file)
)

(defun asura-scm-mode ()
  "Scheme editing mode for ASURA"
  (interactive)
  (set-syntax-table asura-scm-mode-syntax-table)

  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'fill-paragraph-function)
  (setq fill-paragraph-function 'lisp-fill-paragraph)
  ;; Adaptive fill mode gets in the way of auto-fill,
  ;; and should make no difference for explicit fill
  ;; because lisp-fill-paragraph should do the job.
  (make-local-variable 'adaptive-fill-mode)
  (setq adaptive-fill-mode nil)
  (make-local-variable 'normal-auto-fill-function)
  (setq normal-auto-fill-function 'lisp-mode-auto-fill)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'lisp-indent-line)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (make-local-variable 'outline-regexp)
  (setq outline-regexp ";;; \\|(....")
  (make-local-variable 'comment-start)
  (setq comment-start ";")
  (make-local-variable 'comment-start-skip)
  ;; Look within the line for a ; following an even number of backslashes
  ;; after either a non-backslash or the line beginning.
  (setq comment-start-skip "\\(\\(^\\|[^\\\\\n]\\)\\(\\\\\\\\\\)*\\);+[ \t]*")
  (make-local-variable 'comment-column)
  (setq comment-column 40)
  (make-local-variable 'comment-indent-function)
  (setq comment-indent-function 'lisp-comment-indent)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (make-local-variable 'lisp-indent-function)
  (set lisp-indent-function 'scheme-indent-function)
  (setq mode-line-process '("" scheme-mode-line-process))
  (make-local-variable 'imenu-case-fold-search)
  (setq imenu-case-fold-search t)

  (make-local-variable 'font-lock-defaults)  
  (setq font-lock-defaults
        '((asura-scm-font-lock-keywords
           asura-scm-font-lock-keywords-1 asura-scm-font-lock-keywords-2)
          nil t (("+-*/.<>=!?$%_&~^:" . "w")) beginning-of-defun
          (font-lock-mark-block-function . mark-defun)))

  (use-local-map asura-scm-mode-map)
  (setq major-mode 'asura-scm-mode)
  (setq mode-name "Asura Scheme")
  (run-hooks 'asura-scm-mode-hook))

(defgroup asura-scm nil
  "editing scheme code for ASURA"
  :group 'lisp)

(defcustom asura-scm-mode-hook nil
  "Normal hook (list of functions) run when entering scheme-mode.
See `run-hooks'."
  :type 'hook
  :group 'asura-scm)

(defconst asura-scm-font-lock-keywords-1
  (eval-when-compile
    (list
     ;;
     ;; Declarations.  Hannes Haug <hannes.haug@student.uni-tuebingen.de> says
     ;; this works for SOS, STklos, SCOOPS, Meroon and Tiny CLOS.
     (list (concat "(\\(define\\*?\\("
		   ;; Function names.
		   "\\(\\|-public\\|-method\\|-generic\\(-procedure\\)?\\)\\|"
		   ;; Macro names, as variable names.  A bit dubious, this.
		   "\\(-syntax\\)\\|"
		   ;; Class names.
		   "-class"
                   ;; Guile modules.
                   "\\|-module"
		   "\\)\\)\\>"
		   ;; Any whitespace and declared object.
		   "[ \t]*(?"
		   "\\(\\sw+\\)?")
	   '(1 font-lock-keyword-face)
	   '(6 (cond ((match-beginning 3) font-lock-function-name-face)
		     ((match-beginning 5) font-lock-variable-name-face)
		     (t font-lock-type-face))
	       nil t))
     ))
  "Subdued expressions to highlight in Scheme modes.")

(defconst asura-scm-font-lock-keywords-2
  (append asura-scm-font-lock-keywords-1
   (eval-when-compile
     (list
      ;;
      ;; Control structures.
      (cons
       (concat
	"(" (regexp-opt
	     '("begin" "call-with-current-continuation" "call/cc"
	       "call-with-input-file" "call-with-output-file" "case" "cond"
	       "else" "if" "lambda"
	       "let" "let*" "let-syntax" "letrec" "letrec-syntax"
	       ;; Hannes Haug <hannes.haug@student.uni-tuebingen.de> wants:
	       "and" "or" "delay"
	       ;; Stefan Monnier <stefan.monnier@epfl.ch> says don't bother:
	       ;;"quasiquote" "quote" "unquote" "unquote-splicing"
	       "syntax" "syntax-rules") t)
	"\\>") 1)
      (cons
       (concat 
	"(" (regexp-opt
	     '("do" "for-earch" "map") t)
	"\\>") font-lock-constant-face)
      ;;
      ;; David Fox <fox@graphics.cs.nyu.edu> for SOS/STklos class specifiers.
      '("\\<<\\sw+>\\>" . font-lock-type-face)
      ;;
      ;; Scheme `:' keywords as builtins.
      '("\\<:\\sw+\\>" . font-lock-builtin-face)
      )))
  "Gaudy expressions to highlight in Scheme modes.")

(defvar asura-scm-font-lock-keywords asura-scm-font-lock-keywords-1
  "Default expressions to highlight in Scheme modes.")

(defvar asura-scm-pipe-buffer " *ASURA-SCM pipe*")
(defvar asura-scm-pipe-name   "stdIn.pipe")

(defun asura-scm-eval-string (string)
  "write string into named-pipe for evaluate string"
  (save-excursion
    (let ((buf (get-buffer-create asura-scm-pipe-buffer)))
      ;; change editing buffer to asura-scm-pipe-buffer
      (set-buffer buf)
      ;; erase old contents
      (setq buffer-read-only nil)
      (erase-buffer)
      ;; write string to buffer
      (insert string)
      (insert "\n") ;; gurantee the end of buffer is already "new-line"
      ;; write contents of this buffer to named-pipe
      (write-region (point-min) (point-max)
		    asura-scm-pipe-name))))

(defun asura-scm-eval-buffer ()
  "evaluate entire contents of current buffer."
  (interactive)
  (save-excursion
    (let ((string (buffer-substring-no-properties (point-min)
						  (point-max))))
      (asura-scm-eval-string string))))

(defun asura-scm-eval-region (beg end)
  "evaluate current region"
  (interactive "r")
  (save-excursion
    (let ((string (buffer-substring-no-properties beg end)))
      (asura-scm-eval-string string))))

(defun asura-scm-eval-last-sexp ()
  "evaluate last s-expression"
  (interactive)
  (save-excursion
    (let (beg end string)
      (backward-sexp)
      (setq beg (point))
      (forward-sexp)
      (setq end (point))

      (setq string (buffer-substring-no-properties beg end))
      (asura-scm-eval-string string))))

(defun asura-scm-eval-file (file)
  "evaluate file"
  (interactive "fFile: ")
  (save-excursion 
    (find-file file)
    (let ((string (buffer-substring-no-properties (point-min) (point-max))))
      (asura-scm-eval-string string))))

(defun asura-scm-aibotool ()
  "aibotool から起動される場合のコマンド"
  (interactive)
  (asura-scm-scratch nil))

(defun asura-scm-scratch (&optional arg)
  (interactive)
  (progn
    ;; "*scratch*" を作成して buffer-list に放り込む
    (set-buffer (get-buffer-create "*asura-scm-scratch*"))
    (funcall 'asura-scm-mode)
    (erase-buffer)
    (or arg (progn (setq arg 0)
		   (switch-to-buffer "*asura-scm-scratch*")))
    (cond ((= arg 0) (message "*asura-scm-scratch* is cleared up."))
	  ((= arg 1) (message "another *asura-scm-scratch* is created")))))

(provide 'asura-scm)

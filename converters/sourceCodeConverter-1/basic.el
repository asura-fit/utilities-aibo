;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 日本語表示の設定
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(if (featurep 'mule)
	(progn
	  (set-language-environment "Japanese")
	  (prefer-coding-system 'euc-jp-unix)
	  (set-keyboard-coding-system 'euc-jp-unix)
	  (if (not window-system) (set-terminal-coding-system 'euc-jp-unix))
	  ;;
	  ))
; 日本語 grep
(if (file-exists-p "/usr/bin/lgrep")
	(setq grep-command "lgrep -n ")
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; stroustrupベースのスタイル
; commentを左に寄せないスタイル
(c-add-style
 "asura2005"
 '(
   (tab-width . 4) ;; 変更
   (indent-tabs-mode . t) ;; 変更
   (c-basic-offset . 4)
   (c-comment-only-line-offset . 0)
   (c-offsets-alist . ((statement-block-intro . +)
					   (substatement-open . 0)
					   (label . 0)
					   (statement-cont . +)
					   (innamespace . 0)	;; 追加
					   (inclass . +)		;; 追加
					   (inextern-lang . 0)	;; 追加
					   (comment-intro . 0)	;; 変更
					   ))))

(defun asura2005-c-mode-common-hook ()
  ;; 必須
  ; stroustrup mode
  (c-set-style "asura2005")
  ; インデントをスペースではなく\tで行なう ? t : f
  (setq indent-tabs-mode t)
  ;; 好きずき
  ; GTAGS
  (gtags-mode t)
  (gtags-make-complete-list)
  ; お節介機能 auto-state, hungry-state
  ; "{" "}"などを入力すると自動的に次の行へ ? t : -1
  (c-toggle-auto-state -1)
  ; 改行時、自動的に次の行をインデント ? t : -1
  (c-toggle-hungry-state -1)
  ; [tab]キーでインデント ? t : -1
  (setq c-tab-always-indent -1)

  ; かんなで "、" を入力できるようにする
  (if (and (boundp 'CANNA) CANNA)
	  (local-set-key "," 'canna-self-insert-command))

  ; RET と C-j を入れ替える
  (local-set-key "\C-j" 'goto-line)
  (local-set-key "\C-m" 'newline-and-indent)

  ; BackSpaceはタブを展開せずに消す
  (local-set-key [backspace] 'backward-delete-char)
  (setq tab-stop-list
		'(4 8 12 16 20 24 28 32 36 40 44 48 52 56 60 64 68 72 76 80 84 88 92 96 100 104 108 112 116 120))
  )
(add-hook 'c-mode-common-hook 'asura2005-c-mode-common-hook)

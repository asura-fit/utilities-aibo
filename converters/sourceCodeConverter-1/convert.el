;; Convert
;;

; ローカル変数リストを無効にする
(setq enable-local-variables nil)

; 与えられたファイルを開く
(find-file my-file-name)

; c++-modeにする
(c++-mode)
; asura2005スタイル
(c-set-style "asura2005")

; 行頭のスペースとタブを削除
(goto-char (point-min))
(while (re-search-forward "^[ 	]" nil t)
  (replace-match "" nil nil))

;(indent-region 0 buffer-saved-size nil)
(indent-region (point-min) (point-max) nil)

; スペースをタブに
(tabify (point-min) (point-max))

; 行末のスペースとタブを削除
(goto-char (point-min))
(while (re-search-forward "\\([^ 	
]+\\)[ 	]+$" nil t)
  (replace-match "\\1" nil nil))

; utf-8-unixで保存
(setq buffer-file-coding-system 'utf-8-unix)
(setq save-buffer-coding-system buffer-file-coding-system)
; 最後にファイルを保存して終了
(save-buffer nil)

;(save-buffers-kill-emacs)
;(save-buffer)


;(execute-extended-command 'indent-regin)
;(indent-region)
; 行頭の4つスペースをタブに
;(goto-char (point-min))
;(while (re-search-forward "^    " nil t)
;  (replace-match "	" nil nil))

; タブでインデント
;(setq indent-tabs-mode t)
;(mark-page)
;(funcall indent-region-function 0 117760) ; buffer-saved-size)

; スペースとタブだけで構成されている行のスペースとタブをを削除
;(goto-char (point-min))
;(while (re-search-forward "^[ 	]+$" nil t)
;  (replace-match "" nil nil))

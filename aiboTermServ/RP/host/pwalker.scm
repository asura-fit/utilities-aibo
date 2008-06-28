;
;関節の値セット
;
;(pwalk-setjoint #(-1 0 0 4 11 101 4 11 101 -37 8 88 -38 8 88 -2 0))
;
;


(set! is-scheduler-enabled #t)


; 半周期ごとに設定するパラメータ
;
; 詳しくはミンチに(定義が間違ってるかも知れません)
(define pwalk-normal-new-style #(
; walk type (1=通常, 8=Enhanced(オフセットまるで無視))
				 34;99
; foward (本当はcm/0.5cycle)
				 0
; left   (本当はcm/0.5cycle)
				 0;-38
; turnCCW(本当はdegree/0.5cycle)
				 0;-4
; speed  (本当はevents/cycle)
				 35
; オフセットが有効なのはwalktype=1 (mm)
; offsets                        hF hB hdF hdB ffO fsO bfO bsO
				 ;80 80 0   15  60  50 -40  10
				 ;80 80 0   20  60  50 -50  15
75 100 13 18 70 15 -40 20
				 ;80 80 5   10  60  60 -40  5
; head & tailtype headpan headtilt bodytilt mouth tail-tilt tail-pan
 			          2       0        0        0     0         0        0 ))
;
; Effectorイベントごとに設定するパラメータ
;/ continue pwalk procedure
;// (pwalk-pstep-continue 
;//             #(headtype headpan headtilt bodytilt headmouth tail-tilt tail-pan))
(define pwalk-normal-cont-style #(2 0 0 0 0 0 0))

;初期設定
(define pwalk-initial-style #(15 0 0 0 40
				 70 80 0   15  60  50 -40  10
			    				 2 0 0 0 0 0 0)
)

(define pwalk-cycle-count 0)
;
;動作毎に初期値に戻すかどうか
;
(define pwalk-initialize #t)
;(define pwalk-initialize #f)

;
;関節の値とり
;
;(pwalk-getjoint)
;


;電源ON/OFF
;
;(poweroff)
;(poweron)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; 実際のディスパッチ
;
(define integer-dispatch (lambda (dispatch-name) 
;			(display dispatch-name)
;			(newline)
			(cond ((= 0  dispatch-name) (update-camera))
			      ((= 1  dispatch-name) (ready-effector))
			      ((= 2  dispatch-name) (scheduling-task)))))
;
; ディスパッチ受け付け
;
(define dispatch integer-dispatch)
;
; カメラ画像更新イベント
;
(define update-camera  
  (lambda () ))
;    (display "range=") (display (/ range2obstacle 10.0)) (newline)))
;   (display joint_tilt) (newline)))


;
; 半周期を何回繰り返すか
;
(define pwalk-count pwalk-cycle-count)
;; Effector準備OKイベント
;
(define ready-effector 
  (lambda ()
;    (display "EFF") 
; return value が 1の場合はpwalkの半周期が終了。
    (if (= (pwalk-pstep-continue pwalk-normal-cont-style) 1)
; 新たなコマンドを受け付ける
	(if (> pwalk-count 0)
	    (let ()
	      (pwalk-pstep-new pwalk-normal-new-style)
	      (set! pwalk-count (- pwalk-count 1))
	      ))
; それ以外は、とくにすることなし。
	)))
(or pwalk-initialize (pwalk-pstep-new pwalk-initial-style))
;))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 最初のpwalk指示
(and pwalk-initialize (pwalk-pstep-new pwalk-normal-new-style) )

; GCを冗長に表示する
;(gc-verbose #t)

; GCを冗長に表示しない
(gc-verbose #f)


; debug logを表示する
;(debuglog #t)

; debug logを表示しない
;(debuglog #f)

; info logを表示する
;(infolog #t)

; info logを表示しない
;(infolog #f)

; Aimonへモニタリング情報を送信
;(monitor #t)

; Aimonへモニタリング情報を送信しない
;(monitor #f)

; AIBO再起動 再起動後に./ooObjectManagerを再起動してください。
; (reboot)



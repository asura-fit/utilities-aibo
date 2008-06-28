;(set! pwalk-collector #t)
; 半周期ごとに設定するパラメータ
;
(define pwalk-count 0)
; 詳しくはミンチに(定義が間違ってるかも知れません)
(define pwalk-normal-new-style #(
; walk type (x1=4常, 8=Enhanced(オフセットまるで無視))
				87
; foward (本当はcm/0.5cycle)
				 0
; left   (本当はcm/0.5cycle)
				 -6
; turnCCW(本当はdegree/0.5cycle)
				 5
; speed  (本当はevents/cycle)
				 30
; オフセットが有効なのはwalktype=1 (mm)
; offsets                        hF hB hdF hdB ffO fsO bfO bsO
;90 80 10 10 30 0 -65 5
;90 80 10 10 30 0 -65 5
;70 100 10 15 65 10 -40 10


;forw ard
;55; 80cm
;70 90 3 15 50 10 -50 10
;70 90 30 25 50 15 -50 0 0
;70 90 30 25 50 15 -50 0 
;60 90 20 15 40 10 -50 0 
;60 90 20 15 50 10 -50 0 
;			 95 100 20 20 0 5 -50 5 

;70 90 10 15 40 10 -60 10
;70 90 10 15 50 0 -70 10
;70 90 10 15 50 0 -70 10

;			 80 90 15 20 40 0 -55 5 
;				 60 100 10 15 70 15 -50 5 
;				 55 90 10 15 40 10 -50 0 
;side
;45 60cm 
; 90 125  10 20   60 0 -60 0
;95 110 10 15 25 5 -60 5 
;twineless turn 37 330do
;turn turn 37 400do
;95 110 15 15 10 15 -40 10 
;70 90 15 15 30 0 -60 0
;leftforward 40 -30 f:74cm r: 55cm
;70 95 5 11 50 5 -4;rightforward 40 30 f:76 l:55
;70 95 5 11 50 5 -40 0 
;back -40 70cm
;90 110 20 30 50 5 -30 0 
90 110 20 30 50 5 -30 0 
                 ;80 80 5   10  60  60 -40  5




; head & tail                    type tilt pan mouth tail-tilt tail-pan
 			         2     0 0 0       0    0    0 ))
;
; Effectorイベントごとに設定するパラメータ
;/ continue pwalk procedure
;// (pwalk-pst ep-continue 
;//             #(headtype headtilt headpan headmouth tail-tilt tail-pan))
(define pwalk-normal-cont-style #(2 0 0 0 0 0 0))


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
;(define pwalk-count 100)
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
;))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; モータの電源ON
(poweron)

; モータの電源OFF
;(poweroff)

; 最初のpwalk指示
(pwalk-pstep-new pwalk-normal-new-style) 

; 例：文字を表示
; (display "this is a pen")

; 現在定義済の変数(読みだしのみ)
; 頭の角度：   joint-pan, joint-tilt, joint-roll (度)
; 距離センサ： range2obstacle (mm)
; 例：距離表示(cm)
; (display (/ range2obstacle 10))


; GCを冗長に表示する
;(gc-verbose #t)

; GCを冗長に表示しない
(gc-verbose #f)


; イベントのディスパッチを有効にする
(set! is-dispatch-enabled #t)

; スケジューラを有効にする
(set! is-scheduler-enabled #t)

; NormalTaskにおいてnextTask名をschemeから変更する場合#tにする
;(set! is-next-task-name-enabled #f)

; NormalTaskにおけるnextTask名の設定
;(set! next-task-name "dribble")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Scheme でのタスクスケジューリング

;; NomralTask でのスケジューリングのdispatch を有効にする
;(set! is-dispatch-scheduling-enabled #t) 
;; NomralTask でのスケジューリングのdispatch を無効にする
																				;(set! is-dispatch-scheduling-enabled #f) 

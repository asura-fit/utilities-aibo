;(set! pwalk-collector #t)
; Ⱦ�������Ȥ����ꤹ��ѥ�᡼��
;
(define pwalk-count 0)
; �ܤ����ϥߥ����(������ְ�äƤ뤫���Τ�ޤ���)
(define pwalk-normal-new-style #(
; walk type (x1=4��, 8=Enhanced(���ե��åȤޤ��̵��))
				87
; foward (������cm/0.5cycle)
				 0
; left   (������cm/0.5cycle)
				 -6
; turnCCW(������degree/0.5cycle)
				 5
; speed  (������events/cycle)
				 30
; ���ե��åȤ�ͭ���ʤΤ�walktype=1 (mm)
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
; Effector���٥�Ȥ��Ȥ����ꤹ��ѥ�᡼��
;/ continue pwalk procedure
;// (pwalk-pst ep-continue 
;//             #(headtype headtilt headpan headmouth tail-tilt tail-pan))
(define pwalk-normal-cont-style #(2 0 0 0 0 0 0))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; �ºݤΥǥ����ѥå�
;
(define integer-dispatch (lambda (dispatch-name) 
;			(display dispatch-name)
;			(newline)
			(cond ((= 0  dispatch-name) (update-camera))
			      ((= 1  dispatch-name) (ready-effector))
			      ((= 2  dispatch-name) (scheduling-task)))))

;
; �ǥ����ѥå������դ�
;
(define dispatch integer-dispatch)


;
; ���������������٥��
;
(define update-camera  
  (lambda () ))
;    (display "range=") (display (/ range2obstacle 10.0)) (newline)))
;   (display joint_tilt) (newline)))


;
; Ⱦ�����򲿲󷫤��֤���
;
;(define pwalk-count 100)
;; Effector����OK���٥��
;
(define ready-effector 
  (lambda ()
;    (display "EFF") 
; return value �� 1�ξ���pwalk��Ⱦ��������λ��
    (if (= (pwalk-pstep-continue pwalk-normal-cont-style) 1)
; �����ʥ��ޥ�ɤ�����դ���
	(if (> pwalk-count 0)
	    (let ()
	      (pwalk-pstep-new pwalk-normal-new-style)
	      (set! pwalk-count (- pwalk-count 1))
	      ))
; ����ʳ��ϡ��Ȥ��ˤ��뤳�Ȥʤ���
	)))
;))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; �⡼�����Ÿ�ON
(poweron)

; �⡼�����Ÿ�OFF
;(poweroff)

; �ǽ��pwalk�ؼ�
(pwalk-pstep-new pwalk-normal-new-style) 

; �㡧ʸ����ɽ��
; (display "this is a pen")

; ��������Ѥ��ѿ�(�ɤߤ����Τ�)
; Ƭ�γ��١�   joint-pan, joint-tilt, joint-roll (��)
; ��Υ���󥵡� range2obstacle (mm)
; �㡧��Υɽ��(cm)
; (display (/ range2obstacle 10))


; GC���Ĺ��ɽ������
;(gc-verbose #t)

; GC���Ĺ��ɽ�����ʤ�
(gc-verbose #f)


; ���٥�ȤΥǥ����ѥå���ͭ���ˤ���
(set! is-dispatch-enabled #t)

; �������塼���ͭ���ˤ���
(set! is-scheduler-enabled #t)

; NormalTask�ˤ�����nextTask̾��scheme�����ѹ�������#t�ˤ���
;(set! is-next-task-name-enabled #f)

; NormalTask�ˤ�����nextTask̾������
;(set! next-task-name "dribble")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Scheme �ǤΥ������������塼���

;; NomralTask �ǤΥ������塼��󥰤�dispatch ��ͭ���ˤ���
;(set! is-dispatch-scheduling-enabled #t) 
;; NomralTask �ǤΥ������塼��󥰤�dispatch ��̵���ˤ���
																				;(set! is-dispatch-scheduling-enabled #f) 

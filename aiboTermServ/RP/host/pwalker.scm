;
;������ͥ��å�
;
;(pwalk-setjoint #(-1 0 0 4 11 101 4 11 101 -37 8 88 -38 8 88 -2 0))
;
;


(set! is-scheduler-enabled #t)


; Ⱦ�������Ȥ����ꤹ��ѥ�᡼��
;
; �ܤ����ϥߥ����(������ְ�äƤ뤫���Τ�ޤ���)
(define pwalk-normal-new-style #(
; walk type (1=�̾�, 8=Enhanced(���ե��åȤޤ��̵��))
				 34;99
; foward (������cm/0.5cycle)
				 0
; left   (������cm/0.5cycle)
				 0;-38
; turnCCW(������degree/0.5cycle)
				 0;-4
; speed  (������events/cycle)
				 35
; ���ե��åȤ�ͭ���ʤΤ�walktype=1 (mm)
; offsets                        hF hB hdF hdB ffO fsO bfO bsO
				 ;80 80 0   15  60  50 -40  10
				 ;80 80 0   20  60  50 -50  15
75 100 13 18 70 15 -40 20
				 ;80 80 5   10  60  60 -40  5
; head & tailtype headpan headtilt bodytilt mouth tail-tilt tail-pan
 			          2       0        0        0     0         0        0 ))
;
; Effector���٥�Ȥ��Ȥ����ꤹ��ѥ�᡼��
;/ continue pwalk procedure
;// (pwalk-pstep-continue 
;//             #(headtype headpan headtilt bodytilt headmouth tail-tilt tail-pan))
(define pwalk-normal-cont-style #(2 0 0 0 0 0 0))

;�������
(define pwalk-initial-style #(15 0 0 0 40
				 70 80 0   15  60  50 -40  10
			    				 2 0 0 0 0 0 0)
)

(define pwalk-cycle-count 0)
;
;ư����˽���ͤ��᤹���ɤ���
;
(define pwalk-initialize #t)
;(define pwalk-initialize #f)

;
;������ͤȤ�
;
;(pwalk-getjoint)
;


;�Ÿ�ON/OFF
;
;(poweroff)
;(poweron)

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
(define pwalk-count pwalk-cycle-count)
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
(or pwalk-initialize (pwalk-pstep-new pwalk-initial-style))
;))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; �ǽ��pwalk�ؼ�
(and pwalk-initialize (pwalk-pstep-new pwalk-normal-new-style) )

; GC���Ĺ��ɽ������
;(gc-verbose #t)

; GC���Ĺ��ɽ�����ʤ�
(gc-verbose #f)


; debug log��ɽ������
;(debuglog #t)

; debug log��ɽ�����ʤ�
;(debuglog #f)

; info log��ɽ������
;(infolog #t)

; info log��ɽ�����ʤ�
;(infolog #f)

; Aimon�إ�˥���󥰾��������
;(monitor #t)

; Aimon�إ�˥���󥰾�����������ʤ�
;(monitor #f)

; AIBO�Ƶ�ư �Ƶ�ư���./ooObjectManager��Ƶ�ư���Ƥ���������
; (reboot)



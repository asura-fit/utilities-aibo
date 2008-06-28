; /////////////////////////////////////////////////////
; info target definitions
; /////////////////////////////////////////////////////
(define powerof2
  (lambda (times)
    (if (= times 0)
	1 
	(* (powerof2 (- times 1)) 2))))
; reserved
(define LOG_NOTHING        0)
(define LOG_ALL           -1)
(define LOG_DEFAULT        (powerof2 0))
; basic
(define LOG_ASURACORE      (powerof2 1))
(define LOG_LOCOMOTION     (powerof2 2))
(define LOG_ENVINFO        (powerof2 3))
(define LOG_EVENT          (powerof2 4))
(define LOG_TASKMANAGER    (powerof2 5))
(define LOG_BENCHMARK      (powerof2 6))
(define LOG_SCHEME         (powerof2 7))
; tasks 
(define LOG_DGETBEHINDBALL (powerof2 10))
(define LOG_BALLTRACKING   (powerof2 11))
(define LOG_FINDBALL       (powerof2 12))
(define LOG_FINDHOLDBALL   (powerof2 13))
(define LOG_LOOKAROUND     (powerof2 14))
(define LOG_CALCOBJSPEED   (powerof2 15))
(define LOG_DRIBBLE        (powerof2 16))
(define LOG_NORMALTASK     (powerof2 17))
(define LOG_GOALIEBEHINDBALL (powerof2 18))
(define LOG_NORMAL_SCH     (powerof2 19))
(define LOG_NORMALENV_MGR  (powerof2 20))
(define LOG_SCHEDULER      (powerof2 21))
(define LOG_OTHERTASK      (powerof2 22))

; components
(define LOG_PWALK          (powerof2 24))
(define LOG_GCD            (powerof2 25))
(define LOG_VISUALCORTEX   (powerof2 26))
(define LOG_GPS            (powerof2 27))
(define LOG_MESSAGE        (powerof2 28))
(define LOG_GLOBALMODEL    (powerof2 29))

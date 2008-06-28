(define LIBERO      0)
(define STRIKER     1)
(define DEFENDER    2)
(define GOALIE      3)
(define AVOIDANCE   4)
(define TECHNOGOFAST 5)

(define init-as
  (lambda (role)
    (if (= role GOALIE)
        (init-as-goalie)
      (init-as-attacker role))))
		  
; initialize as an attacker
;(init-as-attacker LIBERO)
;(init-as-attacker STRIKER)
;(init-as-attacker DEFENDER)
;(init-as-attacker AVOIDANCE)
;(init-as-attacker TECHNOGOFAST)

; initilize as an goalie
;(init-as-goalie)

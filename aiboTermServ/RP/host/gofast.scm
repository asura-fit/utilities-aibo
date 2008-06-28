;
; GOFAST.SCM
;

; ParaWalk default offsets enabled by walktype = 99
(set! pwalk-offset-shoulder-height 90.0)
(set! pwalk-offset-hip-height 80.0)
(set! pwalk-offset-front-leg-lift-height 10.0)
(set! pwalk-offset-back-leg-lift-height 10.0)
(set! pwalk-offset-front-position-forward 30.0) 
(set! pwalk-offset-front-position-side 0.0)
(set! pwalk-offset-back-position-forward -65.0)
(set! pwalk-offset-back-position-side 5.0)


;(set! pwalk-offset-shoulder-height 90.0)
;(set! pwalk-offset-hip-height 80.0)
;(set! pwalk-offset-front-leg-lift-height 10.0)
;(set! pwalk-offset-back-leg-lift-height 10.0)
;(set! pwalk-offset-front-position-forward 30.0) 35
;(set! pwalk-offset-front-position-side 0.0)
;(set! pwalk-offset-back-position-forward -65.0)
;(set! pwalk-offset-back-position-side 5.0)
;(set! pwalk-speed 30)
;
;
; OPEN-R Techno Forum go fast challenge specific variabes
;
; turn if the distance to the beacon becomes less than the value
(set! gofast-dist-to-turn 50.0)

(set! gofast-tilt-offset 25.0)

; run search beacon algorith if lost time goes the thresh
(set! gofast-losttime-search-thresh 10)

; how match turn applies during align to the beacon
(set! gofast-turn-calib-rate  1.0)
; how match left applies during align to the beacon
(set! gofast-left-calib-rate  1.0)

(set! gofast-pan-max   50.0)
(set! gofast-pan-speed 0.4)
(set! gofast-tilt-min -10.0)
(set! gofast-tilt-max  15.0)
(set! gofast-tilt-speed 0.4)
; coefficients for control values
(set! gofast-forward-rate 1.0)
(set! gofast-left-rate    5.0)
(set! gofast-turnccw-rate 1.0)
(set! gofast-mode 1)
(set! gofast-walktype 1)

(set! gofast-marker1 0)
(set! gofast-marker2 0)
(set! gofast-marker2 1)
(set! gofast-marker2 2)
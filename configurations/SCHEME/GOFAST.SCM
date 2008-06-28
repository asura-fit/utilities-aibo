;
; GOFAST.SCM
;

; ParaWalk default offsets enabled by walktype = 99
(set! pwalk-offset-shoulder-height 100.0)
(set! pwalk-offset-hip-height 120.0)
(set! pwalk-offset-front-leg-lift-height 15.0)
(set! pwalk-offset-back-leg-lift-height 18.0)
(set! pwalk-offset-front-position-forward 25.0)
(set! pwalk-offset-front-position-side 20.0)
(set! pwalk-offset-back-position-forward -25.0)
(set! pwalk-offset-back-position-side 20.0)

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
(set! gofast-turn-calib-rate  1.3)
; how match left applies during align to the beacon
(set! gofast-left-calib-rate  0.2)

(set! gofast-pan-max   50.0)
(set! gofast-pan-speed 0.4)
(set! gofast-tilt-min -10.0)
(set! gofast-tilt-max  15.0)
(set! gofast-tilt-speed 0.4)
; coefficients for control values
(set! gofast-forward-rate 1.0)
(set! gofast-left-rate    1.0)
(set! gofast-turnccw-rate 1.0)

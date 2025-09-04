; SECTION "Grafx fast vars", HRAM
; ; hPlayerAnimIndex: ds 1 
; ; hAnimationFrames: ds 1 
; hFrameCounter: ds 1 

SECTION "Grafx vars", WRAM0
wFadeSettings: ds 1
wFadePos: ds 1 
wTTF: ds 1

wScrollCounter: ds 1 

SECTION "GFX Functions", ROMX

wFadeOutLUT: db $f9, $fe, $ff
wFadeInLUT: db  $fe, $f9, $e4
  
; Fade routine that either fades the palette in or out 
; @param wTTF: Duration before next fade 
; @param wfadeSetting: Sets the fade in (0) or out (1)
; -> improve to see whether variables are necessary, try removing variables and using registers  
Fade: 
  
  ; save interrupt configs
  ldh a, [rIE] 
  ld c, a
  
  ; disable $ save other interrupts and only enable timer for fade effects
  xor a, a    
  ldh [rIE], a 
  ld a, IEF_TIMER 
  ldh [rIE], a 
  
  ; save position for reset later 
  ld a, [wTTF]
  ld b, a  

  ei 
 
fadeCheck:
  ld hl, wTTF 
 
.check
  halt  
  xor a, a 
  cp a, [hl] 
  jr nz, .check 
  
  di 
 
  ld a, [wFadeSettings] 
  rra ; cp a, 1 
  jr c, fadeOut
  
fadeIn:
  
  ld a, 16  
  ld [wTTF], a 
  
  ldh a, [rBGP]  
  cp a, $e4
  jr z, reset 
   
  ld hl, wFadePos 
  ld de, wFadeInLUT
  
  ld a, [hl]   
  add a, e   
  ld e, a  
  
  ld a, [de] 
  ldh [rBGP], a
  ldh [rOBP0], a 
  ldh [rOBP1], a   
   
  inc [hl]  
  
  ei 
  jr fadeCheck
  
fadeOut:  
  ld a, 16
  ld [wTTF], a ; reset timer  
  
  ldh a, [rBGP]
  inc a  
  jr c, reset  
  
  ld hl, wFadePos 
  ld de, wFadeOutLUT
  
  ld a, [hl] 
  add a, e 
  ld e, a 
  
  ld a, [de] 
  ldh [rBGP], a
  ldh [rOBP0], a 
  ldh [rOBP1], a 

  inc [hl] 

  ei 
  jr fadeCheck
  
reset: 
  xor a, a   
  ld [wFadeSettings], a
  ld [wFadePos], a 
  
  ld a, b 
  ld [wTTF], a
  
  ld a, c 
  ldh [rIE], a   
  ret 
  
; Load a cutscene background in. Fades out and turns the LCD off 
; @param de: Start address of the cutscene background 
; @param hl: VRAM address to load cutscene background to
; @param bc: Row # and column # 
loadCutscene:
  ld a, 1 
  ld [wFadeSettings], a 
  
  xor a, a 
  ld [wTitleState], a
  
  push de 
  push hl 
  push bc 
  
  call Fade   
  
  ld a, LCDCF_OFF   
  ldh [rLCDC], a
  
  pop bc 
  pop hl 
  pop de 
  
  call MemCpyOpt

  ret 
  
; Scroll the viewport either up or down a set number of times (if enabled) 
; @param d: dX 
; @param e: dY 
; @param b: Number of times to scroll 
Scroll:  
  push de
.checkScrollCounter:    
   ld a, [wScrollCounter]
   or a, a      
   jr nz, .checkScrollCounter
  pop de 
  
  ld a, e 
  ld hl, rSCY
  add a, [hl]
  ld [hl], a 
  
  ld a, d  
  inc hl 
  add a, [hl]
  ld [hl], a

  ld a, c 
  ld [wScrollCounter], a  
  
  dec b  
  jr nz, Scroll
  
  ld a, c 
  ld [wScrollCounter], a
  
  di 
  
  ret 
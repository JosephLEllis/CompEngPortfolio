SECTION "Misc utils", ROMX 

padRead:
; From Jeff Frohweing gb devrs 
	
    ld a, P1F_5
    ldh [rP1], a  ;turn on P15

    ldh a, [rP1]  ;delay
    ldh a, [rP1]
	cpl
    and a, $0f
	swap a
	ld b, a

    ld a, P1F_4
    ldh [rP1], a     ;turn on P14
    ldh a, [rP1]     ;delay
    ldh a, [rP1]
    ldh a, [rP1]
	cpl
    and a, $0f
	or	b
    ld b, a

	ld a, [_PadData]
	xor	a, b
	and	a, b
    ld	[_PadDataEdge],a
	ld	a, b
	ld	[_PadData],a
    push af

	ld a, P1F_5|P1F_4
    ldh [rP1], a

    ld a, [_PadDataEdge]
    ld b, a
    pop af

	ret

pauseUntilJoyPadPressed: 
  call padRead 
  ld a, [_PadData] 
  cp a, c
  jr nz, pauseUntilJoyPadPressed  
  ld a, [_PadDataEdge]
  cp a, c   
  jr nz, pauseUntilJoyPadPressed    
  ret

LCD_ON:
  ld a, b 
  or a, LCDCF_ON  
  ldh [rLCDC], a
  ret   
  

LCD_OFF:
  call waitVBlank
  ld a, b 
  or a, LCDCF_OFF 
  ldh [rLCDC], a 
  ret   
	
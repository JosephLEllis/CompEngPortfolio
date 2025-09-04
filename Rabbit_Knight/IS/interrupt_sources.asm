SECTION "STAT interrupt handlers", ROM0 
STAT_IH:
   push af 
   push hl
   
   ldh a, [rLYC]
   or a, a 
   jr z, .firstScanLine

.eigthScanLine: 

   ; don't call scanline until scanline 8 
   xor a, a 
   ldh [rLYC], a 

   ; only perform any LCDC operations during HBlank

   ld hl, rSTAT 
   ld a, %00000011
.waitHBlank: 
   and a, [hl] 
   jr nz, .waitHBlank

   ; turn LCDC on with no window
   ld a, LCDCF_ON|LCDCF_BGON|LCDCF_OBJON|LCDCF_OBJ16|LCDCF_WINOFF|LCDCF_WIN9C00
   ldh [rLCDC], a

   jr .finish 

.firstScanLine: 
   ld a, 23
   ldh [rLYC], a

   ld a, LCDCF_ON|LCDCF_BGON|LCDCF_OBJOFF|LCDCF_OBJ16|LCDCF_WINON|LCDCF_WIN9C00
   ldh [rLCDC], a

.finish 
   
   pop hl 
   pop af
 
   reti		
		
SECTION "VBlank interrupts handlers", ROM0 
VBLANK_IH:
    push af 
    push bc 
    push de 
    push hl
    
    call checkPlayerPosRelToCam
    call updatePosition
    call updateAnimations  
    call applyGravity 
    call hDMA
    
    pop hl 
    pop de 
    pop bc 
    pop af
   
    reti    
	
SECTION "Timer interrupt handlers", ROM0 
Timer_IH: 
   push af   
   push hl
   
   ld a, [wTitleState]
   
   add a, a  
   
   ld hl, TIMER_ROUTINE_ADDR_LUT

   add a, l 
   ld l, a

   ld a, [hli] 
   ld l, [hl] 
   ld h, a 

   dec [hl]
   
   pop hl 
   pop af 

   reti 
   
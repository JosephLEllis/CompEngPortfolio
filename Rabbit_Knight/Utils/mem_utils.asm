SECTION "Mem vars", WRAM0 
wCpyTileOffset: ds 1 
wConstListOffset: ds 1 
wOffsetDelay: ds 1 

SECTION "Mem", ROMX 

; TODO: Need better alternative to this - try to use HALT
; Wait for VBlank. 
; @param: N/A
waitVBlank: 
   ldh a, [rLY] 
   cp a, 144 
   jr c, waitVBlank
	ret 
   
; DMA routine that takes object data from shadow OAM to OAM. 
; @param a: HIGH(start address) of shadow OAM 
; NOTE: DMA routine is 10 bytes 
DMA: 
   ld a, HIGH(_ShadowOAM) 
   ldh [rDMA], a 
   ld a, 40 
.wait 
   dec a  
   jr nz, .wait     
   reti 
DMA_End: 

; Write a list of 8-bit values to locations with an optional offset. 
; @param a: offset 
; @param de: source 
; @param hl: destination 
; @param b: length
; @param c: bytes to copy before offset is applied  
WriteList:

   ld [wConstListOffset], a 
   ld a, c 
   ld [wOffsetDelay], a 

.loadConstants   
   ld a, [de]
   ld [hli], a
   dec c 
   jr z, .applyOffset
   inc de 
   dec b 
   jr nz, .loadConstants

   ret 

.applyOffset
   dec b 
   inc de 
   push de 

   ld a, [wConstListOffset]
   ld d, $00 
   ld e, a 
   add hl, de 

   pop de 
   jr .loadConstants 

; Copy bytes from one area to another. 
; @param de: source 
; @param hl: destination 
; @param bc: length
MemCpy2:  
   ld a, [de]
   ld [hli], a 
   inc de 
   dec bc 
   ld a, c
   or a, b 
   jr nz, MemCpy2
   ret 

; temp - experiment with stack pointer 
MemCpy:
   ld a, [hld] 
   ld d, a 
   ld a, [hld] 
   ld e, a 
   push de 
   dec bc 
   ld a, c 
   or a, b 
   jr nz, MemCpy  

   ld sp, $fffc 
   pop hl 
   jp hl 
   
; Byte copy for optimised tilemaps. 
; @param de: source 
; @param hl: destination 
; @param b: row length
; @param c: column depth
; @param a: offset 
MemCpyOpt:
   push bc 
.copyTiles
   ld a, [de] 
   ld [hli], a 
   inc de 
   dec b   
   jr z, .incrementCol
   jr .copyTiles 
   
.incrementCol
   add sp, 2  
   dec c   
   ret z
   
   add sp, -2  

   ld a, [wCpyTileOffset]
   ld b, 0 
   ld c, a 
   add hl, bc 
   
   pop bc 
   dec c 
   jr MemCpyOpt 
   
; Clears all tiles in memory block. 
; @param hl: start 
; @param bc: length    
clearTiles: 
   xor a, a 
   ld [hli], a 
   dec bc 
   ld a, c 
   or a, b    
   jr nz, clearTiles 
   ret  

; Quickly load values to specific area in memory using the stack (with a space of +2 betw. addr.). 
; @param hl: Value to push 
; @param bc: number of times to push (twice in each reg.)
loadUsingStack: 
   push hl 
   inc hl  
   inc hl 
   dec c 
   jr nz, loadUsingStack
   
   sla b ; b*2 

   ld l, b
   ld h, 0 
   
   add hl, sp 
   ld sp, hl 

   ret 
   
  
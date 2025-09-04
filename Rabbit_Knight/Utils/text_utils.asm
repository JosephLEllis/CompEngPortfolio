SECTION "Text vars", WRAM0

wStartCounter: ds 1
wCounterBuffer: ds 1 

SECTION "Charmap", ROM0 

CHARMAP "a", 0 
CHARMAP "b", 1 
CHARMAP "c", 2 
CHARMAP "d", 3 
CHARMAP "e", 4 
CHARMAP "f", 5 
CHARMAP "g", 6 
CHARMAP "h", 7 
CHARMAP "i", 8 
CHARMAP "j", 9 
CHARMAP "k", 10 
CHARMAP "l", 11
CHARMAP "m", 12
CHARMAP "n", 13
CHARMAP "o", 14 
CHARMAP "p", 15 
CHARMAP "q", 16 
CHARMAP "r", 17 
CHARMAP "s", 18 
CHARMAP "t", 19 
CHARMAP "u", 20 
CHARMAP "v", 21 
CHARMAP "w", 22 
CHARMAP "x", 23 
CHARMAP "y", 24 
CHARMAP "z", 25 
CHARMAP "0", 26 
CHARMAP "1", 27
CHARMAP "2", 28 
CHARMAP "3", 29 
CHARMAP "4", 30 
CHARMAP "5", 31 
CHARMAP "6", 32 
CHARMAP "7", 33
CHARMAP "8", 34
CHARMAP "9", 35 
CHARMAP ".", 36 
CHARMAP ",", 37 
CHARMAP ";", 38
CHARMAP ":", 39
CHARMAP "!", 40
CHARMAP " ", 41
CHARMAP "D", 42
CHARMAP "L", 43
CHARMAP "W", 44 
CHARMAP "@", 45 

SECTION "Text functions", ROMX

; Print a line of text 
; @param de: Start address of line of text 
; @param hl: VRAM address to load text to 
; @param c: Text line length 
printLine: 
   ld a, [de]
   ; cp a, 255
   inc a  
   ; ret z
   ret c  
   
   ld [hli], a 
   inc de
   dec c 
   jr nz, printLine
   ret 

; Print a paragraph (3 lines of text)
; @param de: Start address of paragraph 
; @param hl: VRAM address for start of paragraph
; @param c: Paragraph length 
printParagraph:
   push hl 
   
.continue 
   ld a, [de]
   ;cp a, 255
   inc a 
   jr c, .nextLine
    
   ld [hli], a
   inc de    
   dec c 
   jr nz, .continue
   pop hl    
   ret 

.nextLine: 
   pop hl
   push de 
   ld de, $20 
   add hl, de 
   pop de   
   inc de 
   push hl 
   jr .continue 
   
; Blink text a specific number of times and at a set speed 
; WStartCounter = the delay between each blink 
; @param de: Text address  
; @param hl: VRAM address to copy text to 
; @param b: Number of times to repeat the blink 
; @param c: Text length 
blinkText:
   
   ld [wCounterBuffer], a
   
.check 
   halt 
   ld a, [wStartCounter]
   or a, a
   jr nz, .check

   di 

   call waitVBlank 
         
   ld sp, $99e4+((12)-1)

   ld h, d 
   ld l, e 

.test:
   ld a, [hld] 
   ld d, a 
   ld a, [hld] 
   ld e, a 
   push de 
   dec c 
   dec c 
   jr nz, @-7 

   ldh a, [hStartToggle]
   xor a, 1 
   ldh [hStartToggle], a

   dec b 
   jr nz, .notDone 
    
   ei 

   ld sp, $fffa 

   jp titleScreen    

.notDone: 
   ld sp, $fffa  ; adjust to correct later 
   ld c, $0c
   
   ld a, [wCounterBuffer]
   ld [wStartCounter], a

   ldh a, [hStartToggle]

   ei 

   or a, a 
   jr z, .Blank  
   ld de, PS+((12))
   jr .check 
.Blank:
   ld de, Blank+((12)-1)
   jr .check  
  
  
   
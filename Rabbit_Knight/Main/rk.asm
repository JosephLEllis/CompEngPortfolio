; A currently in-progress backport of Rabbit Knight by M36 games! 
; The original game can be found at: https://m36games.itch.io/rabbit-knight

; Util libraries: 
INCLUDE "Utils/gbhw.inc"
INCLUDE "Utils/mem_utils.asm"
INCLUDE "Utils/grafx_utils.asm"
INCLUDE "Utils/text_utils.asm"
INCLUDE "Utils/misc_utils.asm"
INCLUDE "IS/interrupt_sources.asm"

; Math libraries: 
INCLUDE "Math/Math.asm"

; Fonts: 
INCLUDE "Fonts/rk_fonts.asm"

; Title tilesets: 
INCLUDE "Tiles/Title-tiles/rk_title_screen_tileset_first.asm"
INCLUDE "Tiles/Title-tiles/rk_title_screen_tileset_second.asm"

; Intro cutscene tilesets: 
INCLUDE "Tiles/Story-tiles/castle_tiles/rk_story_tiles.asm"
INCLUDE "Tiles/Story-tiles/castle_tiles/rk_castle_tiles.asm"
INCLUDE "Tiles/Story-tiles/mushroom_attack_tileset/mushroom_attack_tiles.asm"
INCLUDE "Tiles/Story-tiles/knight_tileset/knight_tileset.asm"
INCLUDE "Tiles/Story-tiles/rory_tileset/rory_tileset_optimised.asm" 
INCLUDE "Tiles/Story-tiles/rory_face_tileset/rory_face_tileset_optimised.asm"
INCLUDE "Tiles/Story-tiles/rory_face_blink_tileset/rory_face_blink_tileset_optimised.asm"
INCLUDE "Tiles/Story-tiles/rory_det_face_tileset/rory_det_face_tileset_optimised.asm"

; Player (meta) tilesets: 
INCLUDE "Tiles/Rory-tiles/walk_anim/rory_walk_anim.asm"
INCLUDE "Tiles/Rory-tiles/jump_anim/rory_jump_anim.asm"
INCLUDE "Tiles/Rory-tiles/sword_anim/rory_sword_anim.asm"
INCLUDE "Tiles/Rory-tiles/sword_fall_anim/sword_fall_anim.asm"
INCLUDE "Tiles/Rory-tiles/roll_anim/rory_roll_anim.asm"

; Player HUD: 
INCLUDE "HUD/HUD.asm"
INCLUDE "HUD/HUD_SYMBOLS.asm"

; Enemy tilesets: 
INCLUDE "Tiles/Enemy-Tiles/Walking_mushroom/walking_mushroom_anim.asm"

; PowerUp tilesets: 
INCLUDE "Tiles/PowerUp-tiles/PowerUp_anim.asm"

; Stage 1 tilesets for sections: 
INCLUDE "Tiles/Game-tiles/Stage-1/Stage_s1_tiles.asm"

; Background for title screen:
INCLUDE "Backgrounds/Title-screens/title_logo_first.asm"

; Background for intro cutscene:
INCLUDE "Backgrounds/Scenes/Castle/rk_castle.asm"
INCLUDE "Backgrounds/Scenes/Blank/rk_text_box.asm"
INCLUDE "Backgrounds/Scenes/Blank/rk_black_background.asm"
INCLUDE "Backgrounds/Scenes/Mushroom_attack/mushroom_attack_optimised.asm"
INCLUDE "Backgrounds/Scenes/Knights/knights_intro_optimised.asm"
INCLUDE "Backgrounds/Scenes/Rory_intro/rory_intro_optimised.asm"
INCLUDE "Backgrounds/Scenes/Rory_face/horrified/rory_face_optimised.asm"
INCLUDE "Backgrounds/Scenes/Rory_face/blink/rory_face_blink_optimised.asm"
INCLUDE "Backgrounds/Scenes/Rory_face/determined/rory_face_determined_optimised.asm"

; Background for stage 1 sections: 
INCLUDE "Backgrounds/Stage_1/Stage_s1.asm"
INCLUDE "Backgrounds/Stage_1/Stage_s2.asm"

; Collision data: 
INCLUDE "CollisionData/s1s1_COLDATA.asm"

; Audio engine:
INCLUDE "Audio/gbt_player/gbt_player.asm"
INCLUDE "Audio/gbt_player/gbt_player_bank1.asm"

SECTION "HRAM General vars", HRAM

hStartToggle: ds 1 

hPlayerCamPosBufferX: ds 2 

hPlayerSpriteOffset: ds 1

hIsGrounded: ds 1 ; default to true 

hPlayerVelX: ds 2 
hPlayerVelY: ds 2

hPlayerPrevVelX: ds 2 
hPlayerPrevVelY: ds 2 

hPlayerPosBufferX: ds 16 ; (2*8) 
hPlayerPosBufferY: ds 16

hScreenX: ds 1
hHasCrossedThreshold: ds 1

hCurrentState: ds 1 
hNextState: ds 1

hCollisionDetected: ds 1 

; uninitiated for the time being: 
hPlayerDirection: ds 1 

hDMA: ds DMA_End - DMA ; reserve space for DMA routine

SECTION "Animation registry vars", HRAM 

SECTION "VBlank interrupt vector", ROM0[$0040]
    jp VBLANK_IH 
	
SECTION "STAT interrupt vector", ROM0[$0048]
    jp STAT_IH

SECTION "Timer interrupt vector", ROM0[$0050]
    jp Timer_IH
   
SECTION "Constants", ROM0 

PLAYER_NUM_SPRITES db 8

NUM_ANIMATIONS equ 4 

MAX_NUM_OBJECTS equ 5 

NUM_COLS_ANIM_REGISTRY equ 7
NUM_COLS_FRAME_TRACKER equ 6 

SECTION "TIMER LUTS", ROM0 
TIMER_ROUTINE_ADDR_LUT: 
   db HIGH(wTTF), LOW(wTTF) 
   db HIGH(wStartCounter), LOW(wStartCounter)
   db HIGH(wScrollCounter), LOW(wScrollCounter)
    
SECTION "ANIMATION REGISTRY TABLE", ROM0
; Anim ID, addr. + # frames, # sprites, OAMptr (2 bytes), animptr (2 bytes)
AnimationRegistry: 
   db (((idleAnimEnd - idleAnim)-5)/5), 5, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(idleAnim), LOW(idleAnim), 6 ; Idle  
   db (((runAnimEnd - runAnim)-5)/5), 5, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(runAnim), LOW(runAnim), 6 ; Running    
   db (((jumpAnimEnd - jumpAnim)-5)/5), 5, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(jumpAnim), LOW(jumpAnim), 6  ; Jumping 
   db (((swordAnimEnd - swordAnim)-8)/8), 8, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(swordAnim), LOW(swordAnim), 3 ; Sword slash
   db (((swordFallAnimEnd - swordFallAnim)-5)/5), 5, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(swordFallAnim), LOW(swordFallAnim), 6 ; Sword fall
   db (((rollAnimEnd - rollAnim)-8)/8), 8, HIGH(_ShadowOAM), LOW(_ShadowOAM+2), HIGH(rollAnim), LOW(rollAnim), 5 
   db (((floatAnimEnd - floatAnim)-3)/3), 3, HIGH(_ShadowOAM+32), LOW(_ShadowOAM+34), HIGH(floatAnim), LOW(floatAnim), 32 ; PowerUp float anim

SECTION "ANIM OFFSET AND TRACKER TABLES", ROM0 
ANIM_OFFSET_TABLE: 
    db 0*NUM_COLS_ANIM_REGISTRY
    db 1*NUM_COLS_ANIM_REGISTRY
    db 2*NUM_COLS_ANIM_REGISTRY
    db 3*NUM_COLS_ANIM_REGISTRY  
    db 4*NUM_COLS_ANIM_REGISTRY
    db 5*NUM_COLS_ANIM_REGISTRY
    db 6*NUM_COLS_ANIM_REGISTRY

; SECTION "ANIM TRACKER OFFSET TABLE", ROM0 
FRAME_TRACKER_OFFSET_TABLE: 
    db 0*NUM_COLS_FRAME_TRACKER
    db 1*NUM_COLS_FRAME_TRACKER
    db 2*NUM_COLS_FRAME_TRACKER 
    db 3*NUM_COLS_FRAME_TRACKER 
    db 4*NUM_COLS_FRAME_TRACKER
    db 5*NUM_COLS_FRAME_TRACKER

SECTION "FRAME TRACKER TABLE", WRAM0 
FRAME_TRACKER_TABLE:
    ds NUM_COLS_FRAME_TRACKER*NUM_ANIMATIONS
    
; Frame delay counter, current frame, current anim., animDone?, frame counter  

SECTION "Shadow OAM", WRAM0
_ShadowOAM: ds 4*40 ; reserve space for all possible sprites

SECTION "Stack-pointer return addr.", WRAM0 
retAddr: ds 2 

SECTION "General vars", WRAM0
wHealthBarVal: ds 1 
wHealthBarPointer: ds 1 ; for the low byte of the addr. of map 

wScoreValPointer: ds 1 ; for low byte of the addr.
wScytheScorePointer: ds 1 ; for low byte of the addr.

wLevelValPointer: ds 1 ; for low byte of the addr. of map

wCurrentActiveAnims: ds MAX_NUM_OBJECTS

wLevelVal: ds 2 ; represents the data to be loaded in for the HUD (e.g. 1-1)

_PadData:      ds     1
_PadDataEdge:  ds      1  

wScytheScoreVal: ds 1 ; BCD val: 0-9 for each half of the byte 

wScoreVal: ds 4 ; each byte holds BCD val for 2 digits. (pad one byte, only need 3 bytes, but pad for 4) 

wAnimListIndexPointer: ds 1 
   
wLevelLoadPointer: ds 1 ; 

wTitleState: ds 1
wMenu: ds 1

SECTION "Level data addr. table", ROM0

; level addr. table: 

levelMapList: db HIGH(Stage_s1) 
              db LOW(Stage_s1)
              db HIGH(Stage_s2)
              db LOW(Stage_s2)


SECTION  "Text", ROM0 ;--> make a clear text function at some point
; title text 
PS db "press  start", 255 
Blank db "            ", 255

; castle text 
CL1 db "long ago, there was ", 255 
CL2 db "a kingdom run by    ", 255 
CL3 db "rabbits...         @", 255
CL4 db "they lived in peace ", 255 
CL5 db "for a very long time", 255 
CL6 db "...                @", 255

; blank text
BL1 db "...                 ", 255
BL2 db "                    ", 255 
BL3 db "                   @", 255 
BL4 db "then the mushrooms  ", 255 
BL5 db "attacked...         ", 255
BL6 db "                   @", 255  

; mushroom attack text 
MA1 db "they took over the  ", 255
MA2 db "land...             ", 255
MA3 db "                   @", 255  
MA4 db "infecting everyone  ", 255 
MA5 db "they touched...     ", 255
MA6 db "                   @", 255

; knight intro text 
KI1 db "every good knight   ", 255 
KI2 db "and warrior they had", 255 
KI3 db "was killed off one @", 255 
KI4 db "by one...           ", 255
KI5 db "                    ", 255
KI6 db "                   @", 255 

; rory intro text 
RI1 db "only one rabbit     ", 255 
RI2 db "remained. a rookie  ", 255 
RI3 db "rabbit named rory. @", 255 
RI4 db "even though he was  ", 255 
RI5 db "horrified...        ", 255
RI6 db "                   @", 255 
RI7 db "he knew he was the  ", 255 
RI8 db "only one who could  ", 255 
RI9 db "stop them!         @", 255 

SECTION "ROM Entry", ROM0[$100]
    jp initRAM 
	
REPT $150 - $104
    db 0 
ENDR

SECTION "Main", ROM0

initRAM:   

   di 
   
   ld hl, DMA+((DMA_End-DMA)-1) 
   ld c, (DMA_End - DMA) / 2 
   ld sp, hDMA+(DMA_End-DMA)

.DMACpy: 
   ld a, [hld] 
   ld d, a 
   ld a, [hld] 
   ld e, a 
   push de 
   dec c 
   jr nz, .DMACpy 
   
   ld sp, _ShadowOAM+160 
   ld hl, $0000 
   ld c, 80 
   
   ; clearOAM
.clear_OAM: 
   push hl 
   dec c 
   jr nz, .clear_OAM
   
   ; initGameVars 
   ld sp, wMenu+1  
   inc h  
   push hl  
   
   xor a, a 

   ld [wFadePos], a 
   
   ld a, 8 
   ld [wStartCounter], a
    
   ; initSound 
   ld sp, rNR52+1 ; $ff27 

   ld hl, $f1f3 
   push hl 
   ld hl, $22bf  
   push hl 
   ld hl, $77f7
   push hl  
   ld hl, $0fff
   push hl
   
   ; resInterrupts  
   ld sp, rTAC+1 ; $ff07 
   
   ld hl, $0400
   push hl

   ld h, l 
   push hl 

   ld a, IEF_TIMER 
   ldh [rIE], a
   
   ld sp, $fffe ; reset stack pointer 
   
  ; initialise graphics 

   ld b, 0 
   call LCD_OFF


   ld hl, @+16 ; load addr. +16 bytes after this instruction 
   push hl

   ld sp, $9000+(fontTilesEnd-fontTiles) 
   ld hl, fontTiles+((fontTilesEnd-fontTiles)-1) 
   ld bc, (fontTilesEnd-fontTiles)/2
   jp MemCpy 

   ld hl, @+16 
   push hl 

   ld sp, $8800+(rk_title_tileset_p1_end - rk_title_tileset_p1)
   ld hl, rk_title_tileset_p1+((rk_title_tileset_p1_end - rk_title_tileset_p1)-1)
   ld bc, (rk_title_tileset_p1_end - rk_title_tileset_p1)/2
   jp MemCpy 

   ld hl, @+16 
   push hl 

   ld sp, $92e0+(rk_title_tileset_p2_end - rk_title_tileset_p2)
   ld hl, rk_title_tileset_p2+((rk_title_tileset_p2_end - rk_title_tileset_p2)-1)
   ld bc, (rk_title_tileset_p2_end - rk_title_tileset_p2)/2
   jp MemCpy

   ld hl, @+16 
   push hl 

   ld sp, $9580+(rk_castle_tiles_end - rk_castle_tiles)
   ld hl, rk_castle_tiles+((rk_castle_tiles_end - rk_castle_tiles)-1)
   ld bc, (rk_castle_tiles_end - rk_castle_tiles)/2
   jp MemCpy

   ld sp, $fffe ; reset stack pointer 

   ld a, $e4
   ldh [rBGP], a
   ldh [rOBP0], a ; remember ID 0 is treated as transparent for objects 
   
titleScreenSetup:

  jp initGameGrafx ; remove later 

  ld a, $ff 
  ldh [rBGP], a 

  ld de, splashScreen
  ld hl, $9800; load background tilemap in 
  ld bc, $1412
  ld a, 12 
  ld [wCpyTileOffset], a 
  call MemCpyOpt
  
  ld de, textBox 
  ld hl, $9c00 ; load text box in  
  ld bc, $1412 
  ld a, 12  
  ld [wCpyTileOffset], a 
  call MemCpyOpt  
  
  ld b, LCDCF_BGON|LCDCF_BG8800 ; turn background off after fade   
  call LCD_ON
  
  ; fade in here 
  xor a, a  
  ld [wFadeSettings], a
  
  ld [wTitleState], a 

  ld a, 16 ; only have to set once, unless changed 
  ld [wTTF], a   
   
  call Fade  
  
  ld a, IEF_TIMER  
  ldh [rIE], a
  
  ld a, 1
  ld [wTitleState], a 

  ei 
   
titleScreen:
  call padRead
  cp a, $08 
  jr z, Start 
  
  ld de, PS+((12)-1)
  ld a, 1 
  ldh [hStartToggle], a 

  ld a, 8 
  ld [wStartCounter], a 

  ld bc, $020c
  jp blinkText
   
Start:   
  ; Sound setup 
  ld a, %10000000
  ldh [rNR44], a
  
  ld a, 2
  ld [wStartCounter], a
  
  ld de, PS  
  ld hl, $99e4 
  ld bc, $040C
   
  call blinkText 
  
  xor a, a ; turn noise channel off 
  ldh [rNR44], a
  
  jr IntroCutScene
  
IntroCutScene:  
  ; TODO: Do not need to load tiles in so many times - simply load into first two blocks/reduce the number of times to load data in   
   
  ld de, story_castle
  ld hl, $9800 
  ld bc, $2013
  xor a, a 
  ld [wCpyTileOffset], a 
  call loadCutscene
  
  ld a, 7 
  ldh [rWX], a 
  
  ld a, 120
  ldh [rWY], a 
  
  ; Long ago, there was a kingdom run by rabbits...
  ld de, CL1 
  ld hl, $9c00 
  ld c, $3c

  call printParagraph   
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  xor a, a 
  ld [wFadeSettings], a 
  
  call Fade  
   
  ld c, $01  
  call pauseUntilJoyPadPressed  
  
  ; ld a, 1
  ld a, c  
  ld [wScrollCounter], a
  ld c, a 
  
  ;ld a, 2
  inc a   
  ld [wTitleState], a 
  
  ld b, 90 
  
  ei
  nop

  ld de, $0100  
  
  call Scroll 

  ; They lived in peace for a very long time...
  ld de, CL4 
  ld hl, $9c00 
  ld c, $3c
  
  call waitVBlank 
  call printParagraph
   
  ld c, $01 
  call pauseUntilJoyPadPressed  
  
  ld de, rk_black_background
  ld hl, $9800 
  ld bc, $2013
  xor a, a 
  ld [wCpyTileOffset], a 
  call loadCutscene  
  
  xor a, a 
  ldh [rSCX], a 
  
  ; ... 
  ld de, BL1 
  ld hl, $9c00 
  ld c, $3c
  
  call printParagraph   
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  xor a, a 
  ld [wFadeSettings], a
  
  ld [wTitleState], a 
  
  call Fade   
  
  ld c, $01 
  call pauseUntilJoyPadPressed 
  
  ; then the mushrooms attacked... 
  ld de, BL4 
  ld hl, $9c00 
  ld c, $3c 
  
  call waitVBlank
  call printParagraph

  ld c, $01 
  call pauseUntilJoyPadPressed   
  
  ld de, mushroom_attack
  ld hl, $9800 
  ld bc, $2013
  xor a, a 
  ld [wCpyTileOffset], a 
  call loadCutscene
  
  ld de, mushroom_attack_tiles 
  ld hl, $92e0
  ld bc, mushroom_attack_tiles_end - mushroom_attack_tiles
  call MemCpy
  
  ; they took over the land 
  ld de, MA1 
  ld hl, $9c00 
  ld c, $3c
  
  call printParagraph
  
  ld a, 50 
  ldh [rSCX], a  
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON   
  
  xor a, a
  ld [wFadeSettings], a
   
  ld [wTitleState], a

  call Fade   
   
  ld c, $01 
  call pauseUntilJoyPadPressed

  ld a, 1 
  ld [wScrollCounter], a
  ld c, a 
  
  ;ld a, 2
  inc a  
  ld [wTitleState], a 
  
  ld b, 50 
  
  ei 
  nop 

  ld de, $ff00  
  
  call Scroll 
  
  ; infecting everyone they touched  
  ld de, MA4 
  ld hl, $9c00 
  ld c, $3c
  
  call waitVBlank
  call printParagraph
  
  ld c, $01 
  call pauseUntilJoyPadPressed
  
  ld de, knights_intro  
  ld hl, $9800 
  ld bc, $1412
  ld a, 12  
  ld [wCpyTileOffset], a 
  call loadCutscene
  
  ld de, knight_tileset 
  ld hl, $92e0
  ld bc, knight_tileset_end - knight_tileset
  call MemCpy
  
  ; every good knight and warrior they had was killed off one...  
  ld de, KI1  
  ld hl, $9c00  
  ld c, $3c
  
  call printParagraph
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  xor a, a 
  ld [wTitleState], a 
  
  ld [wFadeSettings], a
  
  call Fade
  
  ld c, $01 
  call pauseUntilJoyPadPressed

  ; by one...  
  ld de, KI4  
  ld hl, $9c00  
  ld c, $3c
  
  call waitVBlank
  call printParagraph

  ld c, $01 
  call pauseUntilJoyPadPressed 
  
  ld de, rory_intro
  ld hl, $9800 
  ld bc, $1412
  ld a, 12 
  ld [wCpyTileOffset], a
  call loadCutscene   
  
  ld de, rory_tileset
  ld hl, $92e0
  ld bc, rory_tileset_end - rory_tileset
  call MemCpy
  
  ; only one rabbit remained. a rookie rabbit named rory.   
  ld de, RI1   
  ld hl, $9c00  
  ld c, $3c
  
  call printParagraph
  
  ld a, 10 
  ldh [rSCY], a

  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00     
  call LCD_ON
  
  xor a, a 
  ld [wFadeSettings], a 
  
  call Fade
  
  ei 
  nop  

  ld de, $00ff 
  ld b, 10 
      
  call Scroll 

  ld c, $01 
  call pauseUntilJoyPadPressed

  ld a, 1 
  ld [wFadeSettings], a 
 
  xor a, a 
  ld [wTitleState], a 

  call Fade

  ld de, rory_face   
  ld hl, $9800 
  ld bc, $1412
  ld a, 12    
  ld [wCpyTileOffset], a 
  call loadCutscene
  
  ld de, rory_face_tileset
  ld hl, $92e0
  ld bc, rory_face_tileset_end - rory_face_tileset
  call MemCpy
  
  ; even though he was horrified...   
  ld de, RI4   
  ld hl, $9c00  
  ld c, $3c
  
  call printParagraph
    
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  xor a, a 
  ld [wFadeSettings], a 
 
  call Fade 
  
  ld c, $01 
  call pauseUntilJoyPadPressed
  
  call LCD_OFF 
  
  ld de, rory_face_bl_tileset 
  ld hl, $92e0 
  ld bc, rory_face_bl_tileset_end - rory_face_bl_tileset
  
  call MemCpy
  
  ld de, rory_face_blink
  ld hl, $9800; load background tilemap in 
  ld bc, $1412
  ld a, 12 
  ld [wCpyTileOffset], a 
  
  call MemCpyOpt
  
  ; ... 
  ld de, BL1 
  ld hl, $9c00 
  ld c, $3c
  
  call printParagraph
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  ; play sound here 
  
  ld c, $01 
  call pauseUntilJoyPadPressed
  
  call LCD_OFF
  
  ld de, rory_det_tileset
  ld hl, $92e0 
  ld bc, rory_det_tileset_end - rory_det_tileset
  
  call MemCpy
  
  ld de, rory_face_determined  
  ld hl, $9800 
  ld bc, $1412
  ld a, 12    
  ld [wCpyTileOffset], a 
  
  call MemCpyOpt

   ; he knew he was the only one who could stop them!    
  ld de, RI7   
  ld hl, $9c00  
  ld c, $3c
  
  call printParagraph     
  
  ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_WINON|LCDCF_WIN9C00    
  call LCD_ON
  
  ; play sound here 
  
  ld c, $01 
  call pauseUntilJoyPadPressed
    
  ld a, 1 
  ld [wFadeSettings], a 
 
  xor a, a 
  ld [wTitleState], a 
  
  call Fade
  
  jp initGameGrafx

SECTION "Main game", ROMX, BANK[1] 

SECTION "Player FSM state table", ROMX, BANK[1]

tPlayerStateTable: 
   dw Idle 
   dw Running 
   dw Rolling 
   dw Slashing 
   dw Jumping 
   dw inAir 
   dw SwordFall 

SECTION "PlayerObj", ROMX, BANK[1]

player_obj_data: 

; rory sprites 
DB  108, 8, 0, 0
DB  108, 16, 2, 0 
DB  124, 8, 4, 0 
DB  124, 16, 6, 0

; sword sprites 
DB  108, 0, $1c, 0

; sword "flash" sprites
DB  108, 24, $30, 0
DB  124, 24, $30, 0
DB  108, 32, $30, 0 

player_obj_data_end: 

SECTION "PowerUpObj", ROMX, BANK[1]

powerup_obj_data:

   DB 92, 41, $82, 0 
   DB 92, 49, $80, 0 
   DB 92, 57, $84, 0

powerup_obj_data_end:

SECTION "PowerUp animation frames", ROMX, BANK[1]

floatAnim: 
   db  $82, $80, $84 
   db $88, $86, $8a
floatAnimEnd: 

SECTION "Player animation frames", ROMX, BANK[1]

idleAnim: db 0, 2, 4, 6, 20
idleAnimEnd: 

runAnim: db 8, 10, 14, 12, 22   
         db 0, 2, 14, 12, 20      
         db 16, 18, 4, 6, 20
         db 0, 2, 4, 6, 20   
runAnimEnd:  

jumpAnim: db 24, 26, 28, 30, 32
jumpAnimEnd:  

swordAnim: db 34, 36, 38, 40, 50, 48, 48, 48  
           db 42, 44, 38, 46, 48, 52, 54, 56 
           db 42, 44, 38, 46, 48, 58, 60, 48 
           db 0, 2, 4, 6, 48, 48, 48, 48
swordAnimEnd:  
           
swordFallAnim: db 62, 64, 66, 68, 48 
swordFallAnimEnd: 

rollAnim: db 70, 72, 74, 76, 48, 48, 48, 48 
          db 78, 80, 82, 84, 48, 48, 48, 48 
          db 88, 86, 90, 92, 48, 48, 48, 48  
          db 96, 94, 48, 48, 48, 98, 48, 48  
          db 100, 102, 48, 48, 48, 48, 48, 48 
          db 106, 104, 48, 48, 48, 108, 48, 48
          db 88, 86, 90, 92, 48, 48, 48, 48 
          db 78, 80, 82, 84, 48, 48, 48, 48
          db 70, 72, 74, 76, 48, 48, 48, 48  
rollAnimEnd: 

SECTION "HUD constants", ROMX, BANK[1]

ScytheIcon: db $2e, $30, $2f, $31 
HealthBarIcon: db $32, $33, $34, $35, $36
StartScore: db $1a, $1a, $1a, $1a, $1a, $1a
ScytheStartScore: db $1a, $1a 
LevelIndicatorData: db $1b, $1b

updatePosition: 

   ; ldh a, [hCurrentState]
   ; or a, a 
   ; ret z

   ; if camera scrolling, do not move position 
   ldh a, [hHasCrossedThreshold] 
   rra 
   jr c, .playerCrossedThreshold 

   ; playerPos.x += playerVelocity.x 
   
   ld hl, hPlayerVelX 
   ld a, [hli] 
   ld b, a  
   ld c, [hl]
   jr .cont 

.playerCrossedThreshold 
   ld bc, $0000  

.cont 
   ld l, LOW(hPlayerPosBufferX)
   call addFixedPoint

   ld l, LOW(hPlayerPosBufferX+2)   
   call addFixedPoint

   ld l, LOW(hPlayerPosBufferX+8)
   call addFixedPoint 

   ldh a, [hPlayerPosBufferX]
   ldh [hPlayerPosBufferX+4], a

   ldh a, [hPlayerPosBufferX+2]
   ldh [hPlayerPosBufferX+6], a

   ; playerPos.x += playerVelocity.x 
   
   ld hl, _ShadowOAM+1 
   ldh a, [hPlayerPosBufferX]  
   ld [hl], a 
   ld l, 9 
   ld [hl], a  

   ld l, 5 
   ldh a, [hPlayerPosBufferX+2] 
   ld [hl], a 
   ld l, 13 
   ld [hl], a 
   
   ld l, 17 
   ldh a, [hPlayerPosBufferX+8] 
   ld [hl], a 

   ld l, 21 
   ldh a, [hPlayerPosBufferX+10]
   ld [hl], a

   ; playerPos.y += playerVelocity.y
   
   ; keep the same for the time being - change when proper collision detection implemented
   ; ldh a, [hCollisionDetected] 
   ; rra 
   ; jr nc, .noCollisionDetected

   ; ld bc, $ff54 
   ; jr .contY

; .noCollisionDetected
   ld hl, hPlayerVelY
   ld a, [hli]
   ld b, a  
   ld c, [hl] 

; .contY: 
   ld l, LOW(hPlayerPosBufferY)
   call addFixedPoint

   ldh a, [hPlayerPosBufferY]
   ldh [hPlayerPosBufferY+2], a 
   ldh [hPlayerPosBufferY+8], a

   ld l, LOW(hPlayerPosBufferY+4)
   call addFixedPoint

   ldh a, [hPlayerPosBufferY+4]
   ldh [hPlayerPosBufferY+6], a

   ld bc, $0004

   ld hl, _ShadowOAM
   ldh a, [hPlayerPosBufferY]
   ld [hl], a 

   add hl, bc ; 4
   ldh a, [hPlayerPosBufferY+2]
   ld [hl], a 

   add hl, bc ; 8 
   ldh a, [hPlayerPosBufferY+4]
   ld [hl], a 
   
   add hl, bc ; 12 
   ldh a, [hPlayerPosBufferY+6]
   ld [hl], a

   add hl, bc ; 16 
   ldh a, [hPlayerPosBufferY+8]
   ld [hl], a 

   ret 

applyGravity: 

   ; temp snap to grid 
   ld a, [_ShadowOAM]
   cp a, $6C 
   jr nc, .snapToGrid
  
   ; subtract -0.67px from player Y velocity: 
   ld hl, hPlayerVelY
   ld bc, $00ab ; 0.67 
   jp addFixedPoint

; temp 
.snapToGrid:

   ; ld a, 1 
   ; ldh [hIsGrounded], a 

   xor a, a 
   ldh [hPlayerVelX], a 
   ldh [hPlayerVelX+1], a 

   xor a, a 
   ldh [hPlayerVelY], a
   ldh [hPlayerVelY+1], a  

   ld hl, _ShadowOAM
   ld [hl], $6c 
   ld l, 4 
   ld [hl], $6c 
   ld l, 8 
   ld [hl], $7c 
   ld l, 12 
   ld [hl], $7c 
   ld l, 16 
   ld [hl], $6c
   
   ld hl, hPlayerPosBufferY
   xor a, a 
   ld [hl], $6c
   inc l 
   ld [hli], a 

   ld [hl], $6c 
   inc l
   ld [hli], a 

   ld [hl], $7c 
   inc l 
   ld [hli], a  

   ld [hl], $7c 
   inc l 
   ld [hli], a 

   ld [hl], $6c 
   inc l 
   ld [hl], a 

   ret 
   
initGameGrafx: 

   di 

   ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_OBJ16|LCDCF_OBJON|LCDCF_WINON|LCDCF_WIN9C00
   call LCD_ON

   ld b, 0 
   call LCD_OFF
   
   ; Load test section in for now: 
   ld de, Stage_s1_tiles
   ld hl, $93e0
   ld bc, Stage_s1_tiles_end -  Stage_s1_tiles   
   call MemCpy2
   
   ; load map in
   ld de, Stage_s1  
   ld hl, $9800 
   ld bc, $1412
   ld a, 12
   ld [wCpyTileOffset], a 
   call MemCpyOpt

   ; load the HUD background in
   ld de, HUD 
   ld hl, $9c00
   ld bc, $1403 
   call MemCpyOpt

   ld de, HUD_SYMBOLS 
   ld hl, $92e0 
   ld bc, HUD_SYMBOLS_END - HUD_SYMBOLS
   call MemCpy2
   
   ; add symbols to HUD: 
   ld de, HealthBarIcon
   ld hl, $9c07 
   ld bc, $0005 
   call MemCpy2

   ld a, 30 
   ld de, ScytheIcon
   ld hl, $9c0d 
   ld bc, $0402
   call WriteList  
   
   ; add score symbols to HUD
   ld de, StartScore
   ld hl, $9c46 
   ld bc, $0006 
   call MemCpy2

   ld de, ScytheStartScore
   ld hl, $9c4d 
   ld bc, $0002 
   call MemCpy2

   ; load sprite tilsets in: 
   ld de, rory_walk_sprite
   ld hl, $8000
   ld bc, rory_walk_sprite_end - rory_walk_sprite
   call MemCpy2

   ld de, rory_jump_sprite
   ld hl, $8000 + (rory_walk_sprite_end - rory_walk_sprite)
   ld bc, rory_jump_sprite_end - rory_jump_sprite
   call MemCpy2

   ld de, rory_sword_sprite
   ld hl, $8000 + (rory_walk_sprite_end - rory_walk_sprite) + (rory_jump_sprite_end - rory_jump_sprite) ; replace this with something more sensible later
   ld bc, rory_sword_sprite_end - rory_sword_sprite 
   call MemCpy2

   ld de, rory_fall_sprite 
   ld hl, $8000 + (rory_walk_sprite_end - rory_walk_sprite) + (rory_jump_sprite_end - rory_jump_sprite) + (rory_sword_sprite_end - rory_sword_sprite)
   ld bc, rory_fall_sprite_end - rory_fall_sprite 
   call MemCpy2

   ld de, rory_roll_sprite
   ld hl, $8460 ; + (rory_walk_sprite_end - rory_walk_sprite) + (rory_jump_sprite_end - rory_jump_sprite) + (rory_sword_sprite_end - rory_sword_sprite) + (rory_face_bl_tileset_end - rory_fall_sprite)
   ld bc, rory_roll_sprite_end - rory_roll_sprite 
   call MemCpy2

   ; load sprite settings into shadow OAM: 
   ld de, player_obj_data
   ld hl, _ShadowOAM
   ld bc, player_obj_data_end - player_obj_data
   call MemCpy2 ; change later

   ; Load enemy tileset data: 
   ld de, WalkingMushroomTilesStart
   ld hl, $86f0
   ld bc, WalkingMushroomTilesEnd - WalkingMushroomTilesStart
   call MemCpy2
   
   ; Load powerup tileset data:
   ld de, ScytheDispStart
   ld hl, $8800 
   ld bc, ScytheDispEnd - ScytheDispStart
   call MemCpy2

   ld de, powerup_obj_data
   ld hl, _ShadowOAM+32 
   ld bc, powerup_obj_data_end - powerup_obj_data
   call MemCpy2
   
   ; enable VBlank and LCD interrupts: 
   ld a, IEF_VBLANK|IEF_LCDC ; |IEF_TIMER
   ldh [rIE], a

   ld hl, $0000
   
   ld c, l 
   ld b, l 

   ld e, l 
   ld d, l 

   ld a, l 
   
   ld sp, wAnimListIndexPointer+1
   push hl ; wAnimListIndexPointer, wScoreVal 
   push hl ; wScoreVal+1, wScoreVal+2 
   push hl ; wScoreVal+3, wScytheScoreVal  
   push hl ; _PadDataEdge, _PadData 
   
   inc c  
   ld b, c  

   push bc ; wLevelVal, wLevelVal+1
   
   ; ld hl, $ffff
   dec h  
   dec l 
   push hl
   push hl  
   ld l, $52   
   push hl 

   ld hl, $4e4b
   push hl 
   ld hl, $0b02  
   push hl  

   ld sp, hNextState+1  
   ld l, e 
   ld h, e  
   push hl  
   push hl  
   push hl  
   
   push hl 
   push hl  
   push hl 
   push hl  

   push hl  
   push hl  
   push hl  
   push hl  

   push hl  
   push hl 
   push hl 
   push hl 

   push hl 
   push hl 
   push hl 
   push hl 

   ld hl, $ab01 
   push hl
   
   ; dec l 
   ; ld h, l 
   push hl 
   
   ; ld l, $f8 
   push hl 

   ld hl, $01f8 
   push hl 

   push de ; hCameraPosBufferX 
   push de  

   ldh [rLYC], a 

   ld a, $08  
   ldh [hPlayerSpriteOffset], a  

   dec a 
   ldh [rWX], a
   
   ; hardcode for now
   ld sp, hHasCrossedThreshold-2
   
   ld hl, $6c00 
   push hl 

   ld h, $7c 
   push hl 

   ld h, $6c 
   push hl 
   push hl 

   ld h, $7c 
   push hl 
   push hl 

   ld h, $6c
   push hl 
   push hl 

   ld hl, $2000 
   push hl

   ld h, $18 
   push hl 
   push hl

   ld h, l 
   push hl 

   ld h, $10
   push hl 

   ld h, $08 
   push hl 

   ld h, $10 
   push hl 

   ld h, $08 
   push hl 
   
   ld sp, $c0f2  
   ld hl, $00ff  

   push hl 

   ld l, h  
   push hl 
   push hl  
   
   inc h 
   dec l 
   push hl 

   dec h  
   ld l, h  
   push hl  
   push hl  
   
   inc h  
   dec l  
   push hl  

   dec h  
   ld l, h  
   push hl  
   push hl 

   ld a, STATF_LYC
   ldh [rSTAT], a

   ; temp 
   ld a, 1  
   ldh [hPlayerDirection], a 
   ldh [hCollisionDetected], a 

   ; also temp 

   ; load animation lookup addresses to the stack in WRAM. Start at $c119. 
   ; Return addr. is $fffc. 
   ; look at this later

   ; change sp vals later.
   ld sp, $c13b
   ld bc, $0f0f  

   ld hl, FRAME_TRACKER_TABLE+1 
   call loadUsingStack
   
   ld sp, $c165  
   ld hl, AnimationRegistry+1 
   ld bc, $1515 
   call loadUsingStack
   
   ld sp, $fffc  
   
   ld b, LCDCF_BGON|LCDCF_BG9800|LCDCF_OBJ16|LCDCF_OBJON|LCDCF_WINON|LCDCF_WIN9C00
   call LCD_ON

   ei     

   jp playLoop 

updateAnimations: 

   xor a, a
   ld [wAnimListIndexPointer], a 

   ld [retAddr], sp 
   
.iterate: 
   ld hl, wAnimListIndexPointer
   ld c, [hl]

   ld hl, wCurrentActiveAnims 

   ld b, 0 
   add hl, bc

   ld a, $ff  
   cp a, [hl] 
   ret z  

   ; ld d, [hl] 

   ; ld hl, sp+0 
   ; ld sp, $c165 
   ; push hl ; $c163
   
   ; ld sp, $c161

   ; ld hl, FRAME_TRACKER_OFFSET_TABLE ; 3, 2  
   ; add hl, bc ; 2, 1 
   ; ld l, [hl] ; 2, 1 
   ; add hl, sp ; 2, 1 
   ; ld sp, hl ; 2, 1 

   ; add sp, -4 
   ; pop hl 
   ; ld a, [hl] 

   ; cp a, d 
   ; jr nz, .startTableLookup 

   ; dec l 
   ; dec l 
   ; dec l 

   ; add sp, -6 
   ; pop hl 
   ; inc l 
   ; ld a, [hl] 
   
   ; cp a, [hl] 
   ; jr nz, .notReadyToUpdate

   
   ld e, c

   add sp, -2 

   ; current active animations list:
   push hl ; hl = wCurrentActiveAnims

   ld d, [hl] ; get current active anim. ID
   
   ; frame tracker stack: (2, 4, 6)

   ld hl, FRAME_TRACKER_OFFSET_TABLE

   add hl, bc 
   ld c, [hl] 

   ld hl, FRAME_TRACKER_TABLE
   add hl, bc 
   
   inc l ; c0e2  
   push hl 

   inc l ; c0e3  

   inc l ; c0e4
   push hl 

   inc l 

   inc l 
   push hl ; c0e6 
   
   ; anim offset stack:

   ld hl, ANIM_OFFSET_TABLE

   ld c, d 
   add hl, bc 
   ld c, [hl]

   ld hl, AnimationRegistry ; AnimationRegistry (1, 3, 5)
   add hl, bc 

   inc l 
   push hl ; 0001

   inc l 

   inc l 
   push hl ; 0003 

   inc l 

   inc l 
   push hl ; 0005 

   ; check for changes in animation list first 

   inc l 
   ld b, [hl]
   
   ; compare to see if table lookup needs to be initiated: 

   add sp, 8 
   pop hl 

   ld a, [hl]
   cp a, d ; compare anim ID to see if a new lookup is needed 
   jr nz, .startTableLookup

   dec l 
   dec l 
   dec l 

   ld a, b 
   cp a, [hl] 
   jp nz, .notReadyToUpdate

   add sp, 2 
   
   ld a, l 
   add a, 4 
   ld l, a 
   
   ld [hl], 0 ; set animDone? to false 
   jr .cont 

.startTableLookup:
   ; Frame delay counter, current frame, current frame low, current anim., animDone?, frame counter
   ; c0e1,                c0e2,          c0e3,              c0e4,          c0e5,      c0e6   
   ld [hl], d
   inc l 
   ld [hl], 0 
   
   ld c, e ; wAnimListIndexPointer

   ; AnimationRegistry+5
   add sp, -10   
   pop hl  
   ld e, [hl]
   dec l 
   ld d, [hl]  

   add sp, 8   
   pop hl 

   ld [hl], e ; FRAME_TRACKER_TABLE+1
   inc l 
   ld [hl], d ; FRAME_TRACKER_TABLE+2
   
   dec l 
   dec l 
   ld [hl], 0 ; FRAME_TRACKER_TABLE

   ld a, l 
   add a, 4 
   ld l, a 
   
   xor a, a 
   ld [hli], a ; FRAME_TRACKER_TABLE+6
   ld [hl], a
 
.cont: 
   add sp, -8   
   pop hl  
 
   ld b, [hl] ; # sprites for anim.  
   ld c, $04 

   inc l 
   ld d, [hl]  
   inc l 
   ld e, [hl]  
   
   add sp, -6   
   push de ; OAM ptr  

   ; Anim ID, addr. + # frames, # sprites, OAMptr HIGH, OAMptr LOW, animptr (2 bytes), frame delay
   ; Frame delay counter, current frame low, current frame high, current anim., animDone?, frame counter 
   
   add sp, 12  
   pop hl 
   ld e, [hl] ; FRAME_TRACKER_TABLE+1
   inc l 
   ld d, [hl] ; FRAME_TRACKER_TABLE+2  
  
   add sp, -14   
   pop hl   

.updateSpriteTilesForAnimations
   ld a, [de] 
   ld [hl], a 
   inc de 
   
   ld a, l 
   add a, c 
   ld l, a
   
   dec b 
   jr nz, .updateSpriteTilesForAnimations

   ; FRAME_TRACKER_TABLE+1
   add sp, 10  
   pop hl 
   ld [hl], e 
   inc l 
   ld [hl], d 
  
   ; FRAME_TRACKER_TABLE+6
   
   inc l 
   inc l 
   inc l 
   
   ld a, [hl] ; frame counter 

   add sp, -8  
   pop hl 
   dec l 

   cp a, [hl] ; compare current frame to # frames in anim. 
   jr z, .setAnimDone

   pop hl 
   inc [hl] ; increment frame counter

.nextAnim: 
   add sp, 2  
   pop hl 
   dec l 
   ld [hl], 0 ; FRAME_TRACKER_TABLE

   ld hl, wAnimListIndexPointer
   inc [hl] ; increment wAnimListIndexPointer

   ld sp, retAddr 
   pop hl 
   ld sp, hl 

   jp .iterate 

.setAnimDone:
   add sp, -6  
   pop hl ; AnimationRegistry+5
   ld c, [hl]
   dec l 
   ld b, [hl]  

   add sp, 8 
   pop hl ; FRAME_TRACKER_TABLE+1
   dec l 

   xor a, a 
   ld [hli], a ; FRAME_TRACKER_TABLE 
   ld [hl], c ; FRAME_TRACKER_TABLE+1
   inc l 
   ld [hl], b 
   inc l 
   inc l 
   inc [hl] 
   inc l 
   ld [hl], 0 

   ld hl, wAnimListIndexPointer
   inc [hl] 

   ld sp, retAddr 
   pop hl
   ld sp, hl 

   jp .iterate

.notReadyToUpdate:    
   ; FRAME_TRACKER_TABLE+1 
   inc [hl]

   ld hl, wAnimListIndexPointer
   inc [hl] 

   ld sp, retAddr   
   pop hl 
   ld sp, hl 

   jp .iterate 

transitionLogicFSM: 
   ; ld hl, tPlayerStateTable
   ; ldh a, [hCurrentState] 
   ; ld e, a 
   ; ld d, 0 
   ; add hl, de 
   ; add hl, de 
   ; ld a, [hli] 
   ; ld h, [hl] 
   ; ld l, a 
   ; jp hl 
   ; 19 cycles, 14 bytes  

   ld hl, tPlayerStateTable
   ldh a, [hCurrentState]
   add a, a 
   add a, l 
   ld l, a 
   ld a, [hli] 
   ld h, [hl] 
   ld l, a 
   jp hl 
   ; 15 cycles, 12 bytes 

updateFSM: 
   
   ldh a, [hNextState]
   ldh [hCurrentState], a
   
   ret 

Idle:

   ; State logic here: 
   xor a, a  
   ld hl, hPlayerVelX 
   ld [hli], a 
   ld [hli], a  
   ld [hli], a 
   ld [hli], a
   
   call padRead
    
   ld b, a  

   and a, %00110000 ; (Left or Right) pressed 
   jp nz, NextStateRunning

   ld a, b 

   dec a ; A button pressed 
   jp z, NextStateJumping 

   ld a, b 
   
   and a, %00000010 ; B pressed 
   jp nz, NextStateSlashing

   ld a, b 

   and a, %10000000 ; Down pressed
   jp nz, NextStateRolling 

   jp NextStateIdle

NextStateIdle:
   
   ld hl, wCurrentActiveAnims 
   xor a, a 
   ld [hli], a
   
   ; ldh [hPlayerDirection], a 
   
   ; test 
   ; inc l 
   ld [hl], 6
   ; test  

   ldh [hNextState], a ; Idle state
   
   ret

NextStateRunning:

   ld hl, wCurrentActiveAnims
   ld a, 1 
   ld [hl], a 
  
   ldh [hNextState], a ; Running state 
    
   ret

NextStateRolling:

   ld hl, wCurrentActiveAnims
   ld [hl], 5 

   ld a, 2 
   ldh [hNextState], a ; Rolling state
   
   ; ld a, 3 
   ; ldh [hPlayerDirection], a 

   ret 

NextStateSlashing:

   ld hl, wCurrentActiveAnims 
   ld a, 3 
   ld [hl], a 

   ldh [hNextState], a ; Slashing state

   ret 

NextStateJumping:

   ld hl, wCurrentActiveAnims
   ld [hl], 2 

   ld a, 4 
   ldh [hNextState], a ; Jumping state
   
   ; dec a   
   ; ldh [hPlayerDirection], a 

   ret 

NextStateInAir: 

   ld a, 5 
   ldh [hNextState], a 

   ret 

NextStateSwordFall: 

   ld hl, wCurrentActiveAnims
   ld [hl], 4 

   ld a, 6 
   ldh [hNextState], a 

   ret 
   
Running: 

   ; State logic here:
   ld a, [_PadData]

   bit 4, a ; R pressed 
   jr nz, .PadRightPressed 

   xor a, a  
   ldh [hPlayerDirection], a 

   ld a, $fe 
   ldh [hPlayerVelX], a 
   ldh [hPlayerPrevVelX], a 
   
   ld a, $55 
   ldh [hPlayerVelX+1], a
   ldh [hPlayerPrevVelX+1], a 
   
   ld a, -8     
   ld e, 8 
   ldh [hPlayerSpriteOffset], a 

   ld a, 32  ; L pressed
   jr .flip 

.PadRightPressed:
   ld a, $01 

   ldh [hPlayerDirection], a 

   ldh [hPlayerVelX], a
   ldh [hPlayerPrevVelX], a 

   ld a, $ab 
   ldh [hPlayerVelX+1], a
   ldh [hPlayerPrevVelX+1], a 
   
   ld a, 8  
   ld e, -8 
   ldh [hPlayerSpriteOffset], a 
   
   xor a, a

.flip 
   ld hl, _ShadowOAM+3 
   ld bc, $0004

   ld [hl], a
   add hl, bc ; _ShadowOAM+7
   ld [hl], a 
   add hl, bc ; _ShadowOAM+12 
   ld [hl], a 
   add hl, bc ; _ShadowOAM+15 
   ld [hl], a 
   add hl, bc ; _ShadowOAM+19 
   ld [hl], a 
   add hl, bc ; _ShadowOAM+23
   ld [hl], a 
   add hl, bc ; _ShadowOAM+27
   ld [hl], a 
   add hl, bc ; ShadowOAM+31
   ld [hl], a 

   ldh a, [hPlayerPosBufferX+2]   
   
   add a, e 
   ldh [hPlayerPosBufferX], a   

   ldh a, [hPlayerPosBufferX+6]
  
   add a, e 
   ldh [hPlayerPosBufferX+4], a  

   ; sword sprite 
   ldh a, [hPlayerPosBufferX] 
   add a, e 
   ldh [hPlayerPosBufferX+8], a 

.checkNextState:

   ; see if this can be a LUT for states 

   call padRead 

   ld b, a 

   or a, a 
   jp z, NextStateIdle

   rra 
   jp c, NextStateJumping 

   ld a, b 

   and a, %00110000
   jp nz, NextStateRunning

   ld a, b 

   rla ; Down pressed 
   jp c, NextStateRolling

   jp NextStateIdle 

Rolling: 
   ; State logic here:
   ; end of state logic 
    
   ldh a, [hPlayerPosBufferX] 
   ld e, a

   ld hl, hPlayerSpriteOffset
   ld a, [hli] 
   ld b, a 
   cpl 
   inc a 
   ld d, a   

   ld c, 1  

   ld a, 5 
   ld hl, FRAME_TRACKER_TABLE+3

.waitLoaded
   cp a, [hl] 
   jr nz, .waitLoaded

   inc l ; FRAME_TRACKER_TABLE+4 

; Adjust sprite position so that rory rolls on the floor
.wait:
    
   ld a, e  
 
   add a, b 
   add a, b 
   ld [_ShadowOAM+21], a 
    
   add a, b 
   ld [_ShadowOAM+29], a   

   sub a, b 
   ld [_ShadowOAM+25], a   

   ldh a, [hPlayerPrevVelX] 
   ldh [hPlayerVelX], a 
   ldh a, [hPlayerPrevVelX+1]  
   ldh [hPlayerVelX+1], a 
   
   ldh a, [hPlayerPosBufferX+2]
   
   add a, d 
   ldh [hPlayerPosBufferX], a 

   ldh a, [hPlayerPosBufferX+6]
  
   add a, d 
   ldh [hPlayerPosBufferX+4], a

   ldh a, [hPlayerPosBufferX+2]
   sub a, d 
   ldh [hPlayerPosBufferX+10], a 
   
   ld a, c  
   cp a, [hl] 
   jr nz, .wait 
   
   jp NextStateIdle

Slashing:
   ; State logic here:
   ldh a, [hPlayerSpriteOffset]   
   ld b, a 
 
   ldh a, [hPlayerPosBufferX]   
 
   add a, b 
   add a, b  
   ldh [hPlayerPosBufferX+10], a  
    
   add a, b 
   ld [_ShadowOAM+29], a   

   sub a, b 
   ld [_ShadowOAM+25], a
   ; end of state logic

   ld hl, FRAME_TRACKER_TABLE+4 
   ld a, 1 
 
.wait: 
   cp a, [hl] 
   jr nz, .wait  

.cont: 
   call padRead
   ld b, a 

   and a, %10110001  
   jp nz, NextStateIdle 

   ld a, b 

   or a, a 
   jp z, NextStateIdle 

   and a, %00000010 ; B pressed 
   jp nz, NextStateSlashing

   jp NextStateSlashing
   
Jumping:
   
   ; State logic here: 
   xor a, a 
   ldh [hIsGrounded], a

   ldh [hPlayerVelY+1], a  ; 3 
   ld a, -9 ; 2 
   ldh [hPlayerVelY], a ; 3 
   ; end of state logic

   jp NextStateInAir 
   
inAir:
    
   ldh a, [hIsGrounded]
   rra 
   jp c, NextStateIdle
   
   ; If in air and pressing arrow keys, then move left or right
   call padRead 

   ld b, a 
   
   and a, %00010000
   jr nz, .RightDPadPressed 

   ld a, b 
   
   and a, %00100000 
   jr nz, .LeftDPadPressed

   ld a, b 

   ; sword fall - press down in air 
   and a, %10000000
   jp nz, NextStateSwordFall

   jp NextStateInAir

.LeftDPadPressed:
   ld a, $fd 
   ldh [hPlayerVelX], a   
   ld a, $53 
   ldh [hPlayerVelX+1], a 

   xor a, a 
   ldh [hPlayerDirection], a

   jp NextStateInAir
   
.RightDPadPressed:
   ld a, $02 
   ldh [hPlayerVelX], a

   dec a  
   ldh [hPlayerDirection], a 

   ld a, $ad 
   ldh [hPlayerVelX+1], a
   
   jp NextStateInAir 

SwordFall:
   ldh a, [hIsGrounded]
   rra 
   jp c, NextStateIdle

   jp NextStateSwordFall

; Note: Move player sprite only when threshold has not been reached. Otherwise, do not move player sprite. 

checkPlayerPosRelToCam:

   ; better - see if we can improve this 

   ldh a, [hPlayerVelX]
   or a, a 
   jr z, .noScroll 
   
   ; check orientation: 
   ldh a, [hPlayerDirection] 
   rra 
   jr c, .right 

   ; left: 
   ldh a, [hPlayerPosBufferX+2]
   sub a, 8 
   jr nc, .noScroll 
   
   ; hasLeftDeadzoneLeft:
   ld a, 1 
   ldh [hHasCrossedThreshold], a
   
   ret 

.right: 
   ldh a, [hPlayerPosBufferX] 
   add a, 8  

.check: 
   cp a, $50 
   jr nc, .hasLeftDeadzoneRight  
  
.noScroll:
   xor a, a   
   ldh [hHasCrossedThreshold], a 

   ret 

.hasLeftDeadzoneRight: 
   
   ld bc, $01ab 

   ldh a, [hPlayerVelY] 
   or a, a 
   jr z, .updateCam 
    
   inc b 

.updateCam: 
   ld hl, hPlayerCamPosBufferX+1
   
   ld a, [hl]
   add a, c 
   ld [hld], a 
   ld a, [hl] 
   adc a, b 
   ld [hld], a 
   
   ld l, LOW(rSCX) 
   ld [hl], a   

   ld a, 1 
   ldh [hHasCrossedThreshold], a

   ret 

checkPlayerBoundingBox:
   ; check Y axis first (will step X first and then Y later for slopes)
   ldh a, [hPlayerPosBufferX]
   ld e, a 
   ldh a, [hPlayerPosBufferY] 
   ld d, a 

   ld a, e 

   add a, 8 

   rrca 
   rrca 
   rrca 
   and a, %00011111

   ld c, a 
   ld b, c 

   dec b 
   dec b ; lower bounds is always -2 of upper bound
   
   ld a, d 
   add a, 8 

   rrca 
   rrca 
   rrca 
   and a, %00011111

   ; proof of concept, need to change later 

   ld l, a 
   ld d, a 

   add a, a 
   add a, a 
   add a, l 

   ld b, 0 
   ld c, a 

   ld hl, s1colMapStart
   add hl, bc 

   ld a, [hl] 
   inc a  
   jr nz, .noCollision 

   ld a, 1 
   ldh [hIsGrounded], a

   xor a, a 
   ldh [hPlayerVelY], a
   ldh [hPlayerVelY+1], a

   ret  

.noCollision: 
   xor a, a 
   ldh [hIsGrounded], a

   ret 

; TODO: improve animation system by pushing all possible lookup addresses to WRAM using the stack. 
; TODO: see if we can improve the state transition system such that it uses a LUT -> need to change 
; how keys are read (need to be linear 0-N) 

playLoop:
   call updateFSM 
   call transitionLogicFSM
   call checkPlayerBoundingBox
   jr playLoop
   
   
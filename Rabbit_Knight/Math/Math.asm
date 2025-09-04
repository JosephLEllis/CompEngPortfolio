SECTION "Math", ROM0  

; Q8.8 fixed point arithmetic addition: 
; bc = addend 
; de = value to add to 
; hl = address to fetch value from 
addFixedPoint: 
   inc l 
   ld a, [hl] 
   add a, c 
   ld [hld], a 
   ld a, [hl] 
   adc a, b 
   ld [hl], a
   ret   
   
   
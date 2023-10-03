; Test the CRC16 Algorithm in 68000 assembler
; To run this you need the precalculated table in binary format ../target
; Malcolm Harrow August 2023

    section .text                     

kmain::
    ; this code will eventually go into the SD Card read/write routines
    ; so we dont want to trash any registers that aren't trashed by 
    ; the current version of the code or unforseen things could happen
    ; to existing software

    ; C code we try to replicate ..
    ; uint16_t UpdateCRC(uint16_t* table, uint8_t byte, uint16_t prevCRC) {   
    ;     return table[((prevCRC >> 8) ^ byte) & 0xff] ^ (prevCRC << 8);
    ; }
    ; uint16_t CalcCRC(uint16_t* table, uint8_t* bytes, int32_t len, uint16_t initValue) {
    ;     uint16_t prevCRC = initValue;
    ;     for (int i = 0; i < len; i++) {
    ;         prevCRC = UpdateCRC(table, bytes[i], prevCRC);
    ;     }
    ;     return prevCRC;
    ; }

    ; this works, but just proved that the gcc compiler can create better machine code than I can ..
    ; so we just implement as a C header file .. see create_crc_header.c


    ; whats this for - pushing the start to an address I can easily find in the mame debugger ..
    bra mhstart
    incbin "./target/crctable.bin"
    incbin "./target/crctable.bin"

mhstart:
    moveq.l #0,D0                     ; set prevcrc to 0
    move.w  #(512-1),D1               ; loop counter
    lea     testData,A0
    lea     tableCRC,A1
.loop:
    moveq.l #0,D3
    move.b  D0,D3                     ; D3 now has lo byte of prevcrc
    rol.w   #8,D3                     ; D3.W now has (prevcrc << 8)
    clr.b   D0
    rol.w   #8,D0                     ; D0 now has prevcrc hi byte    

    move.b  (A0)+,D4
    eor.b   D4,D0                     ; eor with byte

    add     D0,D0                     ; allow for indexing by word
    move.w  0(A1,D0.w),D0             ; move value from tableCRC to D0
    eor.w   D3,D0                     ; eor with prevcrc hi byte
    dbra    D1,.loop
    rts                               ; answer should be in D0        

    section .data
    align 4
tableCRC:   incbin "./target/crctable.bin"
testData:   blk.b 512,$FF             ; 512 bytes of 0xFF    
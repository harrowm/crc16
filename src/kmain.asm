; Test the CRC16 Algorithm in 68000 assembler
; To run this you need the precalculated table in ../target
; Malcolm Harrow August 2023

    section .text                     

kmain::
    ; this code will eventually go into the SD Card read/write routines
    ; so we dont want to trash any registers that aren't trashed by 
    ; the current version of the code or unforseen things could happen
    ; to existing software

    moveq.l #0,D0                     ; set prevcrc to 0
.loop:
    moveq.l #0,D1                     ; loop counter
    
    move.b  D0,D2                     ; D2 now has prevcrc low byte
    swap.w  D0                        ; D1 low byte now has prevcrc high byte
    move.b

uint16_t UpdateCRC(uint16_t* table, uint8_t byte, uint16_t prevCRC) {   
    return table[((prevCRC >> 8) ^ byte) & 0xff] ^ (prevCRC << 8);
}

uint16_t CalcCRC(uint16_t* table, uint8_t* bytes, int32_t len, uint16_t initValue) {
    uint16_t prevCRC = initValue;
    for (int i = 0; i < len; i++) {
        prevCRC = UpdateCRC(table, bytes[i], prevCRC);
    }
    return prevCRC;




    cmp.w   #512,D1
    bne     .loop

    rts                               

                    align 4
tableCRC:   incbin "../target/crctable.bin"
testData:   blk.b $FF,512            ; 512 bytes of 0xFF    

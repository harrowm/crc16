// Create then test the CRC16 table used by the main routine
// Malcolm Harrow September 2023

// References:
// https://stackoverflow.com/questions/44131951/how-to-generate-16-bit-crc-table-from-a-polynomial

// This code create the table as a file so that it can be later used
// via "incbin" in 68000 assembler

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TABLE_FILENAME     "../target/crctable.bin"

uint16_t UpdateCRC(uint16_t* table, uint8_t byte, uint16_t prevCRC) {   
    return table[((prevCRC >> 8) ^ byte) & 0xff] ^ (prevCRC << 8);
}

uint16_t CalcCRC(uint16_t* table, uint8_t* bytes, int32_t len, uint16_t initValue) {
    uint16_t prevCRC = initValue;
    for (int i = 0; i < len; i++) {
        prevCRC = UpdateCRC(table, bytes[i], prevCRC);
        printf("debug: %04X\n", prevCRC);
    }
    return prevCRC;
}

int main(int argc, char **argv) {
    const uint16_t LengthCRC = 16;
    const uint16_t polynomial = 0x1021;
    const uint16_t mask = (1 << (LengthCRC - 1));
    const int32_t N = 256;
    uint16_t tableCRC[N];
    uint16_t tableCRC68k[N];

    // Make sure we are running on a big endian processor so that the lookup
    // table is generated correctly
    if (BYTE_ORDER == __LITTLE_ENDIAN__) {
        printf("%s: must be run on a big endian machine (e.g. not a 68000!)\n", argv[0]);   
        exit(1);             
    }

    // Create the table
    for (int32_t i = 0; i < N; i++) {
        uint16_t crc = i << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & mask)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
        }
        tableCRC[i] = crc;
    }

    // write out the table to a file
    // if ((fp = fopen(TABLE_FILENAME,"wb")) == NULL) {
    //     printf("%s: can't create file %s\n", argv[0], TABLE_FILENAME);   
    //     exit(1);             
    // }
    // fwrite(tableCRC, sizeof(tableCRC), 1, fp);
    // fclose(fp);

    // // Clear table and re-read from the file, just to be pedantic
    // memset(tableCRC, '*', sizeof(tableCRC));
    // if ((fp = fopen(TABLE_FILENAME,"rb")) == NULL) {
    //     printf("%s: can't copen file %s\n", argv[0], TABLE_FILENAME);   
    //     exit(1);             
    // }
    // fread(tableCRC, sizeof(tableCRC), 1, fp);
    // fclose(fp);

    //Create a table for the 68000 with the right endianess
    for (int32_t i = 0; i < N; i++) {
        tableCRC68k[i] = __builtin_bswap16(tableCRC[i]);
    }

    // .. and write it out to a file for later use in the 68000 assembler code
    FILE *fp;
    if ((fp = fopen(TABLE_FILENAME,"wb")) == NULL) {
        printf("%s: can't create file %s\n", argv[0], TABLE_FILENAME);   
        exit(1);             
    }
    fwrite(tableCRC68k, sizeof(tableCRC68k), 1, fp);
    fclose(fp);


    // Test the calculation
    // Correct result for all 0xFF array should be 0x7FA1 according to
    // https://bits4device.wordpress.com/2017/12/16/sd-crc7-crc16-implementation/

    int32_t len = 512; 
    uint8_t test[len];
    memset(test, 0xFF, sizeof(test));

    uint16_t res = CalcCRC(tableCRC, test, len, 0);
    printf("Result of CRC test: %04X ", res);
    if (res == 0x7FA1) {
        printf("PASS\n");
        exit(0);
    } else {
        printf("**FAIL** - expected 0x7FA1\n");
        exit(1);
    }
}

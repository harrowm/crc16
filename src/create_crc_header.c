// Create then test the CRC16 and CRC7 tables used by the main routine
// Create the header file used by the rosco firmware
// Malcolm Harrow September 2023

// References:
// https://stackoverflow.com/questions/44131951/how-to-generate-16-bit-crc-table-from-a-polynomial
// https://github.com/hazelnusse/crc7/blob/master/crc7.cc


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TABLE_FILENAME     "../include/crc.h"

static uint16_t tableCRC16[256];
static uint8_t tableCRC7[256];

// Note that these 2 functions also get put into the header file that's created, make
// sure they are consistent ..

// returns the CRC16 for a message of "len" bytes
static inline uint16_t CalcCRC16(uint8_t* bytes, int32_t len) {
    uint16_t prevCRC = 0;
    for (int i=0; i<len; i++) {
        prevCRC = tableCRC16[((prevCRC >> 8) ^ bytes[i]) & 0xff] ^ (prevCRC << 8);
    }
    return prevCRC;
}

// returns the CRC7 for a message of "len" bytes
static inline uint8_t CalcCRC7(uint8_t* bytes, int32_t len) {
    uint8_t prevCRC = 0;
    for (int i=0; i<len; i++) {
        prevCRC = tableCRC7[(prevCRC << 1) ^ bytes[i]];
    }
    prevCRC = (prevCRC << 1) | 1;
    return prevCRC;
}





int main(int argc, char **argv) {

    // Create the CRC16 table
    uint16_t LengthCRC = 16;
    uint16_t polynomial = 0x1021;
    uint16_t mask = (1 << (LengthCRC - 1));

    for (int i = 0; i < 256; i++) {
        uint16_t crc = i << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & mask)
                crc = (crc << 1) ^ polynomial;
            else
                crc <<= 1;
        }
        tableCRC16[i] = crc;
    }

    // Create the CRC7 table
    polynomial = 0x89;
    mask = 0x80;

    for (int i = 0; i < 256; i++) {
        tableCRC7[i] = (i & mask) ? i ^ polynomial : i;
        for (uint8_t j = 1; j < 8; j++) {
            tableCRC7[i] <<= 1;
            if (tableCRC7[i] & mask)
                tableCRC7[i] ^= polynomial;
        }
    }


    // Test the calculations


    // First CRC16
    // Correct result for all 0xFF array should be 0x7FA1 according to
    // https://bits4device.wordpress.com/2017/12/16/sd-crc7-crc16-implementation/

    int32_t len = 512; 
    uint8_t test[len];
    memset(test, 0xFF, sizeof(test));

    uint16_t res = CalcCRC16(test, len);
    if (res != 0x7FA1) {
        printf("** CRC16 TEST FAILED ** - expected 0x7FA1\n");
        printf("** No header file generated **\n");
        exit(1);
    }

    // Then test crc7
    len = 5;
    uint8_t test7[len];
    uint8_t crc7 = 0;
    
    // Test 1
    memset(test, 0x00, sizeof(test));
    test[0] = 0x40;

    crc7 = CalcCRC7(test, len);
    if (crc7 != 0x95) {
        printf("** CRC7 TEST 1 FAILED ** - expected 0x95\n");
        printf("** No header file generated **\n");
        exit(1);
    }

    // Test 2
    memset(test, 0x00, sizeof(test));
    test[0] = 0x48;
    test[3] = 0x1;
    test[4] = 0xAA;
    
    crc7 = CalcCRC7(test, len);
    if (crc7 != 0x87) {
        printf("** CRC7 TEST 2 FAILED ** - expected 0x87\n");
        printf("** No header file generated **\n");
        exit(1);
    }

    // Test 3
    memset(test, 0x00, sizeof(test));
    test[0] = 0x69;
    test[1] = 0x40;

    crc7 = CalcCRC7(test, len);
    if (crc7 != 0x77) {
        printf("** CRC7 TEST 3 FAILED ** - expected 0x77\n");
        printf("** No header file generated **\n");
        exit(1);
    }

    // Test 4
    memset(test, 0x00, sizeof(test));
    test[0] = 0x77;

    crc7 = CalcCRC7(test, len);
    if (crc7 != 0x65) {
        printf("** CRC7 TEST 4 FAILED ** - expected 0x65\n");
        printf("** No header file generated **\n");
        exit(1);
    }

    // Test 5
    memset(test, 0x00, sizeof(test));
    test[0] = 0x7A;

    crc7 = CalcCRC7(test, len);
    if (crc7 != 0xFD) {
        printf("** CRC7 TEST 5 FAILED ** - expected 0xFD\n");
        printf("** No header file generated **\n");
        exit(1);
    }


    // .. and write out the tables and constants into the header file
    FILE *fp;
    if ((fp = fopen(TABLE_FILENAME,"wb")) == NULL) {
        printf("%s: can't create file %s\n", argv[0], TABLE_FILENAME);   
        printf("** No header file generated **\n");
        exit(1);             
    }

    fprintf(fp, "/*\n");
    fprintf(fp, " *------------------------------------------------------------\n");
    fprintf(fp, " *                                  ___ ___ _\n");
    fprintf(fp, " *  ___ ___ ___ ___ ___       _____|  _| . | |_\n");
    fprintf(fp, " * |  _| . |_ -|  _| . |     |     | . | . | '_|\n");
    fprintf(fp, " * |_| |___|___|___|___|_____|_|_|_|___|___|_,_|\n");
    fprintf(fp, " *                     |_____|       firmware v2\n");
    fprintf(fp, " * ------------------------------------------------------------\n");
    fprintf(fp, " * Copyright (c)2020-2023 Ross Bamford and contributors\n");
    fprintf(fp, " * See top-level LICENSE.md for licence information.\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * CRC for SD Cards\n");
    fprintf(fp, " * This file has been generated from:\n");
    fprintf(fp, " *     ./crc/create_crc_table.c\n");
    fprintf(fp, " * Do not edit\n");
    fprintf(fp, " * ------------------------------------------------------------\n");
    fprintf(fp, " */\n\n");

    fprintf(fp, "#ifndef ROSCO_M68K_CRC_H\n");
    fprintf(fp, "#define ROSCO_M68K_CRC_H\n\n");
    fprintf(fp, "#include <stdint.h>\n\n");

    fprintf(fp, "const uint8_t tableCRC7[256] = {\n");
    for (int i=0; i<32; i++) {
        fprintf(fp, "    0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X", 
            tableCRC7[i*8+0], tableCRC7[i*8+1], tableCRC7[i*8+2], tableCRC7[i*8+3], 
            tableCRC7[i*8+4], tableCRC7[i*8+5], tableCRC7[i*8+6], tableCRC7[i*8+7]);
        if (i != 31) {
            fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "const uint16_t tableCRC16[256] = {\n");
    for (int i=0; i<32; i++) {
        fprintf(fp, "    0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X", 
            tableCRC16[i*8+0], tableCRC16[i*8+1], tableCRC16[i*8+2], tableCRC16[i*8+3], 
            tableCRC16[i*8+4], tableCRC16[i*8+5], tableCRC16[i*8+6], tableCRC16[i*8+7]);
        if (i != 31) {
            fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "// returns the CRC16 for a message of \"len\" bytes\n");
    fprintf(fp, "static inline uint16_t CalcCRC16(uint8_t* bytes, int32_t len) {\n");
    fprintf(fp, "    uint16_t prevCRC = 0;\n");
    fprintf(fp, "    for (int i=0; i<len; i++) {\n");
    fprintf(fp, "        prevCRC = tableCRC16[((prevCRC >> 8) ^ bytes[i]) & 0xff] ^ (prevCRC << 8);\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    return prevCRC;\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "// returns the CRC7 for a message of \"len\" bytes\n");
    fprintf(fp, "static inline uint8_t CalcCRC7(uint8_t* bytes, int32_t len) {\n");
    fprintf(fp, "    uint8_t prevCRC = 0;\n");
    fprintf(fp, "    for (int i=0; i<len; i++) {\n");
    fprintf(fp, "        prevCRC = tableCRC7[(prevCRC << 1) ^ bytes[i]];\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    prevCRC = (prevCRC << 1) | 1;\n");
    fprintf(fp, "    return prevCRC;\n");
    fprintf(fp, "}\n\n");

    fprintf(fp, "#endif /* ROSCO_M68K_CRC_H */\n");
    fclose(fp);
}

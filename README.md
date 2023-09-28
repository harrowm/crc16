# crc16
CRC16 routines for a SD Card on the rosco_m68k SBC 

Start with a simple C program to test the algorthim and create the table in a binary file that can included into assembler via "incbin" directive.

Compile with:

```cc -o test ./src/create_crc_table.c```

and run with:

```./test```
# crc16
CRC16 (and CRC7) routines for a SD Card on the rosco_m68k SBC 

After trying to implement CRC16 in 68k assembler, I moved to C .. this code now creates a header file with the tables and functiosn required to calcualte CRC7/CRC16

Compile with:

```cc -o test ./src/create_crc_header.c```

and run with:

```./test```

If you get no messages on the command line then tests have passed nad the header file can be found here:

```./include/crc.h```
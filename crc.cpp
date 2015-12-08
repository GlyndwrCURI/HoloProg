#include "stdafx.h"

unsigned long crcTable[256];

//This function should be called before calling C alcCRC4Table()... It just initialises crcTable[]
void crcgen(void){
 unsigned long   crc, poly;
 int i, j;
 poly = 0xEDB88320L;
 for (i=0; i<256; i++) {
    crc = i;
    for (j=8; j>0; j--)
      if (crc&1) 
        crc = (crc >> 1) ^ poly;
      else 
        crc >>= 1;
    crcTable[i] = crc;
   }
}

/************************ C alcCRC4Table *************************************/
//ALWAYS CALL crcgen() before calling C alcCRC4Table (to setup crcTable[])
//The CRC should be placed in the Header as the first four bytes of line 4
unsigned int CalcCRC4Table(int nNoofTableChars, void *bufferpointer)  { 
//unsigned int CalcCRC4Table(int nNoofTableChars, void *bufferpointer, void *shtLineNo)  { 
//char showMe[10000];
//int showMeI = 0;
 register unsigned long crc;
 int c;
 crc = 0xFFFFFFFF;
 char *cp = (char*)bufferpointer;
 while( nNoofTableChars-- ){
    c = *cp++;
//showMe[showMeI++] = c;
//if(showMeI > 9998l)
//  showMeI--;
    crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
   }
/*
 cp = (char*)shtLineNo;
 c = *cp++;
 crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ];
 c = *cp++;
 crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^c) & 0xFF ]; */
 return( crc^0xFFFFFFFF );
}

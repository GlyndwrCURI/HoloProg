#include "stdafx.h"
/* This function is no longer used
BYTE RevBits[256];

void InitializeRevBitsTable(void){
 for (BYTE j = 0; j < 255; j++){
    BYTE newPattern = 0;
    newPattern  = (j &   1) << 7;
    newPattern |= (j &   2) << 5;
    newPattern |= (j &   4) << 3;
    newPattern |= (j &   8) << 1;
    newPattern |= (j &  16) >> 1;
    newPattern |= (j &  32) >> 3;
    newPattern |= (j &  64) >> 5;
    newPattern |= (j & 128) >> 7;
    RevBits[j] = newPattern;
   }
 RevBits[255] = 0xff;
}
*/
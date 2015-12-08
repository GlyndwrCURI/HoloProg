#include "stdafx.h"
extern char chCSVHeaderLine[SIZ_CSV_HEADERp4];

void FillInHeaderForGeneratedFile(int nHCFrecNoofLines, int nHCFrecNoofCols, int nHCFrecFrequencyOfPixelPrintsInHz, int nACCEL_DECEL_PROFILE_SIZE, time_t *ptr_HCFrec_uTimeStamp, char *szHCFrecHologramName){
StringCchCopy(chCSVHeaderLine,SIZ_CSV_HEADER,
   "                        This Is THE Time||                                       *||Lines:        Cols:        LeadIn:     *||Pixel Frequency                       ff||      ||");
 //"                        This Is THE Time||                       The Hologram Name||Lines: xxxx   Cols: yyyy            llll||Pixel Frequency                       ff||  xxxx||");
 chCSVHeaderLine[40] = 13;   //This
 chCSVHeaderLine[41] = 10;   // Overwrites
 chCSVHeaderLine[82] = 13;   //  the
 chCSVHeaderLine[83] = 10;   //   vertical
 chCSVHeaderLine[124] = 13;  //    bars
 chCSVHeaderLine[125] = 10;  //     (above)
 chCSVHeaderLine[166] = 13;  //       with
 chCSVHeaderLine[167] = 10;  //        cr/lf
 chCSVHeaderLine[174] = 13;  //         withv
 chCSVHeaderLine[175] = 10;  //          cr/lf
//                                                                                                        WRITE THE HEADER (probably common amongst all generate functions)
 char szTimeStamp[27];
 int  nLenTimeStampOrNameString;
 errno_t err;
 err = ctime_s(szTimeStamp, 26, ptr_HCFrec_uTimeStamp);                                                                                                     //Convert the timestampt to a string...
 if (err)
   MessageBox (NULL,"ctime_s failed due to an invalid argument.",NULL,MB_OK);
 else{
    nLenTimeStampOrNameString = lstrlen(szTimeStamp) - 1;
    szTimeStamp[nLenTimeStampOrNameString] = 0;//removes linefeed (subscript would be -ve, here, if ctime_s() failed with an error: hence the else
   }
 int nCharInHeaderToReplace = 39;
 while(--nLenTimeStampOrNameString > -1)
   chCSVHeaderLine[nCharInHeaderToReplace--] = szTimeStamp[nLenTimeStampOrNameString];                                                                      // ... and stick it in the header

 nLenTimeStampOrNameString = lstrlen(szHCFrecHologramName);
 nCharInHeaderToReplace = 81;
 while(--nLenTimeStampOrNameString > -1)
   chCSVHeaderLine[nCharInHeaderToReplace--] = szHCFrecHologramName[nLenTimeStampOrNameString];                                                               //Stick hologram name in the header too.

 char szDum[SIZSZDUM12];
 _itoa_s(nHCFrecNoofLines,szDum,SIZSZDUM12,10);
 int nLen_szDum = lstrlen(szDum);
 int nCopyChars = 0;
 int nSpacesAtStart = 4 - nLen_szDum;//Keep The numbers right - justified
 while(nCopyChars < nLen_szDum){
    chCSVHeaderLine[91 + nSpacesAtStart + nCopyChars] = szDum[nCopyChars];  //This Inserts the noof Lines (the xxxx)
    chCSVHeaderLine[170 + nSpacesAtStart + nCopyChars] = szDum[nCopyChars]; //This Inserts the noof Lines (the xxxx)
    nCopyChars++;
   }

 _itoa_s(nHCFrecNoofCols,szDum,SIZSZDUM12,10);
 nLen_szDum = lstrlen(szDum);
 nCopyChars = 0; 
 nSpacesAtStart = 4 - nLen_szDum;//Keep The numbers right - justified
 while(nLen_szDum--){
    chCSVHeaderLine[104 + nSpacesAtStart + nCopyChars] = szDum[nCopyChars]; //This Inserts the noof Cols (the yyyy)
    nCopyChars++;
   }
 _itoa_s(nHCFrecFrequencyOfPixelPrintsInHz,szDum,SIZSZDUM12,10);
 chCSVHeaderLine[164] = szDum[0];
 chCSVHeaderLine[165] = szDum[1];

 _itoa_s(nACCEL_DECEL_PROFILE_SIZE,szDum,SIZSZDUM12,10);
 nLen_szDum = lstrlen(szDum);
 nCopyChars = 0;
 nSpacesAtStart = 4 - nLen_szDum;//Keep the numbers right - justified
 while(nLen_szDum--){
    chCSVHeaderLine[120 + nSpacesAtStart + nCopyChars] = szDum[nCopyChars]; //This Inserts the leadIn (the llll)
    nCopyChars++;
   }
}
#include "stdafx.h"

extern HCFRECORDsCURRENTfolders HCFcfs;
extern char FILENAMEStab[][16];
extern char chCSVHeaderLine[SIZ_CSV_HEADERp4];

extern double dblArDesiredEnergyWP3r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03     M03  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
//Returns zero for success, else an error code.

int GenerateEnergyProfileForWP3(HWND hDlg,
                                 int idxMno, // 2, 14, or 26
                                 int nLines,
                                 int nCols,
                                 int nLeadInAndOut,
                                 double dblMinVolts,
                                 double dblMaxVolts){ //Use Volts to generate dummy data
#if 1
 hDlg; idxMno; nLines; nCols; nLeadInAndOut; dblMinVolts; dblMaxVolts; //stop warnings
 return 0; //cse file has to be a file of doubles, or an exported file of doubles (in text format) from Excel.  As there is no need to add accel/decel to the energies file, I'll just read in whatever Jerzy produces.
#else
    nLeadInAndOut; //remove compiler warning
//For the time being, generate dummy test data (using voltages instead of uJoules).  This will allow testing of WP3 PID
 char szLocalEnergiesProfileFileName[_MAX_DIR]; //"<HologramDIR>\\CSVs\\M03WP3Red.cse"
 StringCchCopy(chCSVHeaderLine,SIZ_CSV_HEADER,
   "                        This Is THE Time||                       The Hologram Name||Lines:        Cols:                    *||Don't Edit This file!!!!               *||");
 //"                        This Is THE Time||                       The Hologram Name||Lines: xxxx   Cols: yyyy               *||Don't Edit This file!!!!               *||");
 chCSVHeaderLine[40] = 13;   //This
 chCSVHeaderLine[41] = 10;   // Overwrites
 chCSVHeaderLine[82] = 13;   //   the
 chCSVHeaderLine[83] = 10;   //    vertical
 chCSVHeaderLine[124] = 13;  //     bars
 chCSVHeaderLine[125] = 10;  //      (above)
 chCSVHeaderLine[166] = 13;  //        with
 chCSVHeaderLine[167] = 10;  //         cr/lf
 char szDum[SIZSZDUM12];
 _itoa_s(nLines,szDum,SIZSZDUM12,10);
 int nLen_szDum = lstrlen(szDum);
 int nCopyChars = 0;
 while(nCopyChars < nLen_szDum){
    chCSVHeaderLine[91 + nCopyChars] = szDum[nCopyChars]; //This Inserts the noof Lines (the xxxx)
    nCopyChars++;
   }
 _itoa_s(nCols,szDum,SIZSZDUM12,10);
 nLen_szDum = lstrlen(szDum);
 nCopyChars = 0;
 while(nLen_szDum--){
    chCSVHeaderLine[104 + nCopyChars] = szDum[nCopyChars]; //This Inserts the noof Cols (the yyyy)
    nCopyChars++;
   }
 //Now Generate some dummy data (Voltages ramping up and down on alternate lines)
 double incrementer = (dblMaxVolts - dblMinVolts) / nCols;
 for(int eAchM = 0; eAchM < nLines; eAchM += 2)
   for(int nSPos = 0; nSPos < nCols; nSPos++)
     dblArDesiredEnergyWP3r[eAchM][nSPos] = dblMinVolts + (incrementer * nSPos); //This is just a way of generating of generating some default StartPosition test data
 //The above might generate, for example, 0, 0.5, 1, 1.5
 incrementer *= -1;
 for(int eAchM = 1; eAchM < nLines; eAchM += 2)
   for(int nSPos = 0; nSPos < nCols; nSPos++)
     dblArDesiredEnergyWP3r[eAchM][nSPos] = dblMaxVolts + (incrementer * nSPos); //This is just a way of generating of generating some default StartPosition test data
 //The above might generate, for example, 2, 1.5, 1, 0.5
 StringCchCopy(szLocalEnergiesProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from but, when generating a hologram, we write to it.
 StringCchCat(szLocalEnergiesProfileFileName,_MAX_DIR,FILENAMEStab[idxMno]);//"<HologramDIR>\\CSVs\\M03WP3Red.cse"
 //p_dblArDesiredEnergyWP3r = &dblArDesiredEnergyWP3r[0][0];
 return mySave_File (hDlg, MAXNOOFPIXELS_IN_ROW * MAX_NOOFLINES * sizeof(double), "cse",0, szLocalEnergiesProfileFileName, (char)idxMno); //returns zero if file saved OK
#endif
}
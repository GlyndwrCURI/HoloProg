#include "stdafx.h"

extern PCFRECORD PCFrec;
int nWavPlate1PcentSplitRGBAr[303];

//The tables with few records from which the ADC lookup tables are built (using interpolation)
ENERGY_CALIB_RREC WavPlate2ForInterpAr[91];  //These tables must be loaded with between 2 and 91 items in (decided by the user when calibrating)
ENERGY_CALIB_RREC WavPlate3ForInterpAr[91];  //91 has been chosen as the maximum because eventually the calibration procedure may be automated, and this
ENERGY_CALIB_RREC WavPlate14ForInterpAr[91]; // would allow that process to do the calibration in 1 degree steps (0 - 90, inclusive)
ENERGY_CALIB_RREC WavPlate15ForInterpAr[91]; //Row zero must contain ADC code zero, and real energy zero.
ENERGY_CALIB_RREC WavPlate26ForInterpAr[91];
ENERGY_CALIB_RREC WavPlate27ForInterpAr[91];
/*
int nWavPlate2RealEnergyAr[4096];
int nWavPlate3RealEnergyAr[4096];
int nWavPlate14RealEnergyAr[4096];
int nWavPlate15RealEnergyAr[4096];
int nWavPlate26RealEnergyAr[4096];
int nWavPlate27RealEnergyAr[4096];
*/
double nWavPlate2RealEnergyAr[4096];
double nWavPlate3RealEnergyAr[4096];
double nWavPlate14RealEnergyAr[4096];
double nWavPlate15RealEnergyAr[4096];
double nWavPlate26RealEnergyAr[4096];
double nWavPlate27RealEnergyAr[4096];

//This function is called by:
// b MotorIsInItsStartPosition()
// M oveToStartPositions()
// V iewMotorStartPointsInHCF_Dialog()
int GetUstepsForThisRatio(int WP1ratio, int Beam){ //Beam is 1,2 or 3 for Red, Green or Blue

 int nStartingIndx = 123; //Again, without this, compiler warns of potentially uninitialised local variable 'nStartingIndx' being used. I think the compiler's wrong!!!
 int nMainPartOfSteps;
 int nInterpolatedPartOfSteps;

 div_t WP1ratioPlusInterp;
 WP1ratioPlusInterp = div(WP1ratio, 1000);
 if ((WP1ratioPlusInterp.quot > 100) || (WP1ratioPlusInterp.rem > 1000) || (WP1ratioPlusInterp.quot < 0) || (Beam < 1) || (Beam > 3)){
    MessageBox(NULL, "Serious Program error1:\nEither div(WP1ratio, 1000) is > 100%\nor rem > 1000\nor Beam is Out Of Range",NULL, MB_OK);
    return 0;
   }
 else if (WP1ratioPlusInterp.quot == 100){
    if (WP1ratioPlusInterp.rem > 0){
       MessageBox(NULL, "Serious Program error2: div(WP1ratio, 1000) is > 100% !!",NULL, MB_OK);
       return 0;
      }
    if(Beam == 1)
      return nWavPlate1PcentSplitRGBAr[100] + PCFrec.nNoofUsteps_Hm2WP1MinEnergy[0];
    else if(Beam == 2)
      return nWavPlate1PcentSplitRGBAr[201] + PCFrec.nNoofUsteps_Hm2WP1MinEnergy[1];
    else //if(Beam == 3)
      return nWavPlate1PcentSplitRGBAr[302] + PCFrec.nNoofUsteps_Hm2WP1MinEnergy[2];
   }
 else{
    if(Beam == 1)
      nStartingIndx = 0;
    else if(Beam == 2)
      nStartingIndx = 101;
    else if(Beam == 3)
      nStartingIndx = 202;

    if(nStartingIndx == 123){
       MessageBox(NULL, "Serious Program error3: nStartingIndx Did not get initialised correctly!!",NULL, MB_OK);
       return 0;
      }

    nMainPartOfSteps = nWavPlate1PcentSplitRGBAr[nStartingIndx + WP1ratioPlusInterp.quot];

    int nGap = nWavPlate1PcentSplitRGBAr[nStartingIndx + WP1ratioPlusInterp.quot + 1] - nWavPlate1PcentSplitRGBAr[nStartingIndx + WP1ratioPlusInterp.quot];
    nInterpolatedPartOfSteps = (int)(   (double)WP1ratioPlusInterp.rem / 1000 * nGap   );

    return nMainPartOfSteps + nInterpolatedPartOfSteps + PCFrec.nNoofUsteps_Hm2WP1MinEnergy[Beam - 1];
   }
}

#if 0 //Use this version if nWavPlateXRealEnergyAr is ints, & the following version if it is doubles
//This function uses interpolation to build the lookup tables for converting ADC codes from the energy meters into calibrated "Real Energy" uJoules
//WavePlate is 2,3,14,15,26 or 27 (ie WPs 2&3 for Red, Green & Blue)
//nNoofGivenPoints must be >= 2, and  <= 91
BOOL BuildRealEnergyADClookups(HWND hWnd, int *nNoofGivenPointsAr){  //returns TRUE if there's an error
 double dblIncrement;
 double dblActualReadingSupplied;
 int nNoofIncrentsToBeApplied;
 //nNoofGivenPoints--;
 ENERGY_CALIB_RREC *pWPTable;
 ENERGY_CALIB_RREC *pWPTablesAr[6];
 pWPTablesAr[0] = WavPlate2ForInterpAr;
 pWPTablesAr[1] = WavPlate3ForInterpAr;
 pWPTablesAr[2] = WavPlate14ForInterpAr;
 pWPTablesAr[3] = WavPlate15ForInterpAr;
 pWPTablesAr[4] = WavPlate26ForInterpAr;
 pWPTablesAr[5] = WavPlate27ForInterpAr;

 int *pWPRealEnergyTable;
 int *pWPRealEnergyTablesAr[6];
 pWPRealEnergyTablesAr[0] = nWavPlate2RealEnergyAr;
 pWPRealEnergyTablesAr[1] = nWavPlate3RealEnergyAr;
 pWPRealEnergyTablesAr[2] = nWavPlate14RealEnergyAr;
 pWPRealEnergyTablesAr[3] = nWavPlate15RealEnergyAr;
 pWPRealEnergyTablesAr[4] = nWavPlate26RealEnergyAr;
 pWPRealEnergyTablesAr[5] = nWavPlate27RealEnergyAr;

 for(int WPtable = 0;WPtable < 6; WPtable++){ //Perform this once for each of the 6 six table being built
    pWPTable = pWPTablesAr[WPtable];
    pWPRealEnergyTable = pWPRealEnergyTablesAr[WPtable];
    //Do some validation on the input files:
    if(pWPTable[0].sADCcode || pWPTable[0].dlbRealEnergy < -0.00000001 || pWPTable[0].dlbRealEnergy > 0.00000001){
       char szDum[16] = "WP";
       int nWPnosAr[6] = {2,3,14,15,26,27};
       _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
       MessageBox(hWnd,"The cal file for the above named Waveplate must have its first record set for ADC code zero,\nand this must be Zero energy!",szDum,MB_OK);
       return 1; //failure
      }
    nNoofGivenPointsAr[WPtable]--;
    for(int nEachSampleRec = 0; nEachSampleRec < nNoofGivenPointsAr[WPtable]; nEachSampleRec++){//rem nNoofGivenPoints is now minus 1 !
       int nCurrentCode = pWPTable[nEachSampleRec].sADCcode;
       int nNextCode = pWPTable[nEachSampleRec+1].sADCcode;
       int noofADCcodesToFillIn = nNextCode - nCurrentCode;
       if(nNextCode <= nCurrentCode){
          char szDum[16] = "WP";
          int nWPnosAr[6] = {2,3,14,15,26,27};
          _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
          MessageBox(hWnd,"The cal file for the above named Waveplate must have ascending numbers for the ADC codes!",szDum,MB_OK);
          return 1; //failure
         }
       dblActualReadingSupplied = pWPTable[nEachSampleRec].dlbRealEnergy;
       nNoofIncrentsToBeApplied = 0;
       dblIncrement = (double)(pWPTable[nEachSampleRec+1].dlbRealEnergy - dblActualReadingSupplied) / noofADCcodesToFillIn;
       if(pWPTable[nEachSampleRec+1].dlbRealEnergy <= dblActualReadingSupplied){
          char szDum[16] = "WP";
          int nWPnosAr[6] = {2,3,14,15,26,27};
          _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
          MessageBox(hWnd,"The cal file for the above named Waveplate must have ascending numbers for the Real Energies!",szDum,MB_OK);
          return 1; //failure
         }
       for(int nEachADCcode = nCurrentCode; nEachADCcode < nNextCode; nEachADCcode++)
         pWPRealEnergyTable[nEachADCcode] = (int)((dblActualReadingSupplied + dblIncrement * nNoofIncrentsToBeApplied++) * 10 + 0.5);
      }
    //The maximum energy the user supplied may not be for ADC code 4095, so fill in the remaining codes up to 4095 assuming a straight line with
    //the same gradient as existed between the highest two points that were supplied (which is in dblIncrement). This allows for the laser to increase in
    // its maximum power after the time that the calibration was done, which is a possibility.
    dblActualReadingSupplied = pWPTable[ nNoofGivenPointsAr[WPtable] ].dlbRealEnergy; //which, here, is HighestReadingSupplied //rem nNoofGivenPoints is now minus 1 !
    nNoofIncrentsToBeApplied = 0;
    for(int nRemainingADCcodes = pWPTable[ nNoofGivenPointsAr[WPtable] ].sADCcode; nRemainingADCcodes < 4096; nRemainingADCcodes++)//rem nNoofGivenPoints is now minus 1 !
      pWPRealEnergyTable[nRemainingADCcodes] = (int)((dblActualReadingSupplied + dblIncrement * nNoofIncrentsToBeApplied++) * 10 + 0.5);
   }
 return 0;
}
#endif
//This function uses interpolation to build the lookup tables for converting ADC codes from the energy meters into calibrated "Real Energy" uJoules
//WavePlate is 2,3,14,15,26 or 27 (ie WPs 2&3 for Red, Green & Blue)
//nNoofGivenPoints must be >= 2, and  <= 91
BOOL BuildRealEnergyADClookups(HWND hWnd, int *nNoofGivenPointsAr){  //returns TRUE if there's an error
 double dblIncrement;
 double dblActualReadingSupplied;
 int nNoofIncrentsToBeApplied;
 //nNoofGivenPoints--;
 ENERGY_CALIB_RREC *pWPTable;
 ENERGY_CALIB_RREC *pWPTablesAr[6];
 pWPTablesAr[0] = WavPlate2ForInterpAr;
 pWPTablesAr[1] = WavPlate3ForInterpAr;
 pWPTablesAr[2] = WavPlate14ForInterpAr;
 pWPTablesAr[3] = WavPlate15ForInterpAr;
 pWPTablesAr[4] = WavPlate26ForInterpAr;
 pWPTablesAr[5] = WavPlate27ForInterpAr;

 double *pWPRealEnergyTable;
 //int *pWPRealEnergyTablesAr[6];
 double *pWPRealEnergyTablesAr[6];
 pWPRealEnergyTablesAr[0] = nWavPlate2RealEnergyAr;
 pWPRealEnergyTablesAr[1] = nWavPlate3RealEnergyAr;
 pWPRealEnergyTablesAr[2] = nWavPlate14RealEnergyAr;
 pWPRealEnergyTablesAr[3] = nWavPlate15RealEnergyAr;
 pWPRealEnergyTablesAr[4] = nWavPlate26RealEnergyAr;
 pWPRealEnergyTablesAr[5] = nWavPlate27RealEnergyAr;

 for(int WPtable = 0;WPtable < 6; WPtable++){ //Perform this once for each of the 6 six table being built
    pWPTable = pWPTablesAr[WPtable];
    pWPRealEnergyTable = pWPRealEnergyTablesAr[WPtable];
    //Do some validation on the input files:
    if(pWPTable[0].sADCcode || pWPTable[0].dlbRealEnergy < -0.00000001 || pWPTable[0].dlbRealEnergy > 0.00000001){
       char szDum[16] = "WP";
       int nWPnosAr[6] = {2,3,14,15,26,27};
       _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
       MessageBox(hWnd,"The cal file for the above named Waveplate must have its first record set for ADC code zero,\nand this must be Zero energy!",szDum,MB_OK);
       return 1; //failure
      }
    nNoofGivenPointsAr[WPtable]--;
    for(int nEachSampleRec = 0; nEachSampleRec < nNoofGivenPointsAr[WPtable]; nEachSampleRec++){//rem nNoofGivenPoints is now minus 1 !
       int nCurrentCode = pWPTable[nEachSampleRec].sADCcode;
       int nNextCode = pWPTable[nEachSampleRec+1].sADCcode;
       int noofADCcodesToFillIn = nNextCode - nCurrentCode;
       if(nNextCode <= nCurrentCode){
          char szDum[16] = "WP";
          int nWPnosAr[6] = {2,3,14,15,26,27};
          _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
          MessageBox(hWnd,"The cal file for the above named Waveplate must have ascending numbers for the ADC codes!",szDum,MB_OK);
          return 1; //failure
         }
       dblActualReadingSupplied = pWPTable[nEachSampleRec].dlbRealEnergy;
       nNoofIncrentsToBeApplied = 0;
       dblIncrement = (double)(pWPTable[nEachSampleRec+1].dlbRealEnergy - dblActualReadingSupplied) / noofADCcodesToFillIn;
       if(pWPTable[nEachSampleRec+1].dlbRealEnergy <= dblActualReadingSupplied){
          char szDum[16] = "WP";
          int nWPnosAr[6] = {2,3,14,15,26,27};
          _itoa_s(nWPnosAr[WPtable],szDum+2,14,10);
          MessageBox(hWnd,"The cal file for the above named Waveplate must have ascending numbers for the Real Energies!",szDum,MB_OK);
          return 1; //failure
         }
       for(int nEachADCcode = nCurrentCode; nEachADCcode < nNextCode; nEachADCcode++)
         //pWPRealEnergyTable[nEachADCcode] = (int)((dblActualReadingSupplied + dblIncrement * nNoofIncrentsToBeApplied++) * 10 + 0.5);
         pWPRealEnergyTable[nEachADCcode] = dblActualReadingSupplied + dblIncrement * nNoofIncrentsToBeApplied++;
      }
    //The maximum energy the user supplied may not be for ADC code 4095, so fill in the remaining codes up to 4095 assuming a straight line with
    //the same gradient as existed between the highest two points that were supplied (which is in dblIncrement). This allows for the laser to increase in
    // its maximum power after the time that the calibration was done, which is a possibility.
    dblActualReadingSupplied = pWPTable[ nNoofGivenPointsAr[WPtable] ].dlbRealEnergy; //which, here, is HighestReadingSupplied //rem nNoofGivenPoints is now minus 1 !
    nNoofIncrentsToBeApplied = 0;
    for(int nRemainingADCcodes = pWPTable[ nNoofGivenPointsAr[WPtable] ].sADCcode; nRemainingADCcodes < 4096; nRemainingADCcodes++)//rem nNoofGivenPoints is now minus 1 !
      pWPRealEnergyTable[nRemainingADCcodes] = (int)((dblActualReadingSupplied + dblIncrement * nNoofIncrentsToBeApplied++) * 10 + 0.5);
   }
 return 0;
}
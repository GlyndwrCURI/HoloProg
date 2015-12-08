#include "stdafx.h"
extern int nLengthOfM39rowsIncLeadInOut_inPixels;

extern char *pCurrentProfileGenerationBuffer;

extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];

extern HINSTANCE hInst;
extern int nHelpMessageToLoad;
extern PCFRECORD PCFrec;

extern char *pMxPacketBuffAr[NOOFMOTORS];
extern HCFRECORDsCURRENTfolders HCFcfs;
//extern char nchCurrentMotorZB;
extern char FILENAMEStab[][16];
extern char chCSVHeaderLine[SIZ_CSV_HEADERp4];

extern char mes[];

extern char szAccelFileNamesDir[_MAX_DIR];         //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//extern int  nLenszAccelFileNamesDir;               // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.
//extern char szTabProfileFileNameUstepEndings[7][8];// ={"1.prn","2.prn","4.prn","8.prn","16.prn","32.prn","64.prn"}; //e.g. "AccProfM40-64.prn"
//extern char szTabProfileFileNameFreqEndings[4][5]; // ={"10Hz","15Hz","30Hz","60Hz"}; //e.g. "AccProfM39-30Hz"
extern int  nAccProfile[];

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40     M40  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
//Returns zero for success, else an error code.

//For the Y axis, the movement can start at Pixel number : "Cols + LeadIn - 1"... up to that pixel, therefore, will be all zeros.
//The movement should all take place within 'LeadOut Pixels' (an unfavoured option could be to split the movement so some occurs during the next LeadIn).
//With all the other axis, to some extent they can move their start postitions so that they will be at the right place after however many pulses occur during the lead in.
//Maybe, after the end of a line a GoTo command could be issued, but it would be far more preferable to have LeadOuts stop in the right place.
//


//M39  X-axis: NOOFprePIXzeroCONSTSpeedPIXELS = 4          e.g. 20 cols give 19 movements  Pixel spacing 0.9mm between centers
//100,200,300,400,500,600,700,800,900,900,900,900     900,900,900,900,900,900,900,900,900,900,900,900,900,900,900,9900,900,900,90000,900,900,900,        ,800,700,600,500,400,300,200,100
//
//M40  Y-axis:                      e.g. 20 cols give 19 movements
//0,0,0,0,0,0,0,0,0,0,0,0     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0     10,20,30,40,45,40,30,20,10,0,0,0
//
//Yaxis Parameters required:
//  LeadOutForM39 (Time in Pixel Periods, in which movement must occur). Should also be used to say when Y movement should start (in conjunction with cols)
//  Distance to Move (nano metres)
//  GearFactor + ThreadPitch (Can be treated as a constant)
//  uStepsPerStep
//  File of Max Y motor Accelerations

//Simplied, this is: (We'll ignore permitted time as this is used only to check that the calculation succeeded - i.e. was possible)
//Number of uSteps to be moved.         e.g. Assume 2mm of movement is required.    This is 7.2 Revs of Motor. i.e 14400 uStep if 1/4 stepping
//                                      e.g. Assume 0.6mm of movement is required.  This is 7.2 Revs of Motor. i.e 14400 * 0.3 (i.e 4320) uStep if 1/4 stepping
//Maximum acceleration profile e.g 100,200,300,400,500,600,..
//User request 10412 usteps should be perform within the leadout.
//100,200,300,400,500,600,700,800,900,1000,900,800,600,500,412,400,300,200,100                          112, 100,100,100,100
//                       2800    4500 5500

//User request 11412 usteps should be perform within the leadout.
//100,200,300,400,500,600,700,800,900,1000,1100,1000,900,800,600,500,400,300,200,100                          112, 100,100,100,100
//                       2800         5500 6600
                         //In the above case we cant use the 6600 because it's not possible to brake, so we'll try this instead:
//100,200,300,400,500,600,700,800,900,1000,1100, 900,800,600,500,400,300,200,100 
//                       2800    4500 5500   if we come straight down it would give 10000 (remainder 1412)     I think that if the 1000

int GenerateProfileForM40(HWND hDlg,int nHCFrecNoofLinesInc2TimesBetweenTheBeams,
                                 int    nHCFrecNoofCols, //When Summed, equals true number of cols (as far as motion is concerned)
                                 int    nHCFrecFrequencyOfPixelPrintsInHz, //Used to choose the Accel Profile, and also put in the csv/csv header.
                                 char   chHCFrecNoofUsteps4EachMotorFromTab,
                                 UINT   uiHCFrecVertDistBetwPixelCenters_nanoM,
                                 time_t *ptr_HCFrec_uTimeStamp,// *ptr_HCFrec_uTimeStamp,
                                 char   *szHCFrecHologramName,
                                 int    nHCFrecYposOfHologsOriginOnPlateMM){

 char szLocalM40ProfileFileName[_MAX_DIR]; //"<HologramDIR>\\CSVs\\M40XYStageY.csv"
 int nLengthOfCalculatedMotorMovementInPixPeriods = 0;

 int nLeadInForM39_40 = (nLengthOfM39rowsIncLeadInOut_inPixels + 1 - nHCFrecNoofCols) / 2;   //This is to check that the calculation succeeded - i.e. was possible in the time allowed.
 double dblStepsPerNanoM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab * 7.2 / (YAXIS_PITCH * 1000000)  ; //500 is the steps per rev for the 5 phase stage motors. 7.2 is Gear Ratio

 short  shStepsToMoveBetweenRows;

 //ReadAccelerationProfile(M40)
 //For arguments sake, assume an acceleration profile always consists of 100 points.  The deceleration profile can be the opposite of this: the same points, but backwards.
 //It could be something like: 10,20,29,38,47,55,62,66,68,69,69,70,70,70,70... 70  (There's no reason why acceleration can't level off to horizontal)
 //Forgetting about files for the time being, I'll hardcode the acceleration table:
 int nAccProfileCumulative[NOOFaccelPROFILES];
 int j;
 int idxMnoP1 = 39 + 1;

 /*
 int COULD_LOAD_FROM_FILE;
     //THE FOLLOWNG CREATES AN ACCELERATION PROFILE - ***WE SHOULD REALLY LOAD IT FROM A FILE***
 #define FASTER_INC 100
 //Assume 800 steps/rev (2000 with proper motor).  This motor will go 2.5 times faster (rotationally) but linear movement is divided by 4 due to thread pitch.  Actual movement will be 0.625
 // times the speed of real motor.  Lets assume we're going to print at 30Hz with pixel centers 0.8mm apart.  On real motors, this requires 12 revs/sec (24mm/s) - 48 KHz with 1/4 stepping.  I
 // can't expect the small motors to do 30 revs/sec, so I'll silently set the driver hardware to 16 uSteps (7.5 revs/sec) or even 8 uSteps (conceivably the small motor may manage 15 revs/sec).
 //This function should therefore produce movements of 12 / 30 * 2000 as the relative movement between pixel centre i.e. 800 uSteps
 for(j = 0; j < NOOFaccelPROFILES/2; j++)
   nAccProfile[j] = j * FASTER_INC + FASTER_INC; //Gives a profile of FASTER_INC,(FASTER_INC+FASTER_INC),(FASTER_INC+FASTER_INC+FASTER_INC) etc. e.g. 20, 40, 60
#define SLOWER_INC 20
 int reducedAccelStartingPoint = nAccProfile[NOOFaccelPROFILES/2 - 1] + SLOWER_INC; //1225 + 15
 for(j = 0; j < NOOFaccelPROFILES/2; j++)                             //THIS CREATES A SLOWER ACCELERATION PROFILE
   nAccProfile[j + 50] = SLOWER_INC * j + reducedAccelStartingPoint; //Gives a profile of 1425,1440,1455 etc.
 // */
/*
 szAccelFileNamesDir[nLenszAccelFileNamesDir] = 0;
 StringCchCat(szAccelFileNamesDir,_MAX_DIR,"AccProfM40-");
 int nIdxToFileNameUstepEndings = DecodeNoofUstepsToAnIndex(chHCFrecNoofUsteps4EachMotorFromTab);
 int nIdxToFileNameFreqEndings =  DecodeFreqToAnIndex(nHCFrecFrequencyOfPixelPrintsInHz);
 if((nIdxToFileNameUstepEndings == 1000001)||(nHCFrecFrequencyOfPixelPrintsInHz == 1000001)){
    MessageBox(hDlg,"Invalid chHCFrecNoofUsteps4EachMotorFromTab or nHCFrecFrequencyOfPixelPrintsInHz parameter","GenerateProfileForM40 failed",MB_OK);
    return 1000001;
   }
 StringCchCat(szAccelFileNamesDir, _MAX_DIR, szTabProfileFileNameFreqEndings[nIdxToFileNameFreqEndings]);
 StringCchCat(szAccelFileNamesDir, _MAX_DIR, szTabProfileFileNameUstepEndings[nIdxToFileNameUstepEndings]);
*/
 if(LoadAndCheckAccelProfilePRNfile(hDlg, szAccelFileNamesDir, idxMnoP1, nAccProfile, chHCFrecNoofUsteps4EachMotorFromTab, nHCFrecFrequencyOfPixelPrintsInHz))
   return 1000002;// <-- L oadAndCheckAccelProfilePRNfile failed
/*
 //if(myOpen_File(hDlg, "prn", szAccelFileNamesDir, UNUSED_MOTORNO))
 //  return 1000002;// <-- The function failed
 ////Do Checks to help ensure Acceleration Profile loaded from file is OK
 //if((nAccProfile[0] < 1)||(nAccProfile[0] > 32767)){
 //   sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
 //   MessageBox(hDlg,"The Acceleration Profile must not start with < 1 uStep\nor > 32,767 uSteps",mes,MB_OK);//"GenerateProfileForM%d failed"
 //   return 1000003; //G enerateProfileForMxx() failed!
 //  }
 //for(j = 1; j < NOOFaccelPROFILES; j++)
 //  if( (nAccProfile[j] < nAccProfile[j-1]) || (nAccProfile[j] > 32767) ){
 //     sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
 //     MessageBox(hDlg,"The Acceleration Profile must not contain decreasing speeds\nor numbers > 32,767",mes,MB_OK);//"GenerateProfileForM%d failed"
 //     return 1000004; //G enerateProfileForMxx() failed!
 //    }
*/


 int shStepsToMoveDividedBy2; //This code *expects* the number of steps to move between rows to be a short int. Note the temporary reuse of this variable for a couple of lines to ensure this is the case.

                      //The line spacing in Nanometers is rounded down to the nearest whole number of uSteps, and this number is used as the spacing between all lines:
 //shStepsToMoveBetweenRows = (short)(uiHCFrecVertDistBetwPixelCenters_nanoM * dblStepsPerNanoM); // ...(Note this code is used in GenerateStartPositionsFile() also - keep identical!)
 shStepsToMoveDividedBy2 = (int)(uiHCFrecVertDistBetwPixelCenters_nanoM * dblStepsPerNanoM);
 if(shStepsToMoveDividedBy2 > 0x7fff){
    MessageBox(hDlg,"The Assumption that the number of steps to move between rows\nis ALWAYS <= 32,767 uSteps failed using the current settings\n\nYou may need to reduce uSteps/Step, the distance between rows, or amend the program","Error",MB_OK);//"GenerateProfileForM%d failed"
    return 1000003;
   }
 else
   shStepsToMoveBetweenRows = (short)(uiHCFrecVertDistBetwPixelCenters_nanoM * dblStepsPerNanoM); // ...(Note this code is used in GenerateStartPositionsFile() also - keep identical!)

//int shStepsToMoveDividedBy2 = shStepsToMoveBetweenRows / 2;
 shStepsToMoveDividedBy2 = shStepsToMoveBetweenRows / 2;

 //From the nAccProfile[] table, build a complementary nAccProfileCumulative[], that contains the Cumulative distance travelled to get to that point, when accelerating using that profile.
 int nTotalStepsToThisPoint = 0;
 int nFoundStopAccelPoint = -1;
 for(j = 0; j < NOOFaccelPROFILES; j++){
    nTotalStepsToThisPoint += nAccProfile[j];
    nAccProfileCumulative[j] = nTotalStepsToThisPoint; //builds the 2nd table listed below.
   }
//User requests, for example, 10412 usteps should be performed within the leadout.
//100,200,300, 400, 500, 600, 700, 800, 900,1000,900,800,600,500,412,400,300,200,100                          112, 100,100,100,100
//100,300,600,1000,1500,2100,2800,3600,4500,5500
 memset( pMxPacketBuffAr[39], 0, 4400); //Make sure the buffer we're about to write into is clear before we start
 BOOL bTheProfileIsCreated =FALSE;
 for(j = 0; j < HALFMAXLEADINorOUT; j++)
   if( nAccProfileCumulative[j] >= shStepsToMoveDividedBy2){
      nFoundStopAccelPoint = j;
      j = NOOFaccelPROFILES;
     }//if (nAccProfileCumulative[j] >= shStepsToMoveDividedBy2)
 if(nFoundStopAccelPoint == -1){
    MessageBox(hDlg,"It is not possible get to the half way shStepsToMoveDividedBy2.\nThe Acceleration Profile doesn't get there quick enough.","GenerateProfileForM40 failed",MB_OK);
    return 1000005; //G enerateProfileForMxx() failed!
   }
 if(nFoundStopAccelPoint == 0){
    if(shStepsToMoveBetweenRows <= nAccProfileCumulative[0]){
       *((short*)(pMxPacketBuffAr[39])) = shStepsToMoveBetweenRows;
       nLengthOfCalculatedMotorMovementInPixPeriods = 1;
      }
    else{
       *((short*)(pMxPacketBuffAr[39])) = (short)nAccProfileCumulative[0] ;
       *((short*)(pMxPacketBuffAr[39]) + 1) = (short)(shStepsToMoveBetweenRows - nAccProfileCumulative[0]);
       nLengthOfCalculatedMotorMovementInPixPeriods = 2;
      }
    bTheProfileIsCreated = TRUE;
   }
 else if( (nAccProfileCumulative[nFoundStopAccelPoint] + nAccProfileCumulative[nFoundStopAccelPoint -1]) > shStepsToMoveBetweenRows) //Can we brake,if we use the current nFoundStopAccelPoint?
   nFoundStopAccelPoint--;     //No, we can't brake, so choose the previous one

 if(!bTheProfileIsCreated){
    int nRemainder;
    if(nFoundStopAccelPoint == 0){ //Although unlikely, nFoundStopAccelPoint could be zero here!!  There must be such a small 'shStepsToMoveBetweenRows' (less than the user is ever likely e.g. ~ 1/14 mm)
       //Just divide shStepsToMoveBetweenRows by 9 (say) into an int, and produce 9 equal movements. Then add a tenth with the remainder.   cheap & nasty!
       div_t OneNinthSteps = div(shStepsToMoveBetweenRows, 9);
       for(int dv = 0; dv < 9; dv++)
         *((short*)(pMxPacketBuffAr[39]) + dv) = (short)OneNinthSteps.quot;
       *((short*)(pMxPacketBuffAr[39]) + 9) = (short)OneNinthSteps.rem;
       nLengthOfCalculatedMotorMovementInPixPeriods = 10;
      }
    else{
       for(int k = 0; k <= nFoundStopAccelPoint; k++){
          *((short*)(pMxPacketBuffAr[39]) + k) = (short)nAccProfile[k];
          nLengthOfCalculatedMotorMovementInPixPeriods++;
         }
       if( ( 2 * nAccProfileCumulative[nFoundStopAccelPoint] ) <= shStepsToMoveBetweenRows){
          //we can completely mirror the acceleration profile for deceleration ... Note we will have to insert the remainder in the appropriate part of the deceleration
          nRemainder = shStepsToMoveBetweenRows - (2 * nAccProfileCumulative[nFoundStopAccelPoint]);
          if(nRemainder){ //we need to insert it
             BOOL bRemainderNotInserted = TRUE;
             int kContinued = nFoundStopAccelPoint + 1;
             if(nRemainder >= nAccProfile[nFoundStopAccelPoint + 1] ){ //We don't think the remainder should be > the next fastest point in the motion profile
                MessageBox(hDlg,"nRemainder > nAccProfile[nFoundStopAccelPoint + 1]\nMOTOR WOULD BE LIKELY TO STALL\n\nFunction failed!","GenerateProfileForM40 - Highly suspicious nRemainder Processing!!",MB_OK);
                return 1000006;
               }
             for(int k = nFoundStopAccelPoint; k > -1; k--){
                if((nRemainder > nAccProfile[k]) && bRemainderNotInserted){
                   *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nRemainder;
                   bRemainderNotInserted = FALSE;
                   k = k + 1;
                   nLengthOfCalculatedMotorMovementInPixPeriods++;
                  }
                else{
                   *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nAccProfile[k];
                   nLengthOfCalculatedMotorMovementInPixPeriods++;
                  }
               }
             if(bRemainderNotInserted){ //because it remainder isnt > than any 
                *((short*)(pMxPacketBuffAr[39]) + kContinued) = (short)nRemainder;
                nLengthOfCalculatedMotorMovementInPixPeriods++;
               }
            }
          else{//Just mirror it
             int kContinued = nFoundStopAccelPoint + 1;
             for(int k = nFoundStopAccelPoint; k > -1; k--){
                *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nAccProfile[k];
                nLengthOfCalculatedMotorMovementInPixPeriods++;
               }
            }
         }
       else{ //Do not repeat maximum acceleration value, just use all previous accel values in reverse order ... Note we will have to insert the remainder in the appropriate part of the deceleration
          nRemainder = shStepsToMoveBetweenRows - (nAccProfileCumulative[nFoundStopAccelPoint] + nAccProfileCumulative[nFoundStopAccelPoint - 1] );
          if(nRemainder){ //we need to insert it
             BOOL bRemainderNotInserted = TRUE;
             int kContinued = nFoundStopAccelPoint + 1;
             if(nRemainder >= nAccProfile[nFoundStopAccelPoint + 1] ){ //We don't think the remainder should be > the next fastest point in the motion profile
                MessageBox(hDlg,"nRemainder > nAccProfile[nFoundStopAccelPoint + 1]\nMOTOR WOULD BE LIKELY TO STALL\n\nFunction failed!","GenerateProfileForM40A - Highly suspicious nRemainder Processing!!",MB_OK);
                return 1000008;
               }
             for(int k = nFoundStopAccelPoint - 1; k > -1; k--){
                if((nRemainder > nAccProfile[k]) && bRemainderNotInserted){
                   *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nRemainder;
                   bRemainderNotInserted = FALSE;
                   k = k + 1;
                   nLengthOfCalculatedMotorMovementInPixPeriods++;
                  }
                else{
                   *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nAccProfile[k];
                   nLengthOfCalculatedMotorMovementInPixPeriods++;
                  }
               }
             if(bRemainderNotInserted){ //because it (the remainder) isnt > than any 
                *((short*)(pMxPacketBuffAr[39]) + kContinued) = (short)nRemainder;
                nLengthOfCalculatedMotorMovementInPixPeriods++;
               }
            }
          else{//Just mirror it
             int kContinued = nFoundStopAccelPoint + 1;
             for(int k = nFoundStopAccelPoint; k > 0;   ){
                *((short*)(pMxPacketBuffAr[39]) + kContinued++) = (short)nAccProfile[--k];
                nLengthOfCalculatedMotorMovementInPixPeriods++;
               }
            }
         }//else "Do not repeat maximum acceleration value,"
      }//else the "Although unlikely, nFoundStopAccelPoint could be zero here!!" was not the case
   }//!bTheProfileIsCreated
 if(nFoundStopAccelPoint < 0){ //It's not possible to accelerate to the required speed.
    MessageBox(hDlg,"It is not possible to accelerate to the required speed/distance.\nThe Acceleration Profile doesn't go that fast (by halfway point).","GenerateProfileForM40 failed",MB_OK);
  //sprintf_s(mes,SIZMES,"Mid-point speed in file :\n\n%s\n\nwas %d uSteps per Pixel Period.\n\nRequired uSteps was %d", szAccelFileNamesDir, nAccProfile[NOOFaccelPROFILES - 1], nSpeedAtStartOfLine);
  //MessageBox(hDlg,mes, "Further Info for GenerateProfileForMotor failure...", MB_OK);
    return 1000009; //G enerateProfileForMxx() failed!
   }
 if(nLengthOfCalculatedMotorMovementInPixPeriods == 0){
    MessageBox(hDlg,"A path was found through the code that didn't increment nLengthOfCalculatedMotorMovementInPixPeriods!!","GenerateProfileForM40 failed",MB_OK); //(or set it > 0)
    return 1000010; //G enerateProfileForMxx() failed!
   }
 if(nLengthOfCalculatedMotorMovementInPixPeriods > nLeadInForM39_40){
    int nLength_mes;
    memset( mes, 0, SIZMES);
    StringCchCopy(mes,SIZMES,"Optimum YaxisMovePeriod takes longer than Xaxis LeadIn or LeadOut.\n\nXaxis LeadIn or LeadOut is: ");
    nLength_mes = lstrlen(mes);
    _itoa_s( nLeadInForM39_40, mes + nLength_mes, SIZMES - nLength_mes, 10);
    StringCchCat(mes,SIZMES,"\n\nnLengthOfCalculatedMotorMovementInPixPeriods is: ");
    nLength_mes = lstrlen(mes);
    _itoa_s(nLengthOfCalculatedMotorMovementInPixPeriods, mes + nLength_mes, SIZMES - nLength_mes, 10);
    StringCchCat(mes,SIZMES,"\n\nSuggestions are:\n-Increase NOOFprePIXzeroCONSTspeedPIXELS (for X axis)\n-Increase M40s acceleration profile (but only if possible)\n-Decrease the pixel frequency");
    MessageBox(hDlg,mes,"GenerateProfileForM40 failed, Y Axis Profile cannot be completed in the Xaxis LeadOut period!",MB_OK);
    return 1000011;
   }
 int nPixPeriodYaxisStartAcceleratingAt = nLengthOfM39rowsIncLeadInOut_inPixels - nLeadInForM39_40;
 memmove(pMxPacketBuffAr[39] + (nPixPeriodYaxisStartAcceleratingAt * 2), pMxPacketBuffAr[39], (nLengthOfCalculatedMotorMovementInPixPeriods * 2));
 memset( pMxPacketBuffAr[39], 0, ( nLeadInForM39_40 + nHCFrecNoofCols -1) * 2);

 for(int nEachLine = 1; nEachLine < (nHCFrecNoofLinesInc2TimesBetweenTheBeams - 1); nEachLine++)        //Copy the calculated y-axis profile from row 0 to all other rows apart from the last
   memcpy ( pMxPacketBuffAr[39] + (nEachLine * 4400), pMxPacketBuffAr[39],4400);

 memset( pMxPacketBuffAr[39] + ((nHCFrecNoofLinesInc2TimesBetweenTheBeams - 1) * 4400), 0, 4400);       //y-axis profile for the last row is all zeros (it does not increment at the end)

//This was is GenerateStartPositionsFile()
  //double dblStepsPerNanoM = (double)500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[39] * 7.2 / (YAXIS_PITCH * 1000000)  ; //500 is the steps per rev for the 5 phase stage motors. 7.2 is Gear Ratio
  //short  shStepsToMoveBetweenRows = (short)(HCFrec.uiVertDistBetwPixelCenters_nanoM * dblStepsPerNanoM);  //(Note this code is used in GenerateProfileForM40() also - keep identical!)
    //Needn't do either of the above 2 statements - this function has already done them

    int nStepsPerMilliMeter = (int)(dblStepsPerNanoM * 1000000);
  //int nStepsToHologramsOriginY = nStepsPerMilliMeter * HCFrec.YposOfHologsOriginOnPlateMM;
    int nStepsToHologramsOriginY = nStepsPerMilliMeter * nHCFrecYposOfHologsOriginOnPlateMM;
 //The AVRs alway think of their positions in absolute uStep coordinates whose origin is Home.  AVRs will be able to travel a short negative distance until they hit the limit switch - The is the Physical Limits Min.
 //The Physical Limits Min will change slightly if the stage, homing switch or limit switch is physically adjusted / tampered with, or if the noof uSteps/Step is changed.  In principle, Point (0,0) may be
 //in negative territory, but it cannot be less than the Physical Limit Min [actually I suppose it could be, as long as we didn't mind that we could never move to (0,0)].  The user will specify their holograms
 //relative to point (0,0) and what's more may well end up with required positions on the negative side of that when the acceleration profile is calculated.  However, we must not let them use profiles that violate
 //the physical limits.
  //int nTrueNoofRowsIncBetweenTheBeams = HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams;  //now nHCFrecNoofLinesInc2TimesBetweenTheBeams
    int nSPos;
    for(nSPos = 0; nSPos < nHCFrecNoofLinesInc2TimesBetweenTheBeams; nSPos++)
      nLineXstartPositionsFEM[39][nSPos] = nStepsToHologramsOriginY + (nSPos * shStepsToMoveBetweenRows);
    if(nLineXstartPositionsFEM[39][0] < PCFrec.nPhysLimMinUsteps[39]){
       StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(PCFrec.nPhysLimMinUsteps[39] - nLineXstartPositionsFEM[39][0],mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Distance Error: ");
       double dblStepsPerMM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab * 7.2 / YAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       double dblMMperStep  = 1 / dblStepsPerMM;
       sprintf_s(mes+lenMes+22, SIZMES - lenMes - 22, "%3.3f mm", dblMMperStep * (PCFrec.nPhysLimMinUsteps[39] - nLineXstartPositionsFEM[39][0]) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 40 (XYstage - Y) would have to move outside it's 'MINIMUM Physical Limit'\nby the above number of micro steps (& distance)\n\nDo you need suggestions?",mes,MB_YESNO)){
          nHelpMessageToLoad = 5;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1000012;
      }
    if(nLineXstartPositionsFEM[39][nHCFrecNoofLinesInc2TimesBetweenTheBeams - 1] > PCFrec.nPhysLimMaxUsteps[39]){
       StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(nLineXstartPositionsFEM[39][nHCFrecNoofLinesInc2TimesBetweenTheBeams - 1] - PCFrec.nPhysLimMaxUsteps[39],mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Distance Error: ");
       double dblStepsPerMM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab * 7.2 / YAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       double dblMMperStep  = 1 / dblStepsPerMM;
       sprintf_s(mes+lenMes+22, SIZMES - lenMes - 22, "%3.3f mm", dblMMperStep * (nLineXstartPositionsFEM[39][nHCFrecNoofLinesInc2TimesBetweenTheBeams - 1] - PCFrec.nPhysLimMaxUsteps[39]) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 40 (XYstage - Y) would have to move outside it's 'MAXIMUM Physical Limit'\nby the above number of micro steps (& distance)\n\nRemember, True Y distance includes 2 * nNoofRowsBetweenBeams\n\nDo you need suggestions?",mes,MB_YESNO)){
          nHelpMessageToLoad = 5;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1000013;
      }


 FillInHeaderForGeneratedFile(nHCFrecNoofLinesInc2TimesBetweenTheBeams, nHCFrecNoofCols, nHCFrecFrequencyOfPixelPrintsInHz, nLeadInForM39_40, ptr_HCFrec_uTimeStamp, szHCFrecHologramName);

 StringCchCopy(szLocalM40ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from but, when generating a hologram, we write to it.
 StringCchCat(szLocalM40ProfileFileName,_MAX_DIR,FILENAMEStab[39]);//"<HologramDIR>\\CSVs\\M40XYStageY.csb"
 int nLen = lstrlen(szLocalM40ProfileFileName);
 szLocalM40ProfileFileName[nLen - 1] = 'v';
 pCurrentProfileGenerationBuffer = pMxPacketBuffAr[39];
 mySave_File (hDlg, MAXNOOFPIXELS_IN_ROW * MAX_NOOFLINES * sizeof(int) + SIZ_CSV_HEADER, "csv",TYPE5CSV, szLocalM40ProfileFileName, MOTORNO_NOT_USED); //returns zero if file saved OK
//nchCurrentMotorZB = 39; surely just a frig to make m ySave_File() work properly... no longer needed because m yOpen_File() forwards this as a parameter
 memset(pMxPacketBuffAr[39],0,SIZ_8832000);
 myOpen_File(hDlg, "csv", szLocalM40ProfileFileName, 39);//The sole reason for calling this, here, is to reopen the csv so that it gets saved as a csb
 return 0; //returns zero if file saved OK
}
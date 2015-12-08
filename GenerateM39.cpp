#include "stdafx.h"
int nLengthOfM39rowsIncLeadInOut_inPixels;     //A return value from this function for m ySave_File //This is also an essential value for the other Generate functions, so they know how long they've got!

extern char *pCurrentProfileGenerationBuffer;  //Tells my SaveFile() where the data is (csb)

extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];

extern HINSTANCE hInst;
extern int nHelpMessageToLoad;
extern PCFRECORD PCFrec;


extern char *pMxPacketBuffAr[NOOFMOTORS];     //Where to put the data (ram)
extern HCFRECORDsCURRENTfolders HCFcfs;       //Where to put the data (file)
//extern char nchCurrentMotorZB;              //Where to put the data (file, indirectly: it tells mySaveFile which buffer to write to file) (csv)
extern char FILENAMEStab[][16];               //Where to put the data (file)

extern char mes[];                         //working storage

char szAccelFileNamesDir[_MAX_DIR];        //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//int  nLenszAccelFileNamesDir;              // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.
//char szTabProfileFileNameUstepEndings[7][8] ={"-1.prn","-2.prn","-4.prn","-8.prn","-16.prn","-32.prn","-64.prn"}; //e.g. "AccProfM39-30Hz-64.prn"
//char szTabProfileFileNameFreqEndings[4][5] ={"10Hz","15Hz","30Hz","60Hz"}; //e.g. "AccProfM39-30Hz"
int  nAccProfile[SIZaccelPROFILES]; //This table has 2 * NOOFaccelPROFILES entries. (first half for the motor accelerating forwards, second half for the motor accelerating backwards)

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39     M39  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//The profile generated here is how many uSteps to move in each pixel period.  This is another way of saying "where the motor should be at the end of each pixel period".
//Note this is independent of the Pixel Frequency... If we print at 60 Hz instead of 30 Hz the pixel periods will be half the length, but we will still move a fixed
//number of pixels in each period. When the period is halved at 60 Hz the motor will be going at twice the speed (as it still does the same number of steps / period).
//M39 has to move a known distance (which is provided by the user as nHCFrecNoofCols and nHCFrecHorzDistBetwPixelCenters_nanoM), plus an 'unknown' accel/decel distance.
//
//
//
//Returns zero for success, else an error code.
int GenerateProfileForM39(HWND hDlg,int nHCFrecNoofLines,
                                 int    nHCFrecNoofCols, //When Summed (-1) with LeadIn and LeadOut, equals true number of cols (as far as motion is concerned).
                                                         // cols are closer together during the accel periods
                                                         //The true number of cols are returned in the global n LengthOfM39rowsIncLeadInOut_inPixels
                                 int    nNOOFprePIXzeroCONSTspeedPIXELSxAxis,
                                 int    nHCFrecFrequencyOfPixelPrintsInHz, //Used to choose the Accel Profile, and also put in the csv/csv header.
                                 char   chHCFrecNoofUsteps4EachMotorFromTab,
                                 int    nHCFrecHorzDistBetwPixelCenters_nanoM,
                                 time_t *ptr_HCFrec_uTimeStamp,
                                 char   *szHCFrecHologramName,
                                 int    nHCFrecXposOfHologsOriginOnPlateMM,
                                 int    nHCFrecNoofRowsBetweenBeams,
                                 int    nM39_XAXISBACKLASH,
                                 int    *ptrPixelAtwhichAccelStops){
int nLengthOfM39rowsIncLeadInOut_inUsteps; //no longer a return value from this function for G enerateStartPositionsFile as this function now update nLineXstartPositionsFEM[][] directly
int nLengthOfM39rowsJustLeadIn_inUsteps;   //no longer a return value from this function for G enerateStartPositionsFile as this function now update nLineXstartPositionsFEM[][] directly

 char szLocalM39ProfileFileName[_MAX_DIR]; //"<HologramDIR>\\CSVs\\M39XYStageX.csv"
//If x axis is quarter-stepping, we would have 2000 pulses per rev, and each pulse would move 1/1000 mm.  If the distance between pixels is
//  1.6 mm and we print @ 30Hz frequency of pulses is 30 * 1600 (== 48 KHz).
 __int64 n64PixDistInNanoMM;
 __int64 n64LastPixDistInNanoMM;
//double dblStepsPerNanoM = (double)(500.0f * chHCFrecNoofUsteps4EachMotorFromTab / XAXIS_PITCH) / 1000000; //500 is the steps per rev for the 5 phase stage motors.
 double dblStepsPerNanoM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab / (XAXIS_PITCH * 1000000); //500 is the steps per rev for the 5 phase stage motors.
 double dblNanoMperStep  = 1 / dblStepsPerNanoM;
 short  shStepsToMove;
 int nDistanceInNanoMetresToMove;
 int nLeftOver = 0;
 int nTotalPixelNo; //e.g. 0 to 999,999 for a 1000 * 1000 hologram (used as the index into pMxPacketBuffAr[39)

 int nSpeedAtStartOfLine = (int)(nHCFrecHorzDistBetwPixelCenters_nanoM * dblStepsPerNanoM); //i.e. the number of steps between the 1st two printable pixels

 //nSpeedAtStartOfLine = 390; //last steps was 375 (but we still need an acceleration, between 380 & 390)
 //nSpeedAtStartOfLine = 391; //last steps was 385 (but we still need an acceleration, between 390 & 391)
 //nSpeedAtStartOfLine = 398; //last steps was 385 (but we still need an acceleration, between 390 & 399)
 //nSpeedAtStartOfLine = 400; //last steps was 385 (but we still need an acceleration, between 390 & 400)
 //nSpeedAtStartOfLine = 401; //last steps was 395 (but we still need an acceleration, between 400 & 401)

 if(nSpeedAtStartOfLine > 32767){
    MessageBox(hDlg,"nSpeedAtStartOfLine > 32767, i.e. the number of steps between the 1st two printable pixels is too big!","GenerateProfileForM39 failed",MB_OK);
    return 1000007; //G enerateProfileForMxx() failed!
   }

 //if(HCFrec.uiHorzDistBetwPixelCenters_nanoM < HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH){
 if(nSpeedAtStartOfLine <= nM39_XAXISBACKLASH){
     MessageBox(hDlg,"M39 X-AXIS BACKLASH must be less than the spacing between pixels\n\nEither, or both:\n-increase the pixel spacing: uiHorzDistBetwPixelCenters_nanoM\n-decrease the backlash in <PrinterConfig>\n\n  and then regenerate\n\n(more info follows)","Excessive Backlash setting was specified",MB_OK);
    sprintf_s(mes,SIZMES,"uSteps between pixels (with your current pixel spacing) is %d\n\nBacklash setting is %d",nSpeedAtStartOfLine,nM39_XAXISBACKLASH);
    MessageBox(hDlg,mes,"further info",MB_OK);
    return 1000010;
   }

 //ReadAccelerationProfile(M39)
 //For arguments sake, assume an acceleration profile always consists of 100 points.  The deceleration profile can be the opposite of this: the same points, but backwards.
 //It could be something like: 10,20,29,38,47,55,62,66,68,69,69,70,70,70,70... 70  (There's no reason why acceleration can't level off to horizontal)
 //Forgetting about files for the time being, I'll hardcode the acceleration table:

 int j;
 int idxMnoP1 = 38 + 1;

 /*
 int COULD_LOAD_FROM_FILE;
     //THE FOLLOWNG CREATES AN ACCELERATION PROFILE - ***WE SHOULD REALLY LOAD IT FROM A FILE***  (and now do!)
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
 StringCchCat(szAccelFileNamesDir,_MAX_DIR,"AccProfM39-");
 int nIdxToFileNameUstepEndings = DecodeNoofUstepsToAnIndex(chHCFrecNoofUsteps4EachMotorFromTab);
 int nIdxToFileNameFreqEndings =  DecodeFreqToAnIndex(nHCFrecFrequencyOfPixelPrintsInHz);
 if((nIdxToFileNameUstepEndings == 1000001)||(nHCFrecFrequencyOfPixelPrintsInHz == 1000001)){
    MessageBox(hDlg,"Invalid chHCFrecNoofUsteps4EachMotorFromTab or nHCFrecFrequencyOfPixelPrintsInHz parameter","GenerateProfileForM39 failed",MB_OK);
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
 //What we need to do is get up to the required speed by the time we get to the first printable pixel.  Effectively we should be able to shift the nAccProfile
 // array to the right if we want to minimize the time taken to accelerate (e.g. shift right by 60 elements, then start at element 60). Or, if we wanted to 
 // maintain a constant speed for, say, 30 pixels we could shift left again by 30 and duplicate the last 30 elements.
 //First, find the position of the first speed >= to nSpeedAtStartOfLine:
 int nFoundMinNoofElements = -1;
 for(j = 0; j < NOOFaccelPROFILES; j++)
   if( nAccProfile[j] >= nSpeedAtStartOfLine){
      nFoundMinNoofElements = j;
      j = NOOFaccelPROFILES;
     }
 if(nFoundMinNoofElements < 0){ //It's not possible to accelerate to the required speed.
    MessageBox(hDlg,"It is not possible to accelerate to the required speed.\nThe Acceleration Profile doesn't go that fast.","GenerateProfileForM39 failed",MB_OK);
    sprintf_s(mes,SIZMES,"Fastest speed in file :\n\n%s\n\nwas %d uSteps per Pixel Period.\n\nRequired uSteps was %d", szAccelFileNamesDir, nAccProfile[NOOFaccelPROFILES - 1], nSpeedAtStartOfLine);
    MessageBox(hDlg,mes, "Further Info for GenerateProfileForMotor failure...", MB_OK);
    return 1000005; //G enerateProfileForMxx() failed!
   }
 if(nFoundMinNoofElements + nNOOFprePIXzeroCONSTspeedPIXELSxAxis > MAXLEADINorOUT){
    StringCchCopy(mes, SIZMES, "Accelerating to the required speed and maintaining the speed for\nnNOOFprePIXzeroCONSTspeedPIXELS\nexceeds the 100 pixel LeadIn limit.\nNOOFprePIXzeroCONSTspeedPIXELSxAxis needs to be reduced to: ");
    _itoa_s(MAXLEADINorOUT - nFoundMinNoofElements,mes+193,SIZMES - 193,10);
    StringCchCat(mes,SIZMES," (or Less)");
    MessageBox(hDlg,mes,"GenerateProfileForM39 failed",MB_OK);
    return 1000006; //G enerateProfileForMxx() failed!
   }

 memset( pMxPacketBuffAr[38], 0, 4400); //Make sure the buffer we're about to write into is clear before we start
 //As nAccProfile[nFoundMinNoofElements] is a speed that is *at least* as fast as the speed we're trying to get to, we don't need to include the final element
 // in the profile. e.g. if element zero is as fast as, or faster than, the number of steps between Pixel 0 and Pixel 1 we don't need to get up to the speed
 // of element zero, we could just print pixel zero at linestart, then move to pixel 1.  In reality, though, we will always have an acceleration with at least
 // one pixel of constant speed at the end (and this speed should be the speed, between pixel 0 and pixel 1).
 //int nNOOFprePIXzeroCONSTspeedPIXELSxAxis = 2;//NEEDS TO BE VARIABLE!!!!(Is now in HCFRec)

//TEMP - JUST SO I CAN VIEW nAccProfile
 int See_nAccProfile[200];
 for(int y = 0; y < 200; y++)
   See_nAccProfile[y] = nAccProfile[y];
 int SeepMxPacketBuffAr[4400];//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Just for Debug xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 int tempJUST_SO_I_CAN_VIEW_nAccProfile;

 int betterAccelProf;
 //Now put the acceleration up to the 'constant speed before Pixel zero' points in (from the nAccProfile[] table up to the nFoundMinNoofElements point). NO! not anymore! Under the new regime nAccProfile is considered to 
 //be truely an acceleration profile (for M39 only)- which gives the speed at the end of the pixel. But we are still going to send the number of steps to move (even though the file we've read in, contains the final
 //speed of the motor at the end of the pixel). We need to calculate what the 'Number of Steps to Move For Each Pixel' is... (but we'll only calculate this for begining part of the table, up to where the motor is up to
 //the required constant speed)
 int idxToTheAccProfilesWeWillUse = nFoundMinNoofElements - 1;
 int NoofSteps2Move4EachPixel[100];
 int nPreviousPixelSpeed = 0;
 for(int n = 0; n < nFoundMinNoofElements; n++){
    NoofSteps2Move4EachPixel[n] = (nPreviousPixelSpeed + nAccProfile[n]) / 2;
    nPreviousPixelSpeed = nAccProfile[n];
   }
 if(!nM39_XAXISBACKLASH){
    for(betterAccelProf = idxToTheAccProfilesWeWillUse; betterAccelProf > -1; betterAccelProf--)
      *((short*)(pMxPacketBuffAr[38]) + betterAccelProf) = (short)NoofSteps2Move4EachPixel[idxToTheAccProfilesWeWillUse--]; //was nAccProfile[]
    //With nAccProfile containing number like 370,380,390,400; NoofSteps2Move4EachPixel contains numbers like 365,375,385,395. 
    //nSpeedAtStartOfLine = 390; //last steps was 375 (but we still need an acceleration, between 380 & 390)  //The above code gives these results, which shows one more
    //nSpeedAtStartOfLine = 391; //last steps was 385 (but we still need an acceleration, between 390 & 391)  // acceleration is needed, but it will probably be
    //nSpeedAtStartOfLine = 399; //last steps was 385 (but we still need an acceleration, between 390 & 399)  //  reduced acceleration, as shown here. 
    //nSpeedAtStartOfLine = 400; //last steps was 385 (but we still need an acceleration, between 390 & 400)
    //nSpeedAtStartOfLine = 401; //last steps was 395 (but we still need an acceleration, between 400 & 401)
    //So let's add in the one last acceleration:
    *((short*)(pMxPacketBuffAr[38]) + nFoundMinNoofElements) = (short)(((nSpeedAtStartOfLine - nAccProfile[nFoundMinNoofElements - 1]) / 2) + nAccProfile[nFoundMinNoofElements - 1]);
    nFoundMinNoofElements += 1;
    //The only problem with the above is that if the desired final speed is an odd number of steps per pixel, we will actually end up with a speed that is 1 less than this (if the AVR assumes a linear acceleration
    // thoughout the pixel). We can't really specify the number of steps to move during one pixel as anything other than a whole number, and if this is fed into a linear algorithm, the final speed always ends up
    // being even.  Of course, if the AVR is smart it could spot that the desired final speed is odd and make some adjustment to the TCs it generates for the final acceleration pixel so that the final speed is
    // correct (whilst still moving exactly the right number of steps). A far easier workaround is to restrict the user's choice of final speeds to only those speeds that are an even number of steps/pixel (or, at least,
    // advise them to use only those distances) This is virtually no different to advising them that the distance between pixel centres should be a whole number of steps [due to mechanical constraints], except I'll
    // be advising them that the distance between pixel centres should be a whole EVEN number of steps [due to software constraints]).
   }
 else{
    *((short*)(pMxPacketBuffAr[38]) + 0) = 0;
    for(betterAccelProf = idxToTheAccProfilesWeWillUse; betterAccelProf > -1; betterAccelProf--)
      *((short*)(pMxPacketBuffAr[38]) + betterAccelProf + 1) = (short)NoofSteps2Move4EachPixel[idxToTheAccProfilesWeWillUse--]; //was nAccProfile[]

    *((short*)(pMxPacketBuffAr[38]) + nFoundMinNoofElements + 1) = (short)(((nSpeedAtStartOfLine - nAccProfile[nFoundMinNoofElements - 1]) / 2) + nAccProfile[nFoundMinNoofElements - 1]);
    nFoundMinNoofElements += 2;
   }


 for(int y = 0; y < 4400; y++)//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Just for Debug xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   SeepMxPacketBuffAr[y] = *((short*)(pMxPacketBuffAr[38]) + y);

 //Put the 'constant speed before Pixel zero' points in:
 for(betterAccelProf = 0; betterAccelProf < nNOOFprePIXzeroCONSTspeedPIXELSxAxis; betterAccelProf++)
   *((short*)(pMxPacketBuffAr[38]) + betterAccelProf + nFoundMinNoofElements) = (short)nSpeedAtStartOfLine /* +1 */ ; // +1 is WRONG, BUT FOR DEBUGGING IT CAN CLEARLY SHOW THE 'constant speed before Pixel zero' points

 for(int y = 0; y < 4400; y++)//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Just for Debug xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   SeepMxPacketBuffAr[y] = *((short*)(pMxPacketBuffAr[38]) + y);

 BOOL bInExactUstepsPerPixel = 0;

 n64LastPixDistInNanoMM = 0;
 nTotalPixelNo = nNOOFprePIXzeroCONSTspeedPIXELSxAxis + nFoundMinNoofElements; //At this point, nTotalPixelNo is set to the LeadIn
 for (__int64 n64Pixel = 1; n64Pixel < nHCFrecNoofCols; n64Pixel++){ //declared as __int64 to avoid casting in the next row. Starting from 1 assumes no movement
    n64PixDistInNanoMM = n64Pixel * nHCFrecHorzDistBetwPixelCenters_nanoM;  // for the 1st pixel.  The absolute position in nano metres
    nDistanceInNanoMetresToMove = (int)(n64PixDistInNanoMM - n64LastPixDistInNanoMM);

    shStepsToMove = (short)(dblStepsPerNanoM * nDistanceInNanoMetresToMove);
    nLeftOver = (int)(nDistanceInNanoMetresToMove - dblNanoMperStep * shStepsToMove);
    *((short*)(pMxPacketBuffAr[38]) + nTotalPixelNo++) = shStepsToMove;
    if(shStepsToMove != nSpeedAtStartOfLine)
      bInExactUstepsPerPixel = 1;
    n64LastPixDistInNanoMM = n64PixDistInNanoMM - nLeftOver; //where we actually moved to
   }

 int nNextUp;      //if we're not happy with the speed the user has chosen, show them some recommended alternatives either side of their chosen pixel spacing.
 int nNextDown;
 if(bInExactUstepsPerPixel){ //if we're not happy with the speed the user has chosen, show them some recommended alternatives either side of their chosen pixel spacing.
    if(nSpeedAtStartOfLine & 0x1){
       nNextUp = nSpeedAtStartOfLine + 1;
       nNextDown = nSpeedAtStartOfLine - 1;
      }
    else{
       nNextUp = nSpeedAtStartOfLine + 2;
       nNextDown = nSpeedAtStartOfLine;
      }
    nNextUp = (int)((double)nNextUp / dblStepsPerNanoM);
    nNextDown  = (int)((double)nNextDown/ dblStepsPerNanoM);
    sprintf_s(mes,SIZMES,"Suggested Values: %d  or: %d", nNextDown, nNextUp);
    if(IDCANCEL == MessageBox(hDlg,"Abort Generation?\n\nFor M39, your chosen distance between pixel centers is not a whole number of uSteps.\nThis is NOT RECOMMENDED as it may introduce vibration and unequally spaced pixels.\n\nPress Cancel to abort (so you can change the spacing)\nPress OK if you really want to try this spacing", mes, MB_OKCANCEL)){
     //MessageBox(hDlg,"If half stepping, each uStep is 0.002 millimeters, if 1/4 stepping each uStep is 0.001 millimeters.\nPlease ensure uiHorzDistBetwPixelCenters_nanoM is set in multiples of these.\ne.g 800000, 802000 (if you're half stepping)","More Help", MB_OK);
       return 1000008; //G enerateProfileForM39 aborted by user
      }
   }
 else{ //User has chosen an exact number of UstepsPerpixel, but it might still be an odd number
    if(nSpeedAtStartOfLine & 0x1){
       nNextUp = nSpeedAtStartOfLine + 1;
       nNextDown = nSpeedAtStartOfLine - 1;
       nNextUp = (int)((double)nNextUp / dblStepsPerNanoM);
       nNextDown  = (int)((double)nNextDown/ dblStepsPerNanoM);
       sprintf_s(mes,SIZMES,"Suggested Values: %d  or: %d", nNextDown, nNextUp);
       if(IDCANCEL == MessageBox(hDlg,"For M39, your chosen distance between pixel centers requires an ODD number of uSteps per Pixel.\n\nThe acceleration will be smoother if you increase or decrease uiHorzDistBetwPixelCenters_nanoM slightly\n**to one of the values suggested above**\n\nThese values allow the motor to move between Pixels in an EVEN number of uSteps.\n\nPress Cancel to abort (so you can change the spacing)\nPress OK if you really want to try this spacing",mes,MB_OKCANCEL))
         return 1000009; //G enerateProfileForM39 aborted by user
      }
   }

 //Now copy the acceleration & 'constant speed before Pixel zero' (points from the beginning) to the end, but in reverse order
 idxToTheAccProfilesWeWillUse = nFoundMinNoofElements - 1;
 for(betterAccelProf = nNOOFprePIXzeroCONSTspeedPIXELSxAxis + nFoundMinNoofElements - 1; betterAccelProf > -1; betterAccelProf--)
   *((short*)(pMxPacketBuffAr[38]) + nTotalPixelNo++) = *((short*)(pMxPacketBuffAr[38]) + betterAccelProf);

 for(int y = 0; y < 4400; y++)//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Just for Debug xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   SeepMxPacketBuffAr[y] = *((short*)(pMxPacketBuffAr[38]) + y);

//Apart from the <= 100 pixel at beginning of first line there will also <= 100 pixels at end of first line (accel / decel)
 int nACCEL_DECEL_PROFILE_SIZE = nFoundMinNoofElements + nNOOFprePIXzeroCONSTspeedPIXELSxAxis;
 int nPulsesThisPixel;
 int FirstLineLen = nHCFrecNoofCols - 1 + nACCEL_DECEL_PROFILE_SIZE * 2;
 int nFirstLineLen = FirstLineLen;
 nLengthOfM39rowsIncLeadInOut_inPixels = FirstLineLen; //This is for m ySave_File... This is the only place this is set.
 //if(nM39_XAXISBACKLASH)
   //nLengthOfM39rowsIncLeadInOut_inPixels += 2;
 nLengthOfM39rowsIncLeadInOut_inUsteps = 0;            //These two are for G enerateStartPositionsFile
 nLengthOfM39rowsJustLeadIn_inUsteps = 0;              //We can subtract this (having converted it to mm) from HCFrec.XposOfHologsOriginOnPlateMM to get the true start position for the stage (i.e. Can
                                                       // the stage actually move to the physical start position required?
 nTotalPixelNo = 2200;
 for(int nReverseTheFirstLine = 0; nReverseTheFirstLine < FirstLineLen; nReverseTheFirstLine++){
    nPulsesThisPixel = *((short*)(pMxPacketBuffAr[38]) + --nFirstLineLen);
    *((short*)(pMxPacketBuffAr[38]) + nTotalPixelNo++) =  (short)(nPulsesThisPixel * -1);
    nLengthOfM39rowsIncLeadInOut_inUsteps += nPulsesThisPixel;
   }
 for(int nGetLeadIn_inUsteps = 0; nGetLeadIn_inUsteps < nACCEL_DECEL_PROFILE_SIZE; nGetLeadIn_inUsteps++){
    nPulsesThisPixel = *((short*)(pMxPacketBuffAr[38]) + nGetLeadIn_inUsteps);
    nLengthOfM39rowsJustLeadIn_inUsteps += nPulsesThisPixel;
   }

//These next two 'for' loops copied the first two lines (0/1) of the x-axis motion to lines 2/3, 4/5 etc. However, in the case of the x-axis, this isn't really necessary because each pair of lines is identical.
//Originally, this was done to keep the UC3 driver code the same for all axis, but now that the UC3 is to treat M39 differently (to allow smooth acceleration) there is less point in having these lines - although UC3
// would require modification.  This would allow the space and bandwidth to be used to send Part-Processed TC data instead (i.e. the acceleration), to reduce the processing workload on the AVR (which struggles
// floats and calculating square-roots, for example).  In theory, the positional data needn't be sent at all because the TC data should be sufficient; However as the positional data is also used to build the table
// of expected positions (used to maintain synchronisation) it would be easier to send the positional data as well.
//Difficulties with sending the positional data just once:
// - AVR will not be able to store it in the original position, because this memory is overwritten at the end of each line by psPosnDifferences
//   I guess this could be put at lines 1999 and 2000 of the hologram as it is unlikely a hologram this big will ever be printed.  It would be better, although trickier, to reserve memory especially for it.
// - The places in the code where the Expected Positions table is built will have to be modified to look to the new place the lines will be stored at.
//Difficulties with sending Part-Processed TC data:
// - This data will be moderately large and, if the hologram has very few lines (e.g. 2!) additional buffers will have to be sent.  Transfer_ALL_FilesAndPerform() will be broken.
// - mySave_File() will be broken.

 for(int eachEvenLine = 2; eachEvenLine < nHCFrecNoofLines; eachEvenLine += 2)
   memcpy( ( pMxPacketBuffAr[38] + (eachEvenLine * 4400)), pMxPacketBuffAr[38], 4400);
 for(int eachOddLine = 3; eachOddLine < nHCFrecNoofLines; eachOddLine += 2)
   memcpy( ( pMxPacketBuffAr[38] + (eachOddLine * 4400)), pMxPacketBuffAr[38] + 4400, 4400);

//I've now tested and timed the float/square-root processing on AVR and it seems that in a 'worse case' this may take only ~3 seconds.  If this can be performed entirely before the print starts, this
// is acceptable, so I'll forget sending Part-Processed TC data (at least for the time being).
//Two additional items of data still need to be sent:
// -The number of steps required to take up play in the lead-screw mechanism.
//  The spec requires this number to be >= 0 and <= the number of steps between one pixel and the next when printing at a constant rate.  If these steps are added once the motor is up to full speed (which is where the
//  UC3 shall add them) they can always be 'fitted in' in, at most, 1 pixel-period.  If no "play-steps" are to be added, there is no need to add this pixel-period when reporting the how long the lead-in/out is, but if
//  1 or more steps are added, the lead-in will always be extended by one pixel-period.
// -A number indicating  at which pixel the acceleration stops.  This could be when a pair of pixels is found that has an identical number of steps to be moved (but this would prevent acceleration profiles
//  being specified by the user that contain zero acceleration in parts of the profile) so it is better to state explicitly where the acceleration stops.  We cannot expect the user to choose a constant
//  speed that aligns exactly on an AccProf element, although they might. Eg AccProf is 50,100,150 etc. but user's constant speed could be 48, or 50, or 52 in these cases the acceleration should be said to 
//  stop after 1, 1 & 2 pixel_periods, respectively. Note this is the original nFoundMinNoofElements + 1, although we've now added 1 or 2 to this [2, if there's backlash].

//How to send those two extra items of data.  Well that's a real tough one... Looking at the current structure of messages there simply isn't any space in any packet that can be used!
//The **only** sensible thing to do is sent an extra packet of data (the other possibilities have been explored to the hilt!).  Finally a bit of inspiration... MsTypE_SET_NEXTLINE2PRINT is 
// always sent prior to the MPOS data and it has space for these two data items.  Should we store the data for that message here?  Well, HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH will be stored by the
// statement "memcpy(&(HCFrec.HCF_PCFrecCopy),&PCFrec,sizeof(PCFrec));" when IDC_VIEWHOLOGINFO_GENERATE_COMPLETE is pressed, but I will store the other bit here:
 if(nM39_XAXISBACKLASH)
   *ptrPixelAtwhichAccelStops = nFoundMinNoofElements - 1; // (along with the corresponding field in the HCF record).
 else
   *ptrPixelAtwhichAccelStops = nFoundMinNoofElements; // (along with the corresponding field in the HCF record).

 for(int y = 0; y < 4400; y++)//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx Just for Debug xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   SeepMxPacketBuffAr[y] = *((short*)(pMxPacketBuffAr[38]) + y);

//This was is GenerateStartPositionsFile()
  //int nStepsPerMilliMeter = (int)(500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38] / XAXIS_PITCH); //500 is the steps per rev for the 5 phase stage motors.
    int nStepsPerMilliMeter = (int)(500.0f * chHCFrecNoofUsteps4EachMotorFromTab / XAXIS_PITCH); //500 is the steps per rev for the 5 phase stage motors.

  //int nStepsToHologramsOriginX = nStepsPerMilliMeter * HCFrec.XposOfHologsOriginOnPlateMM;
    int nStepsToHologramsOriginX = nStepsPerMilliMeter * nHCFrecXposOfHologsOriginOnPlateMM;
    //The AVRs alway think of their positions in absolute uStep coordinates whose origin is Home.  AVRs will be able to travel a short negative distance until they hit the limit switch - The is the Physical Limits Min.
    //The Physical Limits Min will change slightly if the stage, homing switch or limit switch is physically adjusted / tampered with, or if the noof uSteps/Step is changed.  In principle, Point (0,0) may be
    //in negative territory, but it cannot be less than the Physical Limit Min [actually I suppose it could be, as long as we didn't mind that we could never move to (0,0)].  The user will specify their holograms
    //relative to point (0,0) and what's more may well end up with required positions on the negative side of that when the acceleration profile is calculated.  However, we must not let them use profiles that violate
    //the physical limits.
    int nSPos;
    int nTrueNoofRowsIncBetweenTheBeams = nHCFrecNoofLines + 2 * nHCFrecNoofRowsBetweenBeams;
    for(nSPos = 0; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2)
      nLineXstartPositionsFEM[38][nSPos] = PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nStepsToHologramsOriginX - nLengthOfM39rowsJustLeadIn_inUsteps;
    if(nLineXstartPositionsFEM[38][0] < PCFrec.nPhysLimMinUsteps[38]){
       StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(PCFrec.nPhysLimMinUsteps[38] - nLineXstartPositionsFEM[38][0],mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Distance Error: ");
       double dblStepsPerMM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab / XAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       double dblMMperStep  = 1 / dblStepsPerMM;
       sprintf_s(mes+lenMes+22, SIZMES - lenMes - 22, "%3.3f mm", dblMMperStep * (PCFrec.nPhysLimMinUsteps[38] - nLineXstartPositionsFEM[38][0]) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 39 (XYstage - X) would have to move outside it's 'MINIMUM Physical Limit'\nby the above number of micro steps (& distance)\n\nDo you need suggestions?",mes,MB_YESNO)){
          nHelpMessageToLoad = 4;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1100002; //don't return 1 - reserved for m ySave_File
      }
    for(nSPos = 1; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2) //2744, 102256
      nLineXstartPositionsFEM[38][nSPos] = PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nStepsToHologramsOriginX  - nLengthOfM39rowsJustLeadIn_inUsteps + nLengthOfM39rowsIncLeadInOut_inUsteps + nM39_XAXISBACKLASH;
    if(nLineXstartPositionsFEM[38][1] > PCFrec.nPhysLimMaxUsteps[38]){
       StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(nLineXstartPositionsFEM[38][1] - PCFrec.nPhysLimMaxUsteps[38],mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Distance Error: ");
     //double dblStepsPerMM = (double)500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[eAchM] / XAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       double dblStepsPerMM = (double)500.0f * chHCFrecNoofUsteps4EachMotorFromTab / XAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       double dblMMperStep  = 1 / dblStepsPerMM;
       sprintf_s(mes+lenMes+22, SIZMES - lenMes - 22, "%3.3f mm", dblMMperStep * (nLineXstartPositionsFEM[38][1] - PCFrec.nPhysLimMaxUsteps[38]) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 39 (XYstage - X) would have to move outside it's 'MAXIMUM Physical Limit'\nby the above number of micro steps (& distance)\n\nDo you need suggestions?",mes,MB_YESNO)){
          nHelpMessageToLoad = 4;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1100003;
      }

 FillInHeaderForGeneratedFile(nHCFrecNoofLines, nHCFrecNoofCols, nHCFrecFrequencyOfPixelPrintsInHz, nACCEL_DECEL_PROFILE_SIZE, ptr_HCFrec_uTimeStamp, szHCFrecHologramName);

 StringCchCopy(szLocalM39ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from but, when generating a hologram, we write to it.
 StringCchCat(szLocalM39ProfileFileName,_MAX_DIR,FILENAMEStab[38]);//"<HologramDIR>\\CSVs\\M39XYStageX.csb"
 int nLen = lstrlen(szLocalM39ProfileFileName);
 szLocalM39ProfileFileName[nLen - 1] = 'v';
 pCurrentProfileGenerationBuffer = pMxPacketBuffAr[38];
 mySave_File (hDlg, MAXNOOFPIXELS_IN_ROW * MAX_NOOFLINES * sizeof(int) + SIZ_CSV_HEADER, "csv",TYPE5CSV, szLocalM39ProfileFileName, MOTORNO_NOT_USED); //returns zero if file saved OK
//nchCurrentMotorZB = 38; surely just a frig to make m ySave_File() work properly... no longer needed because m yOpen_File() forwards this as a parameter
 memset(pMxPacketBuffAr[38],0,SIZ_8832000);
 myOpen_File(hDlg, "csv", szLocalM39ProfileFileName, 38);//The sole reason for calling this, here, is to reopen the csv so that it gets saved as a csb
 return 0; //returns zero if file saved OK
}
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
//extern char szTabProfileFileNameUstepEndings[7][8];// ={"1.prn","2.prn","4.prn","8.prn","16.prn","32.prn","64.prn"}; //e.g. "AccProfM11-64.prn"
//extern char szTabProfileFileNameFreqEndings[4][5]; // ={"10Hz","15Hz","30Hz","60Hz"}; //e.g. "AccProfM39-30Hz"
extern int  nAccProfile[];

double ldblArrayOfGonioAngles[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];//ie [MAXROWS][MAXCOLS]

//int nLengthOfM11rowsIncLeadInOut_inUsteps; //A return value from this function for G enerateStartPositionsFile
//int nLengthOfM11rowsJustLeadIn_inUsteps;   //A return value from this function for G enerateStartPositionsFile
//int nLengthOfM11rowsJustLeadOut_inUsteps;  //A return value from this function for G enerateStartPositionsFile

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11     M11  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
//Returns zero for success, else an error code.

//For the Goniometers, the movement can start at any Pixel period (or even not at all, depending on the profile that is requested).
//The acceleration to get to pixel zero should take place within 'LeadIn Pixels'.  To some extent, the start postitions can be moved [or set] so that 
//the motors will be at the right place after however many pulses occur during the lead in.  This is achieved by having the previous row's LeadOut stop in the right place.

//M39  X-axis: NOOFprePIXzeroCONSTSpeedPIXELS = 4          e.g. 20 cols give 19 movements  Pixel spacing 0.9mm between centers
//100,200,300,400,500,600,700,800,900,900,900,900     900,900,900,900,900,900,900,900,900,900,900,900,900,900,900,9900,900,900,90000,900,900,900,        ,800,700,600,500,400,300,200,100
//
//M40  Y-axis:                      e.g. 20 cols give 19 movements
//0,0,0,0,0,0,0,0,0,0,0,0     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0     10,20,30,40,45,40,30,20,10,0,0,0
//
//M11  GonHred:                     e.g. 20 cols give 19 movements
//0,0,0,0,0, 0, 0, 2, 4,  6,  8, 10      10,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x, 12   12,10, 8, 6, 4, 4, 2, 0,0,0,0,0   //Note deceleration is slower to provide the increased distance required by next line
//0,0,0,0,0,-2,-4,-6,-8,-10,-12,-13     -13,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,-14  -14,20,30,40,45,40,30,20,10,0,0,0
//
//M11 Parameters required:
//  LeadOutForM39 - Use in conjunction with cols.
//  Distance to Move to Print Pixel zero (uSteps). Alternatively, this could be the absolute postion of the starting point and the absolute position of Pixel Zero. Actually, the starting points for each Line
//   need to be calculated (using a procedure similar to the one used for the X axis, but for each line). Each line will require a different LeadIn distance and time, which must be calculated from the absolute
//   position of Pixel Zero and the speed required at pixel zero [for that line]. We may also want a constand speed to be maintained for x Pixel Periods (like done for the Xaxis). It makes sense to calculate
//   these for every line in advance and store these in a table.
//  ThreadPitch (Can be treated as a constant)
//  uStepsPerStep
//  File of Max M11 motor Accelerations

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

//For Waveplates, 1 degree is 1 rev. For goniometers, 2 degrees is 1 rev.  M11 is a goniometer.

int GenerateProfileForM11(HWND hDlg,
                                 int    idxMno, // 10, 22, or 34
                                 int    nHCFrecNoofLines,
                                 int    nNoofRowsBetweenBeams,
                                 int    nNOOFm11prePIXzeroCONSTspeedPIXELS,
                                 int    nHCFrecNoofCols,
                                 int    nHCFrecFrequencyOfPixelPrintsInHz, //Used to choose the Accel Profile, and also put in the csv/csv header.
                                 char   chHCFrecNoofUsteps4EachMotorFromTab,
                                 UINT   uiHCFrecHorzDistBetwPixelCenters_nanoM,
                                 UINT   uiHCFrecVertDistBetwPixelCenters_nanoM,
                                 int    RefBeamTiltTimes1000,
                                 int    LightCoordsDTimes1000,
                                 int    LightCoordsOmegaTimes1000,
                                 time_t *ptr_HCFrec_uTimeStamp,// *ptr_HCFrec_uTimeStamp,
                                 char   *szHCFrecHologramName,
                                 int    nHCFrecNoofRowsBetweenBeams){
     int    MUSTCHANGENAMEOFnNOOFm11prePIXzeroCONSTspeedPIXELS;//Then delete this line
 //For this next structure, the Finish Position is given (i.e. Provided by the Ratcliffe spreadsheets): it is the motor position required for pixel zero on a given line. I.E. WHERE MOTOR IS WHEN IT HAS FINISHED
 // THE ACCELERATION PROFILE PLUS THE CONST SPEED PIXELS.
 // The spreadsheet has to have its position converted to uSteps from zero degrees. The difference, in uSteps, from pixel 1 to pixel zero is the starting speed.
 //Using a procedure similar to the one used for the X axis, nSpos is then calculated.  For Even Numbered rows 
 //typedef struct  { This is now defined in stdafx.h
 ////First the Accel part
 //  int nSpos;          //is the position at the beginning of line of the Gonio before the LeadIn, in uSteps. Calculated, somehow! This is where "Move To Start Positions" button must position the motor
 //  int nTotalPixelNoM11A; //nTotalPixelNoM11A will be set to the LeadIn *for this motor and for this line*
 //  int nFpos;    //is the position at Pix zero of the Gonio (after the LeadIn) in uSteps. The first position in a row from the spreadsheet, but converted to uSteps.   Wx
 //  int nSpeedAtFinish; //the number of uSteps between the first position in a row (from the spreadsheet) to the second position in the row.
 //  int nAmountAddedToSpos;
 //  int nConstSpeedsAddedToSpos; REMOVED
 ////Now the Decel part
 //  int nEndSpos;          //is the position at the end of line of the Gonio after the LeadOut, in uSteps. Calculated, somehow! This must be identical to nSpos for the next line.
 //  int nEndTotalPixelNoD; //nEndTotalPixelNoD will be set to the LeadOut *for this motor and for this line*. LeadIn and LeadOut (for this line) will probably be different. We can't even say the LeadOut for
 //                         // this line is the same as the LeadIn for next line... the stationary position must be the same, but nFpos & nSpeedAtFinish for the next line may be different.
 //  int nEndFpos; //is the position at Pix last of the Gonio (before the LeadOut) in uSteps. The last position in a row from the spreadsheet, but converted to uSteps.  Wx
 //  int nEndSpeedAtFinish;
 //  int nAmountAddedToEndSpos;
 //  int nConstSpeedsAddedToEndSpos; REMOVED
 // } RECORD_AccelSFS; //Accereration Start(Position) Finish(Position) Speed (at Finish)
#define SIZ_ARR_SFS_M11  1
 RECORD_AccelSFS ArrSFS[SIZ_ARR_SFS_M11];

 char szLocalM11ProfileFileName[_MAX_DIR]; //e.g. "<HologramDIR>\\CSVs\\M11GonHred.csv"

 int nLeadInForM39_11 = (nLengthOfM39rowsIncLeadInOut_inPixels + 1 - nHCFrecNoofCols) / 2;    //This is to check that the calculation succeeded - i.e. was possible in the time allowed.
 //double dblStepsPerDegree = (double)200.0f * chHCFrecNoofUsteps4EachMotorFromTab / GONIO_DEGREES_PER_REV; //200 is the steps per rev for 'normal' motors. GONIO_DEGREES_PER_REV is 2

 int nAccProfileCumulative[NOOFaccelPROFILES];
 int j;
 int idxMnoP1 = idxMno + 1;

 //THE FOLLOWNG LOADS AN ACCELERATION PROFILE FROM FILE 
 //For arguments sake, assume an acceleration profile always consists of 100 points.  The deceleration profile can be the opposite of this: the same points, but backwards.
 //It could be something like: 10,20,29,38,47,55,62,66,68,69,69,70,70,70,70... 70  (There's no reason why acceleration can't level off to horizontal)
 //ReadAccelerationProfile(M11)
/*
 szAccelFileNamesDir[nLenszAccelFileNamesDir] = 0;
 char szConstructedFileName[12] = "AccProfM11-";
 if(idxMnoP1 == 23){
    szConstructedFileName[8] = '2'; //change szConstructedFileName to "AccProfM23-";
    szConstructedFileName[9] = '3';
   }
 else if(idxMnoP1 == 35){
    szConstructedFileName[8] = '3'; //change szConstructedFileName to "AccProfM35-";
    szConstructedFileName[9] = '5';
   }
 StringCchCat(szAccelFileNamesDir,_MAX_DIR,szConstructedFileName);//originally ,"AccProfM11-");

 int nIdxToFileNameUstepEndings = DecodeNoofUstepsToAnIndex(chHCFrecNoofUsteps4EachMotorFromTab);  //turns 1,2,4,8 etc into 0,1,2,3 etc   (returns 1000001 on error)
 int nIdxToFileNameFreqEndings =  DecodeFreqToAnIndex(nHCFrecFrequencyOfPixelPrintsInHz);          //turns 10,15,30,60 into 0,1,2,3.      (returns 1000001 on error)
 if((nIdxToFileNameUstepEndings == 1000001)||(nIdxToFileNameFreqEndings == 1000001)){
    sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
    MessageBox(hDlg,"Invalid chHCFrecNoofUsteps4EachMotorFromTab or nHCFrecFrequencyOfPixelPrintsInHz parameter",mes,MB_OK);//"Generate Profile For M%d failed"
    return 1000001;
   }
 StringCchCat(szAccelFileNamesDir, _MAX_DIR, szTabProfileFileNameFreqEndings[nIdxToFileNameFreqEndings]);
 StringCchCat(szAccelFileNamesDir,_MAX_DIR,szTabProfileFileNameUstepEndings[nIdxToFileNameUstepEndings]);
*/
/*if(nHCFrecNoofLines == 123456789){  //This is solely to help when debugging, so should be removed. The parameter 123456789 tells to return after loading the file
    DisplayFileInEditor(hDlg, szAccelFileNamesDir);
    return 0;
   }*/
 if(LoadAndCheckAccelProfilePRNfile(hDlg, szAccelFileNamesDir, idxMnoP1, nAccProfile, chHCFrecNoofUsteps4EachMotorFromTab, nHCFrecFrequencyOfPixelPrintsInHz))
   return 1000002;// <-- L oadAndCheckAccelProfilePRNfile failed
/*
 //if(myOpen_File(hDlg, "prn", szAccelFileNamesDir, UNUSED_MOTORNO))
 //  return 1000002;// <-- The function failed
 ////Do Checks to help ensure Acceleration Profile loaded from file is OK
 //if((nAccProfile[0] < 1)||(nAccProfile[0] > 32767)){
 //   sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
 //   MessageBox(hDlg,"The Acceleration Profile must not start with < 1 uStep\nor > 32,767 uSteps",mes,MB_OK);//"Generate Profile For M%d failed"
 //   return 1000003; //G enerateProfileForMxx() failed!
 //  }
 //for(j = 1; j < NOOFaccelPROFILES; j++)
 //  if( (nAccProfile[j] < nAccProfile[j-1]) || (nAccProfile[j] > 32767) ){
 //     sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
 //     MessageBox(hDlg,"The Acceleration Profile must not contain decreasing speeds\nor numbers > 32,767",mes,MB_OK);//"Generate Profile For M%d failed"
 //     return 1000004; //G enerateProfileForMxx() failed!
 //    }
*/
 int nTotalStepsToThisPoint = 0;
 for(j = 0; j < NOOFaccelPROFILES; j++){
    nTotalStepsToThisPoint += nAccProfile[j];
    nAccProfileCumulative[j] = nTotalStepsToThisPoint; //builds a 2nd table (Example listed below.
   }
 //100,200,300, 400, 500, 600, 700, 800, 900,1000,1000...
 //100,300,600,1000,1500,2100,2800,3600,4500,5500,6500...

 //Here's some 'typical' values from the spreadsheet (in degrees)
 //7.36925978756995         (0,0)
 //7.35114808006538         (1.6,0)
 //-13.12959644             (998.4,0)   x&y coords of stage in mm
 //-13.18678543             (1000 ,0)

 //What we need to do is get up to the required speed by the time we get to the first printable pixel.
 //First, find the position of the first speed >= to nSpeedAtStartOrEndOfLine:
 //With M39, we did this:
 //int nSpeedAtStartOrEndOfLine = (int)(nHCFrecHorzDistBetwPixelCenters_nanoM * dblStepsPerNanoM);//i.e. the number of steps between the 1st two printable pixels
 //but with M11 we need to calculate the equivalent of nHCFrecHorzDistBetwPixelCenters_nanoM (from the first two positions in the spreadsheet, which means calculating those first).

//The Ratcliffe SS takes (X and Y) in cms as the input variables. However, for the calculation of nFpos, X (on the plate) is always Zero (cms) which is like working from the bottom of the speadsheet
// up (and Wx is always 7.37 in the example [for even numbered lines]).  In fact, for any constant value of X, Wx is constant: this implies that for all rows we print - regardless of
// the Y-Coordinate - Wx is always the same.  In other words the M11 Horz goniometer follows exactly the same profile in every line.  Good - only one line needs to be calculated, and the accel / decel
// & start / stop positions are always the same!

 int y;
 SetUpGlobals((double)RefBeamTiltTimes1000/1000,
              (double)LightCoordsDTimes1000/1000,
              (double)LightCoordsOmegaTimes1000/1000,
                      nHCFrecNoofCols,
              (double)uiHCFrecHorzDistBetwPixelCenters_nanoM/10000000,    //divide by 10,000,000 to get to cm's
                      nHCFrecNoofLines,
              (double)uiHCFrecVertDistBetwPixelCenters_nanoM/10000000  ); //divide by 10,000,000 to get to cm's
//double ldblArrayOfGonioAngles[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];//ie [MAXROWS][MAXCOLS]  declared globally to avoid stack overflow (~32Mbytes)
 double PixelHpositionInCms;
 double PixelVpositionInCms;
 double dblHalfAHorzPixel = (double)uiHCFrecHorzDistBetwPixelCenters_nanoM / 20000000;    //dividing by 10,000,000 converts to cm's...
 double dblHalfAVertPixel = (double)uiHCFrecVertDistBetwPixelCenters_nanoM / 20000000;    // & dividing by 20,000,000 halves that amount
 BOOL bEvenRow = 0;
 //for (int row = 0; row < nHCFrecNoofLines; row++){ // -But if all lines are the same we should use:
 int nDummy_nHCFrecNoofLines = SIZ_ARR_SFS_M11; //SIZ_ARR_SFS_M11 is currently 1
 for (int row = 0; row < nDummy_nHCFrecNoofLines; row++){
    PixelVpositionInCms = (double)uiHCFrecVertDistBetwPixelCenters_nanoM * row / 10000000;    //divide by 10,000,000 to get to cm's
    PixelVpositionInCms += dblHalfAVertPixel;
    bEvenRow = !bEvenRow;
    if(bEvenRow)
      for (int col = 0; col < nHCFrecNoofCols; col++){
         PixelHpositionInCms = (double)uiHCFrecHorzDistBetwPixelCenters_nanoM * col / 10000000; //divide by 10,000,000 to get to cm's
         ldblArrayOfGonioAngles[row][col] = GetWxInDegreesFromSpreadSheet(PixelHpositionInCms + dblHalfAHorzPixel, PixelVpositionInCms);
        }
    else //At present (with SIZ_ARR_SFS_M11 being set to 1), this 'else' is never executed.
      for (int col = nHCFrecNoofCols - 1; col > -1; col--){
         PixelHpositionInCms = (double)uiHCFrecHorzDistBetwPixelCenters_nanoM * col / 10000000; //divide by 10,000,000 to get to cm's
         ldblArrayOfGonioAngles[row][col] = GetWxInDegreesFromSpreadSheet(PixelHpositionInCms + dblHalfAHorzPixel, PixelVpositionInCms);
        }
   }

 int nNoofUstepsPerDegree = PCFrec.chNoofUsteps4EachMotorTab[idxMno] * GONIO_STEPS_PER_DEGREEorMM; //GONIO_STEPS_PER_DEGREE is 100 because 1 revolution is 2 degrees
#pragma region firstloop
//for(y = 0; y < nHCFrecNoofLines; y++){ -But if all lines are the same we should use:
 for(y = 0; y < nDummy_nHCFrecNoofLines; y++){ //nDummy_nHCFrecNoofLines is currently 1
 //Calculate the first and second pixel first:
    ArrSFS[y].nFpos = (int)(ldblArrayOfGonioAngles[y][0] * nNoofUstepsPerDegree + 0.5); //convert degrees to uSteps, rounding to the nearest uStep
    ArrSFS[y].nSpeedAtFinish = (int)(ldblArrayOfGonioAngles[y][1] * nNoofUstepsPerDegree + 0.5); //position of the 2nd point
    ArrSFS[y].nSpeedAtFinish -= ArrSFS[y].nFpos; //subtracting the first point gives speed (in uSteps/Pixel Period) //Must be a short, but no need to test this as the nAccProfile will fail if it's not.
 //Then Calculate the Penultimate and Ultimate Pixel
    ArrSFS[y].nEndFpos = (int)(ldblArrayOfGonioAngles[y][nHCFrecNoofCols-1] * nNoofUstepsPerDegree + 0.5); //convert degrees to uSteps, rounding to the nearest uStep
    ArrSFS[y].nEndSpeedAtFinish = (int)(ldblArrayOfGonioAngles[y][nHCFrecNoofCols-2] * nNoofUstepsPerDegree + 0.5); //position of the penultimate point
    ArrSFS[y].nEndSpeedAtFinish = ArrSFS[y].nEndFpos - ArrSFS[y].nEndSpeedAtFinish;
    //Still need to calculate:-
    //nSpos;    //is the position at the beginning of line of the Gonio before the LeadIn, in uSteps. Calculated, somehow! This is where "Move To Start Positions" button must position the motor.
    //nEndSpos; //is the position at the end of line of the Gonio after the LeadOut, in uSteps. Calculated, somehow! This must be identical to nSpos for the next line.
    //I shall use a method similar to that used for M39 as the initial attempt to calculate the closest point to ArrSFS[y].nFpos (pixel zero, for this motor) that the motor could start from (whilst still being able to accelerate).
    //I shall do this in advance for all rows, and from this the maximum accel / decel distances for the worst lines shall be learnt (and the durations of these).
    //Provided the durations do not exeed those allowed (as set by the XY stage X-axis), the list shall then be modified so that each line finishes at the starting point of the next line.
    //I must then calculate the acceleration profile for each line - given the starting position [nSpos or nEndSpos]), the position the acceleration is to end at [nFpos or nEndFpos], and the speed it must be going [nSpeedAtFinish or nEndSpeedAtFinish]

   //From M39:
    //What we need to do is get up to the required speed by the time we get to the first printable pixel.  Effectively we should be able to shift the nAccProfile
    // array to the right if we want to minimize the time taken to accelerate (e.g. shift right by 60 elements, then start at element 60). Or, if we wanted to 
    // maintain a constant speed for, say, 30 pixels we could shift left again by 30 and duplicate the last 30 elements.
    //First, find the position of the first speed >= to nSpeedAtStartOrEndOfLine:
    int nSpeedAtStartOrEndOfLine = ArrSFS[y].nSpeedAtFinish; //i.e. the number of steps between the 1st two printable pixels
    if(nSpeedAtStartOrEndOfLine < 0)
      nSpeedAtStartOrEndOfLine *= -1;
    int nFoundMinNoofElements = -1;
    for(j = 0; j < NOOFaccelPROFILES; j++)
      if( nAccProfile[j] >= nSpeedAtStartOrEndOfLine){
         nFoundMinNoofElements = j;
         j = NOOFaccelPROFILES;
        }
    if(nFoundMinNoofElements < 0){ //It's not possible to accelerate to the required speed.
       sprintf_s(mes,SIZMES,"Generate Profile For M%d failed:\n\nIt is not possible to accelerate to the required speed (%d uSteps/PixelPeriod)\nas the acceleration profile doesn't go that fast.\n\nSuggestion - look at the file about to be opened...",idxMnoP1,nSpeedAtStartOrEndOfLine);
       MessageBox(hDlg,mes,NULL,MB_OK);
       DisplayFileInEditor(hDlg, szAccelFileNamesDir);
       return 1000005; //G enerateProfileForMxx() failed!
      }
    /* For nNOOFprePIXzeroCONSTspeedPIXELS[forGonHs and other motors] it may be possible (although difficult) to compute allowable values after the rest of the accel/decel profiles have been calculated.
    if(nFoundMinNoofElements + nNOOFprePIXzeroCONSTspeedPIXELS [forGonHs] > 100){
       StringCchCopy(mes, SIZMES, "Accelerating to the required speed and maintaining the speed for\nnNOOFprePIXzeroCONSTspeedPIXELS\nexceeds the 100 pixel LeadIn limit.\nNOOFprePIXzeroCONSTspeedPIXELS needs to be reduced to: ");
       _itoa_s(100 - nFoundMinNoofElements,mes+188,SIZMES - 188,10);
       StringCchCat(mes,SIZMES," (or Less)");
       MessageBox(hDlg,mes,"GenerateProfileForM39 failed",MB_OK);
       return 1000006; //G enerateProfileForMxx() failed!
      } */
    //User request 10412 usteps should be perform within the leadout.
    //100,200,300, 400, 500, 600, 700, 800, 900,1000,900,800,600,500,412,400,300,200,100                          112, 100,100,100,100
    //100,300,600,1000,1500,2100,2800,3600,4500,5500
    int nLengthOfCalculatedMotorMovementInPixPeriods = nFoundMinNoofElements + nNOOFm11prePIXzeroCONSTspeedPIXELS;
    if(nLengthOfCalculatedMotorMovementInPixPeriods > nLeadInForM39_11){
     /*int nLength_mes;
       memset( mes, 0, SIZMES);
       StringCchCopy(mes,SIZMES,"Optimum M11 acceleration takes longer than Xaxis LeadIn or LeadOut.\n\nXaxis LeadIn or LeadOut is: ");
       nLength_mes = lstrlen(mes);
       _itoa_s( nLeadInForM39_11, mes + nLength_mes, SIZMES - nLength_mes, 10);
       StringCchCat(mes,SIZMES,"\t\tnLengthOfCalculatedMotorMovementInPixPeriods is: ");
       nLength_mes = lstrlen(mes);
       _itoa_s(nLengthOfCalculatedMotorMovementInPixPeriods, mes + nLength_mes, SIZMES - nLength_mes, 10);
       StringCchCat(mes,SIZMES,"\n\nSuggestions are:\n-Increase NOOFprePIXzeroCONSTspeedPIXELS (for X axis)\n-Decrease NOOFprePIXzeroCONSTspeedPIXELS (for M11)\n-Increase M11s acceleration profile (but only if possible)\n-Decrease the pixel frequency\n\nPress Cancel to View Accel Profile.");
     */
       sprintf_s(mes,SIZMES,"Optimum M%d acceleration takes longer than Xaxis LeadIn or LeadOut.\n\nXaxis LeadIn or LeadOut is: %d\t\tnLengthOfCalculatedMotorMovementInPixPeriods is: %d\n\nSuggestions are:\n-Increase NOOFprePIXzeroCONSTspeedPIXELS (for X axis)\n-Decrease NOOFprePIXzeroCONSTspeedPIXELS (for M%d)\n-Increase M%ds acceleration profile (but only if possible)\n-Decrease the pixel frequency\n\nPress Cancel to View Accel Profile.",idxMnoP1, nLeadInForM39_11,nLengthOfCalculatedMotorMovementInPixPeriods,idxMnoP1,idxMnoP1);
       if(MessageBox(hDlg,mes,"Generate Profile for Horz Goniometer failed, Acceleration Profile cannot be completed in the Xaxis LeadIn or LeadOut period!",MB_OKCANCEL) == IDCANCEL)
         DisplayFileInEditor(hDlg, szAccelFileNamesDir);
       return 1000008;
      }
    ArrSFS[y].nTotalPixelNoM11A = nFoundMinNoofElements;
//    nSpeedAtStartOrEndOfLine = ArrSFS[y].nSpeedAtFinish; //i.e. the number of steps between the 1st two printable pixels. This restores the sign (i.e. direction)
  //  if(nSpeedAtStartOrEndOfLine < 0)
    //  nSpeedAtStartOrEndOfLine *= -1;
    int nLengthOfCalculatedMotorMovementInUsteps = nAccProfileCumulative[nFoundMinNoofElements]
                                                    - nAccProfile[nFoundMinNoofElements] //because this, more likely than not, would be faster than our desired speed.
                                                    //+ nSpeedAtStartOrEndOfLine
                                                    + (nSpeedAtStartOrEndOfLine * nNOOFm11prePIXzeroCONSTspeedPIXELS); //i.e. the number of steps between the 1st two printable pixels. Effectively this forces a constant speed pixel before pixel zero.
    if(ArrSFS[y].nSpeedAtFinish > 0) //Motor is moving away from home [distance is increasing] so subtract the LengthOfCalculatedMotorMovementInUsteps from the pixel zero start position
      ArrSFS[y].nSpos = ArrSFS[y].nFpos - nLengthOfCalculatedMotorMovementInUsteps;
    else
      ArrSFS[y].nSpos = ArrSFS[y].nFpos + nLengthOfCalculatedMotorMovementInUsteps;
//***
//Now Calculate the finish position for the line
//***
    nSpeedAtStartOrEndOfLine = ArrSFS[y].nEndSpeedAtFinish; //i.e. the number of steps between the last two printable pixels
    if(nSpeedAtStartOrEndOfLine < 0)
      nSpeedAtStartOrEndOfLine *= -1;
    nFoundMinNoofElements = -1;
    for(j = 0; j < NOOFaccelPROFILES; j++)
      if( nAccProfile[j] >= nSpeedAtStartOrEndOfLine){
         nFoundMinNoofElements = j;
         j = NOOFaccelPROFILES;
        }
    if(nFoundMinNoofElements < 0){ //It's not possible to decelerate from the required speed.
       sprintf_s(mes,SIZMES,"Generate Profile For M%d failed:\n\nIt is not possible to decelerate from the required speed (%d uSteps/PixelPeriod)\nas the deceleration profile cannot start that fast.\n\nSuggestion - look at the file about to be opened...",idxMnoP1,nSpeedAtStartOrEndOfLine);
       MessageBox(hDlg,mes,NULL,MB_OK);
       DisplayFileInEditor(hDlg, szAccelFileNamesDir);
       return 1000009; //G enerateProfileForMxx() failed!
      }
    nLengthOfCalculatedMotorMovementInPixPeriods = nFoundMinNoofElements + nNOOFm11prePIXzeroCONSTspeedPIXELS;
    if(nLengthOfCalculatedMotorMovementInPixPeriods > nLeadInForM39_11){
     /*int nLength_mes;
       memset( mes, 0, SIZMES);
       StringCchCopy(mes,SIZMES,"Optimum M11 decceleration takes longer than Xaxis LeadIn or LeadOut.\n\nXaxis LeadIn or LeadOut is: ");
       nLength_mes = lstrlen(mes);
       _itoa_s( nLeadInForM39_11, mes + nLength_mes, SIZMES - nLength_mes, 10);
       StringCchCat(mes,SIZMES,"\t\tnLengthOfCalculatedMotorMovementInPixPeriods is: ");
       nLength_mes = lstrlen(mes);
       _itoa_s(nLengthOfCalculatedMotorMovementInPixPeriods, mes + nLength_mes, SIZMES - nLength_mes, 10);
       StringCchCat(mes,SIZMES,"\n\nSuggestions are:\n-Increase NOOFprePIXzeroCONSTspeedPIXELS (for X axis)\n-Decrease NOOFprePIXzeroCONSTspeedPIXELS (for M11)\n-Increase M11s acceleration profile (but only if possible)\n-Decrease the pixel frequency\n\nPress Cancel to View Accel Profile.");
       if(MessageBox(hDlg,mes,"GenerateProfileForM11 failed, Decceleration Profile cannot be completed in the Xaxis LeadIn or LeadOut period!",MB_OKCANCEL) == IDCANCEL)
         DisplayFileInEditor(hDlg, szAccelFileNamesDir); */
       sprintf_s(mes,SIZMES,"Optimum M%d decceleration takes longer than Xaxis LeadIn or LeadOut.\n\nXaxis LeadIn or LeadOut is: %d\t\tnLengthOfCalculatedMotorMovementInPixPeriods is: %d\n\nSuggestions are:\n-Increase NOOFprePIXzeroCONSTspeedPIXELS (for X axis)\n-Decrease NOOFprePIXzeroCONSTspeedPIXELS (for M%d)\n-Increase M%ds acceleration profile (but only if possible)\n-Decrease the pixel frequency\n\nPress Cancel to View Accel Profile.",idxMnoP1, nLeadInForM39_11,nLengthOfCalculatedMotorMovementInPixPeriods,idxMnoP1,idxMnoP1);
       if(MessageBox(hDlg,mes,"Generate Profile for Horz Goniometer failed, Acceleration Profile cannot be completed in the Xaxis LeadIn or LeadOut period!",MB_OKCANCEL) == IDCANCEL)
         DisplayFileInEditor(hDlg, szAccelFileNamesDir);
       return 1000010;
      }
    ArrSFS[y].nEndTotalPixelNoD = nFoundMinNoofElements;
//    nSpeedAtStartOrEndOfLine = ArrSFS[y].nEndSpeedAtFinish; //i.e. the number of steps between the last two printable pixels. This restores the sign (i.e. direction)
  //  if(nSpeedAtStartOrEndOfLine < 0)
    //  nSpeedAtStartOrEndOfLine *= -1;
    nLengthOfCalculatedMotorMovementInUsteps = nAccProfileCumulative[nFoundMinNoofElements]
                                                - nAccProfile[nFoundMinNoofElements] //because this, more likely than not, would be faster than our desired speed.
                                                //+ nSpeedAtStartOrEndOfLine
                                                + (nSpeedAtStartOrEndOfLine * nNOOFm11prePIXzeroCONSTspeedPIXELS); //i.e. the number of steps between the 1st two printable pixels. Effectively this forces a constant speed pixel before pixel zero.
    if(ArrSFS[y].nEndSpeedAtFinish > 0) //Motor is moving away from home [distance is increasing] so Add the LengthOfCalculatedMotorMovementInUsteps from the ultimate pixel ENDstart position
      ArrSFS[y].nEndSpos = ArrSFS[y].nEndFpos + nLengthOfCalculatedMotorMovementInUsteps;
    else
      ArrSFS[y].nEndSpos = ArrSFS[y].nEndFpos - nLengthOfCalculatedMotorMovementInUsteps;
   }
#pragma endregion
 memset( pMxPacketBuffAr[idxMno], 0, 4400); //Make sure the buffer we're about to write into is clear before we start
 //Assuming that every row for Horizontal Goniometers is identical to every other row, load the buffer with data in much the same way as for M39.
 // The main difference (compared to M39) is that the acceleration and deceleration profiles will be different.  If this assumption is wrong, this
 // function will have to be rewritten to be similar to the way the Vertical Goniometers have their positions calculated.
 int nTotalPixelNoM11;
 int betterAccelProf, nPadAccelProf;
 for(y = 0; y < nDummy_nHCFrecNoofLines; y++){
    //Now put the acceleration up to the 'constant speed before Pixel zero' points in (from the nAccProfile[] table up to the nFoundMinNoofElements point)
//The question is, how many stationary periods do we want to put at the beginning? We need to pad out the LeadIn to make it as long as M39's LeadIn. The simplest way is to subtact the number of Accel profiles we've worked out
// from M39's LeadIn and put in zeros, but it may be better to put in constant speed pixels instead or as well. This can be done only if there's space and only if it doesn't prevent the next line from being achievable (which
// is difficult to evaluate).  For the time being, just put in zeros and a single constant speed pixel. No! 2nd thoughts put in nNOOFm11prePIXzeroCONSTspeedPIXELS, and fewer zeros. This can be hard coded at present, or made
// settable through the user interface, which provides flexibility.  It will also be easier, in future, to automatically generate its value (if nNOOFm11prePIXzeroCONSTspeedPIXELS is explicit in the code).
    //nNoofLeadingZeroMovs = nLeadInForM39_11 - ArrSFS[y].nTotalPixelNoM11A - nNOOFm11prePIXzeroCONSTspeedPIXELS;
    //for(nPadAccelProf = 0; nPadAccelProf < nNoofLeadingZeroMovs; nPadAccelProf++)
    //  *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = 0;
    //In fact we don't need to do the above 3 lines because the buffer is already zeroised... we can just skip past them:
    nTotalPixelNoM11 = nLeadInForM39_11 - ArrSFS[y].nTotalPixelNoM11A - nNOOFm11prePIXzeroCONSTspeedPIXELS;
    int idxToTheAccProfilesWeWillUse = 0;
    if(ArrSFS[y].nSpeedAtFinish < 0)
      for(betterAccelProf = ArrSFS[y].nTotalPixelNoM11A; betterAccelProf > 0; betterAccelProf--)//    ... in which case none of the movements in nAccProfile[] will be used
        *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)nAccProfile[idxToTheAccProfilesWeWillUse++] * -1;
    else
      for(betterAccelProf = ArrSFS[y].nTotalPixelNoM11A; betterAccelProf > 0; betterAccelProf--)//    ... in which case none of the movements in nAccProfile[] will be used
        *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)nAccProfile[idxToTheAccProfilesWeWillUse++];
    //Note the above doesn't leave us at the constant speed, but at a speed that allows movement of the number of steps between pixel 0 and  pixel 1. Following the above, we will
    // be at the pixel zero position - provided we started from the right place (i.e. the position of pixel zero - minus the sum of the steps created in the above loop). 2nd thoughts -  by
    //  adding "+ nSpeedAtStartOrEndOfLine" above, we are forcing a constant speed pixel before pixel zero - so put that in the profile here:

    for(nPadAccelProf = 0; nPadAccelProf < nNOOFm11prePIXzeroCONSTspeedPIXELS; nPadAccelProf++)
      *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)ArrSFS[y].nSpeedAtFinish;
    //Possibly we could put some additional constant speed pixels in here too, if there is time and room.

    int n64PixDistInSteps;
    int n64LastPixDistInSteps = (int)(ldblArrayOfGonioAngles[y][0] * nNoofUstepsPerDegree + 0.5);
    //nTotalPixelNoM11 = nNOOFprePIXzeroCONSTspeedPIXELS + nFoundMinNoofElements; //At this point, nTotalPixelNoM11 is set to the LeadIn   (This what M39 did)
    //nTotalPixelNoM11 = ArrSFS[y].nTotalPixelNoM11A + 1; //At this point, nTotalPixelNoM11 is set to the LeadIn  (and 1 is really nNOOFprePIXzeroCONSTspeedPIXELS)
    for (__int64 n64Pixel = 1; n64Pixel < nHCFrecNoofCols; n64Pixel++){ //declared as __int64 to avoid casting in the next row. Starting from 1 assumes no movement for the 1st pixel.  The absolute position in degrees
       n64PixDistInSteps =  (int)(ldblArrayOfGonioAngles[y][n64Pixel] * nNoofUstepsPerDegree + 0.5);  // for the 1st pixel.  The absolute position in degrees
       *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)(n64PixDistInSteps - n64LastPixDistInSteps); //shStepsToMove is being put in the buffer
       n64LastPixDistInSteps = n64PixDistInSteps;
      }

    for(nPadAccelProf = 0; nPadAccelProf < nNOOFm11prePIXzeroCONSTspeedPIXELS; nPadAccelProf++)
      *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)ArrSFS[y].nEndSpeedAtFinish;

    //Now put the decceleration from the 'constant speed after Pixel Ultimate' points in (from the nAccProfile[] table up to the nFoundMinNoofElements point)
    idxToTheAccProfilesWeWillUse = ArrSFS[y].nEndTotalPixelNoD - 1;                            // <- Note this may set it to -1
    //idxToTheAccProfilesWeWillUse = 0;
    if(ArrSFS[y].nEndSpeedAtFinish < 0)
      for(betterAccelProf = ArrSFS[y].nEndTotalPixelNoD; betterAccelProf > 0; betterAccelProf--)
        *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)nAccProfile[idxToTheAccProfilesWeWillUse--] * -1;
    else
      for(betterAccelProf = ArrSFS[y].nEndTotalPixelNoD; betterAccelProf > 0; betterAccelProf--)
        *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNoM11++) = (short)nAccProfile[idxToTheAccProfilesWeWillUse--];
   }

//Apart from the <= 100 pixel at beginning of first line there will also <= 100 pixels at end of first line (accel / decel)
//int nACCEL_DECEL_PROFILE_SIZE = nFoundMinNoofElements + nNOOFprePIXzeroCONSTspeedPIXELS;
 int nPulsesThisPixel;
 int FirstLineLen = nLengthOfM39rowsIncLeadInOut_inPixels;  //for M39 this was int FirstLineLen = nHCFrecNoofCols - 1 + nACCEL_DECEL_PROFILE_SIZE * 2;
 int nFirstLineLen = FirstLineLen;
 //nLengthOfM39rowsIncLeadInOut_inPixels = FirstLineLen;    //This is for m ySave_File (but it's set when generating M39)
 //nLengthOfM11rowsIncLeadInOut_inUsteps = 0;            //These three are for G enerateStartPositionsFile
 //nLengthOfM11rowsJustLeadIn_inUsteps = 0;              //We can subtract this (having converted it to mm) from HCFrec.XposOfHologsOriginOnPlateMM to get the true start position for the stage (i.e. Can
                                                       // the stage actually move to the physical start position required?
 //nLengthOfM11rowsJustLeadOut_inUsteps = 0;             //We can subtract this (having converted it to mm) from HCFrec.XposOfHologsOriginOnPlateMM to get the true start position for the stage (i.e. Can
                                                       // the stage actually move to the physical start position required?
 int nTotalPixelNo = 2200; //This moves us on to the 2nd Line (which for horiozontal Goniometer is the first line - but exactly reversed)
 for(int nReverseTheFirstLine = 0; nReverseTheFirstLine < FirstLineLen; nReverseTheFirstLine++){
    nPulsesThisPixel = *((short*)(pMxPacketBuffAr[idxMno]) + --nFirstLineLen);
    *((short*)(pMxPacketBuffAr[idxMno]) + nTotalPixelNo++) =  (short)(nPulsesThisPixel * -1);
//  nLengthOfM11rowsIncLeadInOut_inUsteps += nPulsesThisPixel;
   }

/*This almost certainly doesn't do anything anymore
 for(int nGetLeadIn_inUsteps = 0; nGetLeadIn_inUsteps < nLeadInForM39_11; nGetLeadIn_inUsteps++){
    nPulsesThisPixel = *((short*)(pMxPacketBuffAr[idxMno]) + nGetLeadIn_inUsteps);
//  nLengthOfM11rowsJustLeadIn_inUsteps += nPulsesThisPixel;
   }
*/

 for(int eachEvenLine = 2; eachEvenLine < nHCFrecNoofLines; eachEvenLine += 2)
   memcpy( ( pMxPacketBuffAr[idxMno] + (eachEvenLine * 4400)), pMxPacketBuffAr[idxMno],4400);
 for(int eachOddLine = 3; eachOddLine < nHCFrecNoofLines; eachOddLine += 2)
   memcpy( ( pMxPacketBuffAr[idxMno] + (eachOddLine * 4400)), pMxPacketBuffAr[idxMno] + 4400, 4400);

//This was in GenerateStartPositionsFile()
  //int nStepsPerMilliMeter = (int)(500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[idxMno] / XAXIS_PITCH); //500 is the steps per rev for the 5 phase stage motors.
    //For the Gonios, we don't need this. M39 uses it to convert HCFrec.XposOfHologsOriginOnPlateMM, but the Gonios reference position (0 degrees) is specified in uSteps in PCFrec
  //int nStepsToHologramsOriginX = nStepsPerMilliMeter * HCFrec.XposOfHologsOriginOnPlateMM;

    //The AVRs always think of their positions in absolute uStep coordinates whose origin is Home.  AVRs will be able to travel a short negative distance until they hit the limit switch - The is the Physical Limits Min.
    //The Physical Limits Min will change slightly if the stage, homing switch or limit switch is physically adjusted / tampered with, or if the noof uSteps/Step is changed.  In principle, Point (0,0) may be
    //in negative territory, but it cannot be less than the Physical Limit Min [actually I suppose it could be, as long as we didn't mind that we could never move to (0,0)].  The user will specify their holograms
    //relative to point (0,0) and what's more may well end up with required positions on the negative side of that when the acceleration profile is calculated.  However, we must not let them use profiles that violate
    //the physical limits.
    int nSPos;
    int nTrueNoofRowsIncBetweenTheBeams = nHCFrecNoofLines + 2 * nHCFrecNoofRowsBetweenBeams;
  //for(nSPos = 0; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2)
    //nLineXstartPositionsFEM[idxMno][nSPos] = PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nStepsToHologramsOriginX - nLengthOfM11rowsJustLeadIn_inUsteps;
  //for(nSPos = 1; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2)
    //nLineXstartPositionsFEM[idxMno][nSPos] = PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nStepsToHologramsOriginX  - nLengthOfM11rowsJustLeadIn_inUsteps + nLengthOfM39rowsIncLeadInOut_inUsteps;
    int nGONIO_COL_IDX = 0; //0 is Red, 1 is Green, 2 is Blue
    if(idxMno == 22)
      nGONIO_COL_IDX = 1;
    else if (idxMno == 34)
      nGONIO_COL_IDX = 2;
    int nPcfRecNoofUsteps_Hm2GonHzeroDegrees_nGONIO_COL_IDX = PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[nGONIO_COL_IDX];



//For the Red Gonio:                                 NOTE THIS CODE IS DEPENDENT ON ALL THE LINES HAVING THE SAME START AND END POSITIONS AS EACH OTHER.
    if(nGONIO_COL_IDX == 0){                              //The start positions for all the EVEN numbered lines is the same as the start position of the first line
       for(nSPos = 0; nSPos < nHCFrecNoofLines; nSPos+= 2)//The start positions for all the  ODD numbered lines is the same as the end position of the first line
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nSpos;    //The data stored in nLineXstartPositionsFEM for motors M11 - M12 is relative to zero degrees.  If the user subsequently changes the offset from home of zero
       for(nSPos = 1; nSPos < nHCFrecNoofLines; nSPos+= 2)            // degees (in uSteps from home, in the PCF file) the start position for each line will also move.
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nEndSpos;
      }
//For the Green Gonio:                               NOTE THIS CODE IS DEPENDENT ON ALL THE LINES HAVING THE SAME START AND END POSITIONS AS EACH OTHER.
    if(nGONIO_COL_IDX == 1){
       int nHCFrecNoofLinesPlusONEbeam = nHCFrecNoofLines + nHCFrecNoofRowsBetweenBeams;
       for(nSPos = nHCFrecNoofRowsBetweenBeams; nSPos < nHCFrecNoofLinesPlusONEbeam; nSPos+= 2)
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nSpos;    //The data stored in nLineXstartPositionsFEM for motors M11 - M12 is relative to zero degrees.  If the user subsequently changes the offset from home of zero
       for(nSPos = nHCFrecNoofRowsBetweenBeams + 1; nSPos < nHCFrecNoofLinesPlusONEbeam; nSPos+= 2)            // degees (in uSteps from home, in the PCF file) the start position for each line will also move.
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nEndSpos;
      }
//For the Blue Gonio:                                NOTE THIS CODE IS DEPENDENT ON ALL THE LINES HAVING THE SAME START AND END POSITIONS AS EACH OTHER.
    if(nGONIO_COL_IDX == 2){
       for(nSPos = nHCFrecNoofRowsBetweenBeams * 2; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2)
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nSpos;    //The data stored in nLineXstartPositionsFEM for motors M11 - M12 is relative to zero degrees.  If the user subsequently changes the offset from home of zero
       for(nSPos = nHCFrecNoofRowsBetweenBeams * 2 + 1; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos+= 2)            // degees (in uSteps from home, in the PCF file) the start position for each line will also move.
         nLineXstartPositionsFEM[idxMno][nSPos] = ArrSFS[0].nEndSpos; //
      }


    int nMinPhysPos, nMaxPhysPos;
    if(nLineXstartPositionsFEM[idxMno][0] < nLineXstartPositionsFEM[idxMno][1]){
       nMinPhysPos = nLineXstartPositionsFEM[idxMno][0];
       nMaxPhysPos = nLineXstartPositionsFEM[idxMno][1];
      }
    else{
       nMinPhysPos = nLineXstartPositionsFEM[idxMno][1];
       nMaxPhysPos = nLineXstartPositionsFEM[idxMno][0];
      }
    nMinPhysPos +=nPcfRecNoofUsteps_Hm2GonHzeroDegrees_nGONIO_COL_IDX;
    nMaxPhysPos +=nPcfRecNoofUsteps_Hm2GonHzeroDegrees_nGONIO_COL_IDX;
    if(nMinPhysPos < PCFrec.nPhysLimMinUsteps[idxMno]){
     /*StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(PCFrec.nPhysLimMinUsteps[idxMno] - nMinPhysPos,mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Degrees Error: ");
     //double dblStepsPerMM = (double)500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[eAchM] / XAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       //The following was defined above
       //int nNoofUstepsPerDegree = PCFrec.chNoofUsteps4EachMotorTab[idxMno] * GONIO_STEPS_PER_DEGREEorMM; //GONIO_STEPS_PER_DEGREE is 100 because 1 revolution is 2 degrees
       double dblDEGperStep  = 1.0f / nNoofUstepsPerDegree;
       sprintf_s(mes+lenMes+21, SIZMES - lenMes - 21, "%3.3f deg", dblDEGperStep * (PCFrec.nPhysLimMinUsteps[idxMno] - nMinPhysPos) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 11 (Gonio Horz) would have to move outside it's 'MINIMUM Physical Limit'\nby the above number of micro steps (& degrees)\n\nDo you need suggestions?",mes,MB_YESNO)){
       */
       double dblDEGperStep  = 1.0f / nNoofUstepsPerDegree;
       sprintf_s(mes,SIZMES,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nM%d (Gonio Horz) would have to move outside it's 'MINIMUM Physical Limit'\nby %d uSteps (%3.3f degrees)\n\nDo you need suggestions?", idxMnoP1, PCFrec.nPhysLimMinUsteps[idxMno] - nMinPhysPos, dblDEGperStep * (PCFrec.nPhysLimMinUsteps[idxMno] - nMinPhysPos));
       while(IDYES == MessageBox(hDlg, mes, NULL, MB_YESNO)){
          nHelpMessageToLoad = 6;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1000011; //don't return 1 - reserved for m ySave_File
      }
    if(nMaxPhysPos > PCFrec.nPhysLimMaxUsteps[idxMno]){
     /*StringCchCopy(mes, SIZMES, "MicroSteps Error: ");
       _itoa_s(nMaxPhysPos - PCFrec.nPhysLimMaxUsteps[idxMno],mes + 18,SIZMES - 18,10);
       int lenMes = lstrlen(mes);
       StringCchCopy(mes+lenMes, SIZMES-lenMes, "  ==  Degrees Error: ");
     //double dblStepsPerMM = (double)500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[eAchM] / XAXIS_PITCH; //500 is the steps per rev for the 5 phase stage motors.
       //The following was defined above
       //int nNoofUstepsPerDegree = PCFrec.chNoofUsteps4EachMotorTab[idxMno] * GONIO_STEPS_PER_DEGREEorMM; //GONIO_STEPS_PER_DEGREE is 100 because 1 revolution is 2 degrees
       double dblDEGperStep  = 1.0f / nNoofUstepsPerDegree;
       sprintf_s(mes+lenMes+21, SIZMES - lenMes - 21, "%3.3f deg", dblDEGperStep * (nMaxPhysPos - PCFrec.nPhysLimMaxUsteps[idxMno]) );
       while(IDYES == MessageBox(hDlg,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nMotor 11 (Gonio Horz) would have to move outside it's 'MAXIMUM Physical Limit'\nby the above number of micro steps (& degrees)\n\nDo you need suggestions?",mes,MB_YESNO)){
       */
       double dblDEGperStep  = 1.0f / nNoofUstepsPerDegree;
       sprintf_s(mes,SIZMES,"Holoprog is unable to create valid motor positions based on this hologram configuration!\n\nM%d (Gonio Horz) would have to move outside it's 'MAXIMUM Physical Limit'\nby %d uSteps (%3.3f degrees)\n\nDo you need suggestions?", idxMnoP1, nMaxPhysPos - PCFrec.nPhysLimMaxUsteps[idxMno], dblDEGperStep * (nMaxPhysPos - PCFrec.nPhysLimMaxUsteps[idxMno]));
       while(IDYES == MessageBox(hDlg, mes, NULL, MB_YESNO)){
          nHelpMessageToLoad = 6;
          DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
         }
       return 1000012;
      }
 /***************************************************************************************************************************************/
 /* This Block Loads the nLineXstartPositionsFEM[][] table with appropriate values for lines where no printing takes place for that motor*/
 /***************************************************************************************************************************************/
 int RestingPositionAfterColorIsPrinted, StartingPositionBeforeColorIsPrinted;
 if((nHCFrecNoofLines & 1)!= 0)
   RestingPositionAfterColorIsPrinted = ArrSFS[0].nEndSpos;   //nHCFrecNoofLines is an Odd Number (so we end up at the start position of an odd row)
 else
   RestingPositionAfterColorIsPrinted = ArrSFS[0].nSpos;//nHCFrecNoofLines is an Even Number
 if(nGONIO_COL_IDX == 0)
   for(nSPos = nHCFrecNoofLines; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos++)                               //We want no movement after the red printing has finished
     nLineXstartPositionsFEM[idxMno][nSPos] = RestingPositionAfterColorIsPrinted;
 else if(nGONIO_COL_IDX == 1)
   for(nSPos = nHCFrecNoofLines + nHCFrecNoofRowsBetweenBeams; nSPos < nTrueNoofRowsIncBetweenTheBeams; nSPos++) //We want no movement after the green printing has finished
     nLineXstartPositionsFEM[idxMno][nSPos] = RestingPositionAfterColorIsPrinted;
 //else if(nGONIO_COL_IDX == 2) We don't need to pad out the blue resting position because the print finishes at the last line.
 StartingPositionBeforeColorIsPrinted = ArrSFS[0].nSpos;
 if(nGONIO_COL_IDX == 1)
   for(nSPos = 0; nSPos < nHCFrecNoofRowsBetweenBeams; nSPos++)                              //We want no movement before the green printing starts
     nLineXstartPositionsFEM[idxMno][nSPos] = StartingPositionBeforeColorIsPrinted;
 else if(nGONIO_COL_IDX == 2)
   for(nSPos = nHCFrecNoofRowsBetweenBeams * 2 - 1; nSPos > -1; nSPos--)                     //We want no movement before the blue printing starts
     nLineXstartPositionsFEM[idxMno][nSPos] = StartingPositionBeforeColorIsPrinted;
 //else if(nGONIO_COL_IDX == 0) We don't need to worry about padding out the starting position because the print starts at the first line.

 /**************************************************************************************************************************************************/
 /* Finally for the Green and Blue motors, shift the movement profiles down by nHCFrecNoofRowsBetweenBeams or 2 * nHCFrecNoofRowsBetweenBeams rows */
 /**************************************************************************************************************************************************/
 if(nGONIO_COL_IDX == 1){
    memmove( pMxPacketBuffAr[idxMno] + nNoofRowsBetweenBeams * 4400, pMxPacketBuffAr[idxMno], 4400 * nHCFrecNoofLines);
    memset( pMxPacketBuffAr[idxMno], 0, nNoofRowsBetweenBeams * 4400);
   }
 else if (nGONIO_COL_IDX == 2){
    memmove( pMxPacketBuffAr[idxMno] + nNoofRowsBetweenBeams * 8800, pMxPacketBuffAr[idxMno], 4400 * nHCFrecNoofLines);
    memset( pMxPacketBuffAr[idxMno], 0, nNoofRowsBetweenBeams * 8800);
   }



 FillInHeaderForGeneratedFile(nHCFrecNoofLines + 2 * nNoofRowsBetweenBeams, nHCFrecNoofCols, nHCFrecFrequencyOfPixelPrintsInHz, nLeadInForM39_11, ptr_HCFrec_uTimeStamp, szHCFrecHologramName);
 StringCchCopy(szLocalM11ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from but, when generating a hologram, we write to it.
 StringCchCat(szLocalM11ProfileFileName,_MAX_DIR,FILENAMEStab[idxMno]);//e.g. "<HologramDIR>\\CSVs\\M11GonHred.csb"
 int nLen = lstrlen(szLocalM11ProfileFileName);
 szLocalM11ProfileFileName[nLen - 1] = 'v';
 pCurrentProfileGenerationBuffer = pMxPacketBuffAr[idxMno];
 int nRequiredSpace = MAXNOOFPIXELS_IN_ROW * MAX_NOOFLINES * sizeof(int) + SIZ_CSV_HEADER;
 mySave_File (hDlg, nRequiredSpace, "csv",TYPE5CSV, szLocalM11ProfileFileName, MOTORNO_NOT_USED); //returns zero if file saved OK  (Uses the global nLengthOfM39rowsIncLeadInOut_inPixels - set when generating M39)
//nchCurrentMotorZB = (char)idxMno; surely just a frig to make m ySave_File() work properly... no longer needed because m yOpen_File() forwards this as a parameter
 memset(pMxPacketBuffAr[idxMno],0,SIZ_8832000);
 myOpen_File(hDlg, "csv", szLocalM11ProfileFileName, idxMno);//The sole reason for calling this, here, is to reopen the csv so that it gets saved as a csb
 return 0; //returns zero if file saved OK
}
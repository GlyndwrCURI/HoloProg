// stdafx.h : include file for standard system include files, or project specific include files
// that are used frequently, but are changed infrequently
#pragma once
 
#define B_ALLOW_USBCOMMS_ERRORS //don't define this again without 
#define AUTOREFRESHRATE  500      //refresh rate in milliseconds
//#define AUTOREFRESHRATE  6000    //refresh rate in milliseconds
//#define AUTOREFRESHRATE  30000   //refresh rate in milliseconds

//#define DEFAULT_CSV_OUTFILENAMEPPOSLINE   "PixPositionsLineM"//Was "PixPositionsLine.csv". Now motor number will be appended b4 saving, and the .csv will be added by mySave_File()
//#define DEFAULT_CSV_OUTFILENAMEPDIFFLINE  "PixDiffsLineM"    //                                           "                       "
//#define DEFAULT_CSV_OUTFILENAMEPDIFFFILE  "PixDiffsFileM"    //                                           "                       "
//#define DEFAULT_BIN_OUTFILENAMEPDIFFFILE  "PixDiffsFileM"    //Was "PixDiffsFile.bin". Now motor number will be appended b4 saving, and the .bin will be added by mySave_File()
#define DEFAULT_BIN_OUTFILENAMEENERGIES_READ  "EnergiesRead.bin"
#define DEFAULT_CSV_OUTFILENAMEENERGIES_READ  "EnergiesRead.csv"
#define DEFAULT_BIN_OUTFILENAMESTARTPOSITIONS "StartPositions.spf"

#define _WIN32_WINNT 0x0502
//#define _USE_32BIT_TIME_T
//#define DEFAULT_HGRAM_FILENAME "\\Test1.hcf"

#define BACKSLASH  '\\'

#define bDONTCOPY2ENERGYARRAY 0
#define bCOPY2ENERGYARRAY     1
#define DUMMY_GONIO_COL_IDX   0
#define DUMMY_NOOF_ROWSbetweenBEAMS 0

#define NOOFMOTORS             40 //Note Cannot change this without modifying the way the spf file is read //Sentinels used in the code require NOOFMOTORS <=125
#define NOOFMOTORSm1           39 //Keep Linked to above

#define PACKETLENGTH         1472  //1472 is max on my system. Upto 3 packets of shorts are sent per line (732,734,734) plus a 4 byte 'header' per
//#define NOOFLINES2SEND     2000  // packet, plus an additional 4 byte 'header' on 1st packet.
#define NOOFLINES2SEND          2  //#define NOOFROWS_IN_HOLOG   2000
#define NOOFPACKETS_PER_ROW     3
#define NOOFPACKETS2SEND        NOOFLINES2SEND * NOOFPACKETS_PER_ROW

#define LONG_1ST_PACKET_DELAY500 500
#define SHORT_SUBSEQUENT_PACKETS_DELAY1 1

#define SIZ_CSV_HEADER   176 //4 * 40 + 4 * cr/lf pair + the first line of data (NoofLines)
#define SIZ_CSV_HEADERp4 180 //(keep linked)
#define SIZ_CSV_HEADERm8 168 //(keep linked)
#define SIZ3LINES_OF_CSB_HEADER 126 //(keep linked)

#define XAXIS_PITCH 2        //The number of millimeters the XY stage's X axis moves per single revolution of the lead screw
#define YAXIS_PITCH 2        //The number of millimeters the XY stage's X axis moves per single revolution of the lead screw
#define SIZaccelPROFILES       200 //Keep even (as table has two halves)  } keep linked
#define NOOFaccelPROFILES      100 //                                     } keep linked    Also - Keep same as MAXLEADINorOUT
#define GONIO_DEGREES_PER_REV    2
#define GONIO_STEPS_PER_DEGREEorMM 100 //GONIO_STEPS_PER_DEGREEorMM is 100 because 1 revolution is 2 degrees


 //There are now 40 receive buffers, 1 for each motor, so messages from one motor should not get confused with messages from another:
#define SIZ_REC_BUFF 1200      //Why this size? 4 * 2200 is obvious (pixPositions). What else? (spurious stuff, perhaps?). Many packets sent by AVR are 1144 bytes
                               // perhaps a status req might be in there as well.
#define MAXLEADINorOUT        100 //Not fully implemented, but may help identify some parts of the code that will need changing, if this is ever changed. Keep same as NOOFaccelPROFILES
#define HALFMAXLEADINorOUT     50
#define MAXNOOFPIXELS_IN_ROW 2200
#define MAX_NOOFLINES        2000 //Note Cannot change this without modifying the way the spf file is read. It must never exceed 32,767
#define MAINWINDOW_X_SIZE     800
#define MAINWINDOW_Y_SIZE     505

#define NOOF_ENERGYRECS   4320000   // = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)
#define NOOF_ENERGYRECSm2 4319998

#define SIZFILTER              60
#define SIZMES                512
#define SIZMissFileMess       _MAX_PATH + 72
#define SIZszGLWndTitle       _MAX_DIR + 11
#define SIZSZDUM12             12
#define SIZ_8832000       8832000

#define JOGFASTNOOFSTEPS_P    640
#define JOGFASTNOOFSTEPS_M   -640
#define JOGMEDMNOOFSTEPS_P     12
#define JOGMEDMNOOFSTEPS_M    -12
#define JOGSLOWNOOFSTEPS_P      1
#define JOGSLOWNOOFSTEPS_M     -1
//
#define TC_CNT               1600

#define ENAB_POWR                   1
#define DISAB_POWR                  2
#define ENAB_485                    3
#define DISAB_485                   4
#define ENAB_POWR_WITH_VERSION      5 //At startup, windows can now tell the AVR to send its software version number in the status record. I'll start version numbers with 999999 eg 999999001 to make them recognisable.

#define REVERSE_DIR  1
#define FORWARD_DIR  0

//#define WANT_CSV_ENDING 0
//#define WANT_CSB_ENDING 1
#define WANT_PRN_ENDING 2

#define FORCE_UPDATE        TRUE  //Write changes to Registry immediately
#define CACHE_UPDATE       FALSE  //Don't changes to Registry immediately

#define SHOWPPOSLINE  1
#define SHOWPDIFFLINE 2
#define SHOWPDIFFFILE 3

#define bHIDEMESSAGES        0
#define bDONTSTOP_WHEN_HOMED 0
#define bSTOP_WHEN_HOMED     1
#define bHOME_ALL_REGARDLESS 1

#define PIX_TRIGGER_ONLY 1  //Type of Run for R unControlBoxProgram()
#define PARSE            2
#define START_PRINT      3

#define BWARNIfITSAnACCPROFILE      1
#define BDONT_WARNIfITSAnACCPROFILE 0

#define MOTORNO_NOT_USED     0

#define DONT_CHECK_STATIC_MOTOR_CSUMS     0
#define CHECK_STATIC_MOTOR_CSUMS_AS_WELL  1

#define TFnP_BMOVE_NOW         0x01
#define TFnP_BPROPER_CSUM_CHKS 0x02
#define TFnP_BTHIS_IS_FASTMOVE 0x04

#define UNUSED_MOTORNO 99
#define UNUSED_LEADINOUT 0

#define PID_RECT_L  128
#define PID_RECT_R  991
#define PID_RECT_T  663
#define PID_RECT_B 1092

#if 0  //Use Normal Length TimeOuts (but remember 45 TIMEOUT_FOR_GETSTATUS is too short for Long Homing Profile Generation us 95?)
//The following timeouts are in tenths of seconds - dependent Sleep(100) in com_Thr_CheckAllWaiting4reply():
//Keep TIMEOUT_FOR_STARTUP unique
#define TIMEOUT_FOR_STARTUP                        6
#define TIMEOUT_FOR_GETSTATUS                     45 //Was ok on 15 until it came across a long 2nd line in motor position file
//#define TIMEOUT_FOR_GETSTATUS                   95 //Was ok on 15 until it came across a long 2nd line in motor position file
#define TIMEOUT_FOR_GETSTATUSAFTERTransfer_FileAndPerform 185 //After Sending a single file, must wait for ConvertJPMessageDataToTClargeSDarray() &
//#define TIMEOUT_FOR_GETCHECKSUM                100 //This has worked as low as 73, However IT IS DEPENDENT ON THE DATA SENT.  After Motor position data
#define TIMEOUT_FOR_GETCHECKSUM                  160 //This has worked as low as 73, However IT IS DEPENDENT ON THE DATA SENT.  After Motor position data
// is sent, AVR calculates the Checksum and Calculates the TC values for the first line (and is too busy to answer whilst doing this). A lot of high speed
// movement for a long line means many Usteps have to be calculated, which takes a long time compared to short or slow lines.
#define TIMEOUT_FOR_GETPIXFREQ_TABLE               5
#define TIMEOUT_FOR_GETPIXEL_TABLE_SENDPOSLINE    20
#define TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSLINE  20
#define TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSFILE 100 //It takes 4.5 seconds to receive and deEndianize the 8.8Mbyte file on my PC, this allows 8 seconds

#else
//These are all deliberately far too long, but I need to be certain 'lost' messages are not just 'delayed' until AVR has finished doing something else.
//Keep TIMEOUT_FOR_STARTUP unique
#define D1900 300  //Sets a 30 second timeout
//#define D1900 6000  //Sets a 600 second timeout
  #define TIMEOUT_FOR_STARTUP                        6
  #define TIMEOUT_FOR_GETSTATUS                    D1900
  #define TIMEOUT_FOR_GETSTATUSAFTERTransfer_FileAndPerform D1900
  #define TIMEOUT_FOR_GETCHECKSUM                  D1900
  #define TIMEOUT_FOR_GETPIXFREQ_TABLE             D1900
  #define TIMEOUT_FOR_GETPIXEL_TABLE_SENDPOSLINE   D1900
  #define TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSLINE D1900
  #define TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSFILE D1900

#endif

#define MsTyp1_MOTOR_POS_DATA     0x01
#define MsTyp2_ZENDstatREC        0x02
#define MsTyp3_HOME               0x03
#define MsTyp4_RESET              0x04
#define MsTyp5_SING_MM            0x05
#define MsTyp6_ENAB_DISAB         0x06  //subtypes for enab/disab Power and for enab/disab RS485
#define MsTyp7_SING_MM_OTF        0x07
#define MsTyp8_SET_PULSE_LENGTH   0x08
#define MsTyp9_SENDpixPOS_TABLE   0x09
#define MsTypA_SETpixFREQ         0x0A
#define MsTypB_SING_MM_PID        0x0B
//#define MsTypB_STARTSTOP_TOGGLE   0x0B
#define MsTypC_STOP               0x0C  //I'll leave this for the time being, although it's not currently used.
#define MsTypD_SENDchecksumREC    0x0D
//#define MsTypD_CHANGE_PIXFREQ     0x0D
#define MsTypE_SET_NEXTLINE2PRINT 0x0E
#define MsTypF_YOU_ARE_A_PID      0x0F
#define SUBTYPSENDPOSLINE          139  //These numbers were chosen
#define SUBTYPSENDDIFFSLINE        140  // at randon to give unique subtyes
#define SUBTYPSENDDIFFSFILE        141  //  Originally, 139 meant that a
#define SUBTYPSENDFREQTABLE        142  //   reply was expected
#define TYPE1CSV 1
#define TYPE2CSV 2
#define TYPE3CSV 3
#define TYPE4CSV 4
#define TYPE5CSV 5

#define NO_HOLOGRAM_NAME  "No Hologram <-"

#include "resource.h"
#include "targetver.h"

//#include <string>
#include <iostream>

#define WIN32_LEAN_AND_MEAN       // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Commdlg.h>
#include <commctrl.h>
// C RunTime Header Files
#include <stdlib.h>
#include <io.h>
#include <SYS\STAT.H>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <stdio.h>
#include <time.h>
#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <strsafe.h>
#include <assert.h>

// Other Header Files
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <iphlpapi.h>

#include "mmsystem.h"
#include "registry.h"
#include <direct.h>

#include <fcntl.h>

//#include "packet32.h"
#include "ftd2xx.h"
#include <richedit.h>

#include "atlstr.h"
#include "CommunicationData.h"

#include <atltime.h>
#include "GNetClient.h"
#include "mjsTestNetClientDlg.h"


#include <fstream>
#include "myException.h"
#include "myLog.h"

#include "myHostInfo.h"
#include "myClient.h"
//#include <conio.h>
//#include <sys/timeb.h>
#include <time.h>

//#include "wavefile.hpp"

//#include "adc200.h" //for reading voltages from picoscope

//In Windows, this structure starts at the LSB and ends at the MSB
typedef struct  {
   int uStepsFromHome;
   unsigned int uVARa;
   unsigned int uVARb;
   unsigned int uVARc;
   unsigned int usHomingState:2;  //0-3 See comments in main program (1 at startup, 0 once we've homed; 2 & 3 when we're homing)
   unsigned int bRS485Enabled:1;  //No RS422 Comms are to be sent if this is zero
   unsigned int bDriverEnabled:1; //The line to the MDS415 is set so that the motors are powered
   unsigned int ImReadyTPAL:1;    //I'm ready To Print a Line
   unsigned int HomeSwitch:1;
   unsigned int Limit2Switch:1;
   unsigned int Limit1Switch:1;

// unsigned int ImNotAllowed2PutStatusOutLineLow:1; //Prevent this module this module driving StatusOutLineLow (I'm not convinced this is a good idea - the
//   PC should use ethernet to clear all errors [and to find out who is signalling] them).
   unsigned int SeriousWarnings:8; //Things to report, that stopped a print line
   unsigned int Warnings2Report:8; //Unusual program conditions, or things to report, that shouldn't stop a print line

   unsigned int Length:6;
   unsigned int StatusOutLineIsHi:1;     //This board's reading of the AVR_STATUS_OUT line (The line may be pulled low by this board OR another board)
   unsigned int IWantStatusOutLineLow:1; //Set when a condition exists that must be reported to CPC (& the line must be held low).  Stops the laser.
  }STATUSRECORD;

//void CallManagedModule(void); 
void CallUnManagedModule(void); //This really is the Unmanaged module, even though I have the names of the files back to front!!!

ATOM                MyRegisterClass(HINSTANCE hInstance); //Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc                         (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SingleMM_Dialog                 (HWND, UINT, WPARAM, LPARAM); //Single Motor Move dialog
INT_PTR CALLBACK    SingleMM_PID_Dialog             (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ViewHologInfo_Dialog            (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ViewPrinterConfig_Dialog        (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Control_Energies_Dialog         (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    JobsOnDisplayPC_Dialog          (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ViewPCFversusHCF_Dialog         (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ViewMotorStartPointsInHCF_Dialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TransBlock_Dialog               (HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    SelActMotor_Dialog              (HWND, UINT, WPARAM, LPARAM);  //This function in SelActMotor_Dialog.cpp is now renegated to the OldCode folder
INT_PTR CALLBACK    OpenConfigFile_Dialog           (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK StartUpOptions_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK VHI_OtherPrePixZeroConsts_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//INT_PTR CALLBACK    SaveConfigFile_Dialog           (HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK  SaveEnergiesReadFile_Dialog     (HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK  DisplayStatRec_Dialog           (HWND, UINT, WPARAM, LPARAM); not used anymore
//INT_PTR CALLBACK  DisplayStatRec_DialogM          (HWND, UINT, WPARAM, LPARAM); functionality moved into main window
void UseRecvStatRecToUpdateTheFields(int n_MotorNo, STATUSRECORD *pStatRec);
//void UseRecvStatRecToUpdateTheFields(HWND hDlg, int n_MotorNo, STATUSRECORD *pStatRec);
void ClearMostFields(int nnMotorNo);

INT_PTR CALLBACK    HelpDialog1_Dialog              (HWND, UINT, WPARAM, LPARAM);
//LRESULT DisplayMyMessage(HINSTANCE hinst, HWND hwndOwner, LPSTR lpszMessage);
INT_PTR CALLBACK    About                           (HWND, UINT, WPARAM, LPARAM);

int myOpen_File (HWND hWnd, const char *fType, const char *pszFile2Open, int nOFmotorNo); //nOFmotorNo used only for csv/csb files
int myOpen_File_VariableAp_CSV (const char *pszFile2Open,  int nExpectedNofRows,  int nExpectedNofCols, BOOL bCopyToEnergyArray, int nLeadInOut, int nGONIO_COL_IDX, int nNoofRowsBetweenBeams);
BOOL mGetFile(HWND hWnd, UINT uiFilter, BOOL bSavingAFile);
BOOL myGetFolder(HWND hWnd, UINT uiFilter);//, BOOL bSavingAFile);
int  mySave_File (HWND hWnd, long reqSpace, char *fType, int nTypeOffType, char *pszFile2Save, char nch_CurrentMotorZB);
BOOL CreateNewHCFfile(HWND, char*, BOOL*);
int  Transfer_FileAndPerform(HWND, char nchCurrentMotorZBtfNb, BOOL nMoveNowANDdoCheckSumsANDisAfastMove, const char *pszTFnPFileName, int nReqDistance, BOOL bWarnIfItsAnAccProfile);
int  Transfer_ALL_FilesAndPerform(HWND, BOOL bMoveNowRadioButPressed, const char *pszTFnPFileName, unsigned short nNextLine2Print);
int  Transfer_ALL_AccProfsAndPerform(HWND, BOOL);//, const char *pszTFnPFileName);
void StopPIDMovementWhileHomingOrFileTransfering(void);
void restartPIDs(void);
void HomeSingleMotor(HWND hDlg, char nchCurrMotrZB);
void SendMsTyp3_HOMEmessage(HWND hDlg, char emMTSP);
void FastMoveSingleMotor(HWND hDlg, char nchCurrMotrZB, int nReqDistance);
void FastMoveSingleMotorABS(HWND hDlg, char nchCurrMotrZB, int nReqPos, STATUSRECORD *SR4nCurrentPos);

//void InitializeRevBitsTable(void);
//int  SendMuMessagePackets (HWND,char,unsigned char,unsigned int,unsigned int); //Wrapper around SendMessagePackets() controlled by a Mutex
int  SendMessagePackets(HWND hDlg, char chCardNo, unsigned char uchMessType, unsigned int nPulsesOrSubTypOrFOPP, unsigned int nSpeed); //nPulses - now
                               // nPulsesOrSubTypOrFOPP - maybe +ve or -ve, indicating direction.
int  ReceiveMessagePackets (HWND hDlg, char chCardNo, unsigned char uchExpectedMessType, unsigned int LineSubPacket);
int  InitialiseAllSockets  (HWND);
int  DestroyAllSockets     (void);
void SafelyDisplayErrorAndCleanUp(HWND hDlg, TCHAR *pMess2Display, int retVal);
void MessageBoxToDisplaySeqNoError(HWND hDlg, char *pszMesTitle, unsigned char cMessageSeqNoSR, unsigned char chRec_Seqno, int nMotorNum);
void MessageBoxWarnNotConnected(HWND hDlg);
void DisplayErrorMessageBoxWithErrorAndMotorNo(HWND hDlg, char nErrorMotorNum, char *pszMess, char *pszTitle);
void InitialiseAllReceiveBuffers(void);
void DumpReceiveBuffers2File(char *);
//void MakeErrorSound(void); commented out as not currently called
void MakeWarningSound(void);
void MakeAcknowledgeSound(void);
//void WarnIfSoundFileIsMissing(HWND hWnd);
int  Create_CheckAll_MonitoringThread (HWND hWnd);
static DWORD WINAPI com_Thr_CheckAllWaiting4reply(void); //CA == Check All
int  bCheckAlltheChecksumsAreOK(HWND hDlg, BOOL bCheckStatecMotorsCSumsAsWell);
//void U pdateMainWindowsControlsM(HWND, int M otorNo);
int  UpdateMainWindowsControlsM(void);
int  SendMessToDisplayPC(HWND hWnd, HWND hCout, int *nRowCount, int nButtonPressedZB);

//Goniometer Calculation functions
void SetUpGlobals(double RefBeamTilt, double LightCoords, double LightCoordsOmega, double NoofCols, double HorzDistanceBetweenPixelCentersCMS, double NoofLines, double VertDistanceBetweenPixelCentersCMS);
double GetWxInDegreesFromSpreadSheet(double CellB7, double CellC7);
double GetWyInDegreesFromSpreadSheet(double CellB7, double CellC7);
//double GetWxInMMFromSpreadSheetExportFile(int nHorzCoord, int nVertCoord);//Coords in pixels     Currently replaced by m yOpen_File_VariableAp_CSV()
//double GetWyInMMFromSpreadSheetExportFile(int nHorzCoord, int nVertCoord);//Coords in pixels     Currently replaced by m yOpen_File_VariableAp_CSV()

void com_Exit(void);
//void redraw(HWND, RECT*);
void redraw(void);
void redrawPIDrect(void);
//BOOL DriveSpaceShort(long reqSpace, char Drive, double* avail);
BOOL DriveSpaceShort(long reqSpace, char Drive, PULARGE_INTEGER avail);
void myLoadString(UINT);
void StripTrailingSpaces(char*);
void myWaitForAVRdelay(int noofLoops);
void DisplayFileInEditor(HWND hWnd, char* szFileToView);
int  MoveToStartPositions(HWND hDlg, /*BOOL bMove2SPsFast,*/ BOOL bSeeMessages, BOOL bMove2SPsHomeRegardless, BOOL bStopWhenHomed);
void MoveREF_BEAM_TABLEtoLoadPosition(HWND hDlg, int nDesiredRefTablePosM1, int nDesiredRefTablePosM2, BOOL bMoveRefTableFast);
void MoveMotor2AbsolutePosition(HWND hDlg, char nMotorZB, int nDesiredPos, BOOL bMoveMotorFast);
int  GetUstepsForThisRatio(int WP1ratio, int Beam); //Beam is 1,2 or 3 for Red, Green or Blue
BOOL BuildRealEnergyADClookups(HWND hWnd, int *nNoofGivenPointsAr);
//BOOL bCSfromUC3isSameAsCSfromSendBuffer(HWND hDlg, char eaM_cs, int nLine2StartFrom);
BOOL bCSfromUC3isSameAsCSfromSendBuffer(HWND hDlg, char eaM_cs);
void crcgen(void);
unsigned int CalcCRC4Table(int nNoofTableChars, void *bufferpointer);//unsigned int CalcCRC4Table(int nNoofTableChars, void *bufferpointer, void *shtLineNo);
void SetRedWarning(void);
int PrinterConfigMatchesHCFconfig(HWND hDlg);
void RunControlBoxProgram(HWND hWnd, int nTypeOfRun, int nLineLength, int nTotalLines, int nShutterStartPixel, int nShutterStopPixel, int nShutterRedStartLine, int nShutterRedStopLine, int nColorOffset, int nLineDelay, int bAutoClose);
BOOL bCheckSingleDecimalFieldIsValid(HWND hDlg, int *nGood_rNumberTimes100, int nControlID);

//int GenerateStartPositionsFile(HWND hDlg, int nIdxMotor);
//int DecodeNoofUstepsToAnIndex(int chHCFrecNoofUsteps4EachMotorFromTab); (no longer a global function)
//int DecodeFreqToAnIndex(int nFrequencyOfPixelPrintsInHz);               (no longer a global function)
 typedef struct  {
  //First the Accel part
    int nSpos;          //is the position at the beginning of line of the Gonio before the LeadIn, in uSteps. Calculated, somehow! This is where "Move To Start Positions" button must position the motor
    int nTotalPixelNoM11A; //nTotalPixelNoM11A will be set to the LeadIn *for this motor and for this line*
    int nFpos;    //is the position at Pix zero of the Gonio (after the LeadIn) in uSteps. The first position in a row from the spreadsheet, but converted to uSteps.   Wx
    int nSpeedAtFinish; //the number of uSteps between the first position in a row (from the spreadsheet) to the second position in the row.
    int nAmountAddedToSpos;

    //typedef struct{
    int nNoofAdditionalTPsRequired;
    BOOL bIsAnOddNumber;
    int nAmountOfMaxBarRemaining;
    //} ADDITIONAL_TPinfo_STRUCT;

    //int nConstSpeedsAddedToSpos; REMOVED
  //Now the Decel part
    int nEndSpos;          //is the position at the end of line of the Gonio after the LeadOut, in uSteps. Calculated, somehow! This must be identical to nSpos for the next line.
    int nEndTotalPixelNoD; //nEndTotalPixelNoD will be set to the LeadOut *for this motor and for this line*. LeadIn and LeadOut (for this line) will probably be different. We can't even say the LeadOut for
                           // this line is the same as the LeadIn for next line... the stationary position must be the same, but nFpos & nSpeedAtFinish for the next line may be different.
    int nEndFpos; //is the position at Pix last of the Gonio (before the LeadOut) in uSteps. The last position in a row from the spreadsheet, but converted to uSteps.  Wx
    int nEndSpeedAtFinish;
    int nAmountAddedToEndSpos;
    //int nConstSpeedsAddedToEndSpos; REMOVED
    //typedef struct{
    int nEndNoofAdditionalTPsRequired;
    BOOL bEndIsAnOddNumber;
    int nEndAmountOfMaxBarRemaining;
    //} ADDITIONAL_TPinfo_STRUCT;
   } RECORD_AccelSFS; //Accereration Start(Position) Finish(Position) Speed (at Finish)
void SetLenszAccelFileNamesDir(int nUnchangingLength);
int LoadAndCheckAccelProfilePRNfile(HWND hDlg, char *szAccelFileNamesDir, int idxMnoP1, int *nAccProfile, char chHCFrecNoofUsteps4EachMotorFromTab, int nHCFrecFrequencyOfPixelPrintsInHz); //nAccProfile is the array to be loaded
int ConstructedFileName(HWND hDlg, char *szAccelFileNamesDir, int idxMnoP1, char chHCFrecNoofUsteps4EachMotorFromTab, int nHCFrecFrequencyOfPixelPrintsInHz, int nEndingWanted);
int GenerateProfileForM39(HWND hDlg,//This function sets the global nLengthOfM39rowsIncLeadInOut_inPixels
                           int,     // HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams
                           int,     // HCFrec.uiNoofCols
                         //int,     // HCFrec.nNoofColsPlusLeadInOut, // cols (as far as motion is concerned)
                           int,     // nNOOFprePIXzeroCONSTspeedPIXELS
                           int,     //nHCFrecFrequencyOfPixelPrintsInHz //Used to choose the Accel Profile, and also put in the csv/csv header. Currently, Only 10 15 30 & 60 are allowed
                           char,    // HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38]
                           int,     // HCFrec.uiHorzDistBetwPixelCenters_nanoM
                         //int,     // nHCFrecHorzDistBetwPixelCenters_nanoM
                           time_t *,// *ptr_HCFrec_uTimeStamp
                           char*,   // szHCFrecHologramName
                           int,     // nXposOfHologsOriginOnPlateMM
                           int,     // nNoofRowsBetweenBeams
                           int,     //nM39_XAXISBACKLASH
                           int*);   //*ptrPixelAtwhichAccelStops
int GenerateEnergyProfileForWP3(HWND hDlg, int idxMno, int nLines, int nCols, int nLeadInAndOut, double dblMinVolts, double dblMaxVolts); //function prototype - Use Volts to generate dummy data

int GenerateProfileForM40(HWND hDlg,//This function also reads the global nLengthOfM39rowsIncLeadInOut_inPixels
                           int,     // HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams
                           int,     // HCFrec.uiNoofCols
                         //int,     // HCFrec.nNoofColsPlusLeadInOut // cols (as far as motion is concerned)
                           int,     //nHCFrecFrequencyOfPixelPrintsInHz //Used to choose the Accel Profile, and also put in the csv/csv header. Currently, Only 10 15 30 & 60 are allowed
                           char,    // HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38]
                           UINT,    // HCFrec.uiVertDistBetwPixelCenters_nanoM
                           time_t *,// *ptr_HCFrec_uTimeStamp
                           char*,   //szHCFrecHologramName
                           int);    //YposOfHologsOriginOnPlateMM
int GenerateProfileForM11(HWND hDlg,//This function also reads the global nLengthOfM39rowsIncLeadInOut_inPixels
                           int,     //idxMno : 10, 22, or 34
                           int,     //nHCFrecNoofLines
                           int,     //nNoofRowsBetweenBeams
                           int,     //nNOOFm11prePIXzeroCONSTspeedPIXELS
                           int,     //nHCFrecNoofCols
                           int,     //nHCFrecFrequencyOfPixelPrintsInHz //Used to choose the Accel Profile, and also put in the csv/csv header. Currently, Only 10 15 30 & 60 are allowed
                           char,    //chHCFrecNoofUsteps4EachMotorFromTab
                           UINT,    //uiHCFrecHorzDistBetwPixelCenters_nanoM
                           UINT,    //uiHCFrecVertDistBetwPixelCenters_nanoM
                           int,     //RefBeamTiltTimes1000
                           int,     //LightCoordsDTimes1000
                           int,     //LightCoordsOmegaTimes1000
                           time_t *,//ptr_HCFrec_uTimeStamp
                           char*,   //szHCFrecHologramName
                           int );   //nHCFrecNoofRowsBetweenBeams
int GenerateProfileForM12(HWND hDlg,//This function also reads the global nLengthOfM39rowsIncLeadInOut_inPixels
                           int,     //idxMno : 11, 23, or 35
                           int,     //nHCFrecNoofLines
                           int,     //nNoofRowsBetweenBeams
                           int,     //nNOOFm11prePIXzeroCONSTspeedPIXELS
                           int,     //nHCFrecNoofCols
                           int,     //nHCFrecFrequencyOfPixelPrintsInHz //Used to choose the Accel Profile, and also put in the csv/csv header. Currently, Only 10 15 30 & 60 are allowed
                           char,    //chHCFrecNoofUsteps4EachMotorFromTab
                           UINT,    //uiHCFrecHorzDistBetwPixelCenters_nanoM
                           UINT,    //uiHCFrecVertDistBetwPixelCenters_nanoM
                           int,     //RefBeamTiltTimes1000
                           int,     //LightCoordsDTimes1000
                           int,     //LightCoordsOmegaTimes1000
                           time_t *,//ptr_HCFrec_uTimeStamp
                           char*,   //szHCFrecHologramName
                           int );   //nHCFrecNoofRowsBetweenBeams
int GenerateProfileForM7(HWND hDlg,//This function also reads the global nLengthOfM39rowsIncLeadInOut_inPixels
                           int,     //idxMno : 6, 18, or 30
                           int,     //nHCFrecNoofLines
                           int,     //nNoofRowsBetweenBeams
                           int,     //nNOOFm11prePIXzeroCONSTspeedPIXELS
                           int,     //nHCFrecNoofCols
                           int,     //nHCFrecFrequencyOfPixelPrintsInHz //Used to choose the Accel Profile, and also put in the csv/csv header. Currently, Only 10 15 30 & 60 are allowed
                           char,    //chHCFrecNoofUsteps4EachMotorFromTab
                           UINT,    //uiHCFrecHorzDistBetwPixelCenters_nanoM
                           UINT,    //uiHCFrecVertDistBetwPixelCenters_nanoM
                           int,     //RefBeamTiltTimes1000
                           int,     //LightCoordsDTimes1000
                           int,     //LightCoordsOmegaTimes1000
                           time_t *,//ptr_HCFrec_uTimeStamp
                           char*,   //szHCFrecHologramName
                           int );   //nHCFrecNoofRowsBetweenBeams
//int GenerateProfileForM23(HWND hDlg,int,int,int,int,int,char,UINT,UINT,int,int,int,time_t *,char*,int ); //Args as for M11 above
//int GenerateProfileForM35(HWND hDlg,int,int,int,int,int,char,UINT,UINT,int,int,int,time_t *,char*,int ); //Args as for M11 above

void FillInHeaderForGeneratedFile(int nHCFrecNoofLines, int nHCFrecNoofCols, int nHCFrecFrequencyOfPixelPrintsInHz, int nACCEL_DECEL_PROFILE_SIZE, time_t *ptr_HCFrec_uTimeStamp, char *szHCFrecHologramName); //the above 'Generate' functions call this

inline void endian_swap(unsigned int &x) {
 x =(x>>24) |
   ((x<<8) & 0x00FF0000) |
   ((x>>8) & 0x0000FF00) |
    (x<<24);
}
inline void short_endian_swap(unsigned short &x) {
 x =(x>>8) |
    (x<<8);
}

typedef struct  {
 int nUnused[199];
 int nM39_XAXISBACKLASH;
 char ATMEL_IP_ADDRESStab[NOOFMOTORS][16];
 char chNoofUsteps4EachMotorTab[NOOFMOTORS];
 char bMotor2ExpectRepliesFrom[NOOFMOTORS];
 char bMotorIS_A_PID[NOOFMOTORS];

 int  nNoofUsteps_Hm2WP1MinEnergy[3];               //M1
 int  nNoofUsteps_Hm2WP2MinEnergy[3];               //M2
 int  nNoofUsteps_Hm2WP3MinEnergy[3];               //M3
 int  nNoofUsteps_Hm2LOobjXXXmm[3];                 //M4

 int  nNoofUsteps4EachApertureTab[9];               //M5 +gb
 int  nNoofUsteps4EachFocusTab[3];                  //M6 +gb

 int  nNoofUsteps_Hm2CenterPointT[3];               //M7
 int  nNoofUsteps_Hm2CenterPointB[3];               //M8
 int  nNoofUsteps_Hm2CenterPointL[3];               //M9
 int  nNoofUsteps_Hm2CenterPointR[3];               //M10
 int  nNoofUsteps_Hm2GonHzeroDegrees[3];            //M11
 int  nNoofUsteps_Hm2GonVzeroDegrees[3];            //M12

 int  nNoofUsteps4RefBeamTablePrintUnload[4];       //M37, 38  (M37PrintPos,M37LoadPos,M38PrintPos,M38LoadPos)
 int  nNoofUsteps_XYstageHm2Origin[2];              //M39, 40
 int  nPhysLimMinUsteps[NOOFMOTORS];
 int  nPhysLimMaxUsteps[NOOFMOTORS];
} PCFRECORD;

typedef struct  {
 //plus 4 is 78
 time_t		   uTimeStamp;               //nb was 64-bit, now 32 as _USE_32BIT_TIME_T is defined (well, maybe!)
 //plus 42 is 78
 BOOL          bHCFIsComplete;
 //plus 42 is 78
 char          szHologramName[44];      //best to keep this a mulitple of 4 as that is what the compiler allocates
 //fifty six bytes follow:
 unsigned int  uiNoofLines;
 unsigned int  uiNoofCols;                //eg 2000
 unsigned int  nNoofColsPlusLeadInOut;   //eg 2200 THIS IS CURRENTLY UNUSED (ALTHOUGH THE VALUE IS COLLECTED / REDISPLAYED ON THE VHI SCREEN). THE SPACE IS OPEN FOR REUSE.
 unsigned int  uiPixelSizeH_uMetres;
 unsigned int  uiPixelSizeV_uMetres;
 unsigned int  uiHorzDistBetwPixelCenters_nanoM;
 unsigned int  uiVertDistBetwPixelCenters_nanoM;
 unsigned int  uiFrequencyOfPixelPrintsTimes1M; //The refresh rate of the LCD screen time 1,000,000. Currently, Only 10M 15M 30M & 60M exactly are allowed
 int           XposOfHologsOriginOnPlateMM;     //This is the hologram's origin (in mm) relative to the Plate's origin. i.e. where to print on the plate
 int           YposOfHologsOriginOnPlateMM;     //This is the hologram's origin (in mm) relative to the Plate's origin. i.e. where to print on the plate
 int           WP1rRatioTimes1000;
 int           WP1gRatioTimes1000;
 int           WP1bRatioTimes1000;
 int           WP2rObjEnergyTimes1000;
 int           WP2gObjEnergyTimes1000;
 int           WP2bObjEnergyTimes1000;
 int           nApertureSize;        //0 == 0.8mm, 1 == 1.6mm, 2==Pinhole (but pinhole is not valid, for a hologram)
 int           ObjBeamLengthR;
 int           ObjBeamLengthG;
 int           ObjBeamLengthB;
 //int           nLine1StartPositionsFEM[40];//ForEachMotor. But note, Motors 5, 17 & 29 will not be populated: nApertureSize determines these (using the pcf file)
                                           //The start positions for all other lines can be calculated by looking at the movement profiles
                                           //This array is likely to do a duplicate of the first record in "StartPositions.spf" file so, at some future time, either
                                           // this array or the "StartPositions.spf" file is likely to be removed.
 int           nNextLineToPrint;
 int           bThisHoloGramHasABadError;  //E.g. a file is missing, such as StartPostions.spf (1). Zero if OK.
 //char          chNoofUsteps4EachMotorTab[NOOFMOTORS]; //Now in HCF_PCFrecCopy
 int           nNoofRowsBetweenBeams;
 int           UnusedANDavailable;
 int           RefBeamTiltTimes1000;
 int           LightCoordsDTimes1000;
 int           LightCoordsOmegaTimes1000;
 int           NOOFprePIXzeroCONSTspeedPIXELS[NOOFMOTORS]; //Although applicable only to the dynamic motor, It is much more consistent to define this field fo every motor.
 int           nLengthOfM39rowsWhenGenerated_inPixels;
 int           nLeadInForM39;
 int           nNoofLinesIncludingBetweemTheBeams;
 int           nPixelAtwhichAccelStops;
 int           Unused[599]; //This is to help avoid needing to recreate the file every time I add a field.  When adding fields, reduce this number accordingly.
 PCFRECORD     HCF_PCFrecCopy;
 //Yet To be Defined GEOMETRY DATA
}HCFRECORD;      //Hologram Configuration record  (the Hologram General File)
//Complementing the above, whenever an H CFRECORD is read in or a new copy of one is created, all the paths in this next record are
// next record are to be updated.
typedef struct  {
 char szCSVDirName4Reading[_MAX_DIR]; //Where the Intended Positions files are to be read from.
 char szCSVDirName4Writing[_MAX_DIR]; //Where the Actual Positions Results files and Actual Power files are to be placed.
 char szTimeStampedFolderName[_MAX_DIR];
} HCFRECORDsCURRENTfolders;

/* Test data for testing record sizes
typedef struct  {
 time_t		   uTimeStamp;               //nb was 64-bit, now 32 as _USE_32BIT_TIME_T is defined
}HCFRECORD2;      //Hologram Configuration record  (the Hologram General File)

typedef struct  {
 time_t		   uTimeStamp;               //nb was 64-bit, now 32 as _USE_32BIT_TIME_T is defined
 BOOL          bHCFIsComplete;
}HCFRECORD3;      //Hologram Configuration record  (the Hologram General File)

typedef struct  {
 time_t		   uTimeStamp;               //nb was 64-bit, now 32 as _USE_32BIT_TIME_T is defined
 BOOL          bHCFIsComplete;
 char          szHologramName[44];
}HCFRECORD4;      //Hologram Configuration record  (the Hologram General File)
*/

typedef struct  {

 int SendPktErrStatScr5001;
 int SendPktErrStatScrOther;

 int RecPktErrStatScr6666;
 int RecPktErrStatScr123456;
 int RecPktErrStatScrOther;
 int RecPktErrStatScrSeqNo;

 int SendPktErrTFnP5001;
 int SendPktErrTFnPOther;

 int RecPktErrTFnP6666;
 int RecPktErrTFnP123456;
 int RecPktErrTFnPOther;
 int RecPktErrTFnPSeqNo;

} COMMS_ERROR_COUNT;

#define NOOFENERGYRECFIELDS 11
typedef struct  {
 short nEnergyRead[NOOFENERGYRECFIELDS]; //Elements 0 & 1 shall contain lineNo and pixelNo from Altera, Elements 2 - 10 the ADC codes read fro WPs1,2,3 RGB
} ENERGYREC;         //This defines the records received over USB from the altera energy meters

typedef struct  {
 short  sADCcode;
 double dlbRealEnergy;
} ENERGY_CALIB_RREC; //A few of these records are processed into the ADC lookup tables (using interpolation)

typedef struct{
 double dState; // Last position input
 double iState; // Integrator state
 //double iMax, iMin;
 // Maximum and minimum allowable integrator state
 double iGain; // integral gain
 double pGain; // proportional gain
 double dGain; // derivative gain

 double Tab[16];
 //double intValue;
 int ptr;
 int pidID;
} SPid;

typedef struct{
 double errorRead;
 double PIDoutput;
 double pTerm; //Shows how much
 double iTerm; // these components
 double dTerm; //  play a part in the ouput
} PIDdebugStruct;

typedef struct{
 DWORD IPaddr;
 UCHAR MACaddr[6];
} ARP_PAIR;

typedef struct{
 DWORD dwFirstInterfaceIndex;
 BOOL  bFirstInterfaceHasAVRs;
 int   nFirstInterfaceNoofRecs2Add;
 ARP_PAIR FirstIFarpPairAr[40];
 DWORD dwSecondInterfaceIndex;
 BOOL  bSecondInterfaceHasAVRs;
 int   nSecondInterfaceNoofRecs2Add;
 ARP_PAIR SecondIFarpPairAr[40];
}ADDITIONALarpsSTRUC;

double UpdatePID(SPid * pid, double error, double position);

// C alcAddnalTPsReq(ADDITIONAL_TPinfo_STRUCT *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements);
int CalcAddnalTPsReq   (RECORD_AccelSFS *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements); //2 ADDITIONAL_TPinfo_STRUCT's are now embedded in RECORD_AccelSFS struct
int CalcAddnalEndTPsReq(RECORD_AccelSFS *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements); //2 ADDITIONAL_TPinfo_STRUCT's are now embedded in RECORD_AccelSFS struct


typedef FT_STATUS (WINAPI *PtrToOpen)(PVOID, FT_HANDLE *); 
typedef FT_STATUS (WINAPI *PtrToOpenEx)(PVOID, DWORD, FT_HANDLE *);
typedef FT_STATUS (WINAPI *PtrToSetBaudRate)(FT_HANDLE, DWORD);
typedef FT_STATUS (WINAPI *PtrToSetFlowControl)(FT_HANDLE, USHORT, UCHAR, UCHAR);
typedef FT_STATUS (WINAPI *PtrToSetDataCharacteristics)(FT_HANDLE, UCHAR, UCHAR, UCHAR);

typedef FT_STATUS (WINAPI *PtrToListDevices)(PVOID, PVOID, DWORD);
typedef FT_STATUS (WINAPI *PtrToClose)(FT_HANDLE);
typedef FT_STATUS (WINAPI *PtrToRead)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
typedef FT_STATUS (WINAPI *PtrToWrite)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
typedef FT_STATUS (WINAPI *PtrToResetDevice)(FT_HANDLE);
typedef FT_STATUS (WINAPI *PtrToPurge)(FT_HANDLE, ULONG);
typedef FT_STATUS (WINAPI *PtrToSetTimeouts)(FT_HANDLE, ULONG, ULONG);
typedef FT_STATUS (WINAPI *PtrToStopInTask)(FT_HANDLE);
typedef FT_STATUS (WINAPI *PtrToRestartInTask)(FT_HANDLE);
typedef FT_STATUS (WINAPI *PtrToGetQueueStatus)(FT_HANDLE, LPDWORD);

class DLLFuncs{
public:
    HICON m_hIcon;
//	HMODULE m_hmodule;
    FT_HANDLE m_ftHandle;

    PtrToOpen m_pOpen; 
    FT_STATUS Open(PVOID);

    PtrToOpenEx m_pOpenEx; 
    FT_STATUS OpenEx(PVOID, DWORD);

    PtrToListDevices m_pListDevices; 
    FT_STATUS ListDevices(PVOID, PVOID, DWORD);

    PtrToClose m_pClose;
    FT_STATUS Close();

    PtrToSetBaudRate m_pSetBaudRate;
    //FT_STATUS SetBaudRate(LPVOID, DWORD, LPDWORD);
    FT_STATUS SetBaudRate(FT_HANDLE, DWORD);

    PtrToSetFlowControl m_pSetFlowControl;
    FT_STATUS SetFlowControl(FT_HANDLE, UCHAR, UCHAR, UCHAR);

    PtrToSetDataCharacteristics m_pSetDataCharacteristics;
    FT_STATUS SetDataCharacteristics(FT_HANDLE, USHORT, UCHAR, UCHAR);

    PtrToRead m_pRead;
    FT_STATUS Read(LPVOID, DWORD, LPDWORD);

    PtrToWrite m_pWrite;
    FT_STATUS Write(LPVOID, DWORD, LPDWORD);

    PtrToResetDevice m_pResetDevice;
    FT_STATUS ResetDevice();
    
    PtrToPurge m_pPurge;
    FT_STATUS Purge(ULONG);
    
    PtrToSetTimeouts m_pSetTimeouts;
    FT_STATUS SetTimeouts(ULONG, ULONG);

    PtrToStopInTask m_pStopInTask;
    FT_STATUS StopInTask(FT_HANDLE);

    PtrToRestartInTask m_pRestartInTask;
    FT_STATUS RestartInTask(FT_HANDLE);

    PtrToGetQueueStatus m_pGetQueueStatus;
    FT_STATUS GetQueueStatus(LPDWORD);
};

DWORD WINAPI comUSB_Thr_CB(void);
int comUSB_Init(const char *portA,/* const char *portB,*/ HWND hWnd);
int comUSB_Exit(HWND);
//int comUSB_freeBuffer(void);
int LoadDLL(HWND hWnd);
//Function to store Operator Defined Calibration positions in a Calibration File.
//Create a basic HoloConfig Program that can edit the holoconfig file.
//Create a basic Calibration Program to edit/maintain the Calibration file for the 9 motors.

#include "stdafx.h"
//#include "stdafx1.h"

#include <Iphlpapi.h> //but for Vista it should be Ipmib.h

//int CountStatRecsRecvd_TimesPlaySoundCalled;


//extern double dblErrorWP2redDisplay;
//char str[100]; //info about adc
//#include <ERRNO.H>
//using namespace std;

//extern int nJUSTTRYINGSPEED;
//HANDLE ghMutex;
DWORD dwWaitResult;

int nGlobalLastUSBrecNoRecvd = -1;
ENERGYREC sArrEnergRecs[NOOF_ENERGYRECS];//[4320000]; // = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)

extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern HINSTANCE hInst;
extern char mes[];      //Anything can use this for anything (in the main thread) - just don't rely on it keeping its value for long
extern HMENU hMenuMain;
extern char szGlobalFileName[];
extern char szAccelFileNamesDir[_MAX_DIR]; //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//int         nLenszAccelFileNamesDir;       // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.
//extern char szCSVDirName4Reading[];
//extern char szCSVDirName4Writing[];
extern HCFRECORDsCURRENTfolders HCFcfs;

extern char szHGRAMSDirName4Reading[];
extern char szPrinterPCFfilename[];
extern char szWP1CalFilename[]; //This file also contains the data for WP13 and WP25
extern char szWP2CalFilename[];
extern char szWP3CalFilename[];
extern char szWP14CalFilename[];
extern char szWP15CalFilename[];
extern char szWP26CalFilename[];
extern char szWP27CalFilename[];
extern ENERGY_CALIB_RREC WavPlate2ForInterpAr[]; //91
extern ENERGY_CALIB_RREC WavPlate3ForInterpAr[]; //91
extern ENERGY_CALIB_RREC WavPlate14ForInterpAr[];//91
extern ENERGY_CALIB_RREC WavPlate15ForInterpAr[];//91
extern ENERGY_CALIB_RREC WavPlate26ForInterpAr[];//91
extern ENERGY_CALIB_RREC WavPlate27ForInterpAr[];//91

extern SPid PidStructVariableWP2Red; //Exposing these
extern SPid PidStructVariableWP3Red; // here allows
extern SPid PidStructVariableWP2Gre; //  the PIDs
extern SPid PidStructVariableWP3Gre; //   to be
extern SPid PidStructVariableWP2Blu; //    updated
extern SPid PidStructVariableWP3Blu; //     dynamically

extern HCFRECORD HCFrec;
extern HCFRECORD DefaultHCFrec;

extern PCFRECORD PCFrec;
extern PCFRECORD DefaultPCFrec;

//extern int bSTATREC_BOXisBeingDisplayed;
extern BOOL bMoveNowRadioButPressed;
//extern BOOL bReallyThisIsAnAccelProfile;
//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
//extern int nNoofUsteps4RefBeamTablePrintUnload[4]; //part of the pcf file
extern PCFRECORD PCFrec;

//int bMotor2ExpectRepliesFrom[NOOFMOTORS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //now in PCFrec

//NEVER set NOOFTIMESTHRU_B4OBJECTING >= 999,999,997 because that will break the hard-coded use of such numbers
extern int  NOOFTIMESTHRU_B4OBJECTING;
extern unsigned char cMessageSeqNoCS[NOOFMOTORS];
extern unsigned char cMessageSeqNoSR[NOOFMOTORS];

extern int bWaitingForReply[];
BOOL bTimerHasBeenKilled;


extern double PidOutputToActOn[];
extern int nNoofCatastrophys;

//extern BOOL bFirstTime; //Using to set the initial postion in the PID routine- ALSO when reenabling the motors!
//extern SPid PidStructVariableWP2Red;
//extern SPid PidStructVariableWP3Red;
//extern SPid PidStructVariableWP2Gre;
//extern SPid PidStructVariableWP3Gre;
//extern SPid PidStructVariableWP2Blu;
//extern SPid PidStructVariableWP3Blu;
extern volatile BOOL bStopPIDsendingMessages;
//extern BOOL bPIDMovementStoppedForHoming;
 
extern char TabDEFAULT_CSV_OUTFILENAMEPPOSLINE[NOOFMOTORS][24];
extern char TabDEFAULT_CSV_OUTFILENAMEPDIFFLINE[NOOFMOTORS][20];
extern char TabDEFAULT_BIN_OUTFILENAMEPDIFFFILE[NOOFMOTORS][20];
extern char TabDEFAULT_CSV_OUTFILENAMEPDIFFFILE[NOOFMOTORS][20];
//extern char TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[NOOFMOTORS][20];
char TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[1][21] ={"PixFreqsFileAllM.csv"};
char szHoloProgHelperFullFileName[_MAX_PATH]; //HoloProgHelper.exe must reside in same directory as HoloProg.exe
char szHoloPIDHelperFullFileName[_MAX_PATH];  //HoloPIDHelper.exe must reside in same directory as HoloProg.exe
extern char szPingMall_bat_FullFileName[_MAX_PATH];  //PingMall.bat must reside in same directory as HoloProg.exe

char FILENAMEStab[NOOFMOTORS][16] ={

//M1 Controls energy Split (hcf+pcf calc)  hcf      M5 0.8-1.6mm   FocusInPCF     Geometry affects only motors 7, 8, 9, 10, 11 & 12 (& now M3)
//M1  static      M2              M3              M4  static        M5  static        M6  static        M7                M8                M9                M10               M11              M12
 "M01WP1Red.csb","M02WP2Red.cse","M03WP3Red.cse","M04LOobjRed.csb","M05ObjAPred.csb","M06LCfocRed.csb","M07RBapTred.csb","M08RBapBred.csb","M09RBapLred.csb", "M10RBapRred.csb","M11GonHred.csb","M12GonVred.csb",
//M13 static      M14             M15             M16 static        M17 static        M18 static        M19               M20               M21               M22               M23              M24
 "M13WP1Gre.csb","M14WP2Gre.cse","M15WP3Gre.cse","M16LOobjGre.csb","M17ObjAPgre.csb","M18LCfocGre.csb","M19RBapTgre.csb","M20RBapBgre.csb","M21RBapLgre.csb","M22RBapRgre.csb","M23GonHgre.csb","M24GonVgre.csb",
//M25 static      M26             M27             M28 static        M29 static        M30 static        M31               M32               M33               M34               M35              M36
 "M25WP1Blu.csb","M26WP2Blu.cse","M27WP3Blu.cse","M28LOobjBlu.csb","M29ObjAPblu.csb","M30LCfocBlu.csb","M31RBapTblu.csb","M32RBapBblu.csb","M33RBapLblu.csb","M34RBapRblu.csb","M35GonHblu.csb","M36GonVblu.csb",
//M37 static      M38 static      M39             M40
 "M37RBTabM1.csb","M38RBTabM2.csb","M39XYStageX.csb","M40XYStageY.csb" };

 char PCENTSPLITFILE[] = {"WavPlate1PcentSplitRGB.cal"};

//We don't really need buffers to store the csb/csv files for all motors, as some are static (So some of the filenames above are hypothetical really! and marked 'static')
//However, during development it may be desirable to use the block motor move function to send files to any motor - including static ones, so I shall keep them all

char pM00PacketBuff[SIZ_8832000];//M1                                   //There are 2200 motor positons for 2000 Lines    //MAX_NOOFLINES
//char pM01PacketBuff[SIZ_8832000];                                       // = 8,800,000 bytes of motor position data.
double dblArDesiredEnergyWP2r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];      //These are transferred as 2000 Lines, but owing  //MAX_NOOFLINES
double dblArDesiredEnergyWP3r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];      //These are transferred as 2000 Lines, but owing  //MAX_NOOFLINES
char pM03PacketBuff[SIZ_8832000];//M4                                   // to ethernet packet size limits the 4400 bytes 
char pM04PacketBuff[SIZ_8832000];//M5                                   // for each line are sent as 3 packets of 1472 bytes.
char pM05PacketBuff[SIZ_8832000];//M6                                   //3*1472 is 4416, so 16 bytes of data for each line
char pM06PacketBuff[SIZ_8832000];                                       // is used for SeqNos, Message Type & other data.
char pM07PacketBuff[SIZ_8832000];
char pM08PacketBuff[SIZ_8832000];
char pM09PacketBuff[SIZ_8832000];
char pM10PacketBuff[SIZ_8832000];
char pM11PacketBuff[SIZ_8832000];
char pM12PacketBuff[SIZ_8832000];//M13
//char pM13PacketBuff[SIZ_8832000];
//char pM14PacketBuff[SIZ_8832000];
double dblArDesiredEnergyWP2g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
double dblArDesiredEnergyWP3g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
char pM15PacketBuff[SIZ_8832000];//M16
char pM16PacketBuff[SIZ_8832000];//M17
char pM17PacketBuff[SIZ_8832000];//M18
char pM18PacketBuff[SIZ_8832000];
char pM19PacketBuff[SIZ_8832000];
char pM20PacketBuff[SIZ_8832000];
char pM21PacketBuff[SIZ_8832000];
char pM22PacketBuff[SIZ_8832000];
char pM23PacketBuff[SIZ_8832000];
char pM24PacketBuff[SIZ_8832000];//M25
//char pM25PacketBuff[SIZ_8832000];
//char pM26PacketBuff[SIZ_8832000];
double dblArDesiredEnergyWP2b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
double dblArDesiredEnergyWP3b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
char pM27PacketBuff[SIZ_8832000];//M28
char pM28PacketBuff[SIZ_8832000];//M29
char pM29PacketBuff[SIZ_8832000];//M30
char pM30PacketBuff[SIZ_8832000];
char pM31PacketBuff[SIZ_8832000];
char pM32PacketBuff[SIZ_8832000];
char pM33PacketBuff[SIZ_8832000];
char pM34PacketBuff[SIZ_8832000];
char pM35PacketBuff[SIZ_8832000];
char pM36PacketBuff[SIZ_8832000];//M37
char pM37PacketBuff[SIZ_8832000];//M38
char pM38PacketBuff[SIZ_8832000];
char pM39PacketBuff[SIZ_8832000];
char CopyUC3Buff[SIZ_8832000]; //This is a copy of the Data UC3 Will receive.  It is not quite the same as the data that is sent in the buffers above because
// they includes the headers (MessageType, Seqno, Flags, etc). This copy is just the Motor Postions, & is used for calculating the checksum. It is built 
// after the Endian swap to ensure the checksum is the same.

char *pMxPacketBuffAr[NOOFMOTORS];  //An array of pointers to the above buffers
BOOL bListOfStaticMotors[NOOFMOTORS];
extern int nPixFreqsAr[];
extern char szExplorerDir2StartIn[];
extern int nHelpMessageToLoad;
BOOL bSubtle = 1;

HWND hGLWnd;
char szGLWndTitle[SIZszGLWndTitle]; //Eg: HoloProg: <filename>
int  nShowHologInfoBecauseFileOpenedOK;
BOOL bSaveDiffFileAsBinary = 1;

BOOL bRunningManually;  //When Set, MsTypB_SING_MM_PID messages from the PID code are not sent by the PC

//Initialize the Registry-mapped variables (See Article www.codeproject.com/KB/system/registryvars.aspx)
///CRegStdString mystring("SOFTWARE\\OpTIC Technium\\HoloProg\\MyString", "defaultvalue", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdString LastCsvFileOpened  ("SOFTWARE\\OpTIC Technium\\HoloProg\\LastCsvFileOpened",   "unset", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdString LastHGramFileOpened("SOFTWARE\\OpTIC Technium\\HoloProg\\LastHGramFileOpened", "unset", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdString USBportToOpenForEnergyMeters("SOFTWARE\\OpTIC Technium\\HoloProg\\USBportToOpenForEnergyMeters", "Dual RS232 A", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdString USBportToOpenForControlBox("SOFTWARE\\OpTIC Technium\\HoloProg\\USBportToOpenForControlBox", "Dual RS232 B", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdString EditorForViewing("SOFTWARE\\OpTIC Technium\\HoloProg\\EditorForViewing", "C:\\Program Files\\ULTRAEDIT\\UEDIT32.EXE", FORCE_UPDATE, HKEY_LOCAL_MACHINE);
//CRegStdWORD   myword  ("SOFTWARE\\OpTIC Technium\\HoloProg\\mydword", 100, CACHE_UPDATE, HKEY_LOCAL_MACHINE);

CRegStdString LastCsvFileOpened  ("SOFTWARE\\OpTIC Technium\\HoloProg\\LastCsvFileOpened",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString LastHGramFileOpened("SOFTWARE\\OpTIC Technium\\HoloProg\\LastHGramFileOpened", "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString USBportToOpenForEnergyMeters("SOFTWARE\\OpTIC Technium\\HoloProg\\USBportToOpenForEnergyMeters", "Dual RS232 A", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString USBportToOpenForControlBox("SOFTWARE\\OpTIC Technium\\HoloProg\\USBportToOpenForControlBox", "Dual RS232 B", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString EditorForViewing("SOFTWARE\\OpTIC Technium\\HoloProg\\EditorForViewing", "C:\\Program Files\\ULTRAEDT\\UEDIT32.EXE", FORCE_UPDATE, HKEY_CURRENT_USER);
//To add new registry variables, add a line like the following.  It will be created automatically in the registry (if it doesn't already exist) with the default value.
CRegStdWORD   bregMakeSounds("SOFTWARE\\OpTIC Technium\\HoloProg\\nMakeSounds", 0, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bregDisplayPIDdebugMsgs("SOFTWARE\\OpTIC Technium\\HoloProg\\bDisplayPIDdebugMsgs", 0, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   nEnableControlBoxSW("SOFTWARE\\OpTIC Technium\\HoloProg\\nEnableControlBoxSW", 0, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bStartWithPIDsDisabled("SOFTWARE\\OpTIC Technium\\HoloProg\\bStartWithPIDsDisabled", 0, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bStartWithViewHologramInfoOpened("SOFTWARE\\OpTIC Technium\\HoloProg\\bStartWithViewHologramInfoOpened", 1, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bAllowStartIfInitialiseAllSocketsFails("SOFTWARE\\OpTIC Technium\\HoloProg\\bAllowStartIfInitialiseAllSocketsFails", 0, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bDoProperCheckSumChecks_MposFiles("SOFTWARE\\OpTIC Technium\\HoloProg\\bDoProperCheckSumChecks_MposFiles", 1, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bDoProperCheckSumChecks_AccelProfs("SOFTWARE\\OpTIC Technium\\HoloProg\\bDoProperCheckSumChecks_AccelProfs", 1, FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdWORD   bUseCSBs4AccProfs("SOFTWARE\\OpTIC Technium\\HoloProg\\bUseCSBs4AccProfs", 0, FORCE_UPDATE, HKEY_CURRENT_USER);

//Set these two to zero in the registry to remove the temporary fix:
CRegStdWORD   nOverRideM39HomingOutSpeedIfNotZero("SOFTWARE\\OpTIC Technium\\HoloProg\\nOverRideM39HomingOutSpeedIfNotZero", 40004, FORCE_UPDATE, HKEY_CURRENT_USER);//M39 was too fast
CRegStdWORD   nOverRideM40HomingOutSpeedIfNotZero("SOFTWARE\\OpTIC Technium\\HoloProg\\nOverRideM40HomingOutSpeedIfNotZero", 4008, FORCE_UPDATE, HKEY_CURRENT_USER); //M40 was too slow
//CRegStdWORD   nOverRideM39HomingOutSpeedIfNotZero("SOFTWARE\\OpTIC Technium\\HoloProg\\nOverRideM39HomingOutSpeedIfNotZero", 1, FORCE_UPDATE, HKEY_CURRENT_USER);//M39 was too fast
//CRegStdWORD   nOverRideM40HomingOutSpeedIfNotZero("SOFTWARE\\OpTIC Technium\\HoloProg\\nOverRideM40HomingOutSpeedIfNotZero", 1, FORCE_UPDATE, HKEY_CURRENT_USER); //M40 was too slow

//Next Line requires "Characterset == Not Set" in the projects' property pages (under Configuration Properties -> General)
CRegStdString creg_str_PID_WP2_R_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_R_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_G_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_G_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_B_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_B_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);

CRegStdString creg_str_PID_WP2_R_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_R_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_G_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_G_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_B_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_B_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);

CRegStdString creg_str_PID_WP2_R_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_R_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_G_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_G_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP2_B_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP2_B_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);


CRegStdString creg_str_PID_WP3_R_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_R_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_G_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_G_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_B_pGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_B_pGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);

CRegStdString creg_str_PID_WP3_R_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_R_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_G_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_G_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_B_iGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_B_iGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);

CRegStdString creg_str_PID_WP3_R_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_R_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_G_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_G_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);
CRegStdString creg_str_PID_WP3_B_dGAIN  ("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_WP3_B_dGAIN",   "unset", FORCE_UPDATE, HKEY_CURRENT_USER);

CRegStdWORD   nregNoofReadsToAverage("SOFTWARE\\OpTIC Technium\\HoloProg\\PID_NOOF_READS_TO_AVERAGE", 999, FORCE_UPDATE, HKEY_CURRENT_USER);

ADDITIONALarpsSTRUC aARPstrc;

char nchCurrentMotorZB;
HFONT newFont; //Now Global so that JustOneThread can use it too
BOOL bDisplayPIDdebugMsgs;
BOOL bMakeSounds;
int nNOOFreadsToAVERAGE;

BOOL CreateTimeStampedFolder(void); //prototype
//CTestNetClientDlg dlg;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
 int wmId;//, wmEvent;
 int RMP_RetCode;
 static TIMECAPS time_caps;
 static BOOL bInitialisationSucceeded = 0;
 static BOOL ok;
 MMRESULT mm_result;
 PAINTSTRUCT ps;
 HDC hdc;
 char nchEM;
 //static HFONT newFont;
 static HWND hFocusButton;
 static POINT lhPolyPoints[4] = {357,924,454,924,450,904,424,904};
 static POINT rhPolyPoints[4] = {458,924,555,924,484,904,461,904};
 
 double dblPidOutputToActOn;
 static char chLookUpWPmotorNo[6]={1,2,13,14,25,26}; //PidOutputToActOn[] is indexed 0 to 5.. This table maps shows which motor each index relates to (i.e all the energy control motors)
 char nchLookUpWPmotorNo;
 //static  HMODULE GNC;
 //static GNETCLIENT_ConnectPROC     lpGNETCLIENT_Connect;
 //static GNETCLIENT_DisconnectPROC  lpGNETCLIENT_Disconnect;
 //static GNETCLIENT_SendMessagePROC lpGNETCLIENT_SendMessage;
 //static GNETCLIENT_SetMessageCallbackPROC lpGNETCLIENT_SetMessageCallback;
 //static JobListStruct m_JobList[100];
 switch (message){
    case WM_CREATE:{
/*
char szCodeMes[100];
ULONG ulResult = -2147467259;
long  lResult = ulResult;
sprintf(szCodeMes,"What code %d",ulResult);
MessageBox(hWnd,szCodeMes,NULL,MB_OK);

sprintf(szCodeMes,"What code  (signed) = %d",lResult);
MessageBox(hWnd,szCodeMes,NULL,MB_OK);
exit(0);
*/
       //MessageBox(hWnd,"Contains Sillyfrig",NULL,MB_ICONEXCLAMATION);
//CountStatRecsRecvd_TimesPlaySoundCalled = 0;
       //assert(MAXLEADINorOUT == NOOFaccelPROFILES); //NOOFaccelPROFILES was SIZaccelPROFILES (but the table was doubled to include accelerations in the reverse direction).
       if(nOverRideM39HomingOutSpeedIfNotZero || nOverRideM40HomingOutSpeedIfNotZero){
          nOverRideM39HomingOutSpeedIfNotZero = (int)nOverRideM39HomingOutSpeedIfNotZero;
          nOverRideM40HomingOutSpeedIfNotZero = (int)nOverRideM40HomingOutSpeedIfNotZero;
          MessageBox(hWnd,"Either (or both)\n\n\tnOverRideM39HomingOutSpeedIfNotZero\nor\n\tnOverRideM40HomingOutSpeedIfNotZero\n\nwere set > zero in the registry.\n\nThese Timer values are independent of uSteps/Step setting! - So You need to ensure they are set\nproperly for your intended uSteps/Step","OverRide Warning",MB_ICONEXCLAMATION);
          if(nOverRideM39HomingOutSpeedIfNotZero > 59998){
             MessageBox(hWnd,"nOverRideM39HomingOutSpeedIfNotZero was set > 59998 in the registry!\n\nI've reset it to 59998",NULL,MB_ICONEXCLAMATION);
             nOverRideM39HomingOutSpeedIfNotZero = 59998;
            }
          if(nOverRideM40HomingOutSpeedIfNotZero > 59998){
             MessageBox(hWnd,"nOverRideM40HomingOutSpeedIfNotZero was set > 59998 in the registry!\n\nI've reset it to 59998",NULL,MB_ICONEXCLAMATION);
             nOverRideM40HomingOutSpeedIfNotZero = 59998;
            }
         }
       if( (nregNoofReadsToAverage > 0) && (nregNoofReadsToAverage < 61) )
         nNOOFreadsToAVERAGE = nregNoofReadsToAverage;
       else{
          nregNoofReadsToAverage = 8;
          nNOOFreadsToAVERAGE = nregNoofReadsToAverage;
          MessageBox(hWnd,"nNOOFreadsToAVERAGE was not set between 1 and 60 in the registry!\n\nI've reset it to 8","Warning",MB_OK);
         }
       if((bUseCSBs4AccProfs > 1)||(bUseCSBs4AccProfs < 0)){
          MessageBox(hWnd,"bUseCSBs4AccProfs was not 0 or 1 in the registry!\n\nIt's now reset to 0",NULL,MB_ICONEXCLAMATION);
          bUseCSBs4AccProfs = 0;
         }
//     if((bDoProperCheckSumChecks_AccelProfs > 1)||(bDoProperCheckSumChecks_AccelProfs < 0)){
//        MessageBox(hWnd,"bDoProperCheckSumChecks_AccelProfs was not 0 or 1 in the registry!\n\nIt's now reset to 1",NULL,MB_ICONEXCLAMATION);
//        bDoProperCheckSumChecks_AccelProfs = 1;
//       } Unlike bUseCSBs4AccProfs, bDoProperCheckSumChecks_AccelProfs doesn't need validation... it is a true boolean whereas bUseCSBs4AccProfs is sometimes used as an integer subscript
       hGLWnd = hWnd;
       bRunningManually = FALSE; //TRUE FALSE
       bStopPIDsendingMessages = FALSE;
       bDisplayPIDdebugMsgs = bregDisplayPIDdebugMsgs;
       bMakeSounds = bregMakeSounds;
#if 0
      if(!adc200_open_unit (1)) //1 is LPT1
         MessageBox(hWnd,"adc200_open_unit Failed",NULL,MB_ICONEXCLAMATION);
      else{
         unsigned long ns;      //always set to false by the driver
         unsigned char is_slow;
         adc200_set_frequency (0); //controls signal generator - 0 == Off
         //if(1 !=adc200_set_channels (A200_CHANNEL_B))
         if(2 !=adc200_set_channels (A200_BOTH_CHANNELS))
           MessageBox(hWnd,"adc200_set_channels Failed",NULL,MB_ICONEXCLAMATION);
         adc200_set_trigger (FALSE, 0, 0, 0, 1000);//The FALSE arg should turn the trigger off
         if( 0 == adc200_set_range (0, 9) ) //0 is Channel A, 9 is 10 Volt Range
           MessageBox(hWnd,"adc200_set_range A Failed",NULL,MB_ICONEXCLAMATION);
         if( 0 == adc200_set_range (1, 9) ) //1 is Channel B, 9 is 10 Volt Range
           MessageBox(hWnd,"adc200_set_range B Failed",NULL,MB_ICONEXCLAMATION);
         if(0 == adc200_set_oversample (1)) //returns 0 if oversample factor is out of range
           MessageBox(hWnd,"adc200_set_oversample Failed",NULL,MB_ICONEXCLAMATION);
         if(!adc200_set_dc (0, 1))//set channel 0 to DC
           MessageBox(hWnd,"adc200_set_dc channel 0 Failed",NULL,MB_ICONEXCLAMATION);
         if(!adc200_set_dc (1, 1))//set channel 1 to DC
           MessageBox(hWnd,"adc200_set_dc channel 1 Failed",NULL,MB_ICONEXCLAMATION);
         if(!adc200_set_timebase (&ns, &is_slow, 0)) //0 would be the fastest time
           MessageBox(hWnd,"requested timebase invalid",NULL,MB_ICONEXCLAMATION);
        }
#endif
       for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++){
          bListOfStaticMotors[nchEM] = 0;
          cMessageSeqNoCS[nchEM] = 0x10; //initialize to any old number,
          cMessageSeqNoSR[nchEM] = 0xA0; // chosen to be different to help debugging - if sent seqno is very different from received seqno, this indicates a normal
         }                             // Status Request getting confused with a Checksum Status request.
       StringCchCopy(szGlobalFileName,_MAX_DIR, stdstring(EditorForViewing).c_str() );//These two lines are just to ensure the registry key exists. If
       EditorForViewing = szGlobalFileName;                                           // it doesn't it is created.
       StringCchCopy(szGlobalFileName,_MAX_DIR, stdstring(USBportToOpenForEnergyMeters).c_str() );//These two lines are just to ensure the registry key exists. If
       USBportToOpenForEnergyMeters = szGlobalFileName;                                           // it doesn't it is created.
       StringCchCopy(szGlobalFileName,_MAX_DIR, stdstring(USBportToOpenForControlBox).c_str() );//These two lines are just to ensure the registry key exists. If
       USBportToOpenForControlBox = szGlobalFileName;                                           // it doesn't it is created.
//     if( (nchCurrentMotorZB > 39) || (nchCurrentMotorZB < 0) ) {
//        nchCurrentMotorZB = 0;//Actually I'm not sure we really care... It shouldn't be used really unless set to something appropriate first.
        //MessageBox (hWnd,"nchCurrentMotorZB was not within its allowed range of  0 - 39 in the registry\nand has been reset to 37 (i.e. motor 38)\n\n",NULL,MB_YESNO);
        //bListOfStaticMotors[nchCurrentMotorZB] = 0;
//       }
       //nchCurrentMotorZB = nchCurrentMotorZB;  //Starts from zero, so zero is motor 1
       //nchCurrentMotorZB = 12;     //Starts from zero, so zero is motor 1
       //bMotor2ExpectRepliesFrom[nchCurrentMotorZB] = 1; //Motor(s) to be talked to in ShowStatusMultiple
       //bMotor2ExpectRepliesFrom[6] = 1; //Motor(s) to be talked to in ShowStatusMultiple
       memset(&CEC,0,sizeof(CEC));
     //NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS;
int zzzzzzzNEXTlineQuestionable; //go back to above?
       NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_STARTUP; //For getting status recs it should be safe to have short timeouts (But only if the AVR is not busy when it gets it!)
       pMxPacketBuffAr[0]  = pM00PacketBuff;/*NULL;*/ bListOfStaticMotors[0] = 1;
     //pMxPacketBuffAr[1]  = pM01PacketBuff;               //bListOfStaticMotors[1] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
       pMxPacketBuffAr[1]  = (char*)dblArDesiredEnergyWP2r; //bListOfStaticMotors[1] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
       pMxPacketBuffAr[2]  = (char*)dblArDesiredEnergyWP3r; //bListOfStaticMotors[2] = 1; //Position Data.  Now, 'bListOfStaticMotors' has a complementary 'bMotorIS_A_PID' instead.
       pMxPacketBuffAr[3]  = pM03PacketBuff;/*NULL;*/ bListOfStaticMotors[3] = 1;
       pMxPacketBuffAr[4]  = pM04PacketBuff;/*NULL;*/ bListOfStaticMotors[4] = 1;
       pMxPacketBuffAr[5]  = pM05PacketBuff;/*NULL;*/ bListOfStaticMotors[5] = 1;
       pMxPacketBuffAr[6]  = pM06PacketBuff;
       pMxPacketBuffAr[7]  = pM07PacketBuff;
       pMxPacketBuffAr[8]  = pM08PacketBuff;
       pMxPacketBuffAr[9]  = pM09PacketBuff;
       pMxPacketBuffAr[10] = pM10PacketBuff;
       pMxPacketBuffAr[11] = pM11PacketBuff;

       pMxPacketBuffAr[12] = pM12PacketBuff;/*NULL;*/ bListOfStaticMotors[12] = 1;
     //pMxPacketBuffAr[13] = pM13PacketBuff;          //bListOfStaticMotors[13] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
       pMxPacketBuffAr[13]  = (char*)dblArDesiredEnergyWP2g; //bListOfStaticMotors[13] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
     //pMxPacketBuffAr[14] = pM14PacketBuff;          //bListOfStaticMotors[14] = 1; //Position Data.  Now, 'bListOfStaticMotors' has a complementary 'bMotorIS_A_PID' instead.
       pMxPacketBuffAr[14]  = (char*)dblArDesiredEnergyWP3g;
       pMxPacketBuffAr[15] = pM15PacketBuff;/*NULL;*/ bListOfStaticMotors[15] = 1;
       pMxPacketBuffAr[16] = pM16PacketBuff;/*NULL;*/ bListOfStaticMotors[16] = 1;
       pMxPacketBuffAr[17] = pM17PacketBuff;/*NULL;*/ bListOfStaticMotors[17] = 1;
       pMxPacketBuffAr[18] = pM18PacketBuff;
       pMxPacketBuffAr[19] = pM19PacketBuff;
       pMxPacketBuffAr[20] = pM20PacketBuff;
       pMxPacketBuffAr[21] = pM21PacketBuff;
       pMxPacketBuffAr[22] = pM22PacketBuff;
       pMxPacketBuffAr[23] = pM23PacketBuff;

       pMxPacketBuffAr[24] = pM24PacketBuff;/*NULL;*/ bListOfStaticMotors[24] = 1;
     //pMxPacketBuffAr[25] = pM25PacketBuff;          //bListOfStaticMotors[25] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
       pMxPacketBuffAr[25]  = (char*)dblArDesiredEnergyWP2b; //bListOfStaticMotors[25] = 1; //PIDs are not really 'static', but this variable is used to avoid sending these motors files of Motor
     //pMxPacketBuffAr[26] = pM26PacketBuff;          //bListOfStaticMotors[26] = 1; //Position Data.  Now, 'bListOfStaticMotors' has a complementary 'bMotorIS_A_PID' instead.
       pMxPacketBuffAr[26]  = (char*)dblArDesiredEnergyWP3b;
       pMxPacketBuffAr[27] = pM27PacketBuff;/*NULL;*/ bListOfStaticMotors[27] = 1;
       pMxPacketBuffAr[28] = pM28PacketBuff;/*NULL;*/ bListOfStaticMotors[28] = 1;//MessageBox (hWnd,"bListOfStaticMotors 29 & 30 are removed from list!","Warning",MB_OK);
       pMxPacketBuffAr[29] = pM29PacketBuff;/*NULL;*/ bListOfStaticMotors[29] = 1;
       pMxPacketBuffAr[30] = pM30PacketBuff;
       pMxPacketBuffAr[31] = pM31PacketBuff;
       pMxPacketBuffAr[32] = pM32PacketBuff;
       pMxPacketBuffAr[33] = pM33PacketBuff;
       pMxPacketBuffAr[34] = pM34PacketBuff;
       pMxPacketBuffAr[35] = pM35PacketBuff;

       pMxPacketBuffAr[36] = pM36PacketBuff;/*NULL;*/ bListOfStaticMotors[36] = 1;
       pMxPacketBuffAr[37] = pM37PacketBuff;/*NULL;*/ bListOfStaticMotors[37] = 1;
       pMxPacketBuffAr[38] = pM38PacketBuff;
       pMxPacketBuffAr[39] = pM39PacketBuff;

//if(bListOfStaticMotors[nchCurrentMotorZB])
//  if (IDYES == MessageBox (hWnd,"nchCurrentMotorZB is in bListOfStaticMotors!!\n\nThis will send Invalid message-types if pressing Print Hologram button\nAs no movement profile will be loaded!\n\nShall I remove from list?","Warning",MB_YESNO))
//    bListOfStaticMotors[nchCurrentMotorZB] = 0;

       //mystring = "TestValue25";     //"TestValue" is stored in the registry

       int MustWarningAboutTimeWrappingAfter42Days; //see timeSendWasCalled in PacketSend.cpp
       int WarningI_hid_pragmaDeprectatedWarningsInHeaders; //search for "//mj hidden" in C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include\atlbase.h
                                                            //                and also in C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include\cstringt.h





int D1900changedTo24000;  //It should be 300 (but even that is far too long)  A forty minute timeout should give me long enough to step thru AVR code without Windows Complaining, but
                          // this should be put back before handing back to Jerzy.






     //bSTATREC_BOXisBeingDisplayed = 0;
       bMoveNowRadioButPressed = 0;
//     bReallyThisIsAnAccelProfile = 0;
       //InitializeRevBitsTable();
       mm_result = timeGetDevCaps(&time_caps, sizeof(TIMECAPS) );
       if(mm_result != TIMERR_NOERROR){
          MessageBox (hWnd,"timeGetDevCaps() function failed",NULL,MB_OK);
          DestroyWindow(hWnd);
          break;
         }
       mm_result = timeBeginPeriod( time_caps.wPeriodMin );
       if(mm_result != TIMERR_NOERROR){
          MessageBox (hWnd,"timeBeginPeriod() function failed",NULL,MB_OK);
          DestroyWindow(hWnd);
          break;
         }
       INITCOMMONCONTROLSEX iccx;
       iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
       iccx.dwICC = ICC_WIN95_CLASSES;
       if(!InitCommonControlsEx(&iccx)){
          MessageBox (hWnd,"InitCommonControlsEx Failed",NULL,MB_OK);
          DestroyWindow(hWnd);
          break;
         }
       HBITMAP  hMyMenuBitmap;  //The bitmap displayed in the menubar.
     //hMyMenuBitmap =  (HBITMAP)LoadImage(NULL, "C:\\MyProjects\\HoloProg\\menuopticcol1.bmp", IMAGE_BITMAP, 130, 19, LR_LOADFROMFILE|LR_DEFAULTCOLOR );
       hMyMenuBitmap =  (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE (MENUBMP), IMAGE_BITMAP, 130, 19, LR_DEFAULTCOLOR );
     //AppendMenu(hMenuMain, MF_BITMAP, NULL, (LPCTSTR)hMyMenuBitmap);
       AppendMenu(hMenuMain, MF_BITMAP, IDM_ABOUT, (LPCTSTR)hMyMenuBitmap);
       SetMenu(hWnd,hMenuMain);
       if (!GetModuleFileName(hInst, szGlobalFileName, _MAX_DIR)){  //szGlobalFileName to TEMPORARILY hold the directory the progam resides in
          MessageBox (hWnd,"GetModuleFileName Failed",NULL,MB_OK);
          DestroyWindow(hWnd);
          break;
         }

       char *ptrBslash;
       ptrBslash = strrchr(szGlobalFileName,BACKSLASH);             //reverse search for the final backslash
       szGlobalFileName[(int)(ptrBslash-szGlobalFileName + 1)]=0;   //szGlobalFileName now TEMPORARILY holds the directory the progam resides in
       StringCchCopy(szPrinterPCFfilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szAccelFileNamesDir, _MAX_PATH, szGlobalFileName); //szAccelFileNamesDir now PERMANENTLY holds the directory the progam resides in (provided it is truncated
       SetLenszAccelFileNamesDir( lstrlen(szAccelFileNamesDir));        // with nLenszAccelFileNamesDir)
       StringCchCopy(szWP1CalFilename, _MAX_PATH, szGlobalFileName); //This file also contains the data for WP13 and WP25
       StringCchCopy(szWP2CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szWP3CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szWP14CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szWP15CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szWP26CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szWP27CalFilename, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szHoloProgHelperFullFileName, _MAX_PATH, szGlobalFileName);
       StringCchCopy(szHoloPIDHelperFullFileName, _MAX_PATH, szGlobalFileName);
       StringCchCat(szHoloProgHelperFullFileName, _MAX_PATH, "HoloProgHelper.exe");
       StringCchCat(szHoloPIDHelperFullFileName, _MAX_PATH, "HoloPIDHelper.exe");
       StringCchCopy(szPingMall_bat_FullFileName, _MAX_PATH, szGlobalFileName);
       StringCchCat(szPingMall_bat_FullFileName, _MAX_PATH, "PingMall.bat");

       char szStaticARPsFilename[_MAX_PATH];
       StringCchCopy(szStaticARPsFilename, _MAX_PATH, szGlobalFileName);
       StringCchCat(szStaticARPsFilename, _MAX_PATH, "StaticARPs.arp");

       if( myOpen_File(hWnd, "arp", szStaticARPsFilename, UNUSED_MOTORNO) ){ //The StaticARPs.arp file did not open!
          if( IDYES == MessageBox(hWnd,"The program will exit because a valid StaticARPs.arp file was not found.\n\nWould you like me to create the above named file\nand populate it with the default settings?\n\nThis will at least give you a file to edit - but you MUST check it!!.",szPrinterPCFfilename,MB_YESNO)){
             mySave_File( hWnd, 3000, "arp",0, szStaticARPsFilename, MOTORNO_NOT_USED);
             DisplayFileInEditor(hWnd, szStaticARPsFilename);
            }
          DestroyWindow(hWnd); //exit(1);
          break;
         }
       //#include <Iphlpapi.i> //For Vista and later, it should be Ipmib.h
       #define SIZ2500 2500
       PMIB_IPNETTABLE pmibIPnetTab;
       unsigned long pdwSize = SIZ2500;
       BOOL  bOrder = FALSE; //sort table in ip address order
       BYTE ipNetTabBuffer[SIZ2500]; //It looks to me like each entry in the ARP table is ~ 24 bytes, so this buffer could load > 80 entries. If we find > 86 entries in the table (or there isn't sufficient space),
       pmibIPnetTab = (PMIB_IPNETTABLE)ipNetTabBuffer; // we'll assume it's already been loaded (after all having static entries is not strictly necessary anyway)
       DWORD dwRes = GetIpNetTable(pmibIPnetTab,&pdwSize,bOrder);      //pdwSize should hold the size of the table
             //MIB_IPNET_TYPE_OTHER = 1, //MIB_IPNET_TYPE_INVALID = 2, //MIB_IPNET_TYPE_DYNAMIC = 3, //MIB_IPNET_TYPE_STATIC  = 4,
       if( ((dwRes == NO_ERROR) && (pmibIPnetTab->dwNumEntries < 87)) || (dwRes == ERROR_NO_DATA) ){ // then load the ARP table (ERROR_NO_DATA is not an error... it just means the table is empty)
        //MessageBox(hWnd,"ARP table size was < 37entries\nThis is expected on a first run.\n\nAbout to load ARP table", "Remove This Message After Testing", MB_OK);
          int bStopFurtherMessages = 0;
          for(int eachInterface = 0; eachInterface < 2; eachInterface++){
             MIB_IPNETROW ArpEntry;
             BOOL bAddAVRsToThisInterface;
             if(eachInterface){
                ArpEntry.dwIndex = aARPstrc.dwSecondInterfaceIndex; //This is the Network card's interface index e.g.0x10004. Type "ARP -a" and it's listed on the first line.  Optic's PC may have two - one for the Display PC and one for the Motors
                bAddAVRsToThisInterface = aARPstrc.bSecondInterfaceHasAVRs;
               }
             else{
                ArpEntry.dwIndex = aARPstrc.dwFirstInterfaceIndex; //This is the Network card's interface index e.g.0x10003. Type "ARP -a" and it's listed on the first line.  Optic's PC may have two - one for the Display PC and one for the Motors
                bAddAVRsToThisInterface = aARPstrc.bFirstInterfaceHasAVRs;
               }
             ArpEntry.dwPhysAddrLen = 6;
             ArpEntry.dwType = MIB_IPNET_TYPE_STATIC;
             if(bAddAVRsToThisInterface){
                ArpEntry.bPhysAddr[0] = 0;
                ArpEntry.bPhysAddr[1] = 0x04;
                ArpEntry.bPhysAddr[2] = 0x25;
                ArpEntry.bPhysAddr[3] = 0x41;
                ArpEntry.bPhysAddr[4] = 0x56;
              //ArpEntry.bPhysAddr[5] = 0xf0;           //This is the MAC address corresponding to 192.168.0.240
              //ArpEntry.bPhysAddr[5] = 0xef;           //This is the MAC address corresponding to 192.168.0.239
                ArpEntry.bPhysAddr[6] = 0;//Not used
                ArpEntry.bPhysAddr[7] = 0;//Not used
              //ArpEntry.dwAddr = 0xf000a8c0;           //This is the IP address 192.168.0.240
                ArpEntry.dwAddr = 0xef00a8c0;           //This is the IP address 192.168.0.239 (ef will be repeatedly overwritten, but the other 3 bytes remain constant)
              //Note that the MAC addresses for the AVRs are virtually identical (only their sixth byte varies - in the range 240 (0xf0) down to 201).
              //Similarly, their IP address are virtually identical (only their first byte varies, also - in the range 240 (0xf0) down to 201).
                unsigned char *pusFirstByteOf_dwAddr = ((unsigned char*)&ArpEntry.dwAddr) + 3; //+ 3 because of its endianess
                for(unsigned char nAdd = 240; nAdd > 200; nAdd--){
                   ArpEntry.bPhysAddr[5] = nAdd;
                   *pusFirstByteOf_dwAddr = nAdd;
                   dwRes = SetIpNetEntry(&ArpEntry);
                   if((dwRes != NO_ERROR) && (!bStopFurtherMessages)){
                      bStopFurtherMessages = 1;
                      if(dwRes == ERROR_ACCESS_DENIED)
                        MessageBox (hWnd,"ERROR_ACCESS_DENIED","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_INVALID_PARAMETER)
                        MessageBox (hWnd,"ERROR_INVALID_PARAMETER","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_NOT_SUPPORTED)
                        MessageBox (hWnd,"ERROR_NOT_SUPPORTED","SetIpNetEntry problem",MB_OK);
                      else{
                       //FormatMessage( dwRes,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                       //FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                         FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  dwRes,  NULL,  mes,  SIZMES,  NULL);
                         MessageBox(hWnd, mes, "SetIpNetEntry (other error)", MB_ICONEXCLAMATION);
                        }
                     }
                  }
               }
             if(eachInterface){
                for(int nNoofIFrecToAdd = 0; nNoofIFrecToAdd < aARPstrc.nSecondInterfaceNoofRecs2Add; nNoofIFrecToAdd++){
                   memcpy(ArpEntry.bPhysAddr, aARPstrc.SecondIFarpPairAr[nNoofIFrecToAdd].MACaddr, ArpEntry.dwPhysAddrLen);
                   ArpEntry.dwAddr = aARPstrc.SecondIFarpPairAr[nNoofIFrecToAdd].IPaddr;
                   dwRes = SetIpNetEntry(&ArpEntry);
                   if((dwRes != NO_ERROR) && (!bStopFurtherMessages)){
                      bStopFurtherMessages = 2;
                      if(dwRes == ERROR_ACCESS_DENIED)
                        MessageBox (hWnd,"ERROR_ACCESS_DENIED","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_INVALID_PARAMETER)
                        MessageBox (hWnd,"ERROR_INVALID_PARAMETER","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_NOT_SUPPORTED)
                        MessageBox (hWnd,"ERROR_NOT_SUPPORTED","SetIpNetEntry problem",MB_OK);
                      else{
                       //FormatMessage( dwRes,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                       //FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                         FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  dwRes,  NULL,  mes,  SIZMES,  NULL);
                         MessageBox(hWnd, mes, "SetIpNetEntry (other error)", MB_ICONEXCLAMATION);
                        }
                     }
                  }
               }
             else{
                for(int nNoofIFrecToAdd = 0; nNoofIFrecToAdd < aARPstrc.nFirstInterfaceNoofRecs2Add; nNoofIFrecToAdd++){
                   memcpy(ArpEntry.bPhysAddr, aARPstrc.FirstIFarpPairAr[nNoofIFrecToAdd].MACaddr, ArpEntry.dwPhysAddrLen);
                   ArpEntry.dwAddr = aARPstrc.FirstIFarpPairAr[nNoofIFrecToAdd].IPaddr;
                   dwRes = SetIpNetEntry(&ArpEntry);
                   if((dwRes != NO_ERROR) && (!bStopFurtherMessages)){
                      bStopFurtherMessages = 3;
                      if(dwRes == ERROR_ACCESS_DENIED)
                        MessageBox (hWnd,"ERROR_ACCESS_DENIED","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_INVALID_PARAMETER)
                        MessageBox (hWnd,"ERROR_INVALID_PARAMETER","SetIpNetEntry problem",MB_OK);
                      else if(dwRes == ERROR_NOT_SUPPORTED)
                        MessageBox (hWnd,"ERROR_NOT_SUPPORTED","SetIpNetEntry problem",MB_OK);
                      else{
                       //FormatMessage( dwRes,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                       //FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
                         FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  dwRes,  NULL,  mes,  SIZMES,  NULL);
                         MessageBox(hWnd, mes, "SetIpNetEntry (other error)", MB_ICONEXCLAMATION);
                        }
                     }
                  }
               }
            }
          if(bStopFurtherMessages == 1)
            MessageBox (hWnd,"The attempt to Automatically Generate ARP entries for the AVRs on one interface failed\nThis could be because you specified an invalid Interface Index in StaticARP.arp\nor you may have failed UAC","SetIpNetEntry problem",MB_OK);
          else if(bStopFurtherMessages == 2)
            MessageBox (hWnd,"The attempt to add an ARP entry on your Second Interface Index specified in StaticARP.arp failed\nThis could be because you specified an invalid Second Interface Index\nor you may have failed UAC","SetIpNetEntry problem",MB_OK);
          else if(bStopFurtherMessages == 3)
            MessageBox (hWnd,"The attempt to add an ARP entry on your First Interface Index specified in StaticARP.arp failed\nThis could be because you specified an invalid First Interface Index\nor you may have failed UAC","SetIpNetEntry problem",MB_OK);
         }
     //else if( (dwRes == NO_ERROR) && (pmibIPnetTab->dwNumEntries > 86) &&  (pmibIPnetTab->dwNumEntries < 81) )
       //MessageBox(hWnd,"ARP table size was 37 to 80 entries\nThis is expected if the program has run once already.\nAssuming table is loaded, so doing nothing", "Remove This Message After Testing", MB_OK);
       else{
          MessageBox(hWnd,"Either ARP table > 86 entries or some other error stopped\nthe Static ARP table being loaded.\n\nThis is unexpected but probably isn't critical\n\n...More ...", "Warning", MB_OK);
          if(dwRes == ERROR_INSUFFICIENT_BUFFER)
            MessageBox(hWnd,"GetIpNetTable returned ERROR_INSUFFICIENT_BUFFER", "Additional info", MB_OK);
          else if(dwRes == ERROR_INVALID_PARAMETER)
            MessageBox(hWnd,"GetIpNetTable returned ERROR_INVALID_PARAMETER", "Additional info", MB_OK);
          else if(dwRes == ERROR_NOT_SUPPORTED)
            MessageBox(hWnd,"GetIpNetTable returned ERROR_NOT_SUPPORTED", "Additional info", MB_OK);
          else{
             FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
            //MessageBox(hWnd, "Couldn't create process - i.e. start Notepad, UltraEdit or your chosen editor\n\nPlease check the registry key :\n\n     HKEY_LOCAL_MACHINE\\SOFTWARE\\OpTIC Technium\\HoloProg\\EditorForViewing\n\nMake sure it contains the full path and name of the editor you want to use.",mes, MB_ICONEXCLAMATION);
             MessageBox(hWnd, mes, "Additional info from FormatMessage() (may be nonsense?)", MB_ICONEXCLAMATION);
            }
         }


       StringCchCat(szPrinterPCFfilename,_MAX_DIR,"Printer.pcf"); //szPrinterPCFfilename never changes after this initialisation
       if( myOpen_File(hWnd, "pcf", szPrinterPCFfilename, UNUSED_MOTORNO) ){ //The Printer.pcf file did not open!
          if( IDYES == MessageBox(hWnd,"The program will exit because a valid Printer.pcf file was not found.\n\nWould you like me to create the above named file\nand populate it with the default settings?\n\nThis will at least give you a file to edit - but you MUST check it!!.",szPrinterPCFfilename,MB_YESNO)){
             PCFrec = DefaultPCFrec;
             mySave_File( hWnd, 3000, "pcf",0, szPrinterPCFfilename, MOTORNO_NOT_USED);
            }
          DestroyWindow(hWnd);//exit(1);
          break;
         }
 //    HCFcfs.szCSVDirName4Reading[(int)(ptrBslash-HCFcfs.szCSVDirName4Reading + 1)]=0; //szCSVDirName4Reading again TEMPORARILY holds the directory the progam resides in
       StringCchCopy(szHGRAMSDirName4Reading,_MAX_DIR,szGlobalFileName);
       StringCchCat (szHGRAMSDirName4Reading,_MAX_DIR,"HGrams");	//szHGRAMSDirName4Reading should not be changed again from this point forward

       if( _access(szHGRAMSDirName4Reading, 0) == 0){ //Make certain the HGrams folder exists
          struct _stat64i32 status; //Path exists, but is it a file?
          _stat(szHGRAMSDirName4Reading, &status);
          if(!(status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
             MessageBox(hWnd,"A Folder called HGrams must exist in the same folder as Holoprog.exe.\nInstead there is a file called HGrams, which must be removed.","An Essential Folder was missing.. please rectify this",MB_OK);
             exit(0);
            }
         }
       else{//The path doesn't exist, so create it.
          if(_mkdir(szHGRAMSDirName4Reading) == 0){ //the directory was created
             MessageBox(hWnd,"I had to create folder 'HGrams' in the same folder as Holoprog.exe.\nIt is supposed to exist there without me needing to create it!","Warning",MB_OK);
             errno = 0;
            }
          else{ //even _mkdir failed
             MessageBox(hWnd,"Worse still, Even _mkdir() was unable to create it!\nMust exit.","Essential Folder 'HGrams' was missing!",MB_OK);
             exit(0);
            }
         }

     //StringCchCat(szGlobalFileName, _MAX_PATH, "WavPlate1PcentSplitRGB.cal");            //szGlobalFileName now holds something else
       StringCchCat(szWP1CalFilename, _MAX_PATH, "WavPlate1_13_25PcentSplitRGB.cal");
       if( myOpen_File(hWnd, "cal", szWP1CalFilename, UNUSED_MOTORNO) ){  //The file didn't open properly
          MessageBox (hWnd,"Error: The program will exit because a valid calibration file was not found.\n\nThis file, named in the title bar of this box, is essential.",szWP1CalFilename,MB_OK);
          DestroyWindow(hWnd); //Once the file is read into to the table nWavPlate1PcentSplitRGBAr, the table is never changed, and the file is not looked at again.
          break;
         }
       StringCchCat(szWP2CalFilename, _MAX_PATH,  "WavPlate2ADCtoRealEnergy.cal");
       StringCchCat(szWP3CalFilename, _MAX_PATH,  "WavPlate3ADCtoRealEnergy.cal");
       StringCchCat(szWP14CalFilename, _MAX_PATH, "WavPlate14ADCtoRealEnergy.cal");
       StringCchCat(szWP15CalFilename, _MAX_PATH, "WavPlate15ADCtoRealEnergy.cal");
       StringCchCat(szWP26CalFilename, _MAX_PATH, "WavPlate26ADCtoRealEnergy.cal");
       StringCchCat(szWP27CalFilename, _MAX_PATH, "WavPlate27ADCtoRealEnergy.cal");
       char *pszWPfilenameTable;
       char *pszWPfilenameTablesAr[6];
       pszWPfilenameTablesAr[0] = szWP2CalFilename;
       pszWPfilenameTablesAr[1] = szWP3CalFilename;
       pszWPfilenameTablesAr[2] = szWP14CalFilename;
       pszWPfilenameTablesAr[3] = szWP15CalFilename;
       pszWPfilenameTablesAr[4] = szWP26CalFilename;
       pszWPfilenameTablesAr[5] = szWP27CalFilename;

       ENERGY_CALIB_RREC *pWPTable;
       ENERGY_CALIB_RREC *pWPTablesAr[6];
       pWPTablesAr[0] = WavPlate2ForInterpAr;
       pWPTablesAr[1] = WavPlate3ForInterpAr;
       pWPTablesAr[2] = WavPlate14ForInterpAr;
       pWPTablesAr[3] = WavPlate15ForInterpAr;
       pWPTablesAr[4] = WavPlate26ForInterpAr;
       pWPTablesAr[5] = WavPlate27ForInterpAr;

       int nNoofRowsAr[6];
       for(int WPtable = 0;WPtable < 6; WPtable++){ //Perform this loop once for each of the 6 six table being built
          pszWPfilenameTable = pszWPfilenameTablesAr[WPtable];  //Get the filename
          pWPTable = pWPTablesAr[WPtable];                      // ... and the table we are reading it into
          FILE *load_file;
          errno_t er;
          nNoofRowsAr[WPtable] = 0;
          if((er=fopen_s(&load_file,pszWPfilenameTable,"rb"))!=0){
             StringCchCopy(mes,SIZMES,"Unable to open file ...\n\n");
             StringCchCat(mes,SIZMES,pszWPfilenameTable);
             MessageBox(hWnd,mes,NULL, MB_OK);
             exit(0);
            }
          else{
             int numberScaned = fscanf_s ( load_file,"%5u,%lf",&pWPTable[0].sADCcode,&pWPTable[0].dlbRealEnergy,sizeof(short),sizeof(double));
             while((numberScaned == 2)&&(nNoofRowsAr[WPtable]++ < 91))
               numberScaned = fscanf_s ( load_file,"%5u,%lf",&pWPTable[ nNoofRowsAr[WPtable] ].sADCcode,&pWPTable[ nNoofRowsAr[WPtable] ].dlbRealEnergy,sizeof(short),sizeof(double));
             fclose(load_file);
             if(nNoofRowsAr[WPtable] < 2){
                StringCchCopy(mes,SIZMES,"There are too few calibration pairs in the file ...\n\n");
                StringCchCat(mes,SIZMES,pszWPfilenameTable);
                MessageBox(hWnd,mes,NULL, MB_OK);
                exit(0);
               }
            }
         }
       if(BuildRealEnergyADClookups(hWnd, nNoofRowsAr)) //The array says how long each table to interpolate from is
         exit(0);

       StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");
       StringCchCopy(szGlobalFileName, _MAX_PATH, stdstring(LastHGramFileOpened).c_str() );//szGlobalFileName now holds something else
       if( !lstrcmp(szGlobalFileName, "unset") ){
        //LastHGramFileOpened = szHGRAMSDirName4Reading;
        //LastHGramFileOpened += DEFAULT_HGRAM_FILENAME; //will now be "C:\\>installDir>\\Test1.hcf
          StringCchCat (szGLWndTitle, SIZszGLWndTitle, szGlobalFileName);  //append "unset" to szGLWndTitle
          HCFcfs.szCSVDirName4Reading[0] = 0;
          HCFcfs.szCSVDirName4Writing[0] = 0;
          MessageBox (hWnd,"Before proceeding further, You are advised to either :\n\n- Open an existing Hologram\n\n   OR\n\n- Select <File> , <New> , <Hologram Config File>\n   (and then type a name for a new Hologram)","There doesn't appear to be a 'Last Hologram Opened' setting !",MB_OK);
         }
       else{ //Open the hcf file for the last hologram opened.  "CSVs" and "CSVsOUT" folder need to be located in each hologram's folder.  We must know where the hcf is
             // located in order to read it, so at the point of reading it, we should also set HCFcfs's fields. (Looking a few lines up, note it is in szGlobalFileName)
          if( myOpen_File(hWnd, "hcf", stdstring(LastHGramFileOpened).c_str(), UNUSED_MOTORNO) ){ //The file did not open!
             //SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp1_MOTOR_POS_DATA, 0, 0);
             StringCchCopy(szGLWndTitle + 10,SIZszGLWndTitle - 10, "Error opening the last Hologram's hcf file!");
             HCFcfs.szCSVDirName4Reading[0] = 0;
             HCFcfs.szCSVDirName4Writing[0] = 0;
             HCFrec = DefaultHCFrec;
            }
          else{ //The hcf file did open
             StringCchCopy(szGLWndTitle + 10,SIZszGLWndTitle - 10, szGlobalFileName);
             StringCchCopy(HCFcfs.szCSVDirName4Reading,_MAX_DIR, szGlobalFileName);
             ptrBslash = strrchr(HCFcfs.szCSVDirName4Reading,BACKSLASH);    //reverse search for the final backslash
             HCFcfs.szCSVDirName4Reading[(int)(ptrBslash-HCFcfs.szCSVDirName4Reading + 1)]=0; //szCSVDirName4Reading now holds the directory the hcf file resides in.
             StringCchCopy(HCFcfs.szCSVDirName4Writing,_MAX_DIR,HCFcfs.szCSVDirName4Reading);
             StringCchCat (HCFcfs.szCSVDirName4Reading,_MAX_DIR,"CSVs\\");	//szCSVDirName4Reading should be changed every time a new hcf file is used in a different location
             StringCchCat (HCFcfs.szCSVDirName4Writing,_MAX_DIR,"CSVsOUT\\");	//szCSVDirName4Writing should be changed every time a new hcf file is used in a different location
             if (HCFrec.bHCFIsComplete){ //Read in "StartPositions.spf"
                char szLocalSPFfileName3[_MAX_DIR];
                StringCchCopy(szLocalSPFfileName3,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from, but when generating the hologram we write to it.
                StringCchCat(szLocalSPFfileName3,_MAX_DIR,DEFAULT_BIN_OUTFILENAMESTARTPOSITIONS);//"StartPositions.spf"
                if( myOpen_File(hWnd, "spf", szLocalSPFfileName3, UNUSED_MOTORNO) ){  //The file didn't open properly
                   MessageBox (hWnd,"StartPositions.spf  did not open properly for this hologram.\n\nThe control data for printing is not complete.\n\nDo not attempt to print this holgram.",NULL,MB_OK);
                   HCFrec.bThisHoloGramHasABadError = 1;
                  }
                else
                  if(HCFrec.bThisHoloGramHasABadError == 1)
                    HCFrec.bThisHoloGramHasABadError = 0; //clear the error, because we have successfully opened the file this time
                  else if (HCFrec.bThisHoloGramHasABadError)
                    MessageBox (hWnd,"This holgram has an unspecified 'Bad Error' (i.e. > 2)\nPlease report\nYou will probabably need to work on another one!",NULL,MB_OK);

               }
            }
         }
       RunControlBoxProgram(hWnd, PIX_TRIGGER_ONLY, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,0);
       SetWindowText(hGLWnd,szGLWndTitle);
       //StringCchCopy( szGlobalFileName, _MAX_DIR, stdstring(LastHGramFileOpened).c_str() );
       //WarnIfSoundFileIsMissing(hWnd);
     /*if( NULL == (ghMutex = CreateMutex(NULL, FALSE, NULL)) ){//default security attributes //initially not owned //unnamed mutex
          MessageBox (hWnd,"ghMutex has failed. The program must abort, sorry.",NULL,MB_OK);
          DestroyWindow(hWnd);//exit(1);
          break;
         }*/
       if( InitialiseAllSockets(hWnd) ){
          if(bAllowStartIfInitialiseAllSocketsFails)
             MessageBox (hWnd,"InitialiseAllSockets() failed,  but the registry says the program is permitted to start anyway!","Warning",MB_OK|MB_ICONEXCLAMATION);
          else{
             MessageBox (hWnd,"InitialiseAllSockets() has failed. This is an essential\nfunction, so the program must abort, sorry.",NULL,MB_OK);
             bAllowStartIfInitialiseAllSocketsFails = 1;
             bAllowStartIfInitialiseAllSocketsFails = 0;
             DestroyWindow(hWnd);
             break;
            }
         }
       bInitialisationSucceeded = TRUE;
       if( Create_CheckAll_MonitoringThread(hWnd) ){
          MessageBox (hWnd,"Create_CheckAll_MonitoringThread() Failed!\n\nSorry\n\nBye Bye",NULL,MB_OK);
          DestroyWindow(hWnd);
          break;
         }
       InitialiseAllReceiveBuffers();

       for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++)
         if(PCFrec.bMotor2ExpectRepliesFrom[nchEM]){ //MsTyp6_ENAB_DISAB is the only message sent during WM_CREATE
            SendMessagePackets(hWnd, nchEM, MsTyp6_ENAB_DISAB, ENAB_POWR, 0);   //Enable the Power Driver on MDS542   (Accelerator Key E for Enable)
            Sleep(15);
           }

#if 1
//int thePortToOpenShouldBeInRegistry;//It is now.  Also n30SECS_BUFFERLENGTH probably doesn't need to be specified if we don't use a buffer. (now removed)
       int ComInitRetcode = comUSB_Init( stdstring(USBportToOpenForEnergyMeters).c_str(),/*stdstring(USBportToOpenForControlBox).c_str(),*/ hWnd); //By default these are "Dual RS232 A",  "Dual RS232 B"

       if(ComInitRetcode == -1){//port, buffersize.. Now calls LoadDLL(hWnd); //2^22
#ifdef B_ALLOW_USBCOMMS_ERRORS
//        MessageBox(hWnd,"This version of the program allows you to continue despite this.","USB Communications initialisation failed.", MB_OK);
#else
          MessageBox(hWnd,"Now shuting down... please restart the program once\nthe problem has been resolved (e.g. Check the cabling).","USB Communications initialisation failed.", MB_OK);
          DestroyWindow(hWnd);
          break;
#endif
         }
#endif

       HDC hdc = GetDC(NULL);
       int lfHeight = -MulDiv(8,GetDeviceCaps(hdc,LOGPIXELSY),72);
       newFont = CreateFont(lfHeight,0,0,0,0,FALSE,0,0,0,0,0,0,0,"Helv");
       ReleaseDC(NULL, hdc);
int IDB_STATREC_POS2to40 = IDB_STATREC_POS;
int IDB_STATREC_HOME_STATE2to40  = IDB_STATREC_HOME_STATE;

int IDB_STATREC_EXTRA_SW2to40    = IDB_STATREC_EXTRA;

int IDB_STATREC_HOME_SW2to40     = IDB_STATREC_HOME_SW;
int IDB_STATREC_LIMIT_SWa2to40   = IDB_STATREC_LIMIT_SWa;
int IDB_STATREC_LIMIT_SWb2to40   = IDB_STATREC_LIMIT_SWb;
int IDB_STATREC_DRIVER_ENAB2to40 = IDB_STATREC_DRIVER_ENAB;
int IDB_STATREC_RS485_ENAB2to40  = IDB_STATREC_RS485_ENAB;
int IDB_STATREC_GENERALWARN2to40 = IDB_STATREC_GENERALWARN;
int IDB_STATREC_SERIOUSWARN2to40 = IDB_STATREC_SERIOUSWARN;
int IDB_STATREC_REDYTPAL2to40    = IDB_STATREC_REDYTPAL;
int IDB_STATREC_IWANTLow2to40    = IDB_STATREC_IWANTLow;
int IDB_STATREC_IT_IS_Low2to40   = IDB_STATREC_IT_IS_Low;
int IDB_STATREC_VARa2to40 = IDB_STATREC_VARa;
int IDB_STATREC_VARb2to40 = IDB_STATREC_VARb;
int IDB_STATREC_VARc2to40 = IDB_STATREC_VARc;
int IDB_STATREC_RESETm02to40      = IDB_STATREC_RESETm01;
int IDB_STATREC_SINGMMm02to40     = IDB_STATREC_SINGMMm01;
int IDB_STATREC_TRANSBLOCKm02to40 = IDB_STATREC_TRANSBLOCKm01;

#define LOWERY 34
#define REQROWS 40
int upper = LOWERY + 21 * REQROWS + 30; //30 allows for the spaces
       int Ypos, YposP1;
       for(Ypos = LOWERY; Ypos < upper; Ypos+=21){
          if(Ypos == 286)  //add a space between rows after motor 12
            Ypos += 10;
          if(Ypos == 548)  //add a space between rows after motor 24
            Ypos += 10;
          if(Ypos == 810)  //add a space between rows after motor 36
            Ypos += 10;
          YposP1 = Ypos + 1;
          CreateWindowEx(NULL,"BUTTON","S", WS_CHILD | WS_VISIBLE | WS_TABSTOP,                                         //The 2 LH columns of buttons Focusable
                    1, YposP1, 13, 20,  hWnd, (HMENU)IDB_STATREC_SINGMMm02to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_SINGMMm02to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(NULL,"BUTTON","B", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                    16, YposP1, 13, 20,  hWnd, (HMENU)IDB_STATREC_TRANSBLOCKm02to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_TRANSBLOCKm02to40++,WM_SETFONT,(WPARAM)newFont,0);

          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Position
                    152, Ypos, 104, 22,  hWnd, (HMENU)IDB_STATREC_POS2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_POS2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Home State
                    263, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_HOME_STATE2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_HOME_STATE2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //EXTRA
                    290, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_EXTRA_SW2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_EXTRA_SW2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Home Switch
                    313, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_HOME_SW2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_HOME_SW2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Limit Switch A
                    344, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_LIMIT_SWa2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_LIMIT_SWa2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Limit Switch B
                    377, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_LIMIT_SWb2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_LIMIT_SWb2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    428, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_DRIVER_ENAB2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_DRIVER_ENAB2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                   460, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_RS485_ENAB2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_RS485_ENAB2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    512, Ypos, 271, 22,  hWnd, (HMENU)IDB_STATREC_GENERALWARN2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_GENERALWARN2to40++,WM_SETFONT,(WPARAM)newFont,0);


          CreateWindowEx(NULL,"BUTTON","R", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                   785, YposP1, 13, 20,  hWnd, (HMENU)IDB_STATREC_RESETm02to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_RESETm02to40++,WM_SETFONT,(WPARAM)newFont,0);


          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    800, Ypos, 271, 22,  hWnd, (HMENU)IDB_STATREC_SERIOUSWARN2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_SERIOUSWARN2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1096, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_IWANTLow2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_IWANTLow2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1142, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_IT_IS_Low2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_IT_IS_Low2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1214, Ypos, 25, 22,  hWnd, (HMENU)IDB_STATREC_REDYTPAL2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_REDYTPAL2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1252, Ypos, 77, 22,  hWnd, (HMENU)IDB_STATREC_VARa2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_VARa2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                   1342, Ypos, 77, 22,  hWnd, (HMENU)IDB_STATREC_VARb2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_VARb2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                   1430, Ypos, 77, 22,  hWnd, (HMENU)IDB_STATREC_VARc2to40,hInst,NULL);
          SendDlgItemMessage(hWnd,IDB_STATREC_VARc2to40++,WM_SETFONT,(WPARAM)newFont,0);	  
         }
       //All the rows are done.  Now create the Buttons at the bottom.
       int YposP22 = Ypos + 22;
       int YposP43 = Ypos + 43;
           YposP1  = Ypos + 1;
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Enable All Drives", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                357, YposP22, 100, 20,  hWnd, (HMENU)IDB_STATREC_DRIVER_ENABall,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_DRIVER_ENABall,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Disable All Drives", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                357, YposP43, 100, 20,  hWnd, (HMENU)IDB_STATREC_DRIVER_DISABall,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_DRIVER_DISABall,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Disable All PIDs", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                257, YposP43, 100, 20,  hWnd, (HMENU)IDB_STATREC_DRIVER_DISAB_PIDS,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_DRIVER_DISAB_PIDS,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Enable All RS485", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                457, YposP22, 100, 20,  hWnd, (HMENU)IDB_STATREC_RS485_ENABall,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_RS485_ENABall,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Disable All RS485", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                457, YposP43, 100, 20,  hWnd, (HMENU)IDB_STATREC_RS485_DISABall,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_RS485_DISABall,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Auto - Refresh", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                1057, YposP43, 200, 41,  hWnd, (HMENU)IDB_STATREC_AUTOREFRESH,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_AUTOREFRESH,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Refresh Once", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                1307, YposP43, 200, 41,  hWnd, (HMENU)IDB_STATREC_REFRESH,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_REFRESH,WM_SETFONT,(WPARAM)newFont,0);


       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Help", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                357, Ypos+64, 200, 20,  hWnd, (HMENU)IDB_STATREC_HELP,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_HELP,WM_SETFONT,(WPARAM)newFont,0);


       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Home All", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                247, YposP1, 60, 20,  hWnd, (HMENU)IDB_STATREC_HOMEALL,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_HOMEALL,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Reset All", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                765, YposP1, 60, 20,  hWnd, (HMENU)IDB_STATREC_RESETmALL,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_STATREC_RESETmALL,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","PID debug Msgs", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                1, Ypos+87, 123, 22,  hWnd, (HMENU)IDB_DISPLAY_PID_DEBUG_MSGs,hInst,NULL);
       SendDlgItemMessage(hWnd,IDB_DISPLAY_PID_DEBUG_MSGs,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","", WS_VISIBLE | WS_TABSTOP,  //This is a bodge! Accelerators stop working as soon as any of the above controls in the main
                5, 5, 22, 22,  hWnd, (HMENU)IDB_STATREC_DUMMY,hInst,NULL);// have the focus. By Automatically setting the focus to this dummy button, which is not WS_CHILD
              //3000, 3000, 2, 2,  hWnd, (HMENU)IDB_STATREC_DUMMY,hInst,NULL);// have the focus. By Automatically setting the focus to this dummy button, which is not WS_CHILD
       hFocusButton = GetDlgItem(hWnd,IDB_STATREC_DUMMY);                        // & cant be seen, Accelerators keep working!
      //bSTATREC_BOXisBeingDisplayed = TRUE;
      //for(int MotorNo = 0; MotorNo < NOOFMOTORS; MotorNo++)
      //  U pdateMainWindowsControlsM(hWnd, MotorNo);  The timer below will do this
       bTimerHasBeenKilled = 0;
       if(!SetTimer(hWnd, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
         MessageBox(hWnd,"IDT_TIMER_WNDPROC didn't start @1!",NULL,MB_OK);
       if(bStartWithPIDsDisabled)
         PostMessage(hWnd,WM_COMMAND,IDB_STATREC_DRIVER_DISAB_PIDS,0);
       if(bStartWithViewHologramInfoOpened)
         PostMessage(hWnd,WM_COMMAND,IDM_VIEWHOLOGINFO,0);
       else
         PostMessage(hWnd,WM_COMMAND,IDM_STARTUP_OPTIONS,0);
       
       PostMessage(hWnd,WM_INJECT_FROM_PID_HELPERPROG,0,0);
      }
      break;
    case WM_COMMAND:
      wmId    = LOWORD(wParam);
    //wmEvent = HIWORD(wParam);
      //if ((wmId > ID_NON_FOCUSABLE_WINDOW_LWR)&&(wmId < ID_NON_FOCUSABLE_WINDOW))
        //wmId = ID_NON_FOCUSABLE_WINDOW;
      switch (wmId){     // Parse the menu selections:
         case IDM_MM_PID_MOVE:{
#define  STEPNUMERATOR 64000
#define NSTEPNUMERATOR STEPNUMERATOR * -1
#define  STEPDENOMINATOR 3000 //the lower this number, the faster the motor will move (may cause stalling)
#define NSTEPDENOMINATOR STEPDENOMINATOR * -1
          //if(!bPIDMovementStoppedForHoming){
            if(!bStopPIDsendingMessages){
               for(int nWPs2and3_rgb = 0; nWPs2and3_rgb < 6; nWPs2and3_rgb++){
                  dblPidOutputToActOn = PidOutputToActOn[nWPs2and3_rgb];
                  nchLookUpWPmotorNo = chLookUpWPmotorNo[nWPs2and3_rgb];  //{1,2,13,14,25,26}
                  if(PCFrec.bMotor2ExpectRepliesFrom[nchLookUpWPmotorNo]){
                     if(dblPidOutputToActOn >= 0){
          //              if(  ((int)dblPidOutputToActOn < 30)  && ((int)dblPidOutputToActOn > 1)       ){
            //               int intermediate = (int)dblPidOutputToActOn * 1000;
              //             dblPidOutputToActOn = 30000 - intermediate;
                //          }
                  //      else
                        dblPidOutputToActOn = STEPNUMERATOR / dblPidOutputToActOn;
                        if(dblPidOutputToActOn < STEPDENOMINATOR){
                           SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "F");  //The Fast limit (forwards) was hit
                           dblPidOutputToActOn = STEPDENOMINATOR;
                          }
                        else if(dblPidOutputToActOn > STEPNUMERATOR){
                           SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "f");  //The Slow limit (either direction) was hit
                           dblPidOutputToActOn = STEPNUMERATOR;
                           dblPidOutputToActOn = 60000;
                          }
                        else
                          SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "1");
                        //if(nchLookUpWPmotorNo == 1)
                        SendMessagePackets(NULL, nchLookUpWPmotorNo, MsTypB_SING_MM_PID, FORWARD_DIR, (int)dblPidOutputToActOn); //really the message is an OTF2 ,message
                       }
                     else{
          //             if(  ((int)dblPidOutputToActOn > -30)  && ((int)dblPidOutputToActOn < -1)       ){
            //               int intermediate = (int)dblPidOutputToActOn * 1000;
              //             dblPidOutputToActOn = -30000 - intermediate;
                //          }
                  //      else
                        dblPidOutputToActOn = STEPNUMERATOR / dblPidOutputToActOn;
                        if(dblPidOutputToActOn > NSTEPDENOMINATOR){
                           SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "B");  //The Fast limit (backwards) was hit
                           dblPidOutputToActOn = NSTEPDENOMINATOR;
                          }
                        else if(dblPidOutputToActOn < NSTEPNUMERATOR){
                           SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "b");  //The Slow limit (either direction) was hit
                           dblPidOutputToActOn = NSTEPNUMERATOR;
                           dblPidOutputToActOn = -60000;
                          }
                        else
                          SetDlgItemText(hWnd, IDB_STATREC_EXTRA + nchLookUpWPmotorNo, "2");
                        //if(nchLookUpWPmotorNo == 1)
                        SendMessagePackets(NULL, nchLookUpWPmotorNo, MsTypB_SING_MM_PID, REVERSE_DIR, (int)(dblPidOutputToActOn * -1)); //really the message is an OTF2 ,message
                       }
                    } //(nchLookUpWPmotorNo == 13)
                 }
              } //if(!bStopPIDsendingMessages)
           }
           break;
         case IDM_MM_PID_STOPMOVES:
           SendMessagePackets(hWnd,1, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           //Sleep(5);
           SendMessagePackets(hWnd,2, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           //Sleep(5);
           SendMessagePackets(hWnd,13, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           //Sleep(5);
           SendMessagePackets(hWnd,14, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           //Sleep(5);
           SendMessagePackets(hWnd,25, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           //Sleep(5);
           SendMessagePackets(hWnd,26, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
           break;
         case IDM_SENDCHECKSUMPLEASE:{
#if 0
//            int eaM_cs;
//            int n ReturnCodeFromSendPackets;
//           #define SIZ_TwEC 56
//            for(eaM_cs = 38; eaM_cs < 39; eaM_cs++)
//              if(PCFrec.bMotor2ExpectRepliesFrom[eaM_cs]){
//                 nReturnCodeFromSendPackets = SendMessagePackets(NULL, eaM_cs, M sTypD_SENDchecksumREC, ++c MessageSeqNoCS[eaM_cs], 0);
//                 if(nReturnCodeFromSendPackets){ //"nReturnCodeFromSendPackets Error"
//                    if(nReturnCodeFromSendPackets == 5001)
//                      DisplayErrorMessageBoxWithErrorAndMotorNo(hWnd, eaM_cs, "Send() function took > 0.3 seconds from Motor: ", "Error checking motors' CheckSums...");
//                    else{
//                       char szTitleWithErrorCode[SIZ_TwEC];
//                       StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"nReturnCodeFromSendPackets Error code: ");
//                       _itoa_s(nReturnCodeFromSendPackets, szTitleWithErrorCode + 39, SIZ_TwEC - 39, 10);
//                       DisplayErrorMessageBoxWithErrorAndMotorNo(hWnd, eaM_cs, "Error1 occurred whilst checking motors' CheckSum, for Motor: ",szTitleWithErrorCode);
//                      }
//                    return eaM_cs + 1001;
//                   }
//                 Sleep(3);
//                }
//            for(eaM_cs = 3; eaM_cs < 4; eaM_cs++){
//               if( PCFrec.bMotor2ExpectRepliesFrom[eaM_cs]){
//                  if (!b CSfromUC3isSameAsCSfromSendBuffer(hWnd, eaM_cs) )
//                    MessageBox (hWnd,"The Checksums differed!",NULL,MB_OK);
//                 }
//               //Sleep(3);
//              }
#else
             MessageBox (hWnd,"IDM_SENDCHECKSUMPLEASE code commented out!",NULL,MB_OK);
#endif
           }
           break;
         case IDM_OPEN_CONFIGFILE:
           nShowHologInfoBecauseFileOpenedOK = 0; //Is set if file opened OK - to 1 if opened for Printing, and to 2 if opened for Editing.
           //OpenConfigFile_Dialog collects the filename and reads the hcf file in (either for editing or for printing).  It prevents complete & incomplete files
           // being opened for the wrong purpose, but allows complete files to be copied to a new folder (making it incomplete & ready for editing).
           DialogBox(hInst, MAKEINTRESOURCE(IDD_OPEN_CONFIG_FILE), hWnd, OpenConfigFile_Dialog);
           if(nShowHologInfoBecauseFileOpenedOK == 1){
              char szLocalSPFfileName2[_MAX_DIR]; //"StartPositions.spf"
              StringCchCopy(szLocalSPFfileName2,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from, but when generating the hologram we write to it.
              StringCchCat(szLocalSPFfileName2,_MAX_DIR,DEFAULT_BIN_OUTFILENAMESTARTPOSITIONS);//"StartPositions.spf"
              if( !myOpen_File(hWnd, "spf", szLocalSPFfileName2, UNUSED_MOTORNO) )  //The file opened Ok
                DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWHOLOGINFO4PRINTING), hWnd, ViewHologInfo_Dialog);
              else{
                 MessageBox (hWnd,"StartPositions.spf  did not open properly for this hologram @2.\n\nThe control data for printing is not complete.\n\nDo not attempt to print this holgram.",NULL,MB_OK);
                 HCFrec.bThisHoloGramHasABadError = 1;
                }
             }
           else if(nShowHologInfoBecauseFileOpenedOK == 2)
             DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWHOLOGINFO4EDITING), hWnd, ViewHologInfo_Dialog);        
           break;
         case IDM_CONTROL_ENERGIES:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTROL_ENERGIES), hWnd, Control_Energies_Dialog);
           break;
         case IDM_STARTUP_OPTIONS:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_REGISTRYOPTIONS), hWnd, StartUpOptions_Dialog);
           break;
         case IDM_VIEWPRINTERCONFIG:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEW4PRINTERCONFIG), hWnd, ViewPrinterConfig_Dialog);
           break;
         case IDM_GEOLASHOWDIALOGBOX:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_GEOLASHOWDIALOGBOX), hWnd, JobsOnDisplayPC_Dialog);
           break;
         case IDM_JERZYSHOWDIALOGBOX:
           //CallManagedModule(hWnd);
           //CallManagedModule();
           //MessageBox (hWnd,"CallManagedModule removed","warning",MB_OK);
           break;
         case IDM_JERZYSHOWDIALOGBOX_UNMANAGED:
           //CallManagedModule(hWnd);
           //CallUnManagedModule();   
           break;
//       case IDM_SAVE_CONFIGFILE:
//         DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVE_CONFIG_FILE), hWnd, SaveConfigFile_Dialog);
//         break;
         case IDM_SAVE_ENERGIESREADFILE:{
            if(!lstrcmp(NO_HOLOGRAM_NAME,HCFrec.szHologramName)){
               MessageBox (hWnd,"You must have a Hologram Open, as this determines where the file should be written.","Cannot Save file...",MB_OK);
               break;
              }
          //DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVE_ENERGIES_READ_FILE), hWnd, SaveEnergiesReadFile_Dialog); //This function in SaveEnergies.cpp is now renegated to the OldCode folder
            char szLocalEnergiesReadfileName[_MAX_DIR]; //"EnergiesRead.bin"
            StringCchCopy(szLocalEnergiesReadfileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
            StringCchCat (szLocalEnergiesReadfileName,_MAX_DIR,DEFAULT_BIN_OUTFILENAMEENERGIES_READ); //"EnergiesRead.bin"
            mySave_File (hWnd, 77760000, "bin", 1, szLocalEnergiesReadfileName, MOTORNO_NOT_USED);// = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)
            MakeWarningSound();
//For the time being, Write the file as a CSV as well:
            StringCchCopy(szLocalEnergiesReadfileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
            StringCchCat (szLocalEnergiesReadfileName,_MAX_DIR,DEFAULT_CSV_OUTFILENAMEENERGIES_READ); //"EnergiesRead.csv"
            mySave_File (hWnd, 77760000, "csv", TYPE3CSV, szLocalEnergiesReadfileName, MOTORNO_NOT_USED);
            MakeWarningSound();
           }
           break;
         case IDM_NEW_CONFIGFILE:{
            HCFRECORD OrigCopyOfHCFrec;
            BOOL bSillyNameMessShown = TRUE;
            OrigCopyOfHCFrec = HCFrec;
            HCFrec = DefaultHCFrec;
            while(bSillyNameMessShown)
              CreateNewHCFfile(hWnd, "You need to type the name here!", &bSillyNameMessShown);
            if (HCFrec.uTimeStamp == DefaultHCFrec.uTimeStamp) //User must have pressed cancel while being prompted for a filename
              HCFrec = OrigCopyOfHCFrec;
            else
              DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWHOLOGINFO4EDITING), hWnd, ViewHologInfo_Dialog);
           }
           break;
         case IDM_ABOUT:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
           break;
//       case IDM_ZEND_STATUSREC:   //Send Request for the StatusRec
//          MessageBox (hWnd,"The functionality of the 'show status recs' modeless dialog is now incorporated into the main window","message",MB_OK);
//         MoveWindow(hWnd, 0, 70, MAINWINDOW_X_SIZE, MAINWINDOW_Y_SIZE, TRUE);
//         if (!bSTATREC_BOXisBeingDisplayed)
//           CreateDialog (hInst, MAKEINTRESOURCE(IDD_DISP_STATREC_BOX), hWnd, DisplayStatRec_DialogM);//Dialog calls SendMessagePackets(..., M sTyp2_ZENDstatREC, 139, ...)
//         BringWindowToTop(hWnd);
//         break;
         case IDM_VIEWHOLOGINFO:
           if (HCFrec.bHCFIsComplete)
             DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWHOLOGINFO4PRINTING), hWnd, ViewHologInfo_Dialog);
           else
             DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWHOLOGINFO4EDITING), hWnd, ViewHologInfo_Dialog);
           break;
//Above messages open dialog boxes, the following SendMessagePackets:
//       case IDM_STARTSTOPTOGGLE: //Was 0x02 (Start/Stop Message), 98 in nPulsesOrSubTypOrFOPP parameter is instruction to Toggle. 98 NOT USED ANYMORE.  NOW JUST TOGGLE
//         SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypB_STARTSTOP_TOGGLE, 98, 0);
//         break;
//       case IDM_STOP: //Was 0x02 (Start/Stop Message), 97 in nPulsesOrSubTypOrFOPP parameter was instruction to STOP. 97 NOT USED ANYMORE.  NOW JUST STOP - msTyp 0x0c
//         SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypC_STOP, 97, 0);
//         break;
//       case IDM_HOME:{
//        //int ThisShouldNOTbe_CurrentMotorNo2; //It should be either Motor 5, 17, or 29
//          int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrentMotorZB];//nCurrent_uSteps can be 1,2,4,8,16,32,or64
//          SendMessagePackets(hWnd,
//                  nchCurrentMotorZB, //for development only. (It should be either Motor 5, 17, or 29)
//                      MsTyp3_HOME,
//                  nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
//                  nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant Fast speed to move at, when homing in
//           //The limits on 'how many steps to move' is still determined by UC3 hardcode (e.g. 110 revolutions)
//           // We could use some of the 12 unused bytes in the 22 byte message to send more information. e.g. whether or not to use fast acceleration when homing.
//           // Fast acceleration causes slippage because the motor cannot stop instantly from high speed when home is hit, so not really suitable for homing
//           // check during printing.
//         }
//         break;
         case IDM_REF_BEAM_TABLE_2LOAD_POS_FAST:
           MoveREF_BEAM_TABLEtoLoadPosition(hWnd, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1], PCFrec.nNoofUsteps4RefBeamTablePrintUnload[3], 1);
           break;
         case IDM_REF_BEAM_TABLE_2PRINT_POS_FAST: //We shouldn't really need this, as it's really part of the general M oveToStartPositions() for all motors
           MoveREF_BEAM_TABLEtoLoadPosition(hWnd, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0], PCFrec.nNoofUsteps4RefBeamTablePrintUnload[2], 1);
           break;
         case IDM_REF_BEAM_TABLE_2LOAD_POS:
           MoveREF_BEAM_TABLEtoLoadPosition(hWnd, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1], PCFrec.nNoofUsteps4RefBeamTablePrintUnload[3], 0);
           break;
         case IDM_REF_BEAM_TABLE_2PRINT_POS: //We shouldn't really need this, as it's really part of the general M oveToStartPositions() for all motors
           MoveREF_BEAM_TABLEtoLoadPosition(hWnd, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0], PCFrec.nNoofUsteps4RefBeamTablePrintUnload[2], 0);
           break;
//       case IDM_RESET:
//         SendMessagePackets(hWnd, nchCurrentMotorZB, M sTyp4_RESET, 0, 0);   //0x04 (RESET Message)
//         break;
//       case IDM_JOG_P:     //Jog Plus
//         SendMessagePackets(hWnd, nchCurrentMotorZB, M sTyp5_SING_MM, JOGSLOWNOOFSTEPS_P,    1600);   //0x05 (JOG Message)
//         break;
//       case IDM_JOG_M:     //Jog Minus
//         SendMessagePackets(hWnd, nchCurrentMotorZB, M sTyp5_SING_MM, JOGSLOWNOOFSTEPS_M,    1600);
//         break;
//       case IDM_JOG_PF:    //Jog Plus Fast
//         SendMessagePackets(hWnd, nchCurrentMotorZB, M sTyp5_SING_MM, JOGFASTNOOFSTEPS_P,    1600);
//         break;
//       case IDM_JOG_MF:    //Jog Minus Fast
//         SendMessagePackets(hWnd, nchCurrentMotorZB, M sTyp5_SING_MM, JOGFASTNOOFSTEPS_M,    1600);
//         break;
//This is dead code, with the new interface, especially now the accelerators keys 'e' and 'd' are removed
//       case IDM_POWER_ENABLE:     //Enable the Power Driver on MDS542  (Accelerator Key E for Enable)
//         for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++){
//            if( SendMessagePackets(hWnd, nchEM, MsTyp6_ENAB_DISAB,  ENAB_POWR, 0) )
//              nchEM =  1000;
//            Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
//           }
//         if(nchEM > 999)
//           MessageBox(hWnd,"As a message to enable power on one of the motors failed,\nI didn't send the 'enable power' message to other motors.\n\nThis may mean that some of (or all) the motors are not energised.","Warning",MB_OK);
//         break;
//       case IDM_POWER_DISABLE:    //Disable the Power Driver on MDS542 (Accelerator Key O for Off)
//         for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++){
//            if( SendMessagePackets(hWnd, nchEM, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0) )
//               nchEM =  1000;
//            Sleep(10);
//           }
//         if(nchEM > 999)
//           MessageBox(hWnd,"As a message to disable power on one of the motors failed,\nI didn't send the 'disable power' message to other motors.\n\nThis may mean that some of (or all) the motors are still energised.","Warning",MB_OK);
//         break;
//       case IDM_RS485ON:{     //Produce RS485 messages
//          for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++){
//             if(nchEM != nchCurrentMotorZB) //Don't try to disable the current motor... aftr all we are trying to enable it!
//               if( SendMessagePackets(hWnd, nchEM, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
//                 nchEM =  1000;
//             Sleep(5);
//            }
//          if(nchEM > 999)
//            MessageBox(hWnd,"Normally, before enabling RS485 for the active motor, 'Disable RS485' messages\nare sent to all the other motors.\n\nOne of these 'Disable RS485' messages failed (on this occassion) so I gave up on\nsending this message to the other motors. (Suggest you view the Status Screen)\n\nThis may mean the is contention for the RS485 bus.\n\nWhen you press 'OK' I will still send the 'Enable RS485' message to the current motor","Warning - contention for the RS485 bus is a possibility",MB_OK);
//          SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp6_ENAB_DISAB,   ENAB_485, 0); //Enable
//         }
//         break;
//       case IDM_RS485OFF:     //No RS485 messages
//         for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++){
//            if( SendMessagePackets(hWnd, nchEM, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
//              nchEM =  1000;
//            Sleep(5);
//           }
//         if(nchEM > 999)
//           MessageBox(hWnd,"Whilst sending 'Disable RS485' messages to all motors, one message failed.\nConsequently, I didn't send the message to the remaining motors. (Suggest you view the Status Screen)\n\nThis may mean RS485 is still enabled.","Warning - RS485 bus may still be active",MB_OK);
//         break;                   //0x07 MsTyp7_SING_MM_OTF is a form of Single Motor Move Message, but for "On The Fly"
//       case IDM_PULSE_LEN_PLUS:   //Increase The Pulse Length
//         SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp8_SET_PULSE_LENGTH, 1, 0);
//         break;
//       case IDM_PULSE_LENMINUS:   //Decrease The Pulse Length
//         SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp8_SET_PULSE_LENGTH, 0, 0);
//         break;
/*       case IDM_SETpixFREQ:      //Send FrequencyOfPixPrints so EVK knows what to expect (used to calculate TimerDriftCorrection)
           //SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypA_SETpixFREQ,    60000000,       0); //Param is Freq times 1000000
           //SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypA_SETpixFREQ,  30000000,       0); //Param is Freq times 1000000
           //SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypA_SETpixFREQ,  15000000,       0); //Param is Freq times 1000000
           break; */
         case IDM_PIXFREQ10: //TC_CLOCK_SOURCE_TC2 counts to 33000 in  1 millisecond  (This shows PBA is running at 66MHz)
         case IDM_PIXFREQ15: //TC_CLOCK_SOURCE_TC3 counts to 33000 in  4 milliseconds
         case IDM_PIXFREQ30: //TC_CLOCK_SOURCE_TC4 counts to 33000 in 16 milliseconds  30Hz is 33.3 mSecs, so not possible to count
         case IDM_PIXFREQ60: //TC_CLOCK_SOURCE_TC5 counts to 33000 in 64 milliseconds  Conceivably we could measure an 8 Hz set of pulses. @ 30Hz theoretical cnt is 17187.5
         //SendMessagePackets(hWnd, nchCurrentMotorZB, MsTypA_SETpixFREQ, wmId - IDM_PIXFREQ10, 0);//Set subtype between 0 and 3 for TC_CLOCK_SOURCE_TC5 to TC_CLOCK_SOURCE_TC4
           if(!lstrcmp(NO_HOLOGRAM_NAME,HCFrec.szHologramName)){
              MessageBox (hWnd,"You must have a Hologram Open, as this controls which frequency is valid.","Cannot Set Pixel Frequencies...",MB_OK);
              break;
             }
           bTimerHasBeenKilled = TRUE; //Temporary Suspension while Pix Frequency is checked
           for(int nEachFreqInArr = NOOFMOTORS * 40 - 1; nEachFreqInArr > -1; nEachFreqInArr--)
             nPixFreqsAr[nEachFreqInArr] = -99;
           //memset(nPixFreqsAr,-1,NOOFMOTORS * 4); //fills Header with ascii zeros (48 IS zeros, not header length)
           KillTimer(hWnd, IDT_TIMER_WNDPROC); //in IDM_PIXFREQ10 15 30 or 60
           for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++)
             if(PCFrec.bMotor2ExpectRepliesFrom[nchEM]){
                SendMessagePackets(hWnd, nchEM, MsTypA_SETpixFREQ, wmId - IDM_PIXFREQ10, 0);
                Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
               }
           Sleep(1500);//Allow time for the AVRs to take the measurements, then tell the AVR we want the table sent back to the PC...
           for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++)
             if(PCFrec.bMotor2ExpectRepliesFrom[nchEM]){
                SendMessagePackets(hWnd, nchEM, MsTyp9_SENDpixPOS_TABLE, SUBTYPSENDFREQTABLE, 0);
                Sleep(5);
               }
           for(nchEM = 0; nchEM < NOOFMOTORS; nchEM++)
             if(PCFrec.bMotor2ExpectRepliesFrom[nchEM]){
                bWaitingForReply[nchEM] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
                NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETPIXFREQ_TABLE; //1 Second
                RMP_RetCode = ReceiveMessagePackets (hWnd, nchEM, MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDFREQTABLE, 0);
                NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
                if (RMP_RetCode){
                   char szCode[12];
                   StringCchCopy(mes,SIZMES,"Receive Message Packets() at 8 returned code: ");
                   if(RMP_RetCode == 6666)
                     StringCchCat(mes,SIZMES,"\n\nNo Reply");
                   else if(RMP_RetCode == 54321)
                     StringCchCat(mes,SIZMES,"\n\n'chReceiveBuffer[chCardNo][1100] != 0x33' error1");
                   else{
                      _itoa_s(RMP_RetCode, szCode, 12, 10);
                      StringCchCat(mes,SIZMES,szCode);
                     }
                   MessageBox(hWnd,mes,NULL,MB_OK);
                  }
               }
           if(!RMP_RetCode){
              char szRMPsLocalFileName[_MAX_DIR];
              StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
              StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[0]);//From now on, ALL motor's freq data is in 1 file (not 40 seperate files)
              mySave_File (hWnd, 123, "csv",TYPE4CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
              PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR7, SHOWPDIFFFILE);
              bTimerHasBeenKilled = FALSE;  //Resume Timer after Pix Frequency was checked
              if(!SetTimer(hWnd, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
                MessageBox(hWnd,"IDT_TIMER_WNDPROC didn't start @2!",NULL,MB_OK);
             }
           else
             MessageBox(hWnd,"The Update-Screen Timer was not restarted because of an error.\n\nPress 'Auto - Refresh' button if you want to start the Timer again, but\nyou may need to resolve the error first.","Warning",MB_OK);
           break;
         case IDM_PIXFREQINFO:
           MessageBox(hWnd,"Desired    51563,  34375, 17188,  34375\n\nMin           48235,  32157, 16078,  32161\n\nMax          55177,  36745, 18372,  36745","Hardcoded Pixel Freq in AVR are: (10, 15, 30, 60 Hz)",MB_OK);
           break;
         case IDM_SENDpixPOSITIONsLINE:{    //Send Pixel Positions Table (Where motor was during transitions of the pixel signal)
            char pktNo;
            for(pktNo = 0; pktNo < 8; pktNo++ ){
               SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE, SUBTYPSENDPOSLINE, pktNo); //Last param is LineSubPacket
               NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETPIXEL_TABLE_SENDPOSLINE;
               bWaitingForReply[nchCurrentMotorZB] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
               RMP_RetCode = ReceiveMessagePackets (hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDPOSLINE, pktNo);
               NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
               if (RMP_RetCode){
                  char szCode[12];
                  StringCchCopy(mes,SIZMES,"Receive Message Packets() at 9 returned code: ");
                  if(RMP_RetCode == 6666)
                    StringCchCat(mes,SIZMES,"\n\nNo Reply");
                  else if(RMP_RetCode == 54321)
                    StringCchCat(mes,SIZMES,"\n\nSequence number error");
                  else{
                     _itoa_s(RMP_RetCode, szCode, 12, 10);
                     StringCchCat(mes,SIZMES,szCode);
                    }
                  MessageBox(hWnd,mes,NULL,MB_OK);
                  pktNo = 9; //break out of the pktNo loop
                 }
              }
            if(!RMP_RetCode)
              PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR1, SHOWPPOSLINE);
           }
           break;
         case IDM_SENDpixDIFFERENCEsLINE:{    //Send Pixel Positions Table (Where motor was during transitions of the pixel signal)
            char pktNo;
            for(pktNo = 0; pktNo < 4; pktNo++ ){
               SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE, SUBTYPSENDDIFFSLINE, pktNo); // 139 + 1.  Last param is LineSubPacket
             //myWaitForAVRdelay(1000000); //we need to pause before AVR will return the packet
               NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSLINE;
               bWaitingForReply[nchCurrentMotorZB] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
               RMP_RetCode = ReceiveMessagePackets (hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDDIFFSLINE, pktNo);
               NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
               if (RMP_RetCode){
                  char szCode[12];
                  StringCchCopy(mes,SIZMES,"Receive Message Packets() at 10 returned code: ");
                  if(RMP_RetCode == 6666)
                    StringCchCat(mes,SIZMES,"\n\nNo Reply");
                  else if(RMP_RetCode == 54321)
                    StringCchCat(mes,SIZMES,"\n\nSequence number error");
                  else{
                     _itoa_s(RMP_RetCode, szCode, 12, 10);
                     StringCchCat(mes,SIZMES,szCode);
                    }
                  MessageBox(hWnd,mes,NULL,MB_OK);
                  pktNo = 5; //break out of the pktNo loop
                 }
              }
            if(!RMP_RetCode)
              PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR2, SHOWPDIFFLINE);
           }
           break;
         case IDM_SENDpixDIFFERENCEsFILE:{    //Send Pixel Positions Table (Where motor was during transitions of the pixel signal)
          //if(CreateTimeStampedFolder())
          //  break;
          //For a single file I won't create a time stamped folder, but instead store the file in CSVsOUT. This will be overwritten every time it is downloaded
          // but this is more convenient for when setting  the printer.
            StringCchCopy(HCFcfs.szTimeStampedFolderName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);

            SendMessagePackets(hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE, SUBTYPSENDDIFFSFILE, 0); // 139 + 2.  Last param is LineSubPacket
/*
            //Create a TimeStamped Folder to store all the files in
            //Also need to record first and last line numbers
            // -Only download dynamic files
            // -When User restarts printing,  prompt for saving
            struct tm newtime;
            __time32_t aclock;
            char buffer[32];
            _time32( &aclock );                  // Get time in seconds.
            _localtime32_s( &newtime, &aclock ); // Convert time to struct tm form.
            errno_t errNum = asctime_s(buffer, 32, &newtime);
            if(errNum) {
               char szCode[12];
               StringCchCopy(mes,SIZMES,"... asctime_S() gave error code: ");
               _itoa_s((int)errNum, szCode, 12, 10);
               StringCchCat(mes,SIZMES,szCode);
               MessageBox(hWnd,mes,"Cannot create a time-stamped Folder because...",MB_OK);
               break;
              }
            buffer[20] = 0;//Truncated the Date to remove the year part, also we'll ignore the first 4 chars (to remove the day part)
            buffer[13] = '.';//Colons are illegal characters for a folder name
            buffer[16] = '.';
            buffer[19] = '\\';
            StringCchCopy(HCFcfs.szTimeStampedFolderName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
            StringCchCat(HCFcfs.szTimeStampedFolderName,_MAX_DIR,buffer+4);
            errno_t err;
            if( _mkdir(HCFcfs.szTimeStampedFolderName) ){
               _get_errno( &err );
               char ErrorMess[80] = "Error - Unable to create the sub-folder (named above).\nError Code: ";
               _itoa_s(err,ErrorMess+67,12,10);
               MessageBox (hWnd,ErrorMess,HCFcfs.szTimeStampedFolderName,MB_OK);
               break;
              }
*/
            bWaitingForReply[nchCurrentMotorZB] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
            NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSFILE;
            RMP_RetCode = ReceiveMessagePackets (hWnd, nchCurrentMotorZB, MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDDIFFSFILE, 8000);
            NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
            if (RMP_RetCode){
               char szCode[12];
               StringCchCopy(mes,SIZMES,"Receive Message Packets() at 11 returned code: ");
               if(RMP_RetCode == 6666)
                 StringCchCat(mes,SIZMES,"\n\nNo Reply");
               else if(RMP_RetCode == 54321)
                 StringCchCat(mes,SIZMES,"\n\nSequence number error");
               else{
                  _itoa_s(RMP_RetCode, szCode, 12, 10);
                  StringCchCat(mes,SIZMES,szCode);
                 }
               MessageBox(hWnd,mes,NULL,MB_OK);
              }

            if(!RMP_RetCode  && !bSaveDiffFileAsBinary) //When Positioning Error Files are saved as binary, don't open in an editor
              PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR3, SHOWPDIFFFILE);
            bSaveDiffFileAsBinary = 1;
           }
           break;
         case IDM_SEND_ALLpixDIFFERENCEsFILEs:{ //Send Pixel Positions Table (Where motor was during transitions of the pixel signal)
            bTimerHasBeenKilled = TRUE; //Important to do this, it seems //Temporary Suspension while Diffs File is collected
            KillTimer(hWnd, IDT_TIMER_WNDPROC); //in IDM_SEND_ALLpixDIFFERENCEsFILEs
            char nchMotNo;
            for(nchMotNo = 0; nchMotNo < NOOFMOTORS; nchMotNo++)
              if(PCFrec.bMotor2ExpectRepliesFrom[nchMotNo])
                SendMessagePackets(hWnd, nchMotNo, MsTyp9_SENDpixPOS_TABLE, SUBTYPSENDDIFFSFILE, 0); // 139 + 2.  Last param is LineSubPacket

            if(CreateTimeStampedFolder())
              break;
/*
            //Create a TimeStamped Folder to store all the files in
            //Also need to record first and last line numbers
            // -Only download dynamic files
            // -When User restarts printing,  prompt for saving
            struct tm newtime;
            __time32_t aclock;
            char buffer[32];
            _time32( &aclock );                  // Get time in seconds.
            _localtime32_s( &newtime, &aclock ); // Convert time to struct tm form.
            errno_t errNum = asctime_s(buffer, 32, &newtime);
            if(errNum) {
               char szCode[12];
               StringCchCopy(mes,SIZMES,"... asctime_S() gave error code: ");
               _itoa_s((int)errNum, szCode, 12, 10);
               StringCchCat(mes,SIZMES,szCode);
               MessageBox(hWnd,mes,"Cannot create a time-stamped Folder because...",MB_OK);
               break;
              }
            buffer[20] = 0;//Truncated the Date to remove the year part, also we'll ignore the first 4 chars (to remove the day part)
            buffer[13] = '.';//Colons are illegal characters for a folder name
            buffer[16] = '.';
            buffer[19] = '\\';
            StringCchCopy(HCFcfs.szTimeStampedFolderName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
            StringCchCat(HCFcfs.szTimeStampedFolderName,_MAX_DIR,buffer+4);
            errno_t err;
            if( _mkdir(HCFcfs.szTimeStampedFolderName) ){
               _get_errno( &err );
               char ErrorMess[80] = "Error - Unable to create the sub-folder (named above).\nError Code: ";
               _itoa_s(err,ErrorMess+67,12,10);
               MessageBox (hWnd,ErrorMess,HCFcfs.szTimeStampedFolderName,MB_OK);
               break;
              }
*/
            //bWaitingForReply[35] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
            for(nchMotNo = 0; nchMotNo < NOOFMOTORS; nchMotNo++)
              if(PCFrec.bMotor2ExpectRepliesFrom[nchMotNo]){
                 bWaitingForReply[nchMotNo] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
                 NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETPIXEL_TABLE_SENDDIFFSFILE;
                 RMP_RetCode = ReceiveMessagePackets (hWnd, (int)nchMotNo, MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDDIFFSFILE, 8000);
                 NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
                 if (RMP_RetCode){
                    char szCode[12];
                    StringCchCopy(mes,SIZMES,"Receive Message Packets() at 11a returned code: ");
                    if(RMP_RetCode == 6666)
                      StringCchCat(mes,SIZMES,"\n\nNo Reply");
                    else if(RMP_RetCode == 54321)
                      StringCchCat(mes,SIZMES,"\n\nSequence number error");
                    else{
                       _itoa_s(RMP_RetCode, szCode, 12, 10);
                       StringCchCat(mes,SIZMES,szCode);
                      }
                    MessageBox(hWnd,mes,NULL,MB_OK);
                   }
              }
//I dont know about opening all files in an editor... I doubt it is desirable or required.  It would be safe to open just the nchCurrentMotorZB's file
// if we get here from the Transfer Block screen, but possibly not if we're simply at the end of a hologram or saving during an interrupted hologram.
            //if(!RMP_RetCode  && !bSaveDiffFileAsBinary) //When Positioning Error Files are saved as binary, don't open in an editor
            //  PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR3, SHOWPDIFFFILE);
            bSaveDiffFileAsBinary = 1;
            bTimerHasBeenKilled = FALSE; //Resume Timer after Diffs File was collected
            if(!SetTimer(hWnd, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
              MessageBox(hWnd,"IDT_TIMER_WNDPROC didn't start @3!",NULL,MB_OK);
           }
           break;
//Above messages SendMessagePackets, the following run external programs (eg terminal, UltraEdit):
/*       case IDPOSTEDBRINGEDITOR2TOP:{
            BringWindowToTop(FindWindow(NULL,"UltraEdit-32"));
            BringWindowToTop(hWnd);
           }
           break;*/
         case IDM_EXEC_EDITOR1:
         case IDM_EXEC_EDITOR2:
         case IDM_EXEC_EDITOR3:
         case IDM_EXEC_EDITOR4:
         case IDM_EXEC_EDITOR5:
         case IDM_EXEC_EDITOR6:
         case IDM_EXEC_EDITOR7:{
            char szCsvOutlFileName[_MAX_DIR];
            //char szCurrentMotorDotCSV[5] = ".CSV";
            StringCchCopy(szCsvOutlFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);

            if(wmId == IDM_EXEC_EDITOR1)       //(lParam == SHOWPPOSLINE)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPPOSLINE[nchCurrentMotorZB]);
            else if(wmId == IDM_EXEC_EDITOR2)  //(lParam == SHOWPDIFFSLINE)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPDIFFLINE[nchCurrentMotorZB]); 
            else if(wmId == IDM_EXEC_EDITOR3){  //(lParam == SHOWPDIFFFILE) (csv)
              //StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPDIFFFILE[nchCurrentMotorZB]);
               StringCchCopy(szCsvOutlFileName,_MAX_DIR,HCFcfs.szTimeStampedFolderName);
               StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPDIFFFILE[nchCurrentMotorZB]);
              }
            else if(wmId == IDM_EXEC_EDITOR4)  //(lParam == SHOWPDIFFFILE) (bin)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_BIN_OUTFILENAMEPDIFFFILE[nchCurrentMotorZB]);
            else if(wmId == IDM_EXEC_EDITOR5)  //(lParam == SHOWPPOSLINE)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,DEFAULT_BIN_OUTFILENAMEENERGIES_READ);
            else if(wmId == IDM_EXEC_EDITOR6)  //(lParam == SHOWPPOSLINE)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,DEFAULT_CSV_OUTFILENAMEENERGIES_READ);
            else if(wmId == IDM_EXEC_EDITOR7)  //(lParam == SHOWPPOSLINE)
              StringCchCat(szCsvOutlFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[0]); //From now on, ALL motor's freq data is in 1 file (not 40 seperate files)
//          else{
//             MessageBox (hWnd,"IDM_EXEC_EDITOR had invalid wParam",NULL,MB_OK);
//             break;
//            }
            DisplayFileInEditor(hWnd, szCsvOutlFileName);
           }
           break;               //If the filename for DisplayFileInEditor() has ':\' as the 2nd and 3rd bytes the name is assumed to be fully qualified, otherwise
         case IDM_VIEWSOMEHELP: //the file is assumed to be in the same folder as holoprog.exe (or a subfolder of this folder), like this:
           DisplayFileInEditor(hWnd, "SomeHelp.txt");
           break;
         case IDM_EXEC_TERMINAL:{
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNOACTIVATE;
            ZeroMemory( &pi, sizeof(pi) );
            if( !CreateProcess( "C:\\Program Files\\Terminal\\Terminal.exe", //Hard-Coded for my machine
                   NULL,           // Command line
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   0,              // No creation flags
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &si,            // Pointer to STARTUPINFO structure
                   &pi )) {        // Pointer to PROCESS_INFORMATION structure
               //GetLastError();
               MessageBox (hWnd,"CreateProcess Failed (C:\\Program Files\\Terminal\\Terminal.exe)",NULL,MB_OK);
              }
            PostMessage(hWnd, WM_COMMAND, IDPOSTEDBRINGTERMINAL2TOP, 0x0);
           }
           break;
         case IDM_EXEC_EXPLORER:{
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNOACTIVATE;
            ZeroMemory( &pi, sizeof(pi) );
            if( !CreateProcess( "C:\\Windows\\explorer.exe", //Hard-Coded for my machine
                   //"e, C:\\HGrams\\test30HzA",           //e.g. Command line
                   szExplorerDir2StartIn, //Note the "e, " must be prepended
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   0,              // No creation flags
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &si,            // Pointer to STARTUPINFO structure
                   &pi )) {        // Pointer to PROCESS_INFORMATION structure
               //GetLastError();
               MessageBox (hWnd,"CreateProcess Failed (C:\\Windows\\explorer.exe)",NULL,MB_OK);
              }
            PostMessage(hWnd, WM_COMMAND, IDPOSTEDBRINGTERMINAL2TOP, 0x0);
           }
           break;
         case IDM_EXEC_HOLOPROGHELPER:{
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNOACTIVATE;
            ZeroMemory( &pi, sizeof(pi) );
            if( !CreateProcess( szHoloProgHelperFullFileName, //Hard-Coded for my machine
                   NULL,
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   0,              // No creation flags
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &si,            // Pointer to STARTUPINFO structure
                   &pi )) {        // Pointer to PROCESS_INFORMATION structure
               //GetLastError();
               MessageBox (hWnd,"CreateProcess Failed (HoloProgHelper)",NULL,MB_OK);
              }
            //PostMessage(hWnd, WM_COMMAND, IDPOSTEDBRINGTERMINAL2TOP, 0x0);
           }
           break;
         case IDM_EXEC_PIDHELPER_PROG:{
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNOACTIVATE;
            ZeroMemory( &pi, sizeof(pi) );
            if( !CreateProcess( szHoloPIDHelperFullFileName, //Hard-Coded for my machine
                   NULL,
                   NULL,           // Process handle not inheritable
                   NULL,           // Thread handle not inheritable
                   FALSE,          // Set handle inheritance to FALSE
                   0,              // No creation flags
                   NULL,           // Use parent's environment block
                   NULL,           // Use parent's starting directory
                   &si,            // Pointer to STARTUPINFO structure
                   &pi )) {        // Pointer to PROCESS_INFORMATION structure
               //GetLastError();
               MessageBox (hWnd,"CreateProcess Failed (HoloPIDHelper)",NULL,MB_OK);
              }
            //PostMessage(hWnd, WM_COMMAND, IDPOSTEDBRINGTERMINAL2TOP, 0x0);
           }
           break;
         case IDPOSTEDBRINGTERMINAL2TOP:{
            BringWindowToTop(FindWindow(NULL,"Terminal"));
            BringWindowToTop(hWnd);
           }
           break;
         case IDM_CLEAR_TERMINAL_WINDOW:{
            SendMessage(FindWindow(NULL,"Terminal"), WM_COMMAND, 0x9c4d, 0x0);
            PostMessage(hWnd, WM_COMMAND, IDPOSTEDBRINGTERMINAL2TOP, 0x0);
           }
           break;
   //    case ID_NON_FOCUSABLE_WINDOW:         //Any Controls on the main window that is not allow to have the focus (because when they
   //      SetFocus(hFocusButton);             // (because when they do, the keyboard accelerators stop working)
   //      break;
         case IDB_DISPLAY_PID_DEBUG_MSGs:
           bDisplayPIDdebugMsgs = !bDisplayPIDdebugMsgs;
           bregDisplayPIDdebugMsgs = bDisplayPIDdebugMsgs;
           if( !bDisplayPIDdebugMsgs )
             redrawPIDrect();
           SetFocus(hFocusButton);
           break;
         case IDB_STATREC_REFRESH:             //The 'Refresh Once' button has been pressed
           bTimerHasBeenKilled = TRUE;  //Killed by User pressing 'Refresh Once'
           KillTimer(hWnd, IDT_TIMER_WNDPROC); // in IDB_STATREC_REFRESH
         //for(int MotorNo = 0; MotorNo < NOOFMOTORS; MotorNo++)
         //  U pdateMainWindowsControlsM(hWnd, MotorNo);
           UpdateMainWindowsControlsM();
           SetFocus(hFocusButton);
           break;
//       case IDB_STATREC_REFRESH_NOKILLTIMER: //Sent repeatedly by the timer (So doesn't kill the timer)... There is no button with this ID/
//         if(!b TimerHasBeenKilled){
//            for(int MotorNo = 0; MotorNo < NOOFMOTORS; MotorNo++)
//              U pdateMainWindowsControlsM(hWnd, MotorNo);
//            if(!S etTimer(hWnd, I DT_TIMER1, A UTOREFRESHRATE, (TIMERPROC) NULL))
//              MessageBox(hWnd,"STATREC_AUTO_TIMER problem!",NULL,MB_OK);
//           }
//         break;
         case IDB_STATREC_AUTOREFRESH:
           bTimerHasBeenKilled = FALSE;      //Resumed by User pressing IDB_STATREC_AUTOREFRESH
           if(!SetTimer(hWnd, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
             MessageBox(hWnd,"IDT_TIMER_WNDPROC didn't start @4!",NULL,MB_OK);
           SetFocus(hFocusButton);
           break;
         case IDB_STATREC_DRIVER_ENAB:   case IDB_STATREC_DRIVER_ENAB2:  case IDB_STATREC_DRIVER_ENAB3:  case IDB_STATREC_DRIVER_ENAB4:  case IDB_STATREC_DRIVER_ENAB5:
         case IDB_STATREC_DRIVER_ENAB6:  case IDB_STATREC_DRIVER_ENAB7:  case IDB_STATREC_DRIVER_ENAB8:  case IDB_STATREC_DRIVER_ENAB9:  case IDB_STATREC_DRIVER_ENAB10:
         case IDB_STATREC_DRIVER_ENAB11: case IDB_STATREC_DRIVER_ENAB12: case IDB_STATREC_DRIVER_ENAB13: case IDB_STATREC_DRIVER_ENAB14: case IDB_STATREC_DRIVER_ENAB15:
         case IDB_STATREC_DRIVER_ENAB16: case IDB_STATREC_DRIVER_ENAB17: case IDB_STATREC_DRIVER_ENAB18: case IDB_STATREC_DRIVER_ENAB19: case IDB_STATREC_DRIVER_ENAB20:
         case IDB_STATREC_DRIVER_ENAB21: case IDB_STATREC_DRIVER_ENAB22: case IDB_STATREC_DRIVER_ENAB23: case IDB_STATREC_DRIVER_ENAB24: case IDB_STATREC_DRIVER_ENAB25:
         case IDB_STATREC_DRIVER_ENAB26: case IDB_STATREC_DRIVER_ENAB27: case IDB_STATREC_DRIVER_ENAB28: case IDB_STATREC_DRIVER_ENAB29: case IDB_STATREC_DRIVER_ENAB30:
         case IDB_STATREC_DRIVER_ENAB31: case IDB_STATREC_DRIVER_ENAB32: case IDB_STATREC_DRIVER_ENAB33: case IDB_STATREC_DRIVER_ENAB34: case IDB_STATREC_DRIVER_ENAB35:
         case IDB_STATREC_DRIVER_ENAB36: case IDB_STATREC_DRIVER_ENAB37: case IDB_STATREC_DRIVER_ENAB38: case IDB_STATREC_DRIVER_ENAB39: case IDB_STATREC_DRIVER_ENAB40:{
            #define SIZITEMTEXT 4
            if(HIWORD(wParam) == EN_CHANGE){

               int nControlActivated = LOWORD(wParam) - IDB_STATREC_DRIVER_ENAB;
               if( (nControlActivated < 0) || (nControlActivated > 39) ){//Checks the values defined in resource.h are contiguous
                  MessageBox(hWnd,"nControlActivated out of bounds - Programming error5!",NULL,MB_OK);
                  break;
                 }

               char szItemText[SIZITEMTEXT];
               GetDlgItemText(hWnd,LOWORD(wParam),szItemText,SIZITEMTEXT);
               if( (szItemText[0] == 'E') || (szItemText[0] == 'e') ){
                  if((nControlActivated == 1)||(nControlActivated == 2)||(nControlActivated == 13)||(nControlActivated == 14)||(nControlActivated == 25)||(nControlActivated == 26)){
                     MessageBox(hWnd,"The safest way to enable the PID motors is  by pressing the 'Enable All Drives' button","Warning - the system did not enable the motor...",MB_OK);
                    }
                  else{
                     SendMessagePackets(hWnd, (char)nControlActivated, MsTyp6_ENAB_DISAB,  ENAB_POWR, 0);
                     SetFocus(hFocusButton);
                    }
                 }
               else if( (szItemText[0] == 'D') || (szItemText[0] == 'd') ){
                  SendMessagePackets(hWnd, (char)nControlActivated, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0);
                  SetFocus(hFocusButton);
                 }
              }
           }
           break;
         case IDB_STATREC_DRIVER_ENABall:{
            char nchEM_SR5;
            //PidStructVariableWP2Red.iState = 0;
            //PidStructVariableWP3Red.iState = 0;
            //PidStructVariableWP2Gre.iState = 0;
            //PidStructVariableWP3Gre.iState = 0;
            //PidStructVariableWP2Blu.iState = 0;
            //PidStructVariableWP3Blu.iState = 0;
            //b StopPIDsendingMessages = TRUE;
            //b FirstTime = 1;
            //b PIDMovementStoppedForHoming = FALSE;
            restartPIDs(); //does the above

            for(nchEM_SR5 = 0; nchEM_SR5 < NOOFMOTORS; nchEM_SR5++){
               if(PCFrec.bMotor2ExpectRepliesFrom[nchEM_SR5]){
                  if( SendMessagePackets(hWnd, nchEM_SR5, MsTyp6_ENAB_DISAB,  ENAB_POWR, 0) )
                    nchEM_SR5 = 126;
                  Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
                 }
              }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR5 > 125)
              MessageBox(hWnd,"As a message to ENABLE one of the motors failed,\nI didn't send the ENABLE message to other motors.\n\nThis may mean that some of (or all) the motors are not ENABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_DRIVER_DISABall:{
            char nchEM_SR6;
            bStopPIDsendingMessages = TRUE;
            for(nchEM_SR6 = 0; nchEM_SR6 < NOOFMOTORS; nchEM_SR6++){ //No harm in sending this message to motors that aren't present according to pcf file
               if( SendMessagePackets(hWnd, nchEM_SR6, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0) )
                 nchEM_SR6 = 126;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR6 > 125)
              MessageBox(hWnd,"As a message to DISABLE one of the motors failed,\nI didn't send the DISABLE message to other motors.\n\nThis may mean that some of (or all) the motors are not DISABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_DRIVER_DISAB_PIDS:{
            char nchEM_SR6;
            bStopPIDsendingMessages = TRUE;
            for(nchEM_SR6 = 0; nchEM_SR6 < NOOFMOTORS; nchEM_SR6++){ //No harm in sending this message to motors that aren't present according to pcf file
               if( PCFrec.bMotorIS_A_PID[nchEM_SR6] && SendMessagePackets(hWnd, nchEM_SR6, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0) )
                 nchEM_SR6 = 126;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR6 > 125)
              MessageBox(hWnd,"As a message to DISABLE one of the PID motors failed,\nI didn't send the DISABLE message to the other PID motors.\n\nThis may mean that some of (or all) the PID motors are not DISABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RS485_ENAB:    case IDB_STATREC_RS485_ENAB2:   case IDB_STATREC_RS485_ENAB3:   case IDB_STATREC_RS485_ENAB4:   case IDB_STATREC_RS485_ENAB5:
         case IDB_STATREC_RS485_ENAB6:   case IDB_STATREC_RS485_ENAB7:   case IDB_STATREC_RS485_ENAB8:   case IDB_STATREC_RS485_ENAB9:   case IDB_STATREC_RS485_ENAB10:
         case IDB_STATREC_RS485_ENAB11:  case IDB_STATREC_RS485_ENAB12:  case IDB_STATREC_RS485_ENAB13:  case IDB_STATREC_RS485_ENAB14:  case IDB_STATREC_RS485_ENAB15:
         case IDB_STATREC_RS485_ENAB16:  case IDB_STATREC_RS485_ENAB17:  case IDB_STATREC_RS485_ENAB18:  case IDB_STATREC_RS485_ENAB19:  case IDB_STATREC_RS485_ENAB20:
         case IDB_STATREC_RS485_ENAB21:  case IDB_STATREC_RS485_ENAB22:  case IDB_STATREC_RS485_ENAB23:  case IDB_STATREC_RS485_ENAB24:  case IDB_STATREC_RS485_ENAB25:
         case IDB_STATREC_RS485_ENAB26:  case IDB_STATREC_RS485_ENAB27:  case IDB_STATREC_RS485_ENAB28:  case IDB_STATREC_RS485_ENAB29:  case IDB_STATREC_RS485_ENAB30:
         case IDB_STATREC_RS485_ENAB31:  case IDB_STATREC_RS485_ENAB32:  case IDB_STATREC_RS485_ENAB33:  case IDB_STATREC_RS485_ENAB34:  case IDB_STATREC_RS485_ENAB35:
         case IDB_STATREC_RS485_ENAB36:  case IDB_STATREC_RS485_ENAB37:  case IDB_STATREC_RS485_ENAB38:  case IDB_STATREC_RS485_ENAB39:  case IDB_STATREC_RS485_ENAB40:{
            #define SIZITEMTEXT 4
            if(HIWORD(wParam) == EN_CHANGE){
               char nchEM_SR1;
               char nchSelctdMotor;// = (char)(LOWORD(wParam) - IDB_STATREC_RS485_ENAB);

               int nControlActivated = LOWORD(wParam) - IDB_STATREC_RS485_ENAB;
               if( (nControlActivated < 0) || (nControlActivated > 39) ){//Detects if the values defined in resource.h aren't contiguous
                  MessageBox(hWnd,"nControlActivated out of bounds - Programming error6!",NULL,MB_OK);
                  break;
                 }
               else
                 nchSelctdMotor = (char)nControlActivated;

               char szItemText[SIZITEMTEXT];
               GetDlgItemText(hWnd,LOWORD(wParam),szItemText,SIZITEMTEXT);
             //if(szItemText[0] == '4'){
               if( (szItemText[0] == 'E') || (szItemText[0] == 'e') ){
                  if(szItemText[0] == 'e'){ //For lower case enabling of RS485, we disable RS485 for all the other motor, so only 1 motor is RS485 ACTIVE
                     for(nchEM_SR1 = 0; nchEM_SR1 < NOOFMOTORS; nchEM_SR1++){
                        if(nchEM_SR1 != nchSelctdMotor) //Don't try to disable the selected motor... aftr all we are trying to enable it!
                          if( SendMessagePackets(hWnd, nchEM_SR1, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
                            nchEM_SR1 = 126;
                        Sleep(5);
                       }
                     if(nchEM_SR1 > 125)
                       MessageBox(hWnd,"Normally, before enabling RS485 for the selected motor, 'Disable RS485' messages\nare sent to all the other motors.\n\nOne of these 'Disable RS485' messages failed (on this occassion) so I gave up on\nsending this message to the other motors. (Suggest you view the Status Screen)\n\nThis may mean the is contention for the RS485 bus.\n\nWhen you press 'OK' I will still send the 'Enable RS485' message to the current motor","Warning - contention for the RS485 bus is a possibility",MB_OK);
                     SendMessagePackets(hWnd, nchSelctdMotor, MsTyp6_ENAB_DISAB,   ENAB_485, 0); //Enable
                    }
                  else //For Upper case enabling of RS485, MANY MOTORS ARE ALLOWED TO BE RS485 ACTIVE (*** which is likely to cause bus contention ***)
                    SendMessagePackets(hWnd, nchSelctdMotor, MsTyp6_ENAB_DISAB,   ENAB_485, 0);
                  SetFocus(hFocusButton);
                 }
             //else if(szItemText[0] == '5'){
               else if( (szItemText[0] == 'D') || (szItemText[0] == 'd') ){
                  SendMessagePackets(hWnd, nchSelctdMotor, MsTyp6_ENAB_DISAB,  DISAB_485, 0);
                  SetFocus(hFocusButton);
                 }
              }
           }
           break;
         case IDB_STATREC_RS485_ENABall:{
            char nchEM_SR2;
            for(nchEM_SR2 = 0; nchEM_SR2 < NOOFMOTORS; nchEM_SR2++)            
              if(PCFrec.bMotor2ExpectRepliesFrom[nchEM_SR2]){
                 if( SendMessagePackets(hWnd, nchEM_SR2, MsTyp6_ENAB_DISAB,  ENAB_485, 0) )
                   nchEM_SR2 =  126;
                 Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
                }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR2 > 125)
              MessageBox(hWnd,"As a message to ENABLE RS485 on one of the motors failed,\nI didn't send the ENABLE RS485 message to other motors.\n\nThis may mean that some of (or all) the motors are not RS485 ENABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RS485_DISABall:{
            char nchEM_SR3;
            for(nchEM_SR3 = 0; nchEM_SR3 < NOOFMOTORS; nchEM_SR3++){ //No harm in sending this message to motors that aren't present according to pcf file
               if( SendMessagePackets(hWnd, nchEM_SR3, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
                 nchEM_SR3 =  126;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR3 > 125)
              MessageBox(hWnd,"As a message to DISABLE RS485 on one of the motors failed,\nI didn't send the DISABLE RS485 message to other motors.\n\nThis may mean that some of (or all) the motors are not RS485 DISABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RESETm01:  case IDB_STATREC_RESETm02:  case IDB_STATREC_RESETm03:  case IDB_STATREC_RESETm04:  case IDB_STATREC_RESETm05:
         case IDB_STATREC_RESETm06:  case IDB_STATREC_RESETm07:  case IDB_STATREC_RESETm08:  case IDB_STATREC_RESETm09:  case IDB_STATREC_RESETm10:
         case IDB_STATREC_RESETm11:  case IDB_STATREC_RESETm12:  case IDB_STATREC_RESETm13:  case IDB_STATREC_RESETm14:  case IDB_STATREC_RESETm15:
         case IDB_STATREC_RESETm16:  case IDB_STATREC_RESETm17:  case IDB_STATREC_RESETm18:  case IDB_STATREC_RESETm19:  case IDB_STATREC_RESETm20:
         case IDB_STATREC_RESETm21:  case IDB_STATREC_RESETm22:  case IDB_STATREC_RESETm23:  case IDB_STATREC_RESETm24:  case IDB_STATREC_RESETm25:
         case IDB_STATREC_RESETm26:  case IDB_STATREC_RESETm27:  case IDB_STATREC_RESETm28:  case IDB_STATREC_RESETm29:  case IDB_STATREC_RESETm30:
         case IDB_STATREC_RESETm31:  case IDB_STATREC_RESETm32:  case IDB_STATREC_RESETm33:  case IDB_STATREC_RESETm34:  case IDB_STATREC_RESETm35:
         case IDB_STATREC_RESETm36:  case IDB_STATREC_RESETm37:  case IDB_STATREC_RESETm38:  case IDB_STATREC_RESETm39:  case IDB_STATREC_RESETm40:{
            char nchSelctdMotor;
            int nControlActivated = LOWORD(wParam) - IDB_STATREC_RESETm01;
            if( (nControlActivated < 0) || (nControlActivated > 39) ){//Detects if the values defined in resource.h aren't contiguous
               MessageBox(hWnd,"nControlActivated out of bounds - Programming error7!",NULL,MB_OK);
               break;
              }
            else
              nchSelctdMotor = (char)nControlActivated;

            SendMessagePackets(hWnd, nchSelctdMotor, MsTyp4_RESET, 0, 0);   //0x04 (RESET Message)
            SetFocus(hFocusButton);
           }
           break;
         case IDB_STATREC_SINGMMm01: case IDB_STATREC_SINGMMm02: case IDB_STATREC_SINGMMm03: case IDB_STATREC_SINGMMm04: case IDB_STATREC_SINGMMm05:
         case IDB_STATREC_SINGMMm06: case IDB_STATREC_SINGMMm07: case IDB_STATREC_SINGMMm08: case IDB_STATREC_SINGMMm09: case IDB_STATREC_SINGMMm10:
         case IDB_STATREC_SINGMMm11: case IDB_STATREC_SINGMMm12: case IDB_STATREC_SINGMMm13: case IDB_STATREC_SINGMMm14: case IDB_STATREC_SINGMMm15:
         case IDB_STATREC_SINGMMm16: case IDB_STATREC_SINGMMm17: case IDB_STATREC_SINGMMm18: case IDB_STATREC_SINGMMm19: case IDB_STATREC_SINGMMm20:
         case IDB_STATREC_SINGMMm21: case IDB_STATREC_SINGMMm22: case IDB_STATREC_SINGMMm23: case IDB_STATREC_SINGMMm24: case IDB_STATREC_SINGMMm25:
         case IDB_STATREC_SINGMMm26: case IDB_STATREC_SINGMMm27: case IDB_STATREC_SINGMMm28: case IDB_STATREC_SINGMMm29: case IDB_STATREC_SINGMMm30:
         case IDB_STATREC_SINGMMm31: case IDB_STATREC_SINGMMm32: case IDB_STATREC_SINGMMm33: case IDB_STATREC_SINGMMm34: case IDB_STATREC_SINGMMm35:
         case IDB_STATREC_SINGMMm36: case IDB_STATREC_SINGMMm37: case IDB_STATREC_SINGMMm38: case IDB_STATREC_SINGMMm39: case IDB_STATREC_SINGMMm40:{
            int  nMnoZB = LOWORD(wParam) - IDB_STATREC_SINGMMm01;
            char nchMnoZB;
            if((nMnoZB < 0)||(nMnoZB > 39)){
               MessageBox(hWnd,"nMnoZB out of bounds - Programming error1!",NULL,MB_OK);
               break;
              }
            else
              nchMnoZB = (char)nMnoZB;
            if(PCFrec.bMotor2ExpectRepliesFrom[nchMnoZB]){
               nchCurrentMotorZB = nchMnoZB;
               PCFrec.bMotor2ExpectRepliesFrom[nchMnoZB] = 1;
               if((nchMnoZB ==1)||(nchMnoZB ==2)||(nchMnoZB ==13)||(nchMnoZB ==14)||(nchMnoZB ==25)||(nchMnoZB ==26))
                 DialogBox(hInst, MAKEINTRESOURCE(IDD_SINGLEMMPIDBOX), hWnd, SingleMM_PID_Dialog);//Can send 0x07 MsTyp7_SING_MM_OTF and/or MsTyp8_SET_PULSE_LENGTH
               else
                 DialogBox(hInst, MAKEINTRESOURCE(IDD_SINGLEMMBOX), hWnd, SingleMM_Dialog);    //Can send 0x07 MsTyp7_SING_MM_OTF and/or MsTyp8_SET_PULSE_LENGTH
              }
            else
              MessageBoxWarnNotConnected(hWnd);
            SetFocus(hFocusButton);
           }
           break;
         case IDB_STATREC_TRANSBLOCKm01: case IDB_STATREC_TRANSBLOCKm02: case IDB_STATREC_TRANSBLOCKm03: case IDB_STATREC_TRANSBLOCKm04: case IDB_STATREC_TRANSBLOCKm05:
         case IDB_STATREC_TRANSBLOCKm06: case IDB_STATREC_TRANSBLOCKm07: case IDB_STATREC_TRANSBLOCKm08: case IDB_STATREC_TRANSBLOCKm09: case IDB_STATREC_TRANSBLOCKm10:
         case IDB_STATREC_TRANSBLOCKm11: case IDB_STATREC_TRANSBLOCKm12: case IDB_STATREC_TRANSBLOCKm13: case IDB_STATREC_TRANSBLOCKm14: case IDB_STATREC_TRANSBLOCKm15:
         case IDB_STATREC_TRANSBLOCKm16: case IDB_STATREC_TRANSBLOCKm17: case IDB_STATREC_TRANSBLOCKm18: case IDB_STATREC_TRANSBLOCKm19: case IDB_STATREC_TRANSBLOCKm20:
         case IDB_STATREC_TRANSBLOCKm21: case IDB_STATREC_TRANSBLOCKm22: case IDB_STATREC_TRANSBLOCKm23: case IDB_STATREC_TRANSBLOCKm24: case IDB_STATREC_TRANSBLOCKm25:
         case IDB_STATREC_TRANSBLOCKm26: case IDB_STATREC_TRANSBLOCKm27: case IDB_STATREC_TRANSBLOCKm28: case IDB_STATREC_TRANSBLOCKm29: case IDB_STATREC_TRANSBLOCKm30:
         case IDB_STATREC_TRANSBLOCKm31: case IDB_STATREC_TRANSBLOCKm32: case IDB_STATREC_TRANSBLOCKm33: case IDB_STATREC_TRANSBLOCKm34: case IDB_STATREC_TRANSBLOCKm35:
         case IDB_STATREC_TRANSBLOCKm36: case IDB_STATREC_TRANSBLOCKm37: case IDB_STATREC_TRANSBLOCKm38: case IDB_STATREC_TRANSBLOCKm39: case IDB_STATREC_TRANSBLOCKm40:{
            int nMnoZB = LOWORD(wParam) - IDB_STATREC_TRANSBLOCKm01;
            char nchMnoZB;
            if((nMnoZB < 0)||(nMnoZB > 39)){
               MessageBox(hWnd,"nMnoZB out of bounds - Programming error2!",NULL,MB_OK);
               break;
              }
            else
              nchMnoZB = (char)nMnoZB;
            if(PCFrec.bMotor2ExpectRepliesFrom[nchMnoZB]){
               nchCurrentMotorZB = nchMnoZB;
               PCFrec.bMotor2ExpectRepliesFrom[nchMnoZB] = 1;
               DialogBox(hInst, MAKEINTRESOURCE(IDD_TRANSBLOCKBOX), hWnd, TransBlock_Dialog);//Sends M sTyp1_MOTOR_POS_DATA & M sTyp5_SING_MM Jog Minus Fast & M sTyp4_RESET & M sTypE_SET_NEXTLINE2PRINT
              }
            else
              MessageBoxWarnNotConnected(hWnd);//"The Printer Configuration does not show this motor as being connected..."
            SetFocus(hFocusButton);
           }
           break;
         case IDB_STATREC_HELP:
         //MessageBox(hDlg,"This button needs a more informative dialogbox!","Note",MB_OK);
           nHelpMessageToLoad = 1;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hWnd, HelpDialog1_Dialog);
            SetFocus(hFocusButton);
           break;
         case IDM_VIEWSOMEHOTKEYHELP:
           nHelpMessageToLoad = 3;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hWnd, HelpDialog1_Dialog);
           break;
         case IDB_STATREC_HOMEALL:{
#if 0
            //int nEM_SR7, nCurrent_uSteps;
            ////for(nEM_SR7 = 0; nEM_SR7 < NOOFMOTORS; nEM_SR7++){
            //for(nEM_SR7 = 0; nEM_SR7 <29; nEM_SR7++){
            //   nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nEM_SR7];//nCurrent_uSteps can be 1,2,4,8,16,32,or64
            //   if(SendMessagePackets(hDlg,nEM_SR7, MsTyp3_HOME,nCurrent_uSteps/*The constant Slow speed to move at, when homing out*/, nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ) ) //The constant Fast speed to move at, when homing in
            //     nEM_SR7 =  1000;
            //   Sleep(20); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
            //  }
            //SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            //if(nEM_SR7 > 999)
            //  MessageBox(hDlg,"As a message to HOME one of the motors failed,\nI didn't send the HOME message to other motors.\n\nThis may mean that some of (or all) the motors are not HOMED.","Warning",MB_OK);
#endif
            if(MoveToStartPositions(hWnd, /*1,*/ bHIDEMESSAGES, bHOME_ALL_REGARDLESS, bSTOP_WHEN_HOMED))//Really this is MoveToHOMPositions() - because of last parameter. If the function returns TRUE,
              PostMessage(hWnd,WM_COMMAND,IDB_STATREC_HOMEALL,0);// user cleared RS485 output & the clear commands were sent. Here, PostMessage starts the function afresh.
            else
              SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
           }
           break;
         case IDB_STATREC_RESETmALL:{
            char nchEM_SR4;
            for(nchEM_SR4 = 0; nchEM_SR4 < NOOFMOTORS; nchEM_SR4++){ //No harm in sending this message to motors that aren't present according to pcf file
               if( SendMessagePackets(hWnd, nchEM_SR4, MsTyp4_RESET, 0, 0) )
                 nchEM_SR4 =  126;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hFocusButton);//This button, being small, looks unsightly if focused
            if(nchEM_SR4 > 125)
              MessageBox(hWnd,"As a message to RESET one of the motors failed,\nI didn't send the RESET message to other motors.\n\nThis may mean that some of (or all) the motors are not RESET.","Warning",MB_OK);
           }
           break;
         case IDM_EXIT:
           DestroyWindow(hWnd);
           break;
         default:
           return DefWindowProc(hWnd, message, wParam, lParam);
        }
      break;
    case WM_INJECT_FROM_PID_HELPERPROG:{ //We must read new PID gains from the registry
      //RefreshThePIDgains();
       #define SIZpidSTRING 50
       char szPIDvalInRegistry[SIZpidSTRING];
       BOOL bAPidValueCouldNotBeRead = 0;

       creg_str_PID_WP2_R_pGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_R_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Red.pGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_G_pGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_G_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Gre.pGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_B_pGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_B_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Blu.pGain = atof(szPIDvalInRegistry);
//////////
       creg_str_PID_WP2_R_iGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_R_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Red.iGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_G_iGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_G_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Gre.iGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_B_iGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_B_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Blu.iGain = atof(szPIDvalInRegistry);
//////////
       creg_str_PID_WP2_R_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_R_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Red.dGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_G_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_G_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Gre.dGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP2_B_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP2_B_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP2Blu.dGain = atof(szPIDvalInRegistry);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
       creg_str_PID_WP3_R_pGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_R_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Red.pGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_G_pGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_G_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Gre.pGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_B_pGAIN.read();                                 
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_B_pGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Blu.pGain = atof(szPIDvalInRegistry);
//////////
       creg_str_PID_WP3_R_iGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_R_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Red.iGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_G_iGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_G_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Gre.iGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_B_iGAIN.read();                                 
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_B_iGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Blu.iGain = atof(szPIDvalInRegistry);
//////////
       creg_str_PID_WP3_R_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_R_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Red.dGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_G_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_G_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Gre.dGain = atof(szPIDvalInRegistry);

       creg_str_PID_WP3_B_dGAIN.read();
       StringCchCopy(szPIDvalInRegistry, SIZpidSTRING, stdstring(creg_str_PID_WP3_B_dGAIN).c_str() );
       if( !lstrcmp(szPIDvalInRegistry, "unset") )
         bAPidValueCouldNotBeRead = 1;
       else
         PidStructVariableWP3Blu.dGain = atof(szPIDvalInRegistry);
       if( bAPidValueCouldNotBeRead ){
          MessageBox(hWnd, "At least one 'PID-gain' in the Registry could not be read...\nSo ALL 'PID-gains' are being reset to their default values", "Warning", MB_OK);
          creg_str_PID_WP2_R_pGAIN = "0.15";               //WP2s
          creg_str_PID_WP2_G_pGAIN = "0.15";
          creg_str_PID_WP2_B_pGAIN = "0.15";
          PidStructVariableWP2Red.pGain = 0.15;
          PidStructVariableWP2Gre.pGain = 0.15;
          PidStructVariableWP2Blu.pGain = 0.15;

          creg_str_PID_WP2_R_iGAIN = "0.002";
          creg_str_PID_WP2_G_iGAIN = "0.002";
          creg_str_PID_WP2_B_iGAIN = "0.002";
          PidStructVariableWP2Red.iGain = 0.002;
          PidStructVariableWP2Gre.iGain = 0.002;
          PidStructVariableWP2Blu.iGain = 0.002;

          creg_str_PID_WP2_R_dGAIN = "0.0";
          creg_str_PID_WP2_G_dGAIN = "0.0";
          creg_str_PID_WP2_B_dGAIN = "0.0";
          PidStructVariableWP2Red.dGain = 0.0;
          PidStructVariableWP2Gre.dGain = 0.0;
          PidStructVariableWP2Blu.dGain = 0.0;

          creg_str_PID_WP3_R_pGAIN = "0.15";               //WP3s
          creg_str_PID_WP3_G_pGAIN = "0.15";
          creg_str_PID_WP3_B_pGAIN = "0.15";
          PidStructVariableWP3Red.pGain = 0.15;
          PidStructVariableWP3Gre.pGain = 0.15;
          PidStructVariableWP3Blu.pGain = 0.15;

          creg_str_PID_WP3_R_iGAIN = "0.002";
          creg_str_PID_WP3_G_iGAIN = "0.002";
          creg_str_PID_WP3_B_iGAIN = "0.002";
          PidStructVariableWP3Red.iGain = 0.002;
          PidStructVariableWP3Gre.iGain = 0.002;
          PidStructVariableWP3Blu.iGain = 0.002;

          creg_str_PID_WP3_R_dGAIN = "0.0";
          creg_str_PID_WP3_G_dGAIN = "0.0";
          creg_str_PID_WP3_B_dGAIN = "0.0";
          PidStructVariableWP3Red.dGain = 0.0;
          PidStructVariableWP3Gre.dGain = 0.0;
          PidStructVariableWP3Blu.dGain = 0.0;
         }
       nregNoofReadsToAverage.read();
       if( (nregNoofReadsToAverage > 0) && (nregNoofReadsToAverage < 61) )
         nNOOFreadsToAVERAGE = nregNoofReadsToAverage;
       else{
          nregNoofReadsToAverage = 8;
          nNOOFreadsToAVERAGE = nregNoofReadsToAverage;
          MessageBox(hWnd,"nNOOFreadsToAVERAGE was not set between 1 and 60 in the registry!\n\nI've reset it to 8","Warning",MB_OK);
         }
      }
      break;
    case WM_TIMER:
      //if(!bMakeSounds)
        ////PlaySound ("beep_09.wav",NULL,SND_ASYNC);
        //PlaySound ("Ack.wav",NULL,SND_ASYNC);
      
      //K illTimer(hWnd, IDT_TIMER_WNDPROC);
      //PostMessage(hWnd, WM_COMMAND, IDB_STATREC_REFRESH_NOKILLTIMER, 0);
      if(!bTimerHasBeenKilled)//A WM_TIMER message may still arrive after other parts of the program have killed the timer (if the timer event has been put
      //for(int MotorNo = 0; MotorNo < NOOFMOTORS; MotorNo++)// in the message queue). This variable must be set immediately before killing the timer so that
      //  U pdateMainWindowsControlsM(hWnd, MotorNo);       // these residual messages are ignored.
        UpdateMainWindowsControlsM();
int zzzzzHUH;
      NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS;
      break;
    case WM_PAINT:     {  //To change Window size, see Holoprog.cpp, line 69
       hdc = BeginPaint(hWnd, &ps);
//int recX, recY;  //Useful for visualising the update region:
//recX = -444 + 465 + 107;
//recY = 930 + 44 + 17;
//int wid = 675 + recX;
//int hit = 101 + recY;
//Rectangle(hdc,recX,recY,wid,hit);

       HGDIOBJ original = SelectObject(hdc,GetStockObject(DC_PEN)); //Save original object.

       //int lfHeight = -MulDiv(8,GetDeviceCaps(hdc,LOGPIXELSY),72);
       //HFONT newFont = CreateFont(lfHeight,0,0,0,0,FALSE,0,0,0,0,0,0,0,"Helv");
       HGDIOBJ oldFont = SelectObject(hdc,(HFONT)newFont);

#if 1
       SelectObject(hdc,GetStockObject(DC_BRUSH));
       if(bSubtle){
          SetDCBrushColor(hdc, RGB(210,210,210) );//                          <-Subtle
          SetDCPenColor(hdc,RGB(190,190,190));    //Change the DC pen color   <-Subtle
         }
       else{
          //SetDCBrushColor(hdc, RGB(40,40,40) );   //                          <-NOT Subtle
          //SetDCBrushColor(hdc, RGB(255,0,0) );   //                          <-NOT Subtle       This is red
          SetDCBrushColor(hdc, RGB(0,192,192) );   //                          <-NOT Subtle       This is bluey-green
          SetDCPenColor(hdc,RGB(20,20,20));       //Change the DC pen color
          //SetTextColor(hdc,RGB(0,223,223));
          //TextOut(hdc, 545,905,"8",1);
          SetTextColor(hdc,RGB(0,0,0));
          _itoa_s(nNoofCatastrophys, mes, SIZMES, 10);
          TextOut(hdc, 542,905,mes,lstrlen(mes));
          SetTextColor(hdc,RGB(0,0,0));
         }

       Polygon(hdc,lhPolyPoints, 4);
       Polygon(hdc,rhPolyPoints, 4);
       SetBkMode(hdc, TRANSPARENT);
       
     //adc200_get_unit_info (str, 100, 1, 1); //called five times in demo but says "ADC200-100 V7 on LPT1", then "Batch AOR63" (4 times)

       TextOut(hdc, 43,  6,"Motor",5);
       TextOut(hdc, 128, 6,"Current Motor Position",22);
       TextOut(hdc, 260, 6,"Homing",6);
       TextOut(hdc, 335, 6,"Switches",8);
       TextOut(hdc, 438, 6,"Enables",7);
       TextOut(hdc, 520, 6,"General",7);
       TextOut(hdc, 800, 6,"Serious",7);
       TextOut(hdc, 1100,6,"Status Line",11);
       TextOut(hdc, 1210,6,"Ready to",8);
       TextOut(hdc, 1352,6,"CheckSum /",10);
       TextOut(hdc, 1440,6,"Homing",6);

       TextOut(hdc, 43,  19,"ID",2);
       TextOut(hdc, 133, 19,"(uSteps from home)",18);
       TextOut(hdc, 263, 19,"State",5);
       TextOut(hdc, 315, 19,"Hm     LmH    LmA         Driv     485             Warning",58);
       TextOut(hdc, 800, 19,"Warning",7);
       TextOut(hdc, 1098,19,"I Want    It Is",15);
       TextOut(hdc, 1210,19,"Print Line?",11);
       TextOut(hdc, 1287,19,"ISR cnt",7);
       TextOut(hdc, 1352,19,"Lines done",10);
       TextOut(hdc, 1440,19,"Error",5);

       TextOut(hdc, 40, 38, "M1  - WP1Red",12);
       TextOut(hdc, 40, 59, "M2  - WP2RedObjEn",17);
       TextOut(hdc, 40, 80, "M3  - WP3RedRefEn",17);
       TextOut(hdc, 40, 101,"M4  - LOobjRed",14);
       TextOut(hdc, 40, 122,"M5  - ObjAPred",14);
       TextOut(hdc, 40, 141,"M6  - LCfocRed",14);
       TextOut(hdc, 40, 164,"M7  - RBapTred",14);
       TextOut(hdc, 40, 185,"M8  - RBapBred",14);
       TextOut(hdc, 40, 206,"M9  - RBapLred",14);
       TextOut(hdc, 40, 227,"M10  - RBapRred",15);
       TextOut(hdc, 40, 248,"M11  - GonHred.",15);
       TextOut(hdc, 40, 269,"M12  - GonVred",14);
       TextOut(hdc, 40, 300,"M13  - WP1Gre",13);
       TextOut(hdc, 40, 321,"M14  - WP2GreObjEn",18);
       TextOut(hdc, 40, 342,"M15  - WP3GreRefEn",18);
       TextOut(hdc, 40, 363,"M16  - LOobjGre",15);
       TextOut(hdc, 40, 384,"M17  - ObjAPGre",15);
       TextOut(hdc, 40, 405,"M18  - LCfocRed",15);
       TextOut(hdc, 40, 426,"M19  - RBapTGre",15);
       TextOut(hdc, 40, 447,"M20  - RBapBGre",15);
       TextOut(hdc, 40, 468,"M21  - RBapLGre",15);
       TextOut(hdc, 40, 489,"M22  - RBapRGre",15);
       TextOut(hdc, 40, 510,"M23  - GonHGre",14);
       TextOut(hdc, 40, 531,"M24  - GonVGre",14);
       TextOut(hdc, 40, 562,"M25  - WP1Blu",13);
       TextOut(hdc, 40, 583,"M26  - WP2BluObjEn",18);
       TextOut(hdc, 40, 604,"M27  - WP3BluRefEn",18);
       TextOut(hdc, 40, 625,"M28  - LOobjBlu",15);
       TextOut(hdc, 40, 646,"M29  - ObjAPBlu",15);
       TextOut(hdc, 40, 667,"M30  - LCfocBlu",15);
       TextOut(hdc, 40, 688,"M31  - RBapTBlu",15);
       TextOut(hdc, 40, 709,"M32  - RBapBBlu",15);
       TextOut(hdc, 40, 730,"M33  - RBapLBlu",15);
       TextOut(hdc, 40, 751,"M34  - RBapRBlu",15);
       TextOut(hdc, 40, 772,"M35  - GonHBlu.",15);
       TextOut(hdc, 40, 793,"M36  - GonVBlu",14);
       TextOut(hdc, 40, 824,"M37  RefBeamTab1",16);
       TextOut(hdc, 40, 845,"M38  RefBeamTab2",16);
       TextOut(hdc, 40, 866,"M39  XY Stage  X",16);
       TextOut(hdc, 40, 887,"M40  XY Stage Y",15);
       
       TextOut(hdc, 1210,904,"Ready",5);
       TextOut(hdc, 1211,919,"if Low",6);


  #define Y_OFFSET 880
  #define X_OFFSET -400
       #define SIZszMotorNrecnoRecString 32
       char szMotorNrecnoRecString[SIZszMotorNrecnoRecString];
       StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"Motor: ");
       _itoa_s(nchCurrentMotorZB+1, szMotorNrecnoRecString+7, SIZszMotorNrecnoRecString - 7,10);
       TextOut(hdc,X_OFFSET + 465, Y_OFFSET + 44, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));

//int recX, recY;  //Useful for visualising the update region:
//recX = X_OFFSET + 465 + 107;
//recY = Y_OFFSET + 44 + 17;
//int wid = 75 + recX;
//int hit = 40 + recY;
//Rectangle(hdc,recX,recY,wid,hit);

       if(nGlobalLastUSBrecNoRecvd == NOOF_ENERGYRECSm2){
          StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"LastEMrecNoRecvd: Table Full! ");
          TextOut(hdc,X_OFFSET + 472, Y_OFFSET + 64, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));
         }
       else{
          StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"LastEMrecNoRecvd: ");
          _itoa_s(nGlobalLastUSBrecNoRecvd, szMotorNrecnoRecString+18, SIZszMotorNrecnoRecString - 18,10);
        //NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
//        _itoa_s(CountStatRecsRecvd_TimesPlaySoundCalled, szMotorNrecnoRecString+18, SIZszMotorNrecnoRecString - 18,10);
          TextOut(hdc,X_OFFSET + 472, Y_OFFSET + 64, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));
         }
       StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"NoofReadsToAverage : "); //reuse of szMotorNrecnoRecString
       _itoa_s(nNOOFreadsToAVERAGE, szMotorNrecnoRecString+21, SIZszMotorNrecnoRecString - 21,10);
       TextOut(hdc,X_OFFSET + 1075, Y_OFFSET + 114, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));


StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"Meter14 ValueRecvd: ");
_itoa_s(sArrEnergRecs[nGlobalLastUSBrecNoRecvd].nEnergyRead[6], szMotorNrecnoRecString+19, SIZszMotorNrecnoRecString - 19,10);
int nlen = lstrlen(szMotorNrecnoRecString);
TextOut(hdc, X_OFFSET + 472, Y_OFFSET + 84, szMotorNrecnoRecString, nlen);
if(nlen == 21){
   nlen++;
   nlen--;
  }

/*
int recX, recY;  //Useful for visualising the update region:
recX = X_OFFSET + 572;
recY = Y_OFFSET + 61;
int wid = 75 + recX;
int hit = 40 + recY;
Rectangle(hdc,recX,recY,wid,hit);
recX = 787;
recY = 992;
wid = 35 + recX;
hit = 20 + recY;
Rectangle(hdc,recX,recY,wid,hit); 787,992,822,1012 is NoofReadsToAverage rectangle
*/

//char Err[50];
//char PidOUt[50];
//char szIsum[50];
//sprintf_s(Err,50, "%3.10f ", dblErrorWP2redDisplay );
//sprintf_s(PidOUt,50, "%3.10f ", PidOutputToActOn[0] );
//sprintf_s(szIsum,50, "%3.10f ", PidStructVariableWP2Red.iState );
//
//TextOut(hdc, X_OFFSET +  538, Y_OFFSET + 114, "M2->      Err", 13 );
//TextOut(hdc, X_OFFSET +  642, Y_OFFSET + 114, Err, lstrlen(Err) );
//TextOut(hdc, X_OFFSET +  782, Y_OFFSET + 114, "PidOUt", 6 );
//TextOut(hdc, X_OFFSET +  842, Y_OFFSET + 114, PidOUt, lstrlen(PidOUt) );
//TextOut(hdc, X_OFFSET +  982, Y_OFFSET + 114, "Integr", 6 );
//TextOut(hdc, X_OFFSET + 1042, Y_OFFSET + 114, szIsum, lstrlen(szIsum) );

#else
       int nLineNo = 0, TopLineYcoord = 44, nLineSpace = 24;
       TextOut(hdc, 50, TopLineYcoord - 28,"The Following Accelerator Keys are Defined:",43);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"A",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Active Motor (Selection dialogbox)",34);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"M",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Display single Motor Move dialogbox",35);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"B",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Display Block motor move dialogbox",34);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"S",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Stops the stepper motor (also Spacebar)",39);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"H",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Homes the stepper motor",23);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"T",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Toggles the stepper motor (Start/Stop)",38);
       TextOut(hdc, 61, TopLineYcoord + nLineSpace * nLineNo  ,"<>",2);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Jog +/- Slow,  ^ v  Jog +/- Fast",32);
       TextOut(hdc, 59, TopLineYcoord + nLineSpace * nLineNo  ,"P/N",3);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Power / No Power to the Driver",30);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"R",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Send Reset (Block Transfer) message to EVK",42);
       TextOut(hdc, 59, TopLineYcoord + nLineSpace * nLineNo  ,"X/C",3);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"eXecute Terminal / Clear Terminal",33);
       TextOut(hdc, 59, TopLineYcoord + nLineSpace * nLineNo  ,"4/5",3);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Turn on/off RS485 Message output",32);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"Z",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Zend me your Status Records",27);
       //TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"P",1);
       //TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Please Provide Pixel Position table",35);
 //    TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"W",1);
 //    TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Where are you now?",18);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"F",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Set Frequency Of Pix Prints",27);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"U",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"UltraEdit last PixPositionsMx.csv for current motor",51);
       TextOut(hdc, 63, TopLineYcoord + nLineSpace * nLineNo  ,"O",1);
       TextOut(hdc, 95, TopLineYcoord + nLineSpace * nLineNo++,"Open Hologram Config File",25);
       if(HCFrec.bHCFIsComplete)
         TextOut(hdc, 295, TopLineYcoord + nLineSpace * nLineNo++,"Hologram opened for Printing",28); //To change Window size, see Holoprog.cpp, line 69
       else
         TextOut(hdc, 295, TopLineYcoord + nLineSpace * nLineNo++,"Hologram opened for Editing",27); //To change Window size, see Holoprog.cpp, line 69
       #define SIZszMotorNrecnoRecString 32
       char szMotorNrecnoRecString[SIZszMotorNrecnoRecString];
       StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"Motor: ");
       _itoa_s(nchCurrentMotorZB+1, szMotorNrecnoRecString+7, SIZszMotorNrecnoRecString - 7,10);
       TextOut(hdc,465, 44, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));

       if(nGlobalLastUSBrecNoRecvd == NOOF_ENERGYRECSm2){
          StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"LastEMrecNoRecvd: Table Full! ");
          TextOut(hdc,472, 64, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));
         }
       else{
         StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"LastEMrecNoRecvd: ");
         _itoa_s(nGlobalLastUSBrecNoRecvd, szMotorNrecnoRecString+18, SIZszMotorNrecnoRecString - 18,10);
          TextOut(hdc,472, 64, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));
         }
StringCchCopy(szMotorNrecnoRecString,SIZszMotorNrecnoRecString,"Meter1 ValueRecvd: ");
_itoa_s(sArrEnergRecs[nGlobalLastUSBrecNoRecvd].nEnergyRead[3], szMotorNrecnoRecString+19, SIZszMotorNrecnoRecString - 19,10);
TextOut(hdc,472, 84, szMotorNrecnoRecString,lstrlen(szMotorNrecnoRecString));
#endif
       SelectObject(hdc,original); //Restore original object.
       SelectObject(hdc,(HFONT)oldFont);
       //DeleteObject(newFont);
       EndPaint(hWnd, &ps);
      }
      break;
    case WM_DESTROY:
    //K illTimer(hWnd, IDT_TIMERSCREENREFRESH);
      bTimerHasBeenKilled = TRUE;
      KillTimer(hWnd, IDT_TIMER_WNDPROC); //in WM_DESTROY
      CoUninitialize();
  //adc200_close_unit (1);//Port no
      //I won't worry about synchronisation here! These messages, and how to prompt the user in general when exiting, are issues that will need to be revisited.

      if(bInitialisationSucceeded){ //If initialisation didn't succeed, don't try to send network messages.
         char em;
         SendMessagePackets(hWnd, 39, MsTyp4_RESET, 0, 0);   //Don't Reboot Motor 40 anymore... do this instead (stops the stage falling under its own weight)
         for(em = NOOFMOTORSm1 - 1; em > -1; em--){       //Don't Reboot Motor 40 anymore
            if( SendMessagePackets(hWnd, em, MsTyp6_ENAB_DISAB, DISAB_POWR, 0) )//WM_DESTROY:Disable the Power Driver on MDS542 (Accelerator Key O for Off)
              em = -10; //Assume message will repeat for all motors - No point in forcing the user to see the message 40 times!
            Sleep(5);
            if( SendMessagePackets(hWnd, em, MsTyp4_RESET, 99, 0) )   //WM_DESTROY:subtype 99 actually resets the AVR32 (stops the motors restarting when holoprog restarts)
              em = -10;
           }
         if(em < -9)
           MessageBox(hWnd,"As a message to disable power on one of the motors failed,\nI didn't send the 'disable power' message to other motors.\n\nThis may mean that some of (or all) the motors are still energised.","Warning",MB_OK);
        }
      //Was:
      //SendMessagePackets(NULL, nchCurrentMotorZB, MsTyp6_ENAB_DISAB, DISAB_POWR, 0);//WM_DESTROY:Disable the Power Driver on MDS542 (Accelerator Key O for Off)
      //SendMessagePackets(NULL, nchCurrentMotorZB, MsTyp4_RESET, 99, 0);    //WM_DESTROY:subtype 99 actually resets the AVR32 (stops the motors restarting when holoprog restarts)
//MessageBox (hWnd,"temp stopped reboot",NULL,MB_OK); //when above message is commented out
      DestroyAllSockets();
      com_Exit(); //Maybe we need to wait for the comms to complete before killing the comms threads?
      comUSB_Exit(hWnd);
    //if (GNC != NULL)
    //  FreeLibrary(GNC);
      mm_result = timeEndPeriod( time_caps.wPeriodMin );
      if(mm_result != TIMERR_NOERROR)
        MessageBox (hWnd,"timeEndPeriod() function failed",NULL,MB_OK);
//    CloseHandle(ghMutex);
      DeleteObject(newFont);
      PostMessage(FindWindow("HOLOPROGHELPER",NULL), WM_DESTROY, 0, 0x0);
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
 return 0;
}

BOOL CreateTimeStampedFolder(void){
 //Create a TimeStamped Folder to store all the files in
 //Also need to record first and last line numbers
 // -Only download dynamic files
 // -When User restarts printing,  prompt for saving
 struct tm newtime;
 __time32_t aclock;
 char buffer[32];
 _time32( &aclock );                  // Get time in seconds.
 _localtime32_s( &newtime, &aclock ); // Convert time to struct tm form.
 errno_t errNum = asctime_s(buffer, 32, &newtime);
 if(errNum) {
    char szCode[12];
    StringCchCopy(mes,SIZMES,"... asctime_S() gave error code: ");
    _itoa_s((int)errNum, szCode, 12, 10);
    StringCchCat(mes,SIZMES,szCode);
    MessageBox(hGLWnd,mes,"Cannot create a time-stamped Folder because...",MB_OK);
    return 1; //can't have a break here
   }
 buffer[13] = '.';//Colons are illegal characters for a folder name
 buffer[16] = '.';
 buffer[19] = '\\';
 buffer[20] = 0;//Truncated the Date to remove the year part, also we'll ignore the first 4 chars (to remove the day part)
 //memmove_s(buffer+7,19, buffer+8,12); //19 is 26 - 7
 //buffer[19] = 0;//Truncated the Date to remove the year part, also we'll ignore the first 4 chars (to remove the day part)
 buffer[7] = buffer[8];  //Move the days part one char left so they touch the month, and leave an extra space after them. This
 buffer[8] = buffer[9];  // makes the folder name easier to read, by distancing the the time from the date
 buffer[9] = buffer[10];

 StringCchCopy(HCFcfs.szTimeStampedFolderName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
 StringCchCat(HCFcfs.szTimeStampedFolderName,_MAX_DIR,buffer+4);
 errno_t err;
 if( _mkdir(HCFcfs.szTimeStampedFolderName) ){
    _get_errno( &err );
    char ErrorMess[80] = "Error - Unable to create the sub-folder (named above).\nError Code: ";
    _itoa_s(err,ErrorMess+67,12,10);
    MessageBox (hGLWnd,ErrorMess,HCFcfs.szTimeStampedFolderName,MB_OK);
    return 1; //can't have a break here
   }
 return 0;//success
}

void SetRedWarning(void){
 RECT rWholeWindowClientRect;
 bSubtle = 0;
 GetClientRect(hGLWnd,&rWholeWindowClientRect);
 InvalidateRect(hGLWnd,&rWholeWindowClientRect,TRUE);
}

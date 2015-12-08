#include "stdafx.h"
extern int nLengthOfM39rowsIncLeadInOut_inPixels;

//extern double SetPointWP2red;//SetPoints are no longer looked at
//extern double SetPointWP2gre;//SetPoints are no longer looked at
//extern double SetPointWP2blu;//SetPoints are no longer looked at

//extern BOOL bTimerHasBeenKilled; //This file can Kill the timer temporarily, During transmission of Position files / checking of the checksum

//extern int  bMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec
extern unsigned int uiTableOfCheckSums[NOOFMOTORS];
int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];

char szExplorerDir2StartIn[_MAX_DIR] = "e, ";
extern BOOL bListOfStaticMotors[NOOFMOTORS];
extern BOOL bUserDecidedNotToPrintOrMove;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;
//                                         ConfigFileDialogs.cpp contains:
//CALLBACK OpenConfigFile_Dialog()
//CALLBACK SaveConfigFile_Dialog()
//CALLBACK ViewHologInfo_Dialog()
//  IDC_VIEWHOLOGINFO_SAVE_INCOMPLETE
//  IDC_VIEWHOLOGINFO_GENERATE_COMPLETE
//  IDB_VIEWHOLOGINFO_EXPLORE_CSVs
//  IDB_VIEWHOLOGINFO_VIEWSTARTPOSNS
//  IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS
//    M oveToStartPositions()
//  IDB_VIEWHOLOGINFO_SHOWSTATUS
//  IDB_VIEWHOLOGINFO_PRINT_HOLOGRAM

//BOOL CreateNewHCFfile(HWND hDlg, char*, &BOOL);

//These function prototypes are local to prevent external linkage:
BOOL bErrorCheckAllVIEWHOLOGINFOFields(HWND hDlg); //function prototype (Returns > 0 if error occurs)
BOOL bMotorsAreInTheirStartPositionsANDhappy(HWND hDlg, int *pArrayMotorIsInRightPlace, int nLineNoToStartPrintFrom);//function prototype (happy means no errors of warnings)
BOOL bMotorIsInItsStartPosition(HWND hDlg, char nchMotrNo, int nLineNoToStartPrintFrom);   //function prototype
BOOL bCheckAllDynamicFilesExist(HWND hDlg);                      //function prototype

extern unsigned char cMessageSeqNoSR[NOOFMOTORS]; //Viewable here just so debugger can see if seqno errors here are receiving SR replies
unsigned char cMessageSeqNoCS[NOOFMOTORS];        //Global mainly so debugger can see it in other modules (to see if seqno errors are receiving CS replies)

//extern HANDLE ghMutex;
//extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern int bWaitingForReply[];
//extern int bSTATREC_BOXisBeingDisplayed;
extern char szGlobalFileName[];
extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern char mes[];
extern int nHelpMessageToLoad;
extern BOOL bMoveNowRadioButPressed;
extern char szHGRAMSDirName4Reading[];
extern char FILENAMEStab[][16];
//extern char szExplorerDir2StartIn[];
extern CRegStdString LastHGramFileOpened;
//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
//extern int nArMaxSpeedsDependentOn_uSteps[65];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
//extern int nNoofUsteps4EachApertureTab[9];         //part of the pcf file
//extern int nNoofUsteps4EachFocusTab[3];            //part of the pcf file
//extern int nNoofUsteps4RefBeamTablePrintUnload[4]; //part of the pcf file
extern PCFRECORD PCFrec;


extern STATUSRECORD *pStatRec;
extern HWND hGLWnd;
extern HINSTANCE hInst;
extern char szGLWndTitle[SIZszGLWndTitle]; // [_MAX_DIR + 11] Eg: HoloProg: <filename>
extern BOOL nShowHologInfoBecauseFileOpenedOK;

#define SIZSPINTEXT 10

char szOrigLastHGramFileOpened[_MAX_DIR];
extern HCFRECORDsCURRENTfolders HCFcfs;
extern int  NOOFTIMESTHRU_B4OBJECTING;

//Fields shared with ChoosDir.cpp:
extern char szGlobDefaultFolderShortName[60]; //eg just the "HG1" bit
extern char szConcatinatedFolderName[_MAX_PATH];
extern char szDirName[_MAX_PATH];
//                 time_t uTimeStamp                //nb was 64-bit, now 32 as _USE_32BIT_TIME_T is defined (well, maybe!)
//                 BOOL bHCFIsComplete
//                              char szHologramName[44]
//                                unsigned int uiNoofLines  (Update nNoofLinesIncludingBetweemTheBeams, below, if amending this)
//                                      unsigned int uiNoofCols
//                              unsigned int nNoofColsPlusLeadInOut
//                                      unsigned int uiPixelSizeH_uMetres
//                                           unsigned int uiPixelSizeV_uMetres
//                                      unsigned int uiHorzDistBetwPixelCenters_nanoM
//                                             unsigned int uiVertDistBetwPixelCenters_nanoM
HCFRECORD DefaultHCFrec = {946684800,0,NO_HOLOGRAM_NAME,10,1000,1200,1600,1600,800000,800000,30000000,10,10,50000,50000,50000,15500,15500,15500,0,1000,1002,999,
//                                                       unsigned int uiFrequencyOfPixelPrintsTimes1M
//                                                                       int XposOfHologsOriginOnPlateMM
//                                                                          int YposOfHologsOriginOnPlateMM
//                                                                                         int WP1rRatioTimes1000
//                                                                                               int WP1gRatioTimes1000
//                                                                                                     int WP1bRatioTimes1000
//                                                                                                       int WP2rObjEnergyTimes1000
//                                                                                                             int WP2gObjEnergyTimes1000
//                                                                                                                   int WP2bObjEnergyTimes1000
//                                                                                                                          int     nApertureSize        //0 == 0.8mm, 1 == 1.6mm, 2==Pinhole (but pinhole is not valid, for a hologram)
//                                                                                                                              int     ObjBeamLengthR
//                                                                                                                                 int       ObjBeamLengthG
//                                                                                                                                      int      ObjBeamLengthB
// int           nNextLineToPrint;
// int           bThisHoloGramHasABadError;  //E.g. a file is missing, such as StartPostions.spf (1). Zero if OK.
   0,0,
// char          chNoofUsteps4EachMotorTab[NOOFMOTORS];
   //32,32,32,32,32,32,32,32,32,32, These are now in HCF_PCFrecCopy below
   //32,32,32,32,32,32,32,32,32,32,
   //32,32,32,32,32,32,32,32,32,32,
   //32,32,32,32,32,32,32,32,32,32,
   50, //nNoofRowsBetweenBeams  (Update nNoofLinesIncludingBetweemTheBeams, below, if amending this)
   9999,  //UnusedANDavailable  (was N OOFprePIXzeroCONSTspeedPIXELS)
   60000, //RefBeamTiltTimes1000
   150000,//LightCoordsDTimes1000
   60000, //LightCoordsOmegaTimes1000
   //NOOFprePIXzeroCONSTspeedPIXELS[NOOFMOTORS]
   0,0,0,0,0,0,5,5,5,5,15,15,
   0,0,0,0,0,0,5,5,5,5,15,15,
   0,0,0,0,0,0,5,5,5,5,15,15,
   0,0,20,0,
   //Unused[1] :
   0,   //n LengthOfM39rowsWhenGenerated_inPixels
   0,   //n LeadInForM39
   110, //nNoofLinesIncludingBetweemTheBeams (dependent on nNoofRowsBetweenBeams & uiNoofLines)
   0,   //nPixelAtwhichAccelStops
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //Unused[599];
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//HCF_PCFrecCopy: This is included, so changes to the PCFRECORD structure are automatically pulled into HCF_PCFrecCopy and given default data.
//From now on HCFrec contain a complete copy of the PCFrec at the time of generation.  Before Printing, therefore, any changes to the current
//printer configuration can be checked, to ensure the generated data is still valid. eg. changes to uSteps, changes to Physical Limits, moving the origin relative to home etc. 
#include "DefaultPCFrec.h"

};  //end of DefaultHCFrec

HCFRECORD HCFrec;
//HCFRECORD2 HCFrec2;            //For
//HCFRECORD3 HCFrec3 = {12,1};   // Testing
//HCFRECORD4 HCFrec4;            //  Record sizes

//Procedure for handling the "Open Hologram config file" DialogBox.  This collects a filename.
//User can attempt IDB_CONFIGFILE_OPEN4PRINTING or IDB_CONFIGFILE_OPEN4EDITING

INT_PTR CALLBACK OpenConfigFile_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 char szLocalOpenConfigFileName[_MAX_DIR];
 switch (message){
    case WM_INITDIALOG:
//Save opening LastHGramFileOpened upon entering the dialog box ready to restore if the user presses the cancel button.
      StringCchCopy(szOrigLastHGramFileOpened, _MAX_DIR, stdstring(LastHGramFileOpened).c_str() );
      SetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szOrigLastHGramFileOpened);
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_TRANSBLOCK_CHOOSEFILE:
           mGetFile(hDlg, IDS_FILTERSTR_HOLO_HCF_FILES, FALSE);//There's no way round it... this changes sz GlobalFileName
           SetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szGlobalFileName);
         //if(szGlobalFileName[0])
         //  LastHGramFileOpened = szGlobalFileName;
           break;
         case IDB_CONFIGFILE_OPEN4PRINTING:  //Open for Printing
           //Read filename from IDC_OPEN_CONFIG_FILE_FILE_CHOSEN and attempt to open then read the file's data in:
           GetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szLocalOpenConfigFileName,_MAX_PATH - 1);
           if( lstrlen(szLocalOpenConfigFileName) > 7 ){ //e.g. C:\a.hcf
              HCFRECORD CopyOfOrigHCFrec = HCFrec;
              if( !myOpen_File(hDlg, "hcf", szLocalOpenConfigFileName, UNUSED_MOTORNO) ){  //The file opened Ok
                 LastHGramFileOpened = szLocalOpenConfigFileName;
                 if (HCFrec.bHCFIsComplete){    // ...so update HCFcfs
                    char *ptrBslash;
                    //StringCchCopy(szGLWndTitle + 10,SIZszGLWndTitle, szLocalOpenConfigFileName);
                    StringCchCopy(HCFcfs.szCSVDirName4Reading,_MAX_DIR, szLocalOpenConfigFileName);
                    ptrBslash = strrchr(HCFcfs.szCSVDirName4Reading, BACKSLASH);     //reverse search for the final backslash
                    HCFcfs.szCSVDirName4Reading[(int)(ptrBslash-HCFcfs.szCSVDirName4Reading + 1)]=0; //szCSVDirName4Reading now holds the directory the hcf file resides in.
                    StringCchCopy(HCFcfs.szCSVDirName4Writing,_MAX_DIR,HCFcfs.szCSVDirName4Reading);
                    StringCchCat (HCFcfs.szCSVDirName4Reading,_MAX_DIR,"CSVs\\");    //szCSVDirName4Reading should be changed every time a new hcf file is used in a different location
                    StringCchCat (HCFcfs.szCSVDirName4Writing,_MAX_DIR,"CSVsOUT\\"); //szCSVDirName4Writing should be changed every time a new hcf file is used in a different location
                    StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");
                    StringCchCat(szGLWndTitle,SIZszGLWndTitle,szLocalOpenConfigFileName);
                    SetWindowText(hGLWnd,szGLWndTitle);
                    nShowHologInfoBecauseFileOpenedOK = 1; //<-- This triggers opening StartPosition.spf in WndProc's IDM_OPEN_CONFIGFILE (which created this dialogox)
                  //PostMessage(FindWindow(NULL,szOrigLastHGramFileOpened), WM_STOPHOMINGMESS_FROM_HELPERPROG, 0, 0x0);
                    PostMessage(FindWindow("HOLOPROGHELPER",NULL), WM_COMMAND, 112, 0x0); //112 is IDM_REDRAW #defined in HoloProgHelper's resource.h
                  //PostMessage(FindWindow(NULL,"HoloProgHelper"), WM_PAINT, 105, 0x0);
                  //PostMessage(FindWindow("HOLOPROGHELPER",NULL), WM_COMMAND, 105, 0x0); //105 is IDM_EXIT
                  //PostMessage(FindWindow(NULL,"HoloProgHelper"), WM_COMMAND, 105, 0x0);
                    EndDialog(hDlg, LOWORD(wParam));
                   }
                 else{
                    HCFrec = CopyOfOrigHCFrec; //This line undoes the update to HCFrec, so no need to update HCFcfs
                    if( IDYES == MessageBox(hDlg,"You cannot open this hologram for printing as it is not 'complete'.\n\nDo you want to open it for Editing instead?\n\n",NULL,MB_YESNO) )
                      PostMessage(hDlg,WM_COMMAND,IDB_CONFIGFILE_OPEN4EDITING,0);
                   }
                }
             }
           else
             MessageBox (hDlg,"You need a valid filename",NULL,MB_OK);
           break;
         case IDB_CONFIGFILE_RUN_EXPLORER:
           //Read filename from IDC_OPEN_CONFIG_FILE_FILE_CHOSEN and open the file's containing folder in Windows explorer:
           GetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szExplorerDir2StartIn+3,_MAX_DIR - 3); //rem szExplorerDir2StartIn is initialised to "e, ")
           if( lstrlen(szExplorerDir2StartIn) > 10 ){ //e.g. "e, C:\a.hcf"
              char *ptrBslash;
              ptrBslash = strrchr(szExplorerDir2StartIn,BACKSLASH);              //reverse search for the final backslash
              if(ptrBslash > (szExplorerDir2StartIn + 5)){
                 szExplorerDir2StartIn[(int)(ptrBslash-szExplorerDir2StartIn + 1)]=0; //szExplorerDir2StartIn now holds the directory
                 PostMessage(hGLWnd,WM_COMMAND,IDM_EXEC_EXPLORER,0);
                }
              else
                MessageBox (hDlg,"Valid filename2 needed, for Explorer to explore this folder",NULL,MB_OK);
             }
           else
             MessageBox (hDlg,"Valid filename needed, for Explorer to explore this folder",NULL,MB_OK);
           break;
         case IDB_CONFIGFILE_OPEN4EDITING:
           //Read filename from IDC_OPEN_CONFIG_FILE_FILE_CHOSEN and attempt to open then read the file's data in:
           GetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szLocalOpenConfigFileName,_MAX_PATH - 1);
           if( lstrlen(szLocalOpenConfigFileName) > 7 ){ //e.g. C:\a.hcf
              HCFRECORD CopyOfOrigHCFrec = HCFrec;
              if( !myOpen_File(hDlg, "hcf", szLocalOpenConfigFileName, UNUSED_MOTORNO) ){ //The file opened Ok
                 LastHGramFileOpened = szLocalOpenConfigFileName;
                 if (!HCFrec.bHCFIsComplete){  //Not complete is fine... we can go ahead and open the file for editing
                    char *ptrBslash;           // ...and update HCFcfs
                    StringCchCopy(HCFcfs.szCSVDirName4Reading,_MAX_DIR, szLocalOpenConfigFileName);
                    ptrBslash = strrchr(HCFcfs.szCSVDirName4Reading, BACKSLASH);     //reverse search for the final backslash
                    HCFcfs.szCSVDirName4Reading[(int)(ptrBslash-HCFcfs.szCSVDirName4Reading + 1)]=0; //szCSVDirName4Reading now holds the directory the hcf file resides in.
                    StringCchCopy(HCFcfs.szCSVDirName4Writing,_MAX_DIR,HCFcfs.szCSVDirName4Reading);
                    StringCchCat (HCFcfs.szCSVDirName4Reading,_MAX_DIR,"CSVs\\");    //szCSVDirName4Reading should be changed every time a new hcf file is used in a different location
                    StringCchCat (HCFcfs.szCSVDirName4Writing,_MAX_DIR,"CSVsOUT\\"); //szCSVDirName4Writing should be changed every time a new hcf file is used in a different location
                    StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");
                    StringCchCat (szGLWndTitle,SIZszGLWndTitle,szLocalOpenConfigFileName);
                    SetWindowText(hGLWnd,szGLWndTitle);
                    nShowHologInfoBecauseFileOpenedOK = 2;
                    EndDialog(hDlg, LOWORD(wParam));
                   }
                 else{//It is complete... we need to get the user to supply a new folder name, and copy the hcf file there, making it incomplete in the process
                    if( IDYES == MessageBox(hDlg,"You cannot open this hologram for Editing as it is marked as 'complete'.\n\nHowever, you can edit a copy of it  - by providing the name of the folder\nwhere you would like the copy to be stored.\n\nDo you want to work on a copy?\n","Message",MB_YESNO) ){
                       BOOL bSillyNameDummy; //Dummy because we ignore the return value
                       if ( CreateNewHCFfile(hDlg, HCFrec.szHologramName, &bSillyNameDummy) )//if this succeeds, it will have set LastHGramFileOpened
                         EndDialog(hDlg, LOWORD(wParam));
                       else
                         HCFrec = CopyOfOrigHCFrec; //This line undoes the update to HCFrec, so no need to update HCFcfs
                      }
                    else
                      HCFrec = CopyOfOrigHCFrec; //This line undoes the update to HCFrec, so no need to update HCFcfs
                   }
                }
             }
           else
             MessageBox (hDlg,"You need a valid filename",NULL,MB_OK);
           break;
         case IDCANCEL:                                                                     //Restore opening LastHGramFileOpened
           LastHGramFileOpened = szOrigLastHGramFileOpened;
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
   }//switch (message){
 return (INT_PTR)FALSE;
}
/*
//Procedure for handling the "Save Hologram config file" DialogBox.  This collects a filename.
INT_PTR CALLBACK SaveConfigFile_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 //char szLocalSaveConfigFileName[_MAX_DIR];
 switch (message){
    case WM_INITDIALOG:
      //StringCchCopy( szGlobalFileName, _MAX_DIR, stdstring(LastHGramFileOpened).c_str() );
      //SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szGlobalFileName);

        MessageBox(hDlg, "If the current hologram is open for Editing, use the View Hologram Info screen\nand, from there, press either:\n   the 'Save Incomplete'\nor\n   the 'Generate, Complete and Save' Button\n\nHolgrams opened for printing cannot be saved [again] - they are already saved",
           "Message - access to 'Save Hologram Config file' is disabled...", MB_OK);

      SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, stdstring(LastHGramFileOpened).c_str() );
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_TRANSBLOCK_CHOOSEFILE:
           mGetFile(hDlg, IDS_FILTERSTR_HOLO_HCF_FILES, TRUE);
           SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szGlobalFileName);
         //if(szGlobalFileName[0])
         //  LastHGramFileOpened = szGlobalFileName;
           break;
         case IDB_CONFIGFILE_SAVE:{
            //Read filename from IDC_SAVE_CONFIG_FILE_FILE_CHOSEN and save the hcf data to it
#if 1
            MessageBox (hDlg,"- Disabled!","Told you",MB_OK);
#else
            GetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szLocalSaveConfigFileName,_MAX_PATH - 1);
            if( lstrlen(szLocalSaveConfigFileName) > 7 ){ //e.g. C:\a.hcf
               int ThisIsWeak; //Unless user does something really silly it works OK.  Chances are, this whole SaveConfigFile_Dialog() routine will be removed from final App.
               if( !mySave_File(hDlg, 1000,"hcf",0, szLocalSaveConfigFileName, MOTORNO_NOT_USED) ){
                  LastHGramFileOpened = szLocalSaveConfigFileName;
                  StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");
                  StringCchCat (szGLWndTitle,SIZszGLWndTitle,szLocalSaveConfigFileName);
                  SetWindowText(hGLWnd,szGLWndTitle);
                 }
               //else file didn't save, but a message will have been displayed to user
               EndDialog(hDlg, LOWORD(wParam));
              }
            else
              MessageBox (hDlg,"You need a valid filename\n(in an existing folder)",NULL,MB_OK);
#endif
           }
           break;
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
   }//switch (message){
 return (INT_PTR)FALSE;
}
*/

 //IDC_VIEWHOLOGINFO_HOLOGNAME     2702
 //IDC_VIEWHOLOGINFO_TIMESTAMP     2703
 //IDC_VIEWHOLOGINFO_NOOFLINES     2704
 //IDC_VIEWHOLOGINFO_NOOFCOLS      2705
 //IDC_VIEWHOLOGINFO_NOOFCOLSpLEADINOUT 2706
 //IDC_VIEWHOLOGINFO_PIXSIZE_H     2707
 //IDC_VIEWHOLOGINFO_PIXSIZE_V     2708
 //IDC_VIEWHOLOGINFO_HORZPIXDIST   2709
 //IDC_VIEWHOLOGINFO_VERTPIXDIST   2710
 //IDC_VIEWHOLOGINFO_PRINTFREQ     2711
 //IDC_VIEWHOLOGINFO_XPOS_ORG_STEPS 2712
 //IDC_VIEWHOLOGINFO_YPOS_ORG_STEPS 2713
INT_PTR CALLBACK ViewHologInfo_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 char szDum[SIZSZDUM12];
 char szFormattedFloatWithUnits[12];
 char szLocalVHI_HCFfileName[_MAX_DIR];
 int ViewHologInfoApertureRowChosen;
 int ViewHologInfoNoofRowsBetweenBeamsRowChosen;
 int ViewHologInfoPIXFREQRowChosen;
 int ViewHologInfoWP1RratioRowChosen;
 int ViewHologInfoWP1GratioRowChosen;
 int ViewHologInfoWP1BratioRowChosen;
 int ViewHologInfoWP2RObjEnergyRowChosen;
 int ViewHologInfoWP2GObjEnergyRowChosen;
 int ViewHologInfoWP2BObjEnergyRowChosen;
 int ViewHologInfoRefBeamTilt______RowChosen;
 int ViewHologInfoLightCoordsD_____RowChosen;
 int ViewHologInfoLightCoordsOmegayRowChosen;
 int ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS;
 static HWND hAPERTURESIZE, hROWSbetweenBEAMS, hPIXFREQ, hWP1Rratio, hWP1Gratio, hWP1Bratio, hWP2R_ObjEnergy, hWP2G_ObjEnergy, hWP2B_ObjEnergy, hNOOFprePIXzeroCONSTspeedPIXELS, hProgess;
 static HWND hRefBeamTilt,  hLightCoordsD    , hLightCoordsOmega;
 static BOOL bSeeMessages = FALSE;
//static BOOL bMove2SPsFast = TRUE;
 static BOOL bMove2SPsHomeRegardless = TRUE;
 
 static HCFRECORD VHICopyOfOrigHCFrec;
 switch (message){
    case WM_INITDIALOG:{
       char szTimeStamp[27];
       if(!lstrcmp(NO_HOLOGRAM_NAME,HCFrec.szHologramName)){
          MessageBox (hDlg,"No Hologram is Opened","Cannot View...",MB_OK);
          EndDialog(hDlg, LOWORD(wParam));
         }
       VHICopyOfOrigHCFrec = HCFrec;
       errno_t err;
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_HOLOGNAME, HCFrec.szHologramName);
       err = ctime_s(szTimeStamp, 26, &HCFrec.uTimeStamp);
       if (err)
         MessageBox (hDlg,"ctime_s failed due to an invalid argument.",NULL,MB_OK);
       else
         szTimeStamp[lstrlen(szTimeStamp)-1] = 0;//removes linefeed (subscript would be -ve, here, if ctime_s() failed with an error: hence the else

       if((HCFrec.nApertureSize != 0)&&(HCFrec.nApertureSize != 1)){
          MessageBox (hDlg,"HCFrec.nApertureSize Not 0 or 1!",NULL,MB_OK);
          EndDialog(hDlg, LOWORD(wParam));
         }
       if((HCFrec.nNoofRowsBetweenBeams < 1)||(HCFrec.nNoofRowsBetweenBeams > 120)){
          MessageBox (hDlg,"HCFrec.nNoofRowsBetweenBeams is < 1 or > 120!",NULL,MB_OK);
          EndDialog(hDlg, LOWORD(wParam));
         }
       int j;
       for (j = 0; j < NOOFMOTORS; j++)
         if((HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[j] < 0)||(HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[j] > 99)){
            MessageBox (hDlg,"HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[] is < 0 or > 99!",NULL,MB_OK);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
           }
       hAPERTURESIZE = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_APERTURESIZE);
       hROWSbetweenBEAMS = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_ROWSBETWEENBEAMS);
       hPIXFREQ      = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_PRINTFREQ);
       hWP1Rratio    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP1_R_RATIO);
       hWP1Gratio    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP1_G_RATIO);
       hWP1Bratio    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP1_B_RATIO);
       //hWP2R_ObjEnergy    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY);
       //hWP2G_ObjEnergy    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY);
       //hWP2B_ObjEnergy    = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY);
       hRefBeamTilt       = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_REF_BEAM_TILT);
       hLightCoordsD      = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_LIGHTCOORDS_D);
       hLightCoordsOmega  = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_LIGHTCOORDS_OMEGA);
       hNOOFprePIXzeroCONSTspeedPIXELS =  GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_prePIXzeroCONSTspeedPIXELS );
       hProgess      = GetDlgItem (hDlg, IDC_VIEWHOLOGINFO_PROGRESS);
       ShowWindow(hProgess, SW_HIDE);

       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_APERTURESIZE,CB_ADDSTRING,0,(LPARAM)"0.8 mm");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_APERTURESIZE,CB_ADDSTRING,0,(LPARAM)"1.6 mm");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_APERTURESIZE,CB_SETCURSEL,HCFrec.nApertureSize, 0L);
       for(j = 1; j <121; j++){ //opened for editing. Give user the option to set the No of Rows Between Beams. 1 is rediculous, but may be useful when setting the printer up
          _itoa_s(j,szDum,SIZSZDUM12,10);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_ROWSBETWEENBEAMS,CB_ADDSTRING,0,(LPARAM)szDum);
         }
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_ROWSBETWEENBEAMS,CB_SETCURSEL,HCFrec.nNoofRowsBetweenBeams - 1, 0L);

       for(j = 0; j < 100; j++){
          _itoa_s(j,szDum,SIZSZDUM12,10);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_prePIXzeroCONSTspeedPIXELS,CB_ADDSTRING,0,(LPARAM)szDum);
         }
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_prePIXzeroCONSTspeedPIXELS,CB_SETCURSEL,HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[38], 0L);
 
       int DecodeFrintFreqIndex = -1;
       if(     HCFrec.uiFrequencyOfPixelPrintsTimes1M == 10000000)
         DecodeFrintFreqIndex = 0;
       else if(HCFrec.uiFrequencyOfPixelPrintsTimes1M == 15000000)
         DecodeFrintFreqIndex = 1;
       else if(HCFrec.uiFrequencyOfPixelPrintsTimes1M == 30000000)
         DecodeFrintFreqIndex = 2;
       else if(HCFrec.uiFrequencyOfPixelPrintsTimes1M == 60000000)
         DecodeFrintFreqIndex = 3;
       if(DecodeFrintFreqIndex == -1){
          MessageBox(hDlg,"HCFrec.uiFrequencyOfPixelPrintsTimes1M not recognised","HCFrec Error Ed",MB_OK);
          EndDialog(hDlg, LOWORD(wParam));
         }
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_PRINTFREQ,CB_ADDSTRING,0,(LPARAM)"10 Hz");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_PRINTFREQ,CB_ADDSTRING,0,(LPARAM)"15 Hz");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_PRINTFREQ,CB_ADDSTRING,0,(LPARAM)"30 Hz");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_PRINTFREQ,CB_ADDSTRING,0,(LPARAM)"60 Hz");
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_PRINTFREQ,CB_SETCURSEL,DecodeFrintFreqIndex, 0L);
       for(j = 0; j <1001; j++){
          _gcvt_s(szFormattedFloatWithUnits, 12, (double)j / 10, 3);
          if( szFormattedFloatWithUnits[lstrlen(szFormattedFloatWithUnits) - 1] == '.' )
            StringCchCat(szFormattedFloatWithUnits,12 , "0 %");
          else
            StringCchCat(szFormattedFloatWithUnits,12 , " %");
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_R_RATIO,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_G_RATIO,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_B_RATIO,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          /*
          _gcvt_s(szFormattedFloatWithUnits, 12, (double)j / 10, 3);
          if( szFormattedFloatWithUnits[lstrlen(szFormattedFloatWithUnits) - 1] == '.' )
            StringCchCat(szFormattedFloatWithUnits,12 , "0  u J");
          else
            StringCchCat(szFormattedFloatWithUnits,12 , "  u J");
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);*/
         }
       /*
       if(HCFrec.WP2rObjEnergyTimes1000/100 < 10){ //Prepend a zero
          szDum[0] = '0';
          _itoa_s(HCFrec.WP2rObjEnergyTimes1000/100,szDum+1,SIZSZDUM12-1,10);
         }
       else
         _itoa_s(HCFrec.WP2rObjEnergyTimes1000/100,szDum,SIZSZDUM12,10);
       int lenStr = lstrlen(szDum);
       char lastchar = szDum[lenStr - 1];
       szDum[lenStr - 1] = '.';
       szDum[lenStr] = lastchar;
       szDum[lenStr + 1] = 0;
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY, szDum);
       if(HCFrec.WP2gObjEnergyTimes1000/100 < 10){ //Prepend a zero
          szDum[0] = '0';
          _itoa_s(HCFrec.WP2gObjEnergyTimes1000/100,szDum+1,SIZSZDUM12-1,10);
         }
       else
         _itoa_s(HCFrec.WP2gObjEnergyTimes1000/100,szDum,SIZSZDUM12,10);
       lenStr = lstrlen(szDum);
       lastchar = szDum[lenStr - 1];
       szDum[lenStr - 1] = '.';
       szDum[lenStr] = lastchar;
       szDum[lenStr + 1] = 0;
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY, szDum);
       if(HCFrec.WP2bObjEnergyTimes1000/100 < 10){ //Prepend a zero
          szDum[0] = '0';
          _itoa_s(HCFrec.WP2bObjEnergyTimes1000/100,szDum+1,SIZSZDUM12-1,10);
         }
       else
         _itoa_s(HCFrec.WP2bObjEnergyTimes1000/100,szDum,SIZSZDUM12,10);
       lenStr = lstrlen(szDum);
       lastchar = szDum[lenStr - 1];
       szDum[lenStr - 1] = '.';
       szDum[lenStr] = lastchar;
       szDum[lenStr + 1] = 0;
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY, szDum);
       */
       char sz_dbMes[14];
       sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2rObjEnergyTimes1000/1000);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2gObjEnergyTimes1000/1000);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2bObjEnergyTimes1000/1000);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY, sz_dbMes);


       for(j = 0; j <901; j++){  //0 to 90 degrees for RefBeamTilt and LightCoordsOmega
          _gcvt_s(szFormattedFloatWithUnits, 12, (double)j / 10, 3);
          if( szFormattedFloatWithUnits[lstrlen(szFormattedFloatWithUnits) - 1] == '.' )
            StringCchCat(szFormattedFloatWithUnits,12 , "0 deg");
          else
            StringCchCat(szFormattedFloatWithUnits,12 , " deg");
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_REF_BEAM_TILT,    CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_LIGHTCOORDS_OMEGA,CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
         }
       for(j = 0; j <3001; j++){  //0 to 300 Centimetres for LightCoordsD
          _gcvt_s(szFormattedFloatWithUnits, 12, (double)j / 10, 3);
          if( szFormattedFloatWithUnits[lstrlen(szFormattedFloatWithUnits) - 1] == '.' )
            StringCchCat(szFormattedFloatWithUnits,12 , "0 cms");
          else
            StringCchCat(szFormattedFloatWithUnits,12 , " cms");
          SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_LIGHTCOORDS_D, CB_ADDSTRING,0,(LPARAM)szFormattedFloatWithUnits);
         }
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_R_RATIO,CB_SETCURSEL,HCFrec.WP1rRatioTimes1000/100, 0L);
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_G_RATIO,CB_SETCURSEL,HCFrec.WP1gRatioTimes1000/100, 0L);
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP1_B_RATIO,CB_SETCURSEL,HCFrec.WP1bRatioTimes1000/100, 0L);
       //SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY,CB_SETCURSEL,HCFrec.WP2rObjEnergyTimes1000/4000, 0L);
       //SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY,CB_SETCURSEL,HCFrec.WP2gObjEnergyTimes1000/4000, 0L);
       //SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY,CB_SETCURSEL,HCFrec.WP2bObjEnergyTimes1000/4000, 0L);

       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_REF_BEAM_TILT,CB_SETCURSEL,HCFrec.RefBeamTiltTimes1000/100, 0L);
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_LIGHTCOORDS_D,CB_SETCURSEL,HCFrec.LightCoordsDTimes1000/100, 0L);
       SendDlgItemMessage(hDlg,IDC_VIEWHOLOGINFO_LIGHTCOORDS_OMEGA,CB_SETCURSEL,HCFrec.LightCoordsOmegaTimes1000/100, 0L);

       if( bSeeMessages )
         CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO);
       else
         CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,FALSE);

     //if( bMove2SPsFast )
     //  CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO);
     //else
     //  CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO,IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO,FALSE);

       if( bMove2SPsHomeRegardless )
         CheckRadioButton(hDlg, IDC_VHI_MOVE2SPsHOME_REGARDLESS, IDC_VHI_MOVE2SPsHOME_REGARDLESS, IDC_VHI_MOVE2SPsHOME_REGARDLESS);
       else
         CheckRadioButton(hDlg, IDC_VHI_MOVE2SPsHOME_REGARDLESS,IDC_VHI_MOVE2SPsHOME_REGARDLESS,FALSE);

       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_TIMESTAMP, szTimeStamp);
       _itoa_s(HCFrec.uiNoofLines,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_NOOFLINES, szDum);
       _itoa_s(HCFrec.uiNoofCols,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_NOOFCOLS, szDum);
       _itoa_s(HCFrec.nNoofColsPlusLeadInOut,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_NOOFCOLSpLEADINOUT, szDum);
       _itoa_s(HCFrec.uiPixelSizeH_uMetres,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_PIXSIZE_H, szDum);
       _itoa_s(HCFrec.uiPixelSizeV_uMetres,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_PIXSIZE_V, szDum);
       _itoa_s(HCFrec.uiHorzDistBetwPixelCenters_nanoM,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_HORZPIXDIST, szDum);
       _itoa_s(HCFrec.uiVertDistBetwPixelCenters_nanoM,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_VERTPIXDIST, szDum);
       _itoa_s(HCFrec.XposOfHologsOriginOnPlateMM,szDum,SIZSZDUM12,10);   //This is the hologram's origin (in mm) relative to the Plate's origin. i.e. where to print on the plate
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_XPOS_ORG_STEPS, szDum);
       _itoa_s(HCFrec.YposOfHologsOriginOnPlateMM,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_YPOS_ORG_STEPS, szDum);
       _itoa_s(HCFrec.ObjBeamLengthR,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENR, szDum);
       _itoa_s(HCFrec.ObjBeamLengthG,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENG, szDum);
       _itoa_s(HCFrec.ObjBeamLengthB,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENB, szDum);

       _itoa_s(HCFrec.nNextLineToPrint + 1,szDum,SIZSZDUM12,10);
       SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_LINE2STARTFROM, szDum); //Perhaps I should query the status rec?



_itoa_s(HCFrec.nLengthOfM39rowsWhenGenerated_inPixels,szDum,SIZSZDUM12,10);
SetDlgItemText(hDlg, IDC_VHI_LENGTH_OF_M39_ROWS_WHEN_GEND, szDum); //Perhaps I should query the status rec?
_itoa_s(HCFrec.nLeadInForM39 - 1,szDum,SIZSZDUM12,10);
SetDlgItemText(hDlg, IDC_VHI_LENGTH_OF_M39_ROW_LEADIN_m1, szDum); //Perhaps I should query the status rec?





       if( bMoveNowRadioButPressed )
         CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO, IDB_TRANSBLOCK_MOVENOW_RADIO, IDB_TRANSBLOCK_MOVENOW_RADIO);
       else
         CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,FALSE);

       int ReadingEnergyFilesWouldBeQuickerIfTheyWereCSEfiles;
       char szLocalM2or3ProfileFileName[_MAX_DIR];
       if(HCFrec.bHCFIsComplete){
          int readFromExcelM2or3Error = 0;
          StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
          StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3r.csv"
          if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                        bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 0, HCFrec.nNoofRowsBetweenBeams))
            readFromExcelM2or3Error = 3;
          else{
             StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
             StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3g.csv"
             if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                           bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 1, HCFrec.nNoofRowsBetweenBeams))
               readFromExcelM2or3Error = 15;
             else{
                StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                              bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 2, HCFrec.nNoofRowsBetweenBeams))
                  readFromExcelM2or3Error = 27;
                else{
                   StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                   StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2r.csv"
                   if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                                 bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 3, HCFrec.nNoofRowsBetweenBeams))
                     readFromExcelM2or3Error = 2;
                   else{
                      StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                      StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2g.csv"
                      if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                    bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 4, HCFrec.nNoofRowsBetweenBeams))
                        readFromExcelM2or3Error = 14;
                      else{
                         StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                         StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                         if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                       bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 5, HCFrec.nNoofRowsBetweenBeams))
                           readFromExcelM2or3Error = 26;
                        }
                     }
                  }
               }
            }
          if(readFromExcelM2or3Error){
             StringCchCopy(mes, SIZMES, "Motor: ");
             _itoa_s(readFromExcelM2or3Error, mes+7,_MAX_DIR - 7,10);
             MessageBox (hDlg,"There was a problem reading (at least) one Energy File.",mes,MB_OK);
            }
         }
       RECT ParentRc;           //Display dialog, usefully positioned 
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, ParentRc.left + 279, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO:
           if( bSeeMessages ){
              bSeeMessages = FALSE;
              CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,FALSE);
             }
           else{
              bSeeMessages = TRUE;
              CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO,IDC_VIEWHOLOGINFO_C_MESSAGES_RADIO);
             }
           break;
       //case IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO:
       //  if( bMove2SPsFast ){
       //     bMove2SPsFast = FALSE;
       //     CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO,IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO,FALSE);
       //    }
       //  else{
       //     bMove2SPsFast = TRUE;
       //     CheckRadioButton(hDlg, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO, IDC_VIEWHOLOGINFO_MOVE2SPsFAST_RADIO);
       //    }
       //  break;
         case IDC_VHI_MOVE2SPsHOME_REGARDLESS:
           if( bMove2SPsHomeRegardless ){
              bMove2SPsHomeRegardless = FALSE;
              CheckRadioButton(hDlg, IDC_VHI_MOVE2SPsHOME_REGARDLESS,IDC_VHI_MOVE2SPsHOME_REGARDLESS,FALSE);
             }
           else{
              bMove2SPsHomeRegardless = TRUE;
              CheckRadioButton(hDlg, IDC_VHI_MOVE2SPsHOME_REGARDLESS, IDC_VHI_MOVE2SPsHOME_REGARDLESS, IDC_VHI_MOVE2SPsHOME_REGARDLESS);
             }
           break;           
         case IDC_VIEWHOLOGINFO_HOLOGNAME:
         case IDC_VIEWHOLOGINFO_TIMESTAMP:
         case IDC_VIEWHOLOGINFO_NOOFLINES:
         case IDC_VIEWHOLOGINFO_NOOFCOLS:
         case IDC_VIEWHOLOGINFO_NOOFCOLSpLEADINOUT:
         case IDC_VIEWHOLOGINFO_PIXSIZE_H:
         case IDC_VIEWHOLOGINFO_PIXSIZE_V:
         case IDC_VIEWHOLOGINFO_HORZPIXDIST:
         case IDC_VIEWHOLOGINFO_VERTPIXDIST:
         case IDC_VIEWHOLOGINFO_PRINTFREQ:
         case IDC_VIEWHOLOGINFO_XPOS_ORG_STEPS:
         case IDC_VIEWHOLOGINFO_YPOS_ORG_STEPS:
           break;
         case IDC_VIEWHOLOGINFO_SAVE_INCOMPLETE:
           if( bErrorCheckAllVIEWHOLOGINFOFields(hDlg) ) //returns true if there's an error - having displayed an approprate MessageBox.
             break;
           ViewHologInfoApertureRowChosen = SendMessage(hAPERTURESIZE, CB_GETCURSEL,0,0);//0.8 or 1.6mm
           if(ViewHologInfoApertureRowChosen == CB_ERR)
             MessageBox(hDlg,"Please select a value","Error reading your Chosen Aperture",MB_OK);
           else{
              ViewHologInfoNoofRowsBetweenBeamsRowChosen = SendMessage(hROWSbetweenBEAMS, CB_GETCURSEL,0,0) + 1;//0 to 119 (i.e. 1 to 120)
              if(ViewHologInfoNoofRowsBetweenBeamsRowChosen == CB_ERR)
                MessageBox(hDlg,"Please select a value","Error reading your Chosen nNoofRowsBetweenBeams",MB_OK);
              else{
                 ViewHologInfoPIXFREQRowChosen = SendMessage(hPIXFREQ, CB_GETCURSEL,0,0);//10,15,30,60 Hz
                 if(ViewHologInfoPIXFREQRowChosen == CB_ERR)
                   MessageBox(hDlg,"Please select a value","Error reading your Chosen Pixel Frequency",MB_OK);
                 else{
                    ViewHologInfoWP1RratioRowChosen = SendMessage(hWP1Rratio, CB_GETCURSEL,0,0);
                    if(ViewHologInfoWP1RratioRowChosen == CB_ERR)
                      MessageBox(hDlg,"Please select a value","Error reading WP1 RED ratio",MB_OK);
                    else{
                       ViewHologInfoWP1GratioRowChosen = SendMessage(hWP1Gratio, CB_GETCURSEL,0,0);

                       if(ViewHologInfoWP1GratioRowChosen == CB_ERR)
                         MessageBox(hDlg,"Please select a value","Error reading WP1 GREEN ratio",MB_OK);
                       else{
                          ViewHologInfoWP1BratioRowChosen = SendMessage(hWP1Bratio, CB_GETCURSEL,0,0);
                          if(ViewHologInfoWP1BratioRowChosen == CB_ERR)
                            MessageBox(hDlg,"Please select a value","Error reading WP1 BLUE ratio",MB_OK);
                          else{
                             //ViewHologInfoWP2RObjEnergyRowChosen = SendMessage(hWP2R_ObjEnergy, CB_GETCURSEL,0,0);
                             //if(ViewHologInfoWP2RObjEnergyRowChosen == CB_ERR)
                             //  MessageBox(hDlg,"Please select a value","Error reading WP2 RED Obj Energy",MB_OK);
                             //else{
                             if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2RObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY) )
                               ;
                             else{
                                //ViewHologInfoWP2GObjEnergyRowChosen = SendMessage(hWP2G_ObjEnergy, CB_GETCURSEL,0,0);
                                //if(ViewHologInfoWP2GObjEnergyRowChosen == CB_ERR)
                                //  MessageBox(hDlg,"Please select a value","Error reading WP2 GREEN Obj Energy",MB_OK);
                                //else{
                                if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2GObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY) )
                                  ;
                                else{
                                   //ViewHologInfoWP2BObjEnergyRowChosen = SendMessage(hWP2B_ObjEnergy, CB_GETCURSEL,0,0);
                                   //if(ViewHologInfoWP2BObjEnergyRowChosen == CB_ERR)
                                   //  MessageBox(hDlg,"Please select a value","Error reading WP2 BLUE Obj Energy",MB_OK);
                                   //else{
                                   if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2BObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY) )
                                     ;
                                   else{
                                      ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS = SendMessage(hNOOFprePIXzeroCONSTspeedPIXELS, CB_GETCURSEL,0,0);
                                      if(ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS == CB_ERR)
                                        MessageBox(hDlg,"Please select a value","Error reading NOOFprePIXzeroCONSTspeedPIXELS",MB_OK);
                                      else{
                                         ViewHologInfoRefBeamTilt______RowChosen = SendMessage(hRefBeamTilt, CB_GETCURSEL,0,0);
                                         if(ViewHologInfoRefBeamTilt______RowChosen == CB_ERR)
                                           MessageBox(hDlg,"Please select a value","Error reading Reference Beam Tilt",MB_OK);
                                         else{
                                            ViewHologInfoLightCoordsD_____RowChosen = SendMessage(hLightCoordsD, CB_GETCURSEL,0,0);
                                            if(ViewHologInfoLightCoordsD_____RowChosen == CB_ERR)
                                              MessageBox(hDlg,"Please select a value","Error reading Light Coords Distance",MB_OK);
                                            else{
                                               ViewHologInfoLightCoordsOmegayRowChosen = SendMessage(hLightCoordsOmega, CB_GETCURSEL,0,0);
                                               if(ViewHologInfoLightCoordsOmegayRowChosen == CB_ERR)
                                                 MessageBox(hDlg,"Please select a value","Error reading Light Coords Omega",MB_OK);
                                               else{
                                                  if((ViewHologInfoNoofRowsBetweenBeamsRowChosen * 2 + HCFrec.uiNoofLines) > 2000){
                                                     MessageBox(hDlg,"2 * number of rows between the beams\n\n\tplus\n\nthe number of lines\n\nexceeds 2000!" ,"There are too many rows in total!",MB_OK);
                                                     break;
                                                    }
                                                  HCFrec.bHCFIsComplete = 0;
                                                  HCFrec.nApertureSize = ViewHologInfoApertureRowChosen;
                                                  if(ViewHologInfoPIXFREQRowChosen == 0)
                                                    HCFrec.uiFrequencyOfPixelPrintsTimes1M = 10000000;
                                                  else if(ViewHologInfoPIXFREQRowChosen == 1)
                                                    HCFrec.uiFrequencyOfPixelPrintsTimes1M = 15000000;
                                                  else if(ViewHologInfoPIXFREQRowChosen == 2)
                                                    HCFrec.uiFrequencyOfPixelPrintsTimes1M = 30000000;
                                                  else if (ViewHologInfoPIXFREQRowChosen == 3)
                                                    HCFrec.uiFrequencyOfPixelPrintsTimes1M = 60000000;
                                                  else
                                                    MessageBox(hDlg,"'Impossible' bug1: ViewHologInfoPIXFREQRowChosen <> range","Serious Error Please Report to Programmer",MB_OK);
                                                  HCFrec.nNoofRowsBetweenBeams = ViewHologInfoNoofRowsBetweenBeamsRowChosen;
                                                  HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[38] = ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS;
                                                  HCFrec.WP1rRatioTimes1000 = ViewHologInfoWP1RratioRowChosen * 100;
                                                  HCFrec.WP1gRatioTimes1000 = ViewHologInfoWP1GratioRowChosen * 100;
                                                  HCFrec.WP1bRatioTimes1000 = ViewHologInfoWP1BratioRowChosen * 100;
                                                  HCFrec.WP2rObjEnergyTimes1000 = ViewHologInfoWP2RObjEnergyRowChosen * 10;
                                                  HCFrec.WP2gObjEnergyTimes1000 = ViewHologInfoWP2GObjEnergyRowChosen * 10;
                                                  HCFrec.WP2bObjEnergyTimes1000 = ViewHologInfoWP2BObjEnergyRowChosen * 10;
                                                  //HCFrec.WP2rObjEnergyTimes1000 = ViewHologInfoWP2RObjEnergyRowChosen * 4000;
                                                  //HCFrec.WP2gObjEnergyTimes1000 = ViewHologInfoWP2GObjEnergyRowChosen * 4000;
                                                  //HCFrec.WP2bObjEnergyTimes1000 = ViewHologInfoWP2BObjEnergyRowChosen * 4000;
                                                  HCFrec.RefBeamTiltTimes1000      = ViewHologInfoRefBeamTilt______RowChosen * 100;
                                                  HCFrec.LightCoordsDTimes1000     = ViewHologInfoLightCoordsD_____RowChosen * 100;
                                                  HCFrec.LightCoordsOmegaTimes1000 = ViewHologInfoLightCoordsOmegayRowChosen * 100;
                                                  StringCchCopy(szLocalVHI_HCFfileName, _MAX_DIR, stdstring(LastHGramFileOpened).c_str());
                                                  if( !mySave_File(hDlg, 1000,"hcf",0, szLocalVHI_HCFfileName, MOTORNO_NOT_USED) ){
                                                     StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");             //These 3
                                                     StringCchCat (szGLWndTitle,SIZszGLWndTitle,szLocalVHI_HCFfileName);   // lines are
                                                     SetWindowText(hGLWnd,szGLWndTitle);                                   //  probably unnecessary
                                                     EndDialog(hDlg, LOWORD(wParam));
                                                    }
                                                  //else file didn't save, but a message will have been displayed to user
                                                 }
                                              }
                                           }
                                        }
                                     }
                                  }
                               }
                            }
                         }
                      }
                   }
                }
             }
           break;
         case IDC_VIEWHOLOGINFO_GENERATE_COMPLETE:{
            time_t uTimeOfGeneration;
            time(&uTimeOfGeneration); //Update the timestamp to the current time ()
            if( bErrorCheckAllVIEWHOLOGINFOFields(hDlg) ) //Returns true if there's an error - having displayed an approprate MessageBox, else many fields are loaded into HCFrec
              break; //Specifically: HCFrec.szHologramName,uiNoofLines,uiNoofCols,nNoofColsPlusLeadInOut,uiPixelSizeH_uMetres,uiPixelSizeV_uMetres,uiHorzDistBetwPixelCenters_nanoM
                     //uiVertDistBetwPixelCenters_nanoM,XposOfHologsOriginOnPlateMM,YposOfHologsOriginOnPlateMM,ObjBeamLengthR,G,&B
            ViewHologInfoApertureRowChosen = SendMessage(hAPERTURESIZE, CB_GETCURSEL,0,0);//0.8 or 1.6mm
            if(ViewHologInfoApertureRowChosen == CB_ERR)
              MessageBox(hDlg,"Please select a value","Error reading your Chosen Aperture",MB_OK);
            else{
               ViewHologInfoNoofRowsBetweenBeamsRowChosen = SendMessage(hROWSbetweenBEAMS, CB_GETCURSEL,0,0) + 1;//0 to 119 (i.e. 1 to 120)
               if(ViewHologInfoNoofRowsBetweenBeamsRowChosen == CB_ERR)
                 MessageBox(hDlg,"Please select a value","Error reading your Chosen nNoofRowsBetweenBeams",MB_OK);
               else{
                  ViewHologInfoPIXFREQRowChosen = SendMessage(hPIXFREQ, CB_GETCURSEL,0,0);//10,15,30,60 Hz
                  if(ViewHologInfoPIXFREQRowChosen == CB_ERR)
                    MessageBox(hDlg,"Please select a value","Error reading your Chosen Pixel Frequency",MB_OK);
                  else{
                     ViewHologInfoWP1RratioRowChosen = SendMessage(hWP1Rratio, CB_GETCURSEL,0,0);
                     if(ViewHologInfoWP1RratioRowChosen == CB_ERR)
                       MessageBox(hDlg,"Please select a value","Error reading WP1 RED ratio",MB_OK);
                     else{
                        ViewHologInfoWP1GratioRowChosen = SendMessage(hWP1Gratio, CB_GETCURSEL,0,0);
                        if(ViewHologInfoWP1GratioRowChosen == CB_ERR)
                          MessageBox(hDlg,"Please select a value","Error reading WP1 GREEN ratio",MB_OK);
                        else{
                           ViewHologInfoWP1BratioRowChosen = SendMessage(hWP1Bratio, CB_GETCURSEL,0,0);
                           if(ViewHologInfoWP1BratioRowChosen == CB_ERR)
                             MessageBox(hDlg,"Please select a value","Error reading WP1 BLUE ratio",MB_OK);
                           else{
                              //ViewHologInfoWP2RObjEnergyRowChosen = SendMessage(hWP2R_ObjEnergy, CB_GETCURSEL,0,0);
                              //if(ViewHologInfoWP2RObjEnergyRowChosen == CB_ERR)
                              //  MessageBox(hDlg,"Please select a value","Error reading WP2 RED Obj Energy",MB_OK);
                              //else{
                              if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2RObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_R_OBJ_ENERGY) )
                                ;
                              else{
                                 //ViewHologInfoWP2GObjEnergyRowChosen = SendMessage(hWP2G_ObjEnergy, CB_GETCURSEL,0,0);
                                 //if(ViewHologInfoWP2GObjEnergyRowChosen == CB_ERR)
                                 //  MessageBox(hDlg,"Please select a value","Error reading WP2 GREEN Obj Energy",MB_OK);
                                 //else{
                                 if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2GObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_G_OBJ_ENERGY) )
                                   ;
                                 else{
                                    //ViewHologInfoWP2BObjEnergyRowChosen = SendMessage(hWP2B_ObjEnergy, CB_GETCURSEL,0,0);
                                    //if(ViewHologInfoWP2BObjEnergyRowChosen == CB_ERR)
                                    //  MessageBox(hDlg,"Please select a value","Error reading WP2 BLUE Obj Energy",MB_OK);
                                    //else{
                                    if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2BObjEnergyRowChosen, IDC_VIEWHOLOGINFO_WP2_B_OBJ_ENERGY) )
                                      ;
                                    else{
                                       ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS = SendMessage(hNOOFprePIXzeroCONSTspeedPIXELS, CB_GETCURSEL,0,0);
                                       if(ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS == CB_ERR)
                                         MessageBox(hDlg,"Please select a value","Error reading NOOFprePIXzeroCONSTspeedPIXELS",MB_OK);
                                       else{
                                         ViewHologInfoRefBeamTilt______RowChosen = SendMessage(hRefBeamTilt, CB_GETCURSEL,0,0);
                                         if(ViewHologInfoRefBeamTilt______RowChosen == CB_ERR)
                                           MessageBox(hDlg,"Please select a value","Error reading Reference Beam Tilt",MB_OK);
                                         else{
                                            ViewHologInfoLightCoordsD_____RowChosen = SendMessage(hLightCoordsD, CB_GETCURSEL,0,0);
                                            if(ViewHologInfoLightCoordsD_____RowChosen == CB_ERR)
                                              MessageBox(hDlg,"Please select a value","Error reading Light Coords Distance",MB_OK);
                                            else{
                                               ViewHologInfoLightCoordsOmegayRowChosen = SendMessage(hLightCoordsOmega, CB_GETCURSEL,0,0);
                                               if(ViewHologInfoLightCoordsOmegayRowChosen == CB_ERR)
                                                 MessageBox(hDlg,"Please select a value","Error reading Light Coords Omega",MB_OK);
                                               else{
                                                   if((ViewHologInfoNoofRowsBetweenBeamsRowChosen * 2 + HCFrec.uiNoofLines) > 2000){
                                                      MessageBox(hDlg,"2 * number of rows between the beams\n\n\tplus\n\nthe number of lines\n\nexceeds 2000!" ,"There are too many rows in total!",MB_OK);
                                                      break;
                                                     }
                                                   HCFrec.uTimeStamp = uTimeOfGeneration; //Update the timestamp to the current time (uTimeOfGeneration is written to most/all of a hologram's config files)
                                                   HCFrec.nApertureSize = ViewHologInfoApertureRowChosen;
                                                   if(ViewHologInfoPIXFREQRowChosen == 0)
                                                     HCFrec.uiFrequencyOfPixelPrintsTimes1M = 10000000;
                                                   else if(ViewHologInfoPIXFREQRowChosen == 1)
                                                     HCFrec.uiFrequencyOfPixelPrintsTimes1M = 15000000;
                                                   else if(ViewHologInfoPIXFREQRowChosen == 2)
                                                     HCFrec.uiFrequencyOfPixelPrintsTimes1M = 30000000;
                                                   else if(ViewHologInfoPIXFREQRowChosen == 3)
                                                     HCFrec.uiFrequencyOfPixelPrintsTimes1M = 60000000;
                                                   else
                                                     MessageBox(hDlg,"'Impossible' bug2: ViewHologInfoPIXFREQRowChosen <> range","Serious Error Please Report to Programmer",MB_OK);
                                                   HCFrec.nNoofRowsBetweenBeams = ViewHologInfoNoofRowsBetweenBeamsRowChosen;
                                                   HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[38] = ViewHologInfonNOOFprePIXzeroCONSTspeedPIXELS;
                                                   HCFrec.WP1rRatioTimes1000 = ViewHologInfoWP1RratioRowChosen * 100;
                                                   HCFrec.WP1gRatioTimes1000 = ViewHologInfoWP1GratioRowChosen * 100;
                                                   HCFrec.WP1bRatioTimes1000 = ViewHologInfoWP1BratioRowChosen * 100;
                                                   HCFrec.WP2rObjEnergyTimes1000 = ViewHologInfoWP2RObjEnergyRowChosen * 10;
                                                   HCFrec.WP2gObjEnergyTimes1000 = ViewHologInfoWP2GObjEnergyRowChosen * 10;
                                                   HCFrec.WP2bObjEnergyTimes1000 = ViewHologInfoWP2BObjEnergyRowChosen * 10;
                                                   //HCFrec.WP2rObjEnergyTimes1000 = ViewHologInfoWP2RObjEnergyRowChosen * 4000;
                                                   //HCFrec.WP2gObjEnergyTimes1000 = ViewHologInfoWP2GObjEnergyRowChosen * 4000;
                                                   //HCFrec.WP2bObjEnergyTimes1000 = ViewHologInfoWP2BObjEnergyRowChosen * 4000;
                                                   HCFrec.RefBeamTiltTimes1000      = ViewHologInfoRefBeamTilt______RowChosen * 100;
                                                   HCFrec.LightCoordsDTimes1000     = ViewHologInfoLightCoordsD_____RowChosen * 100;
                                                   HCFrec.LightCoordsOmegaTimes1000 = ViewHologInfoLightCoordsOmegayRowChosen * 100;
                                                   HCFrec.nNextLineToPrint = 0;
                                                 //memcpy(&(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab),&(PCFrec.chNoofUsteps4EachMotorTab),40);
                                                   memcpy(&(HCFrec.HCF_PCFrecCopy),&PCFrec,sizeof(PCFrec));
                  //Generate all the movement profiles here.
                                                   ShowWindow(hProgess, SW_SHOW);
                                                   SendMessage(hProgess, LB_RESETCONTENT,0,0);
                                                   //for(int t = 0; t < 18;t++) to test scrolling
                                                   SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"Motor Postion data is being generated..." );
                                                   int nTopIndexCount = 1;
                                                   BOOL bGenerationErrorOccurred = FALSE;
                                                   UpdateWindow(hProgess);
                                                   for(int eAchM = 3; eAchM < 28; eAchM += 12){ //Fill in Start positions for the static motors: 4, 16, 28 - Length of Object Beam
                                                      int nStepsPerMilliMeter = 400 * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[eAchM]; //Equivalent to " * 200 /5" where 200 is the steps per rev for the 'normal' motors. 0.5 is the thread pitch of 'normal' motors
                                                      if(eAchM == 3)
                                                        nStepsPerMilliMeter = (int)( (double)nStepsPerMilliMeter * HCFrec.ObjBeamLengthR / 1000 );
                                                      else if(eAchM == 15)
                                                        nStepsPerMilliMeter = (int)( (double)nStepsPerMilliMeter * HCFrec.ObjBeamLengthG / 1000 );
                                                      else //eAchM == 27
                                                        nStepsPerMilliMeter = (int)( (double)nStepsPerMilliMeter * HCFrec.ObjBeamLengthB / 1000 );
                                                      for(int nSPos = 0; nSPos < MAX_NOOFLINES; nSPos++)
                                                        nLineXstartPositionsFEM[eAchM][nSPos] = nStepsPerMilliMeter;
                                                     }
                                                   if(GenerateProfileForM39(hDlg, HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams,
                                                                                  HCFrec.uiNoofCols,//When Summed (-1) with LeadIn and LeadOut, equals true number of cols (as far as motion is concerned).The true
                                                                                  HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[38],// number of cols are returned in the global nLengthOfM39rowsIncLeadInOut_inPixels
                                                                                  HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                  HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38],
                                                                                  HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                  &HCFrec.uTimeStamp,
                                                                                  HCFrec.szHologramName,
                                                                                  HCFrec.XposOfHologsOriginOnPlateMM,
                                                                                  HCFrec.nNoofRowsBetweenBeams,
                                                                                  HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH,
                                                                                  &HCFrec.nPixelAtwhichAccelStops) ){
                                                      //MessageBox(hDlg,"GenerateProfileForM39() failed",NULL,MB_OK);
                                                      ShowWindow(hProgess, SW_HIDE);
                                                      break;
                                                     }
                                                 /*if(GenerateStartPositionsFile(hDlg, 38)){
                                                      MessageBox(hDlg,"GenerateStartPositionsFile38() failed",NULL,MB_OK); // System will be more responsive, however, if the file is created here.
                                                      ShowWindow(hProgess, SW_HIDE);
                                                      break;
                                                     }*/
                                                   SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"GenerateProfileForM39 done" );
                                                   SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                   UpdateWindow(hProgess);
                                                   int nM2RGBidx = 1;
                                                   for( ; nM2RGBidx < 26; nM2RGBidx += 12){//Set nM2RGBidx to 1, 13, or 25
                                                      if( GenerateEnergyProfileForWP3(hDlg, nM2RGBidx, //Probably needs Other Profiles to be generated first (e.g. to calc the "incidences of light"?)
                                                      //Note the above function now simply returns.  For the time being, energy files are to be supplied by excel
#if 1
                                                                                            3,      //int    nLines
                                                                                            100,    //int    cols
                                                                                            20,     //int    nLeadInAndOut
                                                                                            4.39,   //double dblMinVolts        //Use Volts to generate dummy data
                                                                                            7.39) ){//double dblMaxVolts
#else
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.uiNoofCols,

#endif
                                                         //For this dummy version, I've supplied min and Max voltages
                                                         //MessageBox(hDlg,"GenerateProfileForWP3() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM2RGBidx = 999;
                                                         break;
                                                        }
                                                      else{
#if 1
                                                       //sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d created dummy file",nM2RGBidx+1);
                                                         sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d skipped - cse to be supplied",nM2RGBidx+1);
#else
                                                         sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d done",nM2RGBidx+1);
#endif
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
                                                     
                                                   int nM3RGBidx = 2;
                                                   for( ; nM3RGBidx < 27; nM3RGBidx += 12){//Set nM3RGBidx to 2, 14, or 26
                                                      if( GenerateEnergyProfileForWP3(hDlg, nM3RGBidx, //Probably needs Other Profiles to be generated first (e.g. to calc the "incidences of light"?)
                                                      //Note the above function now simply returns.  For the time being, energy files are to be supplied by excel
#if 1
                                                                                            3,      //int    nLines
                                                                                            100,    //int    cols
                                                                                            20,     //int    nLeadInAndOut
                                                                                            4.39,   //double dblMinVolts        //Use Volts to generate dummy data
                                                                                            7.39) ){//double dblMaxVolts
#else
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.uiNoofCols,

#endif
                                                         //For this dummy version, I've supplied min and Max voltages
                                                         //MessageBox(hDlg,"GenerateProfileForWP3() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM3RGBidx = 999;
                                                         break;
                                                        }
                                                      else{
#if 1
                                                       //sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d created dummy file",nM3RGBidx+1);
                                                         sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d skipped - cse to be supplied",nM3RGBidx+1);
#else
                                                         sprintf_s(mes,SIZMES,"GenerateEnergyProfileForWP%d done",nM3RGBidx+1);
#endif
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
#if 1
                                                   if( GenerateProfileForM40(hDlg, HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams,
                                                                                   HCFrec.uiNoofCols,
                                                                                   HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                   HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[39],
                                                                                   HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                   &HCFrec.uTimeStamp,
                                                                                   HCFrec.szHologramName,
                                                                                   HCFrec.YposOfHologsOriginOnPlateMM) ){
                                                      //MessageBox(hDlg,"GenerateProfileForM40() failed",NULL,MB_OK);
                                                      ShowWindow(hProgess, SW_HIDE);
                                                      break;
                                                     }
                                                 /*if(GenerateStartPositionsFile(hDlg, 39)){
                                                      MessageBox(hDlg,"GenerateStartPositionsFile39() failed",NULL,MB_OK); // System will be more responsive, however, if the file is created here.
                                                      ShowWindow(hProgess, SW_HIDE);
                                                      break;
                                                     }*/
                                                   SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"GenerateProfileForM40 done" );
#else
                                                   SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"GenerateProfileForM40 skipped" );
#endif
                                                   SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                   UpdateWindow(hProgess);
                                                   int nM11RGBidx = 10;
#if 1
                                                   for( ; nM11RGBidx < 35; nM11RGBidx += 12){//Set nM11RGBidx to 10, 22 & 34
                                                      if( GenerateProfileForM11(hDlg, nM11RGBidx,
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM11RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM11RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"G enerateProfileForM11() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM11RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM11RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
#else
                                                   SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"G enerateProfileForM11, M23, M35 skipped" );
                                                   SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                   UpdateWindow(hProgess);

#endif
                                                   if(nM11RGBidx > 998) //G enerateProfileForM11 failed
                                                     break;


                                                   int nM12RGBidx = 11;
                                                   for( ; nM12RGBidx < 36; nM12RGBidx += 12){//Set nM12RGBidx to 11, 23 & 35
                                                      if( GenerateProfileForM12(hDlg, nM12RGBidx,
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM12RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM12RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"G enerateProfileForM12() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM12RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM12RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
                                                   if(nM12RGBidx > 998) //G enerateProfileForM12 failed
                                                     break;                                  
//Silly repeats 1
                                                   int nM7RGBidx = 6;
                                                   for( ; nM7RGBidx < 36; nM7RGBidx += 12){//Set nM7RGBidx to 6, 18 & 30
                                                      //myOpen_File_VariableAp_CSV("C:\\Documents and Settings\\Just Me\\My Documents\\TestDouble.csv", 19, 6); Now called early on from within the following function:
                                                      if( GenerateProfileForM12(hDlg, nM7RGBidx,  
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM7RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM7RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"GenerateProfileForM7() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM7RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM7RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
                                                   if(nM7RGBidx > 998) //G enerateProfileForM7 failed
                                                     break;
//Silly repeats 2
                                                   int nM8RGBidx = 7;
                                                   for( ; nM8RGBidx < 36; nM8RGBidx += 12){//Set nM8RGBidx to 7, 19 & 31
                                                      //myOpen_File_VariableAp_CSV("C:\\Documents and Settings\\Just Me\\My Documents\\TestDouble.csv", 19, 6); Now called early on from within the following function:
                                                      if( GenerateProfileForM12(hDlg, nM8RGBidx,
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM8RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM8RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"GenerateProfileForM8() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM8RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM8RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
                                                   if(nM8RGBidx > 998) //G enerateProfileForM8 failed
                                                     break;
//Silly repeats 3
                                                   int nM9RGBidx = 8;
                                                   for( ; nM9RGBidx < 36; nM9RGBidx += 12){//Set nM9RGBidx to 8, 20 & 32
                                                      //myOpen_File_VariableAp_CSV("C:\\Documents and Settings\\Just Me\\My Documents\\TestDouble.csv", 19, 6); Now called early on from within the following function:
                                                      if( GenerateProfileForM12(hDlg, nM9RGBidx,  
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM9RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM9RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"GenerateProfileForM9() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM9RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM9RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }
                                                   if(nM9RGBidx > 998) //G enerateProfileForM9 failed
                                                     break;
//Silly repeats 4
                                                   int nM10RGBidx = 9;
                                                   for( ; nM10RGBidx < 36; nM10RGBidx += 12){//Set nM10RGBidx to 9, 21 & 33
                                                      //myOpen_File_VariableAp_CSV("C:\\Documents and Settings\\Just Me\\My Documents\\TestDouble.csv", 19, 6); Now called early on from within the following function:
                                                      if( GenerateProfileForM12(hDlg, nM10RGBidx,  
                                                                                      HCFrec.uiNoofLines,
                                                                                      HCFrec.nNoofRowsBetweenBeams,
                                                                                      HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[nM10RGBidx],
                                                                                      HCFrec.uiNoofCols,
                                                                                      HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000,
                                                                                      HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nM10RGBidx],
                                                                                      HCFrec.uiHorzDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.uiVertDistBetwPixelCenters_nanoM,
                                                                                      HCFrec.RefBeamTiltTimes1000,
                                                                                      HCFrec.LightCoordsDTimes1000,
                                                                                      HCFrec.LightCoordsOmegaTimes1000,
                                                                                      &HCFrec.uTimeStamp,
                                                                                      HCFrec.szHologramName,
                                                                                      HCFrec.nNoofRowsBetweenBeams) ){
                                                         //MessageBox(hDlg,"GenerateProfileForM10() failed",NULL,MB_OK);
                                                         ShowWindow(hProgess, SW_HIDE);
                                                         bGenerationErrorOccurred = TRUE;
                                                         nM10RGBidx = 999;
                                                        }
                                                      else{
                                                         sprintf_s(mes,SIZMES,"Generate Profile For M%d done",nM10RGBidx+1);
                                                         SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)mes);
                                                         SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
                                                         UpdateWindow(hProgess);
                                                        }
                                                     }

                                                   if(nM10RGBidx > 998) //G enerateProfileForM10 failed
                                                     break;




                                                 /*if(GenerateStartPositionsFile(hDlg, 10)){ Do this inside G enerateProfileForM11()
                                                      MessageBox(hDlg,"GenerateStartPositionsFile11() failed",NULL,MB_OK); // System will be more responsive, however, if the file is created here.
                                                      ShowWindow(hProgess, SW_HIDE);
                                                      break;
                                                     }*/

                                                 //Generate all the start positions for each motor here.
                                                   if(!bGenerationErrorOccurred){
                                                      SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"Writing Start Positions file" );
                                                      SendMessage(hProgess, LB_SETTOPINDEX, nTopIndexCount++,0);
 char szLocalSPFfileName1[_MAX_DIR]; //"StartPositions.spf"
 StringCchCopy(szLocalSPFfileName1,_MAX_DIR, HCFcfs.szCSVDirName4Reading); //Normally a folder to read from, but when generating a hologram we write to it.
 StringCchCat(szLocalSPFfileName1,_MAX_DIR,DEFAULT_BIN_OUTFILENAMESTARTPOSITIONS);//"StartPositions.spf"
 mySave_File (hDlg, MAX_NOOFLINES * NOOFMOTORS * sizeof(int), "spf",0, szLocalSPFfileName1, MOTORNO_NOT_USED);
                                                      //SendMessage(hProgess, LB_ADDSTRING,0,(LPARAM)"GenerateStartPositionsFile done" );
                                                      //UpdateWindow(hProgess); No Point is displaying... Dialogbox ends before user sees the message
                                                      ShowWindow(hProgess, SW_HIDE);
           /*char szBatchBuffer[256];
             sprintf_s(szBatchBuffer,256, "%s%s", HCFcfs.szCSVDirName4Reading, "CopyFiles.bat");
             if( _access(szBatchBuffer, 0) == 0){ //Make certain the batch file exists
                struct _stat64i32 status;         //Path exists, but is it a file?
                _stat(szBatchBuffer, &status);
                if((status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
                   MessageBox(hDlg,"CopyFiles.bat is a Folder!","An Essential file was missing.. please rectify this",MB_OK);
                   break;
                  }
               }
             else{//The path doesn't exist, so create it.
                MessageBox(hDlg,"Huh!\n\nDon't forget it can't be copied without changing the paths within the file","Essential file 'CopyFiles.bat' was missing!",MB_OK);
                break;
               }
             system(szBatchBuffer);*/
                                                      if( bCheckAllDynamicFilesExist(hDlg) ) //Paranoia! - Returns true if any files don't exist
                                                        break;
                                                      HCFrec.bHCFIsComplete = 1;
                                                      //SetPointWP2red = (double)HCFrec.WP2rObjEnergyTimes1000/1000; //SetPoints are no longer looked at
                                                      //SetPointWP2gre = (double)HCFrec.WP2gObjEnergyTimes1000/1000; //SetPoints are no longer looked at
                                                      //SetPointWP2blu = (double)HCFrec.WP2bObjEnergyTimes1000/1000; //SetPoints are no longer looked at
                                                      StringCchCopy(szLocalVHI_HCFfileName, _MAX_DIR, stdstring(LastHGramFileOpened).c_str());
HCFrec.nLengthOfM39rowsWhenGenerated_inPixels = nLengthOfM39rowsIncLeadInOut_inPixels;
HCFrec.nLeadInForM39 = (nLengthOfM39rowsIncLeadInOut_inPixels + 1 - HCFrec.uiNoofCols) / 2; //Derivable, but added
HCFrec.nNoofLinesIncludingBetweemTheBeams = HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams; // for efficiency.
                                                      if( !mySave_File(hDlg, 1000,"hcf",0, szLocalVHI_HCFfileName, MOTORNO_NOT_USED) ){                              // is already set
                                                         StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");             //These 3
                                                         StringCchCat (szGLWndTitle,SIZszGLWndTitle,szLocalVHI_HCFfileName);   // lines are
                                                         SetWindowText(hGLWnd,szGLWndTitle);                                   //  probably unnecessary ???
                                                         EndDialog(hDlg, LOWORD(wParam));
                                                        }
                                                   //else file didn't save, but a message will have been displayed to user
                                                     }
                                                  }
                                               }
                                            }
                                         }
                                      }
                                   }
                                }
                             }
                          }
                       }
                    }
                 }
              }
           }
           break;
         case IDB_VIEWHOLOGINFO_EXPLORE_CSVs:
           StringCchCopy(szExplorerDir2StartIn + 3,_MAX_DIR - 3,HCFcfs.szCSVDirName4Reading); //rem szExplorerDir2StartIn is initialised to "e, ")
           if( lstrlen(szExplorerDir2StartIn) > 10 ){ //e.g. "e, C:\a.hcf"
              char *ptrBslash;
              ptrBslash = strrchr(szExplorerDir2StartIn,BACKSLASH);              //reverse search for the final backslash
              if(ptrBslash > (szExplorerDir2StartIn + 5)){
                 szExplorerDir2StartIn[(int)(ptrBslash-szExplorerDir2StartIn + 1)]=0; //szExplorerDir2StartIn now holds the CSVsOUT folder
                 PostMessage(hGLWnd,WM_COMMAND,IDM_EXEC_EXPLORER,0);
 //The following function call uses the logic in G enerateProfileForM11() to form the filename of the appropriate Acceleration File, then loads it into an editor before returning.
 //This is solely to help when debugging, so should be removed. The parameter 123456789 tells to return after loading the file
/*               G enerateProfileForM11(hDlg, 10, 123456789,
                      HCFrec.nNoofRowsBetweenBeams, 1, HCFrec.uiNoofCols, HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[10],
                      HCFrec.uiHorzDistBetwPixelCenters_nanoM, HCFrec.uiVertDistBetwPixelCenters_nanoM, HCFrec.RefBeamTiltTimes1000, HCFrec.LightCoordsDTimes1000, HCFrec.LightCoordsOmegaTimes1000, &HCFrec.uTimeStamp,
                      HCFrec.szHologramName,HCFrec.nNoofRowsBetweenBeams); // */
                }
              else
                MessageBox (hDlg,"Valid foldername4z needed, for Explorer to explore this folder",NULL,MB_OK);
             }
           else
             MessageBox (hDlg,"Valid foldername3z needed, for Explorer to explore this folder","In View Holog Info",MB_OK);
           break;
         case IDB_VIEWHOLOGINFO_VIEWSTARTPOSNS:
           DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWMOTORSTARTPOINTSINHCF), hDlg, ViewMotorStartPointsInHCF_Dialog);
           _itoa_s(HCFrec.nNextLineToPrint + 1,szDum,SIZSZDUM12,10);
           SetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_LINE2STARTFROM, szDum);
           break;
         case IDB_VIEWHOLOGINFO_RESET:
           for (char emVHIR = 0;emVHIR < NOOFMOTORS; emVHIR++)
             if(PCFrec.bMotor2ExpectRepliesFrom[emVHIR])
               SendMessagePackets(hDlg, emVHIR, MsTyp4_RESET,        0,       0);  //0x04 (RESET Message)
           break;
         case IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS:
            if(HCFrec.bThisHoloGramHasABadError){
               StringCchCopy(mes, SIZMES, "Warning HCFrec Error Code: ");
               _itoa_s(HCFrec.bThisHoloGramHasABadError, mes+27,_MAX_DIR - 27,10);
               if( IDNO == MessageBox(hDlg,"This hologram is tagged as having the above bad error.\n\nAre you sure you want to try moving to start positions?",mes,MB_YESNO|MB_DEFBUTTON2) )
                 break;
              }
            if(PrinterConfigMatchesHCFconfig(NULL)){  //Check uSteps/Step is still the same (for all motors) as when the hologram control data was generated (& other fields)
               bUserDecidedNotToPrintOrMove = TRUE;   //If uSteps/Step NOT the same, returns 1. If something else NOT the same return 2 or 3
               DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWPCF_VERSUS_HCFCONFIG), hDlg, ViewPCFversusHCF_Dialog);
               if(bUserDecidedNotToPrintOrMove)
                 break;
              }
         //NOTE NO BREAK!!! This next case is presently only to cater for when M oveToStartPositions returns true (which means it wanted to be restarted [by the PostMessage()])
         case IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS2:
            if(MoveToStartPositions(hDlg,/*bMove2SPsFast,*/ bSeeMessages, bMove2SPsHomeRegardless, bDONTSTOP_WHEN_HOMED)) //bSeeMessages is set by a radio button.  If func return TRUE, user cleared RS485 output: the
              PostMessage(hDlg,WM_COMMAND,IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS2,0);//the clear commands were sent, but we'll start the function afresh
           break;
//       case IDB_VIEWHOLOGINFO_SHOWSTATUS:
         //if(!bSTATREC_BOXisBeingDisplayed)
//           SendMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
         //BringWindowToTop(hDlg);
//         break;
         case IDB_VIEWHOLOGINFO_PRINT_HOLOGRAMwDEBUG:
         case IDB_VIEWHOLOGINFO_PRINT_HOLOGRAM:{//It currently the users responsibility to ensure the motors are at their start positions... this function merely checks they are (and warns the user if they're not.)
            if(PrinterConfigMatchesHCFconfig(NULL)){  //Check uSteps/Step is still the same (for all motors) as when the hologram control data was generated (& other fields)
               bUserDecidedNotToPrintOrMove = TRUE;   //If uSteps/Step NOT the same, returns 1. If something else NOT the same return 2 or 3
               DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEWPCF_VERSUS_HCFCONFIG), hDlg, ViewPCFversusHCF_Dialog);
               if(bUserDecidedNotToPrintOrMove)
                 break;
              }
            if(HCFrec.bThisHoloGramHasABadError){
               StringCchCopy(mes, SIZMES, "Warning HCFrec Error Code: ");
               _itoa_s(HCFrec.bThisHoloGramHasABadError, mes+27,_MAX_DIR - 27,10);
               MessageBox(hDlg,"This hologram is tagged as having the above bad error.\n\nYou cannot print until this is rectified",mes,MB_OK);
               break;
              }
          //  double dblStepsPerNanoM = (double)500.0f * HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38] / (XAXIS_PITCH * 1000000); //500 is the steps per rev for the 5 phase stage motors.
          //  int nSpeedAtStartOfLine = (int)(HCFrec.uiHorzDistBetwPixelCenters_nanoM * dblStepsPerNanoM);
          ////if(HCFrec.uiHorzDistBetwPixelCenters_nanoM < HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH){
          //  if(nSpeedAtStartOfLine < HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH){
          //     MessageBox(hDlg,"M39 X-AXIS BACKLASH exceeds the spacing between pixels\n\nEither (or both)\n-increase the pixel spacing\n-decrease the backlash\n  and regenerate\nor rewrite the software!\n\n(more info follows)","Excessive Backlash setting was specified",MB_OK);
          //     sprintf_s(mes,SIZMES,"uSteps between pixels (with your current pixel spacing) is %d\n\nBacklash setting when hologram was generated was %d\n\nCurrent Backlash setting is %d",nSpeedAtStartOfLine,HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH, PCFrec.nM39_XAXISBACKLASH);
          //     MessageBox(hDlg,mes,"further info",MB_OK);
          //     break;
          //    }
            int ArrayMotorIsInRightPlace[NOOFMOTORS];
          //memset(ArrayMotorIsInRightPlace, 0, sizeof(ArrayMotorIsInRightPlace));
            for(int eaM = 0; eaM < NOOFMOTORS; eaM++)
              if(PCFrec.bMotorIS_A_PID[eaM])
                ArrayMotorIsInRightPlace[eaM] = 1;
              else
                ArrayMotorIsInRightPlace[eaM] = 0;
//int NeedToTELLavrWhichLineToPrintFromAsWell;
            if( bMotorsAreInTheirStartPositionsANDhappy(hDlg, ArrayMotorIsInRightPlace, HCFrec.nNextLineToPrint) ){ //and happy!
          //int OPUTbackAbove;
          //if( TRUE ){ //and happy!
               StopPIDMovementWhileHomingOrFileTransfering();

               //bTimerHasBeenKilled = TRUE; //I think setting this is sufficient - actually killing the timer is unnecessary, just so long as it is ignored
Sleep(100);

               if(Transfer_ALL_FilesAndPerform(hDlg, bMoveNowRadioButPressed, HCFcfs.szCSVDirName4Reading, (unsigned short)HCFrec.nNextLineToPrint)){ //The only place this is called
//Sleep(2000);
//This seems like a reasonable place to read in the  Energy Files.
                  int ReadingEnergyFilesWouldBeQuickerIfTheyWereCSEfiles;
                  char szLocalM2or3ProfileFileName[_MAX_DIR];
                  int readFromExcelM2or3Error = 0;
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3r.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 0, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 3;
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3g.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 1, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 15;
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 2, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 27;
                  
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2r.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 3, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 2;
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2g.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 4, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 14;
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 5, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 26;

                  int nCSretCode = bCheckAlltheChecksumsAreOK(hDlg, DONT_CHECK_STATIC_MOTOR_CSUMS);
                  //bTimerHasBeenKilled = FALSE;
                  restartPIDs();//In IDB_VIEWHOLOGINFO_PRINT_HOLOGRAM, pids are restarted after T ransfer_ALL_FilesAndPerform() regardless of if the function suceeds of fails
                  if(readFromExcelM2or3Error){
                     StringCchCopy(mes, SIZMES, "Motor: ");
                     _itoa_s(readFromExcelM2or3Error, mes+7,_MAX_DIR - 7,10);
                     MessageBox (hDlg,"@3 After Transfer-ALL-FilesAndPerform() returned OK\n\n There was a problem reading an Energy File",mes,MB_OK);
                    }
                  else if(nCSretCode){
                     if(nCSretCode < 1000){
                        char szWindowsCS[12];
                        StringCchCopy(mes, SIZMES, "Motor: ");
                        _itoa_s(nCSretCode, mes+7,_MAX_DIR - 7,10);
                        StringCchCat(mes, SIZMES, " with Windows Checksum: ");
                        _ultoa_s(uiTableOfCheckSums[nCSretCode-1], szWindowsCS, 12, 10);
                        StringCchCat(mes, SIZMES, szWindowsCS);
                        MessageBox (hDlg,"@1 After Transfer-ALL-FilesAndPerform() returned OK\n\n  bCheckAlltheChecksumsAreOK() found a mismatch!\n\nCheck you haven't also got any other warnings as well\n(as these will probably indicate the cause)",mes,MB_OK);
                       }
                     else{
                        StringCchCopy(mes, SIZMES, "Error: whilst getting checksum for Motor: ");
                        _itoa_s(nCSretCode - 1000, mes+42,_MAX_DIR - 42,10);
                        MessageBox (hDlg,"CheckAlltheChecksumsAreOK() got no reply within the time allowed @ 1.",mes,MB_OK);
                       }
                    }
                  else{ //T ransfer_ALL_FilesAndPerform succeeded() & b CheckAlltheChecksumsAreOK() did too, so Exit the dialog box.
                   //int n LeadInForM39m1 = (HCFrec.nLengthOfM39rowsWhenGenerated_inPixels + 1 - HCFrec.uiNoofCols) / 2;
                   //int n LeadInForM39m1 = HCFrec.nLeadInForM39 - 1;
                   //n LeadInForM39m1 -= 1;
                     if(IDB_VIEWHOLOGINFO_PRINT_HOLOGRAMwDEBUG != LOWORD(wParam)){
                       RunControlBoxProgram(hDlg, PARSE, HCFrec.nLengthOfM39rowsWhenGenerated_inPixels, //nLineLength
                         HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams,                //nTotalLines         //This should be derived from nShutterRedStopLine and nColorOffset!
                         HCFrec.nLeadInForM39 - 1,                                             //ShutterStartPixel (The LeadIn minus One)
                         HCFrec.nLeadInForM39 - 1 + HCFrec.uiNoofCols,                         //ShutterStopPixel  (The LeadIn minus One + Number Of Columns)
                         0,                                                                    //ShutterRedStartLine (Should be Zero [perhaps it will be something else when resuming prints?])
                         HCFrec.uiNoofLines,                                                   //nShutterRedStopLine
                         HCFrec.nNoofRowsBetweenBeams,                                         //nColorOffset
                         1000,
                         1);                                                                //nLineDelay         //This Program doesn't know
                        RunControlBoxProgram(hDlg, START_PRINT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,1);
                       }
                     else
                       RunControlBoxProgram(hDlg, PARSE, HCFrec.nLengthOfM39rowsWhenGenerated_inPixels, //nLineLength
                         HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams,                //nTotalLines         //This should be derived from nShutterRedStopLine and nColorOffset!
                         HCFrec.nLeadInForM39 - 1,                                             //ShutterStartPixel (The LeadIn minus One)
                         HCFrec.nLeadInForM39 - 1 + HCFrec.uiNoofCols,                         //ShutterStopPixel  (The LeadIn minus One + Number Of Columns)
                         0,                                                                    //ShutterRedStartLine (Should be Zero [perhaps it will be something else when resuming prints?])
                         HCFrec.uiNoofLines,                                                   //nShutterRedStopLine
                         HCFrec.nNoofRowsBetweenBeams,                                         //nColorOffset
                         1000,
                         0);                                                                //nLineDelay         //This Program doesn't know
                   //RunControlBoxProgram(hDlg, START_PRINT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                     EndDialog(hDlg, LOWORD(wParam));
                    }
                 }
               else{ //T ransfer_ALL_FilesAndPerform() failed
                  //bTimerHasBeenKilled = FALSE;
                  restartPIDs();//In IDB_VIEWHOLOGINFO_PRINT_HOLOGRAM, pids are restarted after T ransfer_ALL_FilesAndPerform() regardless of if the function suceeds of fails
                 }
              }
            else{  //b MotorsAreInTheirStartPositionsANDhappy() may end up displaying this message:
               int nMotorThatIsntInStartPos = 999;
               for(int eaM = NOOFMOTORSm1; eaM > -1; eaM--)
                 if(ArrayMotorIsInRightPlace[eaM] == 0)       //REM ArrayMotorIsInRightPlace[] was passed to b MotorsAreInTheirStartPositionsANDhappy(), and tells us
                   nMotorThatIsntInStartPos = eaM;             // which motor was not in the right place.
               if(nMotorThatIsntInStartPos == 999)
                 MessageBox(hDlg,"b_MotorsAreInTheirStartPositionsANDhappy() was false but the offending motor couldn't be found!",NULL,MB_OK);
               else{
                  mes[0] = 'M';
                  _itoa_s(nMotorThatIsntInStartPos + 1,mes+1,SIZMES-1,10);
                  MessageBox (hDlg,"All motors have to be in the correct starting position for the\nline that is about to be printed before printing can start.\n\nDid you press Move to Start Positions?\nPerhaps the motor(s) is/are still moving to position?\n\nThe first 'Out of Position' motor is shown above.",mes,MB_OK); 
                 }
              }
           }
           break;
         case IDB_TRANSBLOCK_MOVENOW_RADIO: //This is copied from TransBlockDialog.cpp - and must be removed!
           if( bMoveNowRadioButPressed ){
              bMoveNowRadioButPressed = FALSE;
              CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,FALSE);
             }
           else{
              bMoveNowRadioButPressed = TRUE;
              CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO);
             }
           break;
         case IDC_VIEWHOLOGINFO_prePIXzeroOTHERS:
           if(HCFrec.bHCFIsComplete)
             DialogBox(hInst, MAKEINTRESOURCE(IDD_VHI_OTHER_PRINTING), hDlg, VHI_OtherPrePixZeroConsts_Dialog);
           else
             DialogBox(hInst, MAKEINTRESOURCE(IDD_VHI_OTHER_EDITING), hDlg, VHI_OtherPrePixZeroConsts_Dialog);
           break;
         case IDCANCEL:
           //The following lines are difficult to make work properly without confusing the user as to why, sometimes, they're told about changes, but not other times.  The reason for this is that the fields
           // they interract with are normally read when they press generate, and then the changes are applied to HCFrec.  If the generation fails the code below can detect if the HCFrec has changed.  But
           // if they've only pressed cancel (having changed some fields, but not pressed 'generate') the code won't see the changes (and we don't want to get into reading the fields here because they might
           // contain invalid data).  It's best that cancel simply undoes all changes - always, and consistently.  If they want to keep their changes, they should press 'Save Incomplete' or 'Generate, Complete, & Save'!
           //if( memcmp((void*)&HCFrec,(void*)&VHICopyOfOrigHCFrec, sizeof(HCFRECORD)) )
           //  if(IDNO == MessageBox(hDlg,"Changes have been made to the hologram configuration\nand these will be lost if you cancel.\n\nIs it OK to lose these changes?","Please confirm...",MB_YESNO))
           //    break;
           HCFrec = VHICopyOfOrigHCFrec;
           //SetPointWP2red = (double)HCFrec.WP2rObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           //SetPointWP2gre = (double)HCFrec.WP2gObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           //SetPointWP2blu = (double)HCFrec.WP2bObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}

/* time_t        uTimeStamp;             //nb 64-bit (now 32)
   BOOL          bHCFIsComplete;
   char          szHologramName[44];
   unsigned int  uiNoofLines;
   unsigned int  uiNoofCols;                //eg 2000
   unsigned int  nNoofColsPlusLeadInOut;   //eg 2200
   unsigned int  uiPixelSizeH_uMetres;
   unsigned int  uiPixelSizeV_uMetres;
   unsigned int  uiHorzDistBetwPixelCenters_nanoM;
   unsigned int  uiVertDistBetwPixelCenters_nanoM;
   unsigned int  uiFrequencyOfPixelPrintsTimes1M;  //The refresh rate of the LCD screen time 1,000,000. Currently, Only 10M 15M 30M & 60M exactly are allowed
   int           XposOfHologsOriginOnPlateMM;
   int           YposOfHologsOriginOnPlateMM;
   int           WP1rRatioTimes1000;
   int           WP1gRatioTimes1000;
   int           WP1bRatioTimes1000;
   int           RefBeamTiltTimes1000;
   int           LightCoordsDTimes1000;
   int           LightCoordsOmegaTimes1000;
   EDITTEXT      IDC_VIEWHOLOGINFO_HOLOGNAME,147,8,165,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_TIMESTAMP,147,29,165,12,ES_AUTOHSCROLL | ES_READONLY
   EDITTEXT      IDC_VIEWHOLOGINFO_NOOFLINES,236,50,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_NOOFCOLS,236,71,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_NOOFCOLSpLEADINOUT,236,92,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_PIXSIZE_H,236,113,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_PIXSIZE_V,236,134,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_HORZPIXDIST,236,155,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_VERTPIXDIST,236,176,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_PRINTFREQ,236,197,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_XPOS_ORG_STEPS,236,218,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_YPOS_ORG_STEPS,236,239,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_WP1_R_RATIO,236,260,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_WP1_G_RATIO,236,281,76,12,ES_AUTOHSCROLL
   EDITTEXT      IDC_VIEWHOLOGINFO_WP1_B_RATIO,236,302,76,12,ES_AUTOHSCROLL
   */
BOOL bErrorCheckAllVIEWHOLOGINFOFields(HWND hDlg){

int ALL_RANGE_LIMITS_NEED_2B_DECIDED_BY_OPTIC;

 int nNumberInField;
 BOOL bWasTranslated;
 //Timestamp will need to be generated (probably as part of hologram mpos data calculation), see  //time( &uTimeStamp );
 GetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_HOLOGNAME, HCFrec.szHologramName, sizeof(HCFrec.szHologramName) - 1);
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_NOOFLINES, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 2) || (nNumberInField > MAX_NOOFLINES) ){
       MessageBox(hDlg,"NOOFLINES must be between 2 and 2000",NULL,MB_OK);//dependent on MAX_NOOFLINES
       return 1;
      }
    else
      HCFrec.uiNoofLines = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read NOOFLINES (Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_NOOFCOLS, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 2) || (nNumberInField > 2000) ){
       MessageBox(hDlg,"NOOFCOLS must be between 2 and 2000",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiNoofCols = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read NOOFCOLS (Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_NOOFCOLSpLEADINOUT, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 1) || (nNumberInField > 2200) ){
       MessageBox(hDlg,"NOOFCOLSpLEADINOUT must be between 1 and 2200",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.nNoofColsPlusLeadInOut = nNumberInField; //HCFrec.nNoofColsPlusLeadInOut IS NOT USED Just Read and Redisplayed 
   }
 else{
    MessageBox(hDlg,"Couldn't Read NOOFCOLSpLEADINOUT (Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_PIXSIZE_H, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 500) || (nNumberInField > 2000) ){
       MessageBox(hDlg,"PIXSIZE (uMetres) must be between 500 and 2000\n(Remember, 1000 uMetres is 1 millimetre!)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiPixelSizeH_uMetres = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read PIXSIZE_H (uMetres - Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_PIXSIZE_V, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 500) || (nNumberInField > 2000) ){
       MessageBox(hDlg,"PIXSIZE (uMetres) must be between 500 and 2000\n(Remember, 1000 uMetres is 1 millimetre!)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiPixelSizeV_uMetres = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read PIXSIZE_V (uMetres - Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_HORZPIXDIST, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 500000) || (nNumberInField > 2500000) ){
       //int WarningAboveFigureShouldBe500000;//and now is (i.e. I can remove this warning)
       MessageBox(hDlg,"HORZPIXDIST (nano Metres) must be between 500,000 and 2,500,000\n(Remember, 1,000,000 nano Metres is 1 millimetre!)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiHorzDistBetwPixelCenters_nanoM = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read HORZPIXDIST (nano - Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_VERTPIXDIST, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 500000) || (nNumberInField > 2500000) ){
       // int WarningAboveFigureShouldAlsoBe500000;//and now is (i.e. I can remove this warning)
       MessageBox(hDlg,"VERTPIXDIST (nano Metres) must be between 500,000 and 2,500,000\n(Remember, 1,000,000 nano Metres is 1 millimetre!)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiVertDistBetwPixelCenters_nanoM = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read VERTPIXDIST (nano - Numeric field)",NULL,MB_OK);
    return 1;
   }
 /*
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_PRINTFREQ, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 8000000) || (nNumberInField > 60000000) ){
       MessageBox(hDlg,"PRINTFREQ (times 1,000,000) must be between 8,000,000 and 60,000,000\n(i.e between 8 Hertz and 60 Hertz!)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.uiFrequencyOfPixelPrintsTimes1M = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read PRINTFREQ (times 1,000,000 - Numeric field)",NULL,MB_OK);
    return 1;
   }*/
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_XPOS_ORG_STEPS, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -1200) || (nNumberInField > 1200) ){
     //MessageBox(hDlg,"XPOS_ORG_STEPS must be between -1200 and +1200\nThis is No. of mm from home the XY stage must move\nto get to the start position)",NULL,MB_OK);
       MessageBox(hDlg,"XPOS_ORG_STEPS must be between -1200 and +1200\nThis is No. of mm from the plate's origin\nto the to the print position)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.XposOfHologsOriginOnPlateMM = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read XPOS_ORG_STEPS (Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_YPOS_ORG_STEPS, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -1200) || (nNumberInField > 1200) ){
       MessageBox(hDlg,"YPOS_ORG_STEPS must be between -1200 and +1200\nThis is No. of mm from the plate's origin\nto the to the print position)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.YposOfHologsOriginOnPlateMM = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read YPOS_ORG_STEPS (Numeric field)",NULL,MB_OK);
    return 1;
   }
 /*
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_WP1_R_RATIO, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 1000) || (nNumberInField > 99000) ){
       MessageBox(hDlg,"WP1rRatioTimes1000 (times 1,000) must be between 1,000 and 99,000\n(i.e between 1 % and 99 %)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.WP1rRatioTimes1000 = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read WP1rRatioTimes1000 (times 1,000 - Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_WP1_G_RATIO, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 1000) || (nNumberInField > 99000) ){
       MessageBox(hDlg,"WP1gRatioTimes1000 (times 1,000) must be between 1,000 and 99,000\n(i.e between 1 % and 99 %)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.WP1gRatioTimes1000 = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read WP1gRatioTimes1000 (times 1,000 - Numeric field)",NULL,MB_OK);
    return 1;
   }
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_WP1_B_RATIO, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 1000) || (nNumberInField > 99000) ){
       MessageBox(hDlg,"WP1bRatioTimes1000 (times 1,000) must be between 1,000 and 99,000\n(i.e between 1 % and 99 %)",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.WP1bRatioTimes1000 = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read WP1bRatioTimes1000 (times 1,000 - Numeric field)",NULL,MB_OK);
    return 1;
   }
 */
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENR, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -100000) || (nNumberInField > 100000) ){
       MessageBox(hDlg,"Object Beam Length Red must be between -100,000 and +100,000",NULL,MB_OK);
       return 1;
      }
    else
        HCFrec.ObjBeamLengthR = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read Object Beam Length Red field",NULL,MB_OK);
    return 1;
   }
 
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENG, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -100000) || (nNumberInField > 100000) ){
       MessageBox(hDlg,"Object Beam Length Green must be between -100,000 and +100,000",NULL,MB_OK);
       return 1;
      }
    else
        HCFrec.ObjBeamLengthG = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read Object Beam Length Green field",NULL,MB_OK);
    return 1;
   }
 
 nNumberInField = GetDlgItemInt(hDlg, IDC_VIEWHOLOGINFO_OBJ_BEAM_LENB, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -100000) || (nNumberInField > 100000) ){
       MessageBox(hDlg,"Object Beam Length Blue must be between -100,000 and +100,000",NULL,MB_OK);
       return 1;
      }
    else
      HCFrec.ObjBeamLengthB = nNumberInField;
   }
 else{
    MessageBox(hDlg,"Couldn't Read Object Beam Length Blue field",NULL,MB_OK);
    return 1;
   }


 return 0;
}

BOOL CreateNewHCFfile(HWND hDlg, char *pszCopyName, BOOL *bSillyNameMessShown){//Called either by
       // OpenConfigFile_Dialog (if user is creating a copy of a file already marked as 'complete')
       // or WndProc's IDM_NEW_CONFIGFILE (with pszCopyName set to NULL).
//If these are the only global data shared with myGetFolder, then presumably storing them before calling myGetFolder
// and restoring them after calling myGetFolder (if cancel was pressed) will fully undo anything myGetFolder does
//char ssGlobalFileName[_MAX_PATH];
 char szCopyOfGlobDefaultFolderShortName[60];
 char szCopyOfDirName[_MAX_PATH];
 //char szCopyOfConcatinatedFolderName[_MAX_PATH];
 char szLocalCreateNew_HCFfileName[_MAX_DIR];

 StringCchCopy( szCopyOfGlobDefaultFolderShortName, sizeof(szCopyOfGlobDefaultFolderShortName), szGlobDefaultFolderShortName);
 StringCchCopy( szCopyOfDirName, sizeof(szCopyOfDirName), szDirName);
 //StringCchCopy( szCopyOfConcatinatedFolderName, sizeof(szCopyOfConcatinatedFolderName), szConcatinatedFolderName); //Not sure why I didnt store this before - maybe not needed?

 //if(pszCopyName)
 StringCchCopy(szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), pszCopyName);
 //else
 //  StringCchCopy(szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), "You need to type the name here!");
//StringCchCopy( szDirName, sizeof(szDirName), "C:\\MyProjects\\HoloProg\\Debug\\Hgrams"); //This hard coded name causes problems if the application is moved
 StringCchCopy( szDirName, sizeof(szDirName), szHGRAMSDirName4Reading);

 *bSillyNameMessShown = 0;
 if ( myGetFolder(hDlg, IDS_FILTERSTRNGBMP) ){//This is the ONLY place myGetFolder() is called, & myGetFolder() sets up szConcatinatedFolderName (which is used only in the
    StripTrailingSpaces(szConcatinatedFolderName);// next few lines of code (so we needn't worry about it changing: it will be restored the next time myGetFolder is called)
    if( lstrlen(szGlobDefaultFolderShortName) > 42 )
      MessageBox (hDlg,"Hologram Name is too long (> 42 characters)",NULL,MB_OK);
    else if ( !lstrcmp(szGlobDefaultFolderShortName, "You need to type the name here!")
           || !lstrcmp(szGlobDefaultFolderShortName, NO_HOLOGRAM_NAME)  ){
       *bSillyNameMessShown = 1;
       MessageBox (hDlg,"This is a really silly name for a hologram.\n\nPlease try again and choose a different name!",NULL,MB_OK);
      }
    else if( _mkdir(szConcatinatedFolderName) ){
       StringCchCopy( szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), szCopyOfGlobDefaultFolderShortName); //restore
       StringCchCopy( szDirName, sizeof(szDirName), szCopyOfDirName);                                                          //restore
       MessageBox (hDlg,"Error - Unable to create the new folder (named above)",szConcatinatedFolderName,MB_OK);
      }
    else{//The New folder was created, now create the subdirectories, then write the hologram's hcf file into it (plus any other required files)
       StringCchCat(szConcatinatedFolderName,_MAX_DIR,"\\");  
       int nOrigLenConcFN = lstrlen(szConcatinatedFolderName);    //length includes the appended backslash
       StringCchCat(szConcatinatedFolderName,_MAX_DIR,"CSVs");
       if( _mkdir(szConcatinatedFolderName) ){
          StringCchCopy( szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), szCopyOfGlobDefaultFolderShortName); //restore
          StringCchCopy( szDirName, sizeof(szDirName), szCopyOfDirName);                                                          //restore
          MessageBox (hDlg,"Worrying Error - Able to create the hologram's new folder\nbut not the sub-folder (named above) with this!",szConcatinatedFolderName,MB_OK);
         }
       else{
          szConcatinatedFolderName[nOrigLenConcFN] = 0;     
          StringCchCat(szConcatinatedFolderName,_MAX_DIR,"CSVsOUT");
          if( _mkdir(szConcatinatedFolderName) ){
             StringCchCopy( szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), szCopyOfGlobDefaultFolderShortName); //restore
             StringCchCopy( szDirName, sizeof(szDirName), szCopyOfDirName);                                                          //restore
             MessageBox (hDlg,"Worrying Error - Able to create the hologram's new folder\nbut not the sub-folder (named above) with this!",szConcatinatedFolderName,MB_OK);
            }
          else{
             szConcatinatedFolderName[nOrigLenConcFN] = 0;
             StringCchCopy(szLocalCreateNew_HCFfileName,_MAX_DIR, szConcatinatedFolderName);
             StringCchCat(szLocalCreateNew_HCFfileName, _MAX_DIR, szGlobDefaultFolderShortName);  //Set up the hcf filename - It is Given the same name as the folder it is in
             StringCchCat(szLocalCreateNew_HCFfileName, _MAX_DIR, ".hcf");
             HCFrec.bHCFIsComplete = 0;
             time(&HCFrec.uTimeStamp);
             StringCchCopy(HCFrec.szHologramName,sizeof(HCFrec.szHologramName), szGlobDefaultFolderShortName); // & change the internal hologram name
             if( !mySave_File(hDlg, 1000,"hcf",0, szLocalCreateNew_HCFfileName, MOTORNO_NOT_USED) ){                       // is already set
                char *ptrBslash;
                StringCchCopy(HCFcfs.szCSVDirName4Reading,_MAX_DIR, szLocalCreateNew_HCFfileName);
                ptrBslash = strrchr(HCFcfs.szCSVDirName4Reading, BACKSLASH);     //reverse search for the final backslash
                HCFcfs.szCSVDirName4Reading[(int)(ptrBslash-HCFcfs.szCSVDirName4Reading + 1)]=0; //szCSVDirName4Reading now holds the directory the hcf file resides in.
                StringCchCopy(HCFcfs.szCSVDirName4Writing,_MAX_DIR,HCFcfs.szCSVDirName4Reading);
                StringCchCat (HCFcfs.szCSVDirName4Reading,_MAX_DIR,"CSVs\\");    //szCSVDirName4Reading should be changed every time a new hcf file is used in a different location
                StringCchCat (HCFcfs.szCSVDirName4Writing,_MAX_DIR,"CSVsOUT\\"); //szCSVDirName4Writing should be changed every time a new hcf file is used in a different location
                StringCchCopy(szGLWndTitle,SIZszGLWndTitle,"Holoprog: ");                 //These 3
                StringCchCat (szGLWndTitle,SIZszGLWndTitle,szLocalCreateNew_HCFfileName); // lines ARE
                SetWindowText(hGLWnd,szGLWndTitle);                                       //  necessary
                LastHGramFileOpened = szLocalCreateNew_HCFfileName;
                nShowHologInfoBecauseFileOpenedOK = 2;
                return TRUE; //EndDialog(hDlg, LOWORD(wParam));  returning TRUE means the calling function can end its dialog box
               }
            }
         }
      }
   }
 else{ //cancel was pressed in myGetFolder's called to
    StringCchCopy( szGlobDefaultFolderShortName, sizeof(szGlobDefaultFolderShortName), szCopyOfGlobDefaultFolderShortName);
    StringCchCopy( szDirName, sizeof(szDirName), szCopyOfDirName);
   }
 return FALSE;
}

#define SIZ_TwEC 56  //Size of szTitleWithErrorCode[]

//b CheckAlltheChecksumsAreOK() returns zero if all are ok, else returns motor No (plus 1) for first motor whose Checksum fails or
// motor No (plus 1001) if the a comms problem
int bCheckAlltheChecksumsAreOK(HWND hDlg, BOOL bCheckStaticMotorsCSumsAsWell){
 char eaM_cs;
 int nReturnCodeFromSendPackets;
 for(eaM_cs = 0; eaM_cs < NOOFMOTORS; eaM_cs++)
 //if(PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] && !bListOfStaticMotors[eaM_cs] && !PCFrec.bMotorIS_A_PID[eaM_cs]){
   if(PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] &&                                 !PCFrec.bMotorIS_A_PID[eaM_cs]){//If the caller is not interested in the static motors' checksums, there is still no harm in
      nReturnCodeFromSendPackets = SendMessagePackets(NULL, eaM_cs, MsTypD_SENDchecksumREC, ++cMessageSeqNoCS[eaM_cs], 0); // sending this message to them, but we'll consume, & ignore, their response.
      if(nReturnCodeFromSendPackets){ //"nReturnCodeFromSendPackets Error"
         if(nReturnCodeFromSendPackets == 5001)
           DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Send() function took > 0.3 seconds from Motor: ", "Error checking motors' CheckSums...");
         else{
            char szTitleWithErrorCode[SIZ_TwEC];
            StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"nReturnCodeFromSendPackets Error code: ");
            _itoa_s(nReturnCodeFromSendPackets, szTitleWithErrorCode + 39, SIZ_TwEC - 39, 10);
            DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Error111 occurred whilst checking motors' CheckSum, for Motor: ",szTitleWithErrorCode);
           }
         return eaM_cs + 1001;
        }
      Sleep(3);
     }
 if(bCheckStaticMotorsCSumsAsWell){    
    for(eaM_cs = 0; eaM_cs < NOOFMOTORS; eaM_cs++)
    //if( PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] && !bListOfStaticMotors[eaM_cs]  && !PCFrec.bMotorIS_A_PID[eaM_cs] && !bCSfromUC3isSameAsCSfromSendBuffer(hDlg, eaM_cs, nLine2StartFrom) )
      if( PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] &&                                  !PCFrec.bMotorIS_A_PID[eaM_cs] && !bCSfromUC3isSameAsCSfromSendBuffer(hDlg, eaM_cs) )
        return eaM_cs + 1; //As static motors can now be sent MPOS files (for fast moving, homing etc.) they may be asked for their checksums
   }
 else{
    for(eaM_cs = 0; eaM_cs < NOOFMOTORS; eaM_cs++)
      //It is VITAL that bCSfromUC3isSameAsCSfromSendBuffer() is called before checking !bListOfStaticMotors[eaM_cs] else the reply is not consumed and well get a sequence error later
      if( PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] &&                                  !PCFrec.bMotorIS_A_PID[eaM_cs] && !bCSfromUC3isSameAsCSfromSendBuffer(hDlg, eaM_cs) && !bListOfStaticMotors[eaM_cs]  )
        return eaM_cs + 1; //As static motors can now be sent MPOS files (for fast moving, homing etc.) they may be asked for their checksums
   }
 return 0;     // if PCFrec.bMotor2ExpectRepliesFrom[eaM_cs] is zero), bCSfromUC3isSameAsCSfromSendBuffer() will show the Checksums as Agreeing
}

//Returns TRUE if Checksum on UC3 agrees with Checksum on Windows PC
BOOL bCSfromUC3isSameAsCSfromSendBuffer(HWND hDlg, char eaM_cs){ //(HWND hDlg, char eaM_cs, int nLine2StartFrom){
 int nReturnCodeFromReceivePackets;
 int nResultOfThisStatusRecQuery = 0;
//SendMessagePackets(NULL, eaM_cs, MsTypD_SENDchecksumREC, ++c MessageSeqNoCS[eaM_cs], 0); We Sent a form of Status Record request in b CheckAlltheChecksumsAreOK(, before calling this function for each reply.
//Receive the reply. Note that the reply is in response to a M sTypD_SENDchecksumREC (which is treated by the AVR almost the same as a M sTyp2_ZENDstatREC - EXCEPT THAT
// uVAR_b contains the checksum. The reply is still a0x77 message type. Calculating the checksum takes a lot of AVR time (as well as ConvertJPMessageDataToTClargeSDarray())
// so we need a long timeout while waiting for the reply.  Ideally ALL AVR'S should be sent MsTypD_SENDchecksumREC in parallel and the replies looked at later, but it's not
// vital:  AVR calculates the checksum as soon as all packets are received, and ConvertJPMessageDataToTClargeSDarray() has to be waited for anyway.
// By the time one AVR is ready, they'll probably all be ready.
 NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETCHECKSUM;
 bWaitingForReply[eaM_cs] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
 nReturnCodeFromReceivePackets = ReceiveMessagePackets(NULL, eaM_cs, MsTyp2_ZENDstatREC, 0);  //NULL stops MessageBoxes. The Sent statREC request was actually a MsTypD_SENDchecksumREC
 NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS;
 if(nReturnCodeFromReceivePackets){ //i.e. != 0
    if(nReturnCodeFromReceivePackets == 6666) //0x1A0A
      DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Program source code may need to have TIMEOUT_FOR_GETCHECKSUM increased?\n\nError Code: 6666 - No Reply from Motor: ", "Error occured whilst checking CheckSums.");
    else{
       if(nReturnCodeFromReceivePackets == 123456)
         DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Received something, but not 0x77 message from Motor: ", "Error checking  motors' CheckSums...");
       else{
          char szTitleWithErrorCode[SIZ_TwEC];
          StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"nReturnCodeFromReceivePackets Error code: ");
          _itoa_s(nReturnCodeFromReceivePackets, szTitleWithErrorCode + 42, SIZ_TwEC - 42, 10);
          DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Error2 occurred whilst checking motors' CheckSum, for Motor: ",szTitleWithErrorCode);
         }
      }
    return 0;
   }
 if(chReceiveBuffer[eaM_cs][1] != cMessageSeqNoCS[eaM_cs]){ //MessageBox(hDlg,"I stopped so you can see(6)!","Seqno Error",MB_OK);
    char szTitleWithErrorCode[SIZ_TwEC];
    int nStrLen;
    StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"Seqno Error - Sent Seqno: ");
    _itoa_s(cMessageSeqNoCS[eaM_cs], szTitleWithErrorCode + 26, SIZ_TwEC - 26, 10);

    StringCchCat(szTitleWithErrorCode,SIZ_TwEC," & Received Seqno: ");
    nStrLen = lstrlen(szTitleWithErrorCode);
    _itoa_s(chReceiveBuffer[eaM_cs][1], szTitleWithErrorCode + nStrLen, SIZ_TwEC - nStrLen, 10);

    DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, eaM_cs, "Error3 occurred whilst checking motors' CheckSum, for Motor: ",szTitleWithErrorCode);
    return 0;
   }

 pStatRec = (STATUSRECORD*)&chReceiveBuffer[eaM_cs][2]; //+2 because the reply from AVR contains a message-type in the first 2 bytes
//endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
 endian_swap(*(((unsigned int*)pStatRec)+2));
//...and check the CheckSum is what it is supposed to be:
//if((pStatRec->uVARb == (uiTableOfCheckSums[eaM_cs] + nLine2StartFrom))
if( (pStatRec->uVARb == uiTableOfCheckSums[eaM_cs]) //With this line I have an error with Windows saying the Csum should be 2,850,492,885 but AVR saying 2,850,492,886
  ||( (pStatRec->uVARb == 123321)&&(!bDoProperCheckSumChecks_AccelProfs)) )  //Why was the nLine2StartFrom commented out... Surely I should add nLine2StartFrom at Window's end? Answer: In T ransfer_ALL_FilesAndPerform() see "uiTableOfCheckSums[nMotorBuffNo] += nNextLine2Print;"
   nResultOfThisStatusRecQuery = 1; // <-- The Checksums match
 if(pStatRec->uVARb == 123322){
    mes[0] = 'M';
    _itoa_s(eaM_cs+1,mes+1,SIZMES,10);
    MessageBox(hDlg,"Error: CheckSum 123322 indicates that you asked for\na checksum without sending Motor data\nStatic motor perhaps?\nPerhaps an inconsistent set of motor position files e.g. different number of records?\n\nSee above Motor number and check motor's status.\nIs there a Seqno Error?\nWas a csb file corrupt?",mes,MB_OK);
   }
 return nResultOfThisStatusRecQuery; //This is 1 if the  Checksums match, else zero
}

//Returns TRUE if all motors are in right place.
//This function uses an array to stop checking motors as soon as they indicate they are in the correct place.  The Array must be initialised
// before calling this function, but if the function is called repeatedly (e.g. whilst waiting for the motors to move to their start positions)
// then this initialisation should occur only once.  AT THE MOMENT THE FUNCTION IS NOT CALLED REPEATEDLY
BOOL bMotorsAreInTheirStartPositionsANDhappy(HWND hDlg, int *pArrayMotorIsInRightPlace, int nLineNoToStartPrintFrom){
 for(char eaM = 0; eaM < NOOFMOTORS; eaM++)   //Call b MotorIsInItsStartPosition() only for motors that are not already at home position:
   if( !(*(pArrayMotorIsInRightPlace + eaM)) && !bMotorIsInItsStartPosition(hDlg, eaM, nLineNoToStartPrintFrom) )
     return 0; //... and return false as soon as a motor says it is not homed.
   else
     *(pArrayMotorIsInRightPlace + eaM) = 1; //Record the motor saying it is homed (so we don't ask it again). Note if we're not expecting a reply (because
 return 1;                                   // PCFrec.bMotor2ExpectRepliesFrom[Motor No] is zero) b MotorIsInItsStartPosition() will show the motor as homed
}

//Returns TRUE if motor is in right place
BOOL bMotorIsInItsStartPosition(HWND hDlg, char nchMotrNo, int nLineNoToStartPrintFrom){ //function definition
 int nReturnCodeFromReceivePackets;
 int nReturnCodeFromSendPackets;
 int nResultOfThisStatusRecQuery;
 if(!PCFrec.bMotor2ExpectRepliesFrom[nchMotrNo])
   return 1;
//The following test is unnecessary... bMotorsAreInTheirStartPositionsANDhappy() does not call this function for PIDs because of the way ArrayMotorIsInRightPlace[] is 
//if((nchMotrNo == 1) || (nchMotrNo == 2) || (nchMotrNo == 13) || (nchMotrNo == 14) || (nchMotrNo == 25) || (nchMotrNo == 26)){ // setup immediately before bMotorsAreInTheirStartPositionsANDhappy is called
//    DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Function should NEVER be called for PIDMotor: ", "Error212 checking motors are at Start Positions...");
//    return 0;
//   }
 nResultOfThisStatusRecQuery = 0;
 #define SIZ_TwEC 56  //Size of szTitleWithErrorCode[]

 nReturnCodeFromSendPackets = SendMessagePackets(NULL, nchMotrNo, MsTyp2_ZENDstatREC, ++cMessageSeqNoSR[nchMotrNo], 0);
 if(nReturnCodeFromSendPackets){        //"nReturnCodeFromSendPackets Error"
    if(nReturnCodeFromSendPackets == 5001)
      DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Send() function took > 0.2 seconds from Motor: ", "Error721 checking motors are at Start Positions...");
    else{
       char szTitleWithErrorCode[SIZ_TwEC];
       StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"nReturnCodeFromSendPackets Error code: ");
       _itoa_s(nReturnCodeFromSendPackets, szTitleWithErrorCode + 39, SIZ_TwEC - 39, 10);
       DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Error112 occurred whilst checking motors are at Start Positions, for Motor: ",szTitleWithErrorCode);
      }
    return 0;// <-- The Motor is not in its start position
   }
//We've Sent a Status Record request... There will be a short wait for the reply, so use this time to calculate where the motor is supposed to bez
//The following motors depend on PCF settings, so could change between one print and the next (so positions are looked up in there)
 int nStartPosForThisMotorOnThisLine;
 /*
 if(nchMotrNo == 0)
   nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1);
 else if(nchMotrNo == 12)
   nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2);
 else if(nchMotrNo == 24)
   nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3);
 else if(nchMotrNo == 3)
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0];
 else if(nchMotrNo == 15)
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1];
 else if(nchMotrNo == 27)
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2];
 else if( (nchMotrNo == 4) || (nchMotrNo == 16) || (nchMotrNo == 28) ){
    int RED0p8or1p6NotPin = HCFrec.nApertureSize;
    if(nchMotrNo == 4){
       nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin];
      }
    else if(nchMotrNo == 16){
       int GRE0p8or1p6NotPin = RED0p8or1p6NotPin + 3;
       nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin];
      }
    else{ //if(nchMotrNo == 28){
       int BLU0p8or1p6NotPin = RED0p8or1p6NotPin + 6;
       nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin];
      }
   }
 else if(nchMotrNo == 5)
   nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[0];
 else if(nchMotrNo == 17)
   nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[1];
 else if(nchMotrNo == 29)
   nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[2];
 else if(nchMotrNo == 36)
   nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0];
 else if(nchMotrNo == 37)
   nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1];
 else if(nchMotrNo == 38)
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_XYstageHm2Origin[0];
 else if(nchMotrNo == 39)
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_XYstageHm2Origin[1];
 else
   nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom];
*/
 switch(nchMotrNo){
    case  0:  nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1); break;
    case  3:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0]; break;
    case  5:  nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[0]; break;
    case  6:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointT[0]; break;
    case  7:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointB[0]; break;
    case  8:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointL[0]; break;
    case  9:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointR[0]; break;
    case 10:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0]; break;
    case 11:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0]; break;

    case 12:  nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2); break;
    case 15:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1]; break;
    case 17:  nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[1]; break;
    case 18:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointT[1]; break;
    case 19:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointB[1]; break;
    case 20:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointL[1]; break;
    case 21:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointR[1]; break;
    case 22:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1]; break;
    case 23:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1]; break;

    case 24:  nStartPosForThisMotorOnThisLine = GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3); break;
    case 27:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2]; break;
    case 29:  nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachFocusTab[2]; break;
    case 30:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointT[2]; break;
    case 31:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointB[2]; break;
    case 32:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointL[2]; break;
    case 33:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2CenterPointR[2]; break;
    case 34:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2]; break;
    case 35:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2]; break;

    case 36:  nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0]; break;
    case 37:  nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1]; break;
    case 38:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_XYstageHm2Origin[0]; break;
    case 39:  nStartPosForThisMotorOnThisLine = nLineXstartPositionsFEM[nchMotrNo][nLineNoToStartPrintFrom] + PCFrec.nNoofUsteps_XYstageHm2Origin[1]; break;
    default:{ //picks up 4, 16 & 28
       int RED0p8or1p6NotPin = HCFrec.nApertureSize;
       if(nchMotrNo == 4){
          nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin];
         }
       else if(nchMotrNo == 16){
          int GRE0p8or1p6NotPin = RED0p8or1p6NotPin + 3;
          nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin];
         }
       else if(nchMotrNo == 28){
          int BLU0p8or1p6NotPin = RED0p8or1p6NotPin + 6;
          nStartPosForThisMotorOnThisLine = PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin];
         }
       else{
          MessageBox(hDlg,"b_MotorIsInItsStartPosition() was passed an invalid nchMotrNo",NULL,MB_OK);
          return 0 ;
         }
      }//default
   }//switch

//Receive the reply
 bWaitingForReply[nchMotrNo] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
 nReturnCodeFromReceivePackets = ReceiveMessagePackets(NULL, nchMotrNo, MsTyp2_ZENDstatREC, 0);      // best to avoid MessageBoxes (hence use of NULLs)

//This next if Statement is VERY doubtful... but will help, during debugging, to show if we have not waited long enough for the reply.
//if(nReturnCodeFromReceivePackets == 6666){
//   //K illTimer(hDlg, I DT_TIMER1);
//   int j;
//   for(j = 0; j<5; j++){
//      bWaitingForReply[nchMotrNo] = 1;//set again here because the reply failed
//      nReturnCodeFromReceivePackets = R eceiveMessagePackets(NULL, nchMotrNo, M sTyp2_ZENDstatREC, 0); //will have cleared this.
//      if(!nReturnCodeFromReceivePackets){
//         j = 1000;
//         MessageBox(hDlg,"is now OK2!","nReturnCodeFromSendPackets Error",MB_OK);
//        }
//       //if(j > 200)
//         //if(!S etTimer(hDlg, I DT_TIMER1, AUTOREFRESHRATE, (TIMERPROC) NULL))
//           // MessageBox(hDlg,"STATREC_AUTO_TIMER reset2 problem!",NULL,MB_OK);
//     }
//  }
 if(nReturnCodeFromReceivePackets){ //i.e. != 0
    if(nReturnCodeFromReceivePackets == 6666)
      DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Error Code: 6666 - No Reply from Motor: ", "Error722 checking motors are at Start Positions...");
    else{
       if(nReturnCodeFromReceivePackets == 123456)
         DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Received something, but not 0x77 message from Motor: ", "Error723 checking motors are at Start Positions...");
       else{
          char szTitleWithErrorCode[SIZ_TwEC];
          StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"nReturnCodeFromReceivePackets Error code: ");
           _itoa_s(nReturnCodeFromSendPackets, szTitleWithErrorCode + 42, SIZ_TwEC - 42, 10);
          DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Error2 occurred whilst checking motors are at Start Positions, for Motor: ",szTitleWithErrorCode);
         }
      }
    return 0;// <-- The Motor is not in its start position
   }
 if(chReceiveBuffer[nchMotrNo][1] != cMessageSeqNoSR[nchMotrNo]){ //MessageBox(hDlg,"I stopped so you can see(6)!","Seqno Error",MB_OK);
    char szTitleWithErrorCode[SIZ_TwEC];
    int nStrLen;
    StringCchCopy(szTitleWithErrorCode,SIZ_TwEC,"Seqno Error - Sent Seqno: ");
     _itoa_s(cMessageSeqNoSR[nchMotrNo], szTitleWithErrorCode + 26, SIZ_TwEC - 26, 10);

    StringCchCat(szTitleWithErrorCode,SIZ_TwEC," & Received Seqno: ");
    nStrLen = lstrlen(szTitleWithErrorCode);
     _itoa_s(chReceiveBuffer[nchMotrNo][1], szTitleWithErrorCode + nStrLen, SIZ_TwEC - nStrLen, 10);

    DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nchMotrNo, "Error3 occurred whilst checking motors are at Start Positions, for Motor: ",szTitleWithErrorCode);
    return 0;// <-- The Motor is not in its start position
   }

 pStatRec = (STATUSRECORD*)&chReceiveBuffer[nchMotrNo][2]; //+2 because the reply from AVR contains a message-type in the first 2 bytes
 endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
//...and check the motor is where it is supposed to be:
 if((pStatRec->uStepsFromHome == nStartPosForThisMotorOnThisLine) && !(pStatRec->usHomingState))//usHomingState is zero if homing has occurred, so position is known
    nResultOfThisStatusRecQuery = 1; // <-- The Motor is homed
//       SetDlgItemText(hDlg, IDB_STATREC_POS + nchMotrNo, szuStepsFromHome);
//       _itoa_s(pStatRec->uVARa, szVARa, 15, 10);
//       SetDlgItemText(hDlg, IDB_STATREC_VARa + nchMotrNo, szVARa);
//       _itoa_s(pStatRec->uVARb, szVARb, 15, 10);
//       SetDlgItemText(hDlg, IDB_STATREC_VARb + nchMotrNo, szVARb);
//       _itoa_s(pStatRec->uVARc, szVARc, 15, 10);
//       SetDlgItemText(hDlg, IDB_STATREC_VARc + nchMotrNo, szVARc);
                                ///1st 4b
                                ///2nd 4b
                                ///BYTE
//       if(pStatRec->Warnings2Report > SIZ_WARNS2REPORT_TABLE)
//         StringCchCopy(szString, SIZSTRING, "Need2updateW2RLookup (HoloProg)" );
//       else
//         StringCchCopy(szString, SIZSTRING, *(Warnings2ReportStrings + pStatRec->Warnings2Report) );
                                ///4th BYTE
//       if(pStatRec->StatusOutLineIsHi)
//         SetDlgItemText(hDlg, IDB_STATREC_IT_IS_Low + nchMotrNo, "H");
//       else
//         SetDlgItemText(hDlg, IDB_STATREC_IT_IS_Low + nchMotrNo, "L");
         //pStatRec->Length = 0x3f;
 return nResultOfThisStatusRecQuery; //This is 1 if the motor is homed, else zero
}

int PrinterConfigMatchesHCFconfig(HWND hDlg){
 int retCode = 0;
 //if(memcmp(&(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab),&(PCFrec.chNoofUsteps4EachMotorTab),NOOFMOTORS)){  //Nope, Test only the dynamic motors
 if((HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[ 6] != PCFrec.chNoofUsteps4EachMotorTab[ 6])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[ 7] != PCFrec.chNoofUsteps4EachMotorTab[ 7])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[ 8] != PCFrec.chNoofUsteps4EachMotorTab[ 8])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[ 9] != PCFrec.chNoofUsteps4EachMotorTab[ 9])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[10] != PCFrec.chNoofUsteps4EachMotorTab[10])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[11] != PCFrec.chNoofUsteps4EachMotorTab[11])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[18] != PCFrec.chNoofUsteps4EachMotorTab[18])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[19] != PCFrec.chNoofUsteps4EachMotorTab[19])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[20] != PCFrec.chNoofUsteps4EachMotorTab[20])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[21] != PCFrec.chNoofUsteps4EachMotorTab[21])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[22] != PCFrec.chNoofUsteps4EachMotorTab[22])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[23] != PCFrec.chNoofUsteps4EachMotorTab[23])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[30] != PCFrec.chNoofUsteps4EachMotorTab[30])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[31] != PCFrec.chNoofUsteps4EachMotorTab[31])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[32] != PCFrec.chNoofUsteps4EachMotorTab[32])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[33] != PCFrec.chNoofUsteps4EachMotorTab[33])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[34] != PCFrec.chNoofUsteps4EachMotorTab[34])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[35] != PCFrec.chNoofUsteps4EachMotorTab[35])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[38] != PCFrec.chNoofUsteps4EachMotorTab[38])
  ||(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[39] != PCFrec.chNoofUsteps4EachMotorTab[39])  )
   retCode = 1;
 if(!retCode) //retCode 1 is more important, don't test for retCode 2 if retCode 1 is found
   //if(memcmp(&(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps),&(PCFrec.nPhysLimMinUsteps),NOOFMOTORS * sizeof(int))
   // ||memcmp(&(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps),&(PCFrec.nPhysLimMaxUsteps),NOOFMOTORS * sizeof(int))  )  //Nope, Test only the dynamic motors
   if((HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[ 6] != PCFrec.nPhysLimMinUsteps[ 6])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[ 7] != PCFrec.nPhysLimMinUsteps[ 7])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[ 8] != PCFrec.nPhysLimMinUsteps[ 8])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[ 9] != PCFrec.nPhysLimMinUsteps[ 9])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[10] != PCFrec.nPhysLimMinUsteps[10])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[11] != PCFrec.nPhysLimMinUsteps[11])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[18] != PCFrec.nPhysLimMinUsteps[18])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[19] != PCFrec.nPhysLimMinUsteps[19])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[20] != PCFrec.nPhysLimMinUsteps[20])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[21] != PCFrec.nPhysLimMinUsteps[21])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[22] != PCFrec.nPhysLimMinUsteps[22])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[23] != PCFrec.nPhysLimMinUsteps[23])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[30] != PCFrec.nPhysLimMinUsteps[30])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[31] != PCFrec.nPhysLimMinUsteps[31])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[32] != PCFrec.nPhysLimMinUsteps[32])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[33] != PCFrec.nPhysLimMinUsteps[33])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[34] != PCFrec.nPhysLimMinUsteps[34])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[35] != PCFrec.nPhysLimMinUsteps[35])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[38] != PCFrec.nPhysLimMinUsteps[38])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[39] != PCFrec.nPhysLimMinUsteps[39])  
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[ 6] != PCFrec.nPhysLimMaxUsteps[ 6])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[ 7] != PCFrec.nPhysLimMaxUsteps[ 7])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[ 8] != PCFrec.nPhysLimMaxUsteps[ 8])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[ 9] != PCFrec.nPhysLimMaxUsteps[ 9])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[10] != PCFrec.nPhysLimMaxUsteps[10])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[11] != PCFrec.nPhysLimMaxUsteps[11])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[18] != PCFrec.nPhysLimMaxUsteps[18])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[19] != PCFrec.nPhysLimMaxUsteps[19])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[20] != PCFrec.nPhysLimMaxUsteps[20])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[21] != PCFrec.nPhysLimMaxUsteps[21])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[22] != PCFrec.nPhysLimMaxUsteps[22])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[23] != PCFrec.nPhysLimMaxUsteps[23])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[30] != PCFrec.nPhysLimMaxUsteps[30])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[31] != PCFrec.nPhysLimMaxUsteps[31])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[32] != PCFrec.nPhysLimMaxUsteps[32])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[33] != PCFrec.nPhysLimMaxUsteps[33])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[34] != PCFrec.nPhysLimMaxUsteps[34])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[35] != PCFrec.nPhysLimMaxUsteps[35])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[38] != PCFrec.nPhysLimMaxUsteps[38])
    ||(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[39] != PCFrec.nPhysLimMaxUsteps[39]) )
     retCode = 2;
 if(!retCode) //retCode 1 is most important, don't test for retCode 3 if a retCode is already found. At present, 2 & 3 are treated as equally important, and handled [just about?] identically.
   if((HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointT[0] != PCFrec.nNoofUsteps_Hm2CenterPointT[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointB[0] != PCFrec.nNoofUsteps_Hm2CenterPointB[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointL[0] != PCFrec.nNoofUsteps_Hm2CenterPointL[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointR[0] != PCFrec.nNoofUsteps_Hm2CenterPointR[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonHzeroDegrees[0] != PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonVzeroDegrees[0] != PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointT[1] != PCFrec.nNoofUsteps_Hm2CenterPointT[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointB[1] != PCFrec.nNoofUsteps_Hm2CenterPointB[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointL[1] != PCFrec.nNoofUsteps_Hm2CenterPointL[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointR[1] != PCFrec.nNoofUsteps_Hm2CenterPointR[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonHzeroDegrees[1] != PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonVzeroDegrees[1] != PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointT[2] != PCFrec.nNoofUsteps_Hm2CenterPointT[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointB[2] != PCFrec.nNoofUsteps_Hm2CenterPointB[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointL[2] != PCFrec.nNoofUsteps_Hm2CenterPointL[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointR[2] != PCFrec.nNoofUsteps_Hm2CenterPointR[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonHzeroDegrees[2] != PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonVzeroDegrees[2] != PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[0] != PCFrec.nNoofUsteps_XYstageHm2Origin[0])
    ||(HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[1] != PCFrec.nNoofUsteps_XYstageHm2Origin[1]) 
    ||(HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH != PCFrec.nM39_XAXISBACKLASH)  )
     retCode = 3;
 if(hDlg != NULL){ //Display Messages only if the calling routine wants them displayed
    if(retCode == 1)
      MessageBox(hDlg,"This hologram was generated with a Printer Configuration that had a different number of uSteps\nsetup for at least one of the dynamic motors.\n\n\t\t   ** The fields highlighted in RED show these **\n\nYou cannot print unless you set the printer configuration for these motors back to the same number\nof uSteps,(and, if you do this, you must also set the stepper driver's DIP switches to match)\n\nAlternatively, you could create a copy of this hologram and then regenerate the control data\nusing the current Printer Configuration.",NULL,MB_OK);
    else if(retCode)
      MessageBox(hDlg,"This hologram was generated with a Printer Configuration that had different Physical Limits\nsetup for at least one of the dynamic motors (or had a different home to reference position)\n(or had a different backlash setting)\n\n\t\t ** The fields highlighted in AMBER show these **.\n\nIt make be risky to attempt printing as the new limits were not checked during\nmotor position generation.\n\nEither accept the risk of a physical collision, or create a copy of this hologram\nand regenerate the control data using the current Printer Configuration.","WARNING !",MB_OK);
    else //Calling this function with hDlg set should happen only following a call without it set (that finds a difference) 
      MessageBox(hDlg,"Programming Error - PrinterConfigMatchesHCFconfig() not NULL, yet no retCode was found",NULL,MB_OK); 
   }
 return retCode;
}

//For the time being, this function uses a batch program to create missing files, but the batch program will be soon be removed
BOOL bCheckAllDynamicFilesExist(HWND hDlg){
 char szBatchBuffer[256];
 /*
 sprintf_s(szBatchBuffer,256, "%s%s", HCFcfs.szCSVDirName4Reading, "CopyFiles.bat");
 if( _access(szBatchBuffer, 0) == 0){ //Make certain the batch file exists
    struct _stat64i32 status;         //Path exists, but is it a file?
    _stat(szBatchBuffer, &status);
    if((status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
       MessageBox(hDlg,"CopyFiles.bat is a Folder!","An Essential file was missing.. please rectify this",MB_OK);
       return 1;
      }
   }
 else{//The path doesn't exist, so create it.
    MessageBox(hDlg,"Huh!\n\nDon't forget it can't be copied without changing the paths within the file","Essential file 'CopyFiles.bat' was missing!",MB_OK);
    return 1;
   }
 system(szBatchBuffer);
 */
  
 //Now check all the required files are present:
 for(int nMotrr = 0; nMotrr < NOOFMOTORS; nMotrr++){
    if((!bListOfStaticMotors[nMotrr]) && (!PCFrec.bMotorIS_A_PID[nMotrr]) ){
       sprintf_s(szBatchBuffer,256, "%s%s", HCFcfs.szCSVDirName4Reading, FILENAMEStab[nMotrr]);
       if( _access(szBatchBuffer, 0) == 0){ //Make certain the file exists
          struct _stat64i32 status;         //Path exists, but is it a file?
          _stat(szBatchBuffer, &status);
          if((status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
             MessageBox(hDlg,"An Essential file was missing... it seems to be a folder! Please rectify this",szBatchBuffer,MB_OK);
             return 1;
            }
         }
       else{//The path doesn't exist, so create it.
          MessageBox(hDlg,"An Essential file was missing! Please rectify this",szBatchBuffer,MB_OK);
          return 1;
         }
      }//if it a dynamic motor
   }//for
 return 0; //all OK
}

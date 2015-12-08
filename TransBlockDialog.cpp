#include "stdafx.h"
//                                         TransBlockDialog.cpp contains:
//CALLBACK TransBlock_Dialog()
//void T ransfer_FileAndPerform(HWND hDlg, BOOL bMoveNow) //Read filename from IDC_TRANSBLOCK_FILE_CHOSEN, attempt to open then read data from it, and then send it
//extern int  bMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec
extern unsigned int uiTableOfCheckSums[NOOFMOTORS];

extern char FILENAMEStab[][16];
extern char chCSVHeaderLine[SIZ_CSV_HEADERp4];
extern unsigned char cMessageSeqNoSR[NOOFMOTORS];
extern int  NOOFTIMESTHRU_B4OBJECTING;
//extern HANDLE ghMutex;
extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern int bWaitingForReply[];
extern CRegStdWORD   bDoProperCheckSumChecks_MposFiles;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;

BOOL bMoveNowRadioButPressed;
//BOOL bReallyThisIsAnAccelProfile;
//extern int  bSTATREC_BOXisBeingDisplayed;
extern BOOL bSaveDiffFileAsBinary;
extern char szGlobalFileName[];
extern char szExplorerDir2StartIn[];
extern HCFRECORDsCURRENTfolders HCFcfs;
//extern char szCSVDirName4Reading[];
extern char szAccelFileNamesDir[_MAX_DIR];         //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//extern int  nLenszAccelFileNamesDir;             // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.
extern char *pMxPacketBuffAr[];
extern BOOL bListOfStaticMotors[NOOFMOTORS];
extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern char mes[];      //Anything can use this for anything (in the main thread) - just don't rely on it keeping its value for long
extern STATUSRECORD *pStatRec;
extern HCFRECORD HCFrec;
extern HWND hGLWnd;
extern char nchCurrentMotorZB;
//extern int  nArFastSpeeds4HomingDeptOn_uSteps[];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
extern PCFRECORD PCFrec;

extern CRegStdString LastCsvFileOpened;
#define SIZSPINTEXT 10

void DisplayMustHaveHoloGramOpenMessage(HWND hDlg){
 MessageBox(hDlg,"You must have a Hologram opened before you can download files from Motors\nas this determines where the file should be written.","Cannot Get File",MB_OK);
}

//Procedure for handling the "Block Transfer of Motor Positions" DialogBox.  This asks for a filename, but also has Jog, Start/Stop, & Reset Buttons.
INT_PTR CALLBACK TransBlock_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 HWND hWndSpinFast, hWndSpinMedm, hWndSpinSlow, hWndSpinEditF, hWndSpinEditM, hWndSpinEditS;
 char szSpinText[SIZSPINTEXT];
 static char szDialogTitle[43];
 char szLocalTransBlockFileName[_MAX_DIR];
 switch (message){
    case WM_INITDIALOG:{
       //StringCchCopy( szGlobalFileName, _MAX_DIR, stdstring(LastCsvFileOpened).c_str() );
       //SetDlgItemText(hDlg, IDC_TRANSBLOCK_FILE_CHOSEN, szGlobalFileName);
       //StringCchCopy( szGlobalFileName, _MAX_DIR, stdstring(LastCsvFileOpened).c_str() );
       SetDlgItemText(hDlg, IDC_TRANSBLOCK_FILE_CHOSEN, stdstring(LastCsvFileOpened).c_str() );

       hWndSpinFast =GetDlgItem(hDlg, IDC_SPINJOGFAST);
       hWndSpinMedm =GetDlgItem(hDlg, IDC_SPINJOGMEDM);
       hWndSpinSlow =GetDlgItem(hDlg, IDC_SPINJOGSLOW);
       hWndSpinEditF=GetDlgItem(hDlg, IDC_SPINCONTROL_EDITF);
       hWndSpinEditM=GetDlgItem(hDlg, IDC_SPINCONTROL_EDITM);
       hWndSpinEditS=GetDlgItem(hDlg, IDC_SPINCONTROL_EDITS);
       SendMessage(hWndSpinSlow, UDM_SETBUDDY, (WPARAM)hWndSpinEditS, 0L);
       SendMessage(hWndSpinMedm, UDM_SETBUDDY, (WPARAM)hWndSpinEditM, 0L);
       SendMessage(hWndSpinFast, UDM_SETBUDDY, (WPARAM)hWndSpinEditF, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINJOGFAST, UDM_SETRANGE, 0, MAKELPARAM(1,-1));
       SendDlgItemMessage(hDlg, IDC_SPINJOGMEDM, UDM_SETRANGE, 0, MAKELPARAM(1,-1));
       SendDlgItemMessage(hDlg, IDC_SPINJOGSLOW, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       if( bMoveNowRadioButPressed )
         CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO, IDB_TRANSBLOCK_MOVENOW_RADIO, IDB_TRANSBLOCK_MOVENOW_RADIO);
       else
         CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,FALSE);
       RECT ParentRc;           //Display dialog positioned to cover the bitmap on the menu bar
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, 3, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

       StringCchCopy(szDialogTitle,43,"Block Transfer of Positions - Motor ");
       int nchMno = nchCurrentMotorZB + 1;
       _itoa_s(nchMno,szDialogTitle+35,6,10);
       SetWindowText(hDlg,szDialogTitle);

       return (INT_PTR)TRUE;
      }
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_TRANSBLOCK_CHOOSEFILE:
           mGetFile(hDlg, IDS_FILTERSTR_MOTORPOS_FILES, FALSE);
           SetDlgItemText(hDlg, IDC_TRANSBLOCK_FILE_CHOSEN, szGlobalFileName);
         //if(szGlobalFileName[0])
         //  LastCsvFileOpened = szGlobalFileName;
           break; 
         case IDB_TRANSBLOCK_TRANSMITnPERFORM:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              //Read filename from IDC_TRANSBLOCK_FILE_CHOSEN and attempt to open,
              // then read the data and send it
              GetDlgItemText(hDlg, IDC_TRANSBLOCK_FILE_CHOSEN, szLocalTransBlockFileName,_MAX_PATH - 1);
              int nLengthFname = lstrlen(szLocalTransBlockFileName);
              if(nLengthFname  <= 7 ){ //e.g. C:\a.csv
                 MessageBox (hDlg,"You need a valid filename\n(in an existing folder)",NULL,MB_OK);
                 break;
                }
//#define TFnP_BMOVE_NOW         0x01
//#define TFnP_BPROPER_CSUM_CHKS 0x02
//#define TFnP_BTHIS_IS_FASTMOVE 0x04
              int nMoveNowANDdoCheckSumsANDisAfastMove = 0;
              if(bMoveNowRadioButPressed)
                nMoveNowANDdoCheckSumsANDisAfastMove = 1;
              if(bDoProperCheckSumChecks_MposFiles) //All files sent from T ransfer_FileAndPerform() are treated as MPOS files.
                nMoveNowANDdoCheckSumsANDisAfastMove |= 2;

              if( (!lstrcmp(szLocalTransBlockFileName+(nLengthFname - 3),"csv")) || (!lstrcmp(szLocalTransBlockFileName+(nLengthFname - 3),"csb")) ){
                 LastCsvFileOpened = szLocalTransBlockFileName;   
                                                         //maybe MoveNow, maybe Checksums, never FastMove
                 Transfer_FileAndPerform(hDlg, nchCurrentMotorZB, nMoveNowANDdoCheckSumsANDisAfastMove, szLocalTransBlockFileName, 0, BWARNIfITSAnACCPROFILE); //Never a Fast Move here: for starters this dialog doesn't know how far to move.
                }
              else
                MessageBox (hDlg,"Only .csv or .csb files can be opened",NULL,MB_OK);
             }
           else
             MessageBoxWarnNotConnected(hDlg);      
           break;
         case IDC_SPINCONTROL_EDITS:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, (UINT)JOGSLOWNOOFSTEPS_M, TC_CNT); //Jog Minus Slow
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, JOGSLOWNOOFSTEPS_P, TC_CNT); //Jog  Plus Slow
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS, "0");
           break;
         case IDC_SPINCONTROL_EDITM:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITM, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, (UINT)JOGMEDMNOOFSTEPS_M, TC_CNT); //Jog Minus Slow
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, JOGMEDMNOOFSTEPS_P, TC_CNT); //Jog  Plus Slow
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITM, "0");
           break;
         case IDC_SPINCONTROL_EDITF:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITF, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, (UINT)JOGFASTNOOFSTEPS_M, TC_CNT); //Jog Minus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, JOGFASTNOOFSTEPS_P, TC_CNT); //Jog  Plus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITF, "0");
           break;
         case IDB_TRANSBLOCK_PPPPL: //Please Provide Pix Positions LINE     //Single Lines are always saved as csv and opened in an editor
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              if(HCFcfs.szCSVDirName4Writing[1] == ':')
                SendMessage(hGLWnd, WM_COMMAND, IDM_SENDpixPOSITIONsLINE, 0);
              else
                DisplayMustHaveHoloGramOpenMessage(hDlg);
             }
           else
             MessageBoxWarnNotConnected(hDlg);
           //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPPOSLINE); done automatically at end of processing sent message
           break;
         case IDB_TRANSBLOCK_PPPDL: //Please Provide Pix Differences LINE   //Single Lines are always saved as csv and opened in an editor
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              if(HCFcfs.szCSVDirName4Writing[1] == ':')
                SendMessage(hGLWnd, WM_COMMAND, IDM_SENDpixDIFFERENCEsLINE, 0);
              else
                DisplayMustHaveHoloGramOpenMessage(hDlg);
             }
           else
             MessageBoxWarnNotConnected(hDlg);
           //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPDIFFLINE); done automatically at end of processing sent message
           break;
         case IDB_TRANSBLOCK_PPPDFcsv://Please Provide Pix Differences FILE i.e. MAX_NOOFLINES lines  If user presses this button save as CSV and open in an editor
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              if(HCFcfs.szCSVDirName4Writing[1] == ':'){
                 bSaveDiffFileAsBinary = 0; //(However, during a normal completion of print the file will probably be saved as a binary)
                 SendMessage(hGLWnd, WM_COMMAND, IDM_SENDpixDIFFERENCEsFILE, 0);
               //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPDIFFFILE); done automatically at end of processing sent message
                }
              else
                DisplayMustHaveHoloGramOpenMessage(hDlg);
             }
           else
             MessageBoxWarnNotConnected(hDlg);
           break;
         case IDB_TRANSBLOCK_PPPDFbin://Please Provide Pix Differences FILE i.e. MAX_NOOFLINES lines  If user presses this button save as BIN Dont open in editor
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              if(HCFcfs.szCSVDirName4Writing[1] == ':'){
                 bSaveDiffFileAsBinary = 1;
                 SendMessage(hGLWnd, WM_COMMAND, IDM_SENDpixDIFFERENCEsFILE, 0);
               //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPDIFFFILE); Not done for bin files
                }
              else
                DisplayMustHaveHoloGramOpenMessage(hDlg);
             }
           else
             MessageBoxWarnNotConnected(hDlg);
           break;
         case IDB_TRANSBLOCK_PREVMOTOR: 
           nchCurrentMotorZB = nchCurrentMotorZB - 1;
           if(-1 == nchCurrentMotorZB)
             nchCurrentMotorZB = 39;
           _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+35, 6, 10);
           SetWindowText(hDlg,szDialogTitle);
           break;
         case IDB_TRANSBLOCK_NEXTMOTOR:
           nchCurrentMotorZB = nchCurrentMotorZB + 1;
           if(NOOFMOTORS == nchCurrentMotorZB)
             nchCurrentMotorZB = 0;
           _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+35, 6, 10);
           SetWindowText(hDlg,szDialogTitle);
           break;
         case IDB_TRANSBLOCK_PREV12MOTOR:
           if(nchCurrentMotorZB < 36){
              if(nchCurrentMotorZB > 11)
                nchCurrentMotorZB = nchCurrentMotorZB - 12;
              else
                nchCurrentMotorZB = nchCurrentMotorZB + 24;
              _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+35, 6, 10);
              SetWindowText(hDlg,szDialogTitle);
             }
           else
             MessageBox(hDlg,"You can change colour only if the current motor is 1-12, or 13-24, or 25-36","Cannot Change Motor",MB_OK);
           break;
         case IDB_TRANSBLOCK_NEXT12MOTOR:
           if(nchCurrentMotorZB < 36){
              if(nchCurrentMotorZB < 24)
                nchCurrentMotorZB = nchCurrentMotorZB + 12;
              else
                nchCurrentMotorZB = nchCurrentMotorZB - 24;
              _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+35, 6, 10);
              SetWindowText(hDlg,szDialogTitle);
             }
           else
             MessageBox(hDlg,"You can change colour only if the current motor is 1-12, or 13-24, or 25-36","Cannot Change Motor",MB_OK);
           break;
         case IDB_TRANSBLOCK_PPPallDFcsv:{//Please Provide Pix Differences FILE i.e. MAX_NOOFLINES lines  If user presses this button save as CSV and open in an editor
//          int nCntConnected = 0;
//          for ( ; nCntConnected < NOOFMOTORS; nCntConnected++)
//            if( PCFrec.bMotor2ExpectRepliesFrom[nCntConnected] )
//              nCntConnected = 1000000;              
//          if(nCntConnected > 999999){ //There is at least One Motor to talk to.  Actually, we don't need to check this... Dialogbox cannot display nothing is connected
               if(HCFcfs.szCSVDirName4Writing[1] == ':'){
                  bSaveDiffFileAsBinary = 0; //(However, during a normal completion of print the file will probably be saved as a binary)
                  SendMessage(hGLWnd, WM_COMMAND, IDM_SEND_ALLpixDIFFERENCEsFILEs, 0);
                //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPDIFFFILE); done automatically at end of processing sent message
                 }
               else
                 DisplayMustHaveHoloGramOpenMessage(hDlg);
//            }
//          else
//            MessageBoxWarnNotConnected(hDlg);
           }
           break;
         case IDB_TRANSBLOCK_PPPallDFbin://Please Provide Pix Differences FILE i.e. MAX_NOOFLINES lines  If user presses this button save as BIN Dont open in editor
              if(HCFcfs.szCSVDirName4Writing[1] == ':'){
                 bSaveDiffFileAsBinary = 1;
                 SendMessage(hGLWnd, WM_COMMAND, IDM_SEND_ALLpixDIFFERENCEsFILEs, 0);
               //PostMessage(hGLWnd, WM_COMMAND, IDM_EXEC_EDITOR, SHOWPDIFFFILE); Not done for bin files
                }
              else
                DisplayMustHaveHoloGramOpenMessage(hDlg);
           break;
         case IDB_TRANSBLOCK_DUMP:
           DumpReceiveBuffers2File("C:\\DumpPlaceABuff"); //Don't change this name without changing buffer size in DumpReceiveBuffers2File()
           break;
//       case IDM_STARTSTOPTOGGLE: //Was 0x02 (Start/Stop Message), 98 in nPulsesOrSubTypOrFOPP parameter is instruction to Toggle. 98 NOT USED ANYMORE.  NOW JUST TOGGLE
//         SendMessagePackets(hDlg, nchCurrentMotorZB, MsTypB_STARTSTOP_TOGGLE, 98, 0);  
//         break;
         case IDB_TRANSBLOCK_RESET:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
             SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp4_RESET, 0, 0);   //0x04 (RESET Message)
           else
             MessageBoxWarnNotConnected(hDlg);
           break;
         case IDB_TRANSBLOCK_HOME:     /*
           //if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
           //   int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrentMotorZB];
           //   SendMessagePackets(hDlg,
           //           nchCurrentMotorZB,
           //               MsTyp3_HOME,
           //           nCurrent_uSteps,   //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
           //          nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant Fast speed to move at, when homing in
           //                  //The limits on 'how many steps to move' is still determined by UC3 hardcode
           //  }
           //else
           //  MessageBoxWarnNotConnected(hDlg);   */
/*
           //if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
           //   if(!ConstructedFileName(hDlg, szAccelFileNamesDir, nchCurrentMotorZB + 1, PCFrec.chNoofUsteps4EachMotorTab[nchCurrentMotorZB], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bUseCSBs4AccProfs)){
           //      int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrentMotorZB];
           //      int nMoveNowANDdoCheckSumsANDisAfastMove = 0;
           //      //if(bMoveNow)         Always False Here
           //      //  nMoveNowANDdoCheckSumsANDisAfastMove = 1;
           //      if(bDoProperCheckSumChecks_AccelProfs)
           //        nMoveNowANDdoCheckSumsANDisAfastMove = 2;
           //      if( !Transfer_FileAndPerform(hDlg, nchCurrentMotorZB, nMoveNowANDdoCheckSumsANDisAfastMove, szAccelFileNamesDir) ) //FALSE was "Don't move now", szAccelFileNamesDir has something like "c:\MyProjects\HoloProg\Debug\AccProfM07-15Hz-32.prn" in it
           //        SendMessagePackets(hDlg,                                                        // The parameter is now three bit-fields.
           //              nchCurrentMotorZB,
           //                    MsTyp3_HOME,
           //                nCurrent_uSteps,   //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
           //                NULL );
           //      else
           //        MessageBox(hDlg,"Unable to start homing as an error occurred\nwhen transferring the Acceleration Profile",NULL,MB_OK);
           //     }
           //  }
           //else
           //  MessageBoxWarnNotConnected(hDlg);*/
           HomeSingleMotor(hDlg, nchCurrentMotorZB);
           break;
       //case IDB_TRANSBLOCK_HOMEFAST://Same as above, but fast speed is made odd to tell UC3 to use accelerating homing (will cause slipage, when motor tries stopping abbruptly)
       //  if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
       //     int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrentMotorZB];
       //     SendMessagePackets(hDlg,
       //             nchCurrentMotorZB, //for development only. (It should be either Motor 5, 17, or 29)
       //                 MsTyp3_HOME,
       //             nCurrent_uSteps,   //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //            nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] + 1 ); //The Fast speed is Odd, homing is not a constant speed, but an acceleration
       //                   //The limits on 'how many steps to move' is still determined by UC3 hardcode
       //    }
       //  else
       //    MessageBoxWarnNotConnected(hDlg);
       //  break;
//       case IDB_TRANSBLOCK_SHOWSTATUS:
         //if(!bSTATREC_BOXisBeingDisplayed)
//           SendMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
         //BringWindowToTop(hDlg);
//         break;
         case IDB_TRANSBLOCK_MOVENOW_RADIO:
           if( bMoveNowRadioButPressed ){
              bMoveNowRadioButPressed = FALSE;
              CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,FALSE);
             }
           else{
              bMoveNowRadioButPressed = TRUE;
              CheckRadioButton(hDlg, IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO,IDB_TRANSBLOCK_MOVENOW_RADIO);
             }
           break;
         //For the TransBlockDialog, RUN_EXPLORER1 should open the CSVsOUT folder for the current Hologram, then the user can
         // quickly view files that have just been collected from the AVRs
         case IDB_CONFIGFILE_RUN_EXPLORER1:
           //GetDlgItemText(hDlg, IDC_OPEN_CONFIG_FILE_FILE_CHOSEN, szExplorerDir2StartIn+3,_MAX_DIR - 3); //rem szExplorerDir2StartIn is initialised to "e, ")
           StringCchCopy(szExplorerDir2StartIn + 3,_MAX_DIR - 3,HCFcfs.szCSVDirName4Writing);
           if( lstrlen(szExplorerDir2StartIn) > 10 ){ //e.g. "e, C:\a.hcf"
              char *ptrBslash;
              ptrBslash = strrchr(szExplorerDir2StartIn,BACKSLASH);              //reverse search for the final backslash
              if(ptrBslash > (szExplorerDir2StartIn + 5)){
                 szExplorerDir2StartIn[(int)(ptrBslash-szExplorerDir2StartIn + 1)]=0; //szExplorerDir2StartIn now holds the CSVsOUT folder
                 PostMessage(hGLWnd,WM_COMMAND,IDM_EXEC_EXPLORER,0);
                }
              else
                MessageBox (hDlg,"Valid foldername2 needed, for Explorer to explore this folder",NULL,MB_OK);
             }
           else
             MessageBox (hDlg,"Valid foldername needed, for Explorer to explore this folder.\n\nYou must have a Hologram open.","In Translblock",MB_OK);
           break;
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
   }//switch (message){
 return (INT_PTR)FALSE;
}

//Historic comments (i.e. no longer true)
//If T ransfer_FileAndPerform's pszFolderOrFile contains a '.' four characters before the end of the string, the argument is treated as a file. Just that file is 
// Transferred to UC3, & it will be sent to the current motor. If no '.' is present, the string must contain a folder name - which must contain standard named 
// csb files e.g. 'WP1Red.csb' etc. In this case all 40 standard named files will attempt to be opened and transferred, but no error will be reported if a file
// cannot be opened.  This will help during development, but could also be the mechanism for avoiding data transfer to the static motors or the energy controllers,
// (for the time being, anyway).  The number of files read could be returned.

//Change of Plan. T ransfer_FileAndPerform() will never be called with just a folder name, This will now be passed to a function called T ransfer_ALL_FilesAndPerform().
// T ransfer_ALL_FilesAndPerform() [NOT THIS FUNCTION BELOW] can be kept simpler than T ransfer_FileAndPerform() by skipping all the 'status record checking' for each motor prior to sending. This
// is reasonable because T ransfer_ALL_FilesAndPerform() is called immediately after b MotorsAreInTheirStartPositionsANDhappy() returns, which confirms the status records 
// are all good, just a millisecond or two earlier.
//returns zero if it succeeds
int Transfer_FileAndPerform(HWND hDlg, char nchCurrentMotorZBtfNp, int nMoveNowANDdoCheckSumsANDisAfastMove, const char *pszTFnPFileName, int nReqDistance, BOOL bWarnIfItsAnAccProfile){
 //Read filename from IDC_TRANSBLOCK_FILE_CHOSEN, attempt to open then read data from it, and then send it
 // ... but before doing the above, find out if RS485 is enabled (and set the inter-packet delay speed accordingly)
//int nInterPacketDelay;
 int nInterPacketDelayMilliSecs;

 //jj bWaitingForReply[nchCurrentMotorZBtfNp] = 1;//Set before calling SendMessagePackets() whenever a reply is expected
 int nReturnCodeFromSendPackets = SendMessagePackets (hDlg, nchCurrentMotorZBtfNp, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[nchCurrentMotorZBtfNp], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)

 if(nReturnCodeFromSendPackets){
    if(nReturnCodeFromSendPackets == 5001){
       CEC.SendPktErrTFnP5001++;
       MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","TFnP: nReturnCodeFromSendPackets Error Code:",MB_OK);
      }
    else{
       char sz_aString4[11];
       CEC.SendPktErrTFnPOther++;
       _itoa_s(nReturnCodeFromSendPackets, sz_aString4, 11, 10);
       MessageBox(hDlg,sz_aString4,"TFnP: nReturnCodeFromSendPackets Error Code:",MB_OK); //TFnP - T ransfer_FileAndPerform
      }
    return 1;
   }

 bWaitingForReply[nchCurrentMotorZBtfNp] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
 int TFnP_RMP_RetCode =  ReceiveMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp2_ZENDstatREC, 0);
 if (TFnP_RMP_RetCode){
    char szCode[12];
    StringCchCopy(mes,SIZMES,"TFnP:Receive Message Packets() at 6 returned code: ");
    if(TFnP_RMP_RetCode == 6666){
       CEC.RecPktErrTFnP6666++;
       StringCchCat(mes,SIZMES,"\n\nNo Reply");
      }
    else{
       if(TFnP_RMP_RetCode == 123456){
          CEC.RecPktErrTFnP123456++;
          StringCchCat(mes,SIZMES,"Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
         }
       else{
          CEC.RecPktErrTFnPOther++;
          _itoa_s(TFnP_RMP_RetCode, szCode, 12, 10);
          StringCchCat(mes,SIZMES,szCode);
         }
      }
    MessageBox(hDlg,mes,NULL,MB_OK);
    return 2;
   }
 if( cMessageSeqNoSR[nchCurrentMotorZBtfNp] != chReceiveBuffer[nchCurrentMotorZBtfNp][1] ){
    MessageBoxToDisplaySeqNoError(hDlg, "At 1: ", cMessageSeqNoSR[nchCurrentMotorZBtfNp], chReceiveBuffer[nchCurrentMotorZBtfNp][1], nchCurrentMotorZBtfNp);
    return 3;
   }
 pStatRec = (STATUSRECORD*)&chReceiveBuffer[nchCurrentMotorZBtfNp][2]; //+2 because the reply from AVR contains a message-type & SeqNo in the first 2 bytes

 if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
  //if(bSTATREC_BOXisBeingDisplayed) //i.e Do we need to show the user the 'Current Status for all Motors' Dialog box, or can they already see it?
      MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","TransferFileAndPerform() Aborted before starting!",MB_OK);
  //else{ //as the STATREC dialogbox is not being displayed, let's show it to them:
  //   if(IDYES == MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding\n\nDo you want me to show you?","TransferFileAndPerform() Aborted before starting!",MB_YESNO))
  //     PostMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
  //  }
    return 4;
   }
 //if(pStatRec->bRS485Enabled)     I appear to have removed RS485 messages produced when receiving motor position data packets (ie. type 1 messages)
 //  nInterPacketDelay = 540000;    from the UC3 code, so this increased delay is now unnecessary
 //  nInterPacketDelayMilliSecs = ???
 //else
   //nInterPacketDelay = 160000;
 
 nInterPacketDelayMilliSecs = LONG_1ST_PACKET_DELAY500; //Make the first delay long because the 1st packet of MPOS data gets AVR to clear the 8.8 Mbytes of memory space all the packets will be read into. Having the
                                                        // delay avoids the loss of subsequent packets. 2nd thoughts, I think need for the delay is more to do with RS485 messages causing the loss of packets... the
                                                        // clearing of the memory is very quick.
 //#define alwaysSWITCHOFF485FORFILESEND
 #ifdef alwaysSWITCHOFF485FORFILESEND
//         if(pStatRec->bRS485Enabled){   //Don't leave Critical Section here, We don't want other threads confusing the issue by sending status request here
//            //nInterPacketDelay = 160000;
//            nInterPacketDelayMilliSecs = ???
//            SendMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp6_ENAB_DISAB, DISAB_485, 0); //No RS485 messages
//            myWaitForAVRdelay(nInterPacketDelayMilliSecs);
//            //Sleep(100);
//           }
 #endif
 unsigned short usTotalNoofPackets;
 int nFileNameLength = lstrlen(pszTFnPFileName);
 if( nFileNameLength > 7 ){ //e.g. C:\a.csv or  C:\a.csv
    if (!lstrcmp(&pszTFnPFileName[nFileNameLength - 3],"csb")  || !lstrcmp(&pszTFnPFileName[nFileNameLength - 3],"csv")){
//     if((nFileNameLength > 22) && !memcmp("AccProfM", &pszTFnPFileName[nFileNameLength - 22], 8) )       //Look to see if this is an acceleration profile e.g. AccProfM07-15Hz-32.csv  ()
//         bReallyThisIsAnAccelProfile = 1; Now that AccProf files are sent automatically when homing, user should be allowed to 'perform' this file... it need not be treated as special in any way
       if( !myOpen_File(hDlg, &pszTFnPFileName[nFileNameLength - 3], pszTFnPFileName, nchCurrentMotorZBtfNp)){ //The file (either "csb" or "csv") opened Ok
          if(bWarnIfItsAnAccProfile && *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) & 0x10)
            MessageBox(hDlg,"AccProfile files cannot really be played on their own... During homing AVRs process them\nthen wait for a MsTyp3_HOME message, which you cannot send.\n\nI'll still send the file but don't expect to see much happening!","Warning",MB_OK);
          usTotalNoofPackets = *(short*)(pMxPacketBuffAr[nchCurrentMotorZBtfNp]+6); //6 is the offset for the Total Number of Packets
          short_endian_swap(usTotalNoofPackets);//as myOpen_File() opened the file, usTotalNoofPackets now holds the total number of packets to be sent (minus 1).
        //*((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) |= 0x40; //This  Sets  Byte 1 bit 6 in each packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!!
          if(nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BTHIS_IS_FASTMOVE)
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) |= 0x40; //This  Sets  Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
          else
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)

        //if (nPulsesOrSubTypOrFOPP & 2) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
        //  *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) |= 0x20; //This  Sets  Byte 1 bit 5 in each packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
        //else
        //  *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) &= 0xdf; //This Clears Byte 1 bit 5 in each packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
          else
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BMOVE_NOW) //for uchMessType 1 messages, this parameter is bMoveNow...
            //*((pMxPacketBuffAr[nchCurrentMotorZBtfNp]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag)
          else
            //*((pMxPacketBuffAr[nchCurrentMotorZBtfNp]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
            *((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message

          *(int*)((pMxPacketBuffAr[nchCurrentMotorZBtfNp])+ 208) = nReqDistance;

          for (unsigned int nPacketNumber = 0; nPacketNumber <= usTotalNoofPackets; nPacketNumber++){
           //SendMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp1_MOTOR_POS_DATA, nMoveNowANDdoCheckSumsANDisAfastMove, nPacketNumber);   //0x01 Motor-Position data Message - (Packet x)
             SendMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp1_MOTOR_POS_DATA, NULL, nPacketNumber); //4th Param above is not used, so no point in showing in the parameter list. The bits are now inserted more directly into the packets (above).
           //myWaitForAVRdelay(nInterPacketDelay);         //Delay sending the next packet, briefly to allow AVR time to receive last one. Unnecessary if comms sent in parallel
             myWaitForAVRdelay(nInterPacketDelayMilliSecs);//Delay sending the next packet, briefly to allow AVR time to receive last one. Unnecessary if comms sent in parallel
             nInterPacketDelayMilliSecs = SHORT_SUBSEQUENT_PACKETS_DELAY1; //remaining packets can be processed far quicker than the 1st
            }
         }
//     bReallyThisIsAnAccelProfile = 0;
      }
    else{ //fType is not "c sb" or "c sv"
       MessageBox (hDlg,"Only 'csb' or 'csv' file-types are suppported",NULL,MB_OK);
       return 5;
      }
 #ifdef alwaysSWITCHOFF485FORFILESEND
            if(pStatRec->bRS485Enabled)
              SendMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp6_ENAB_DISAB, ENAB_485, 0);  //Produce RS485 messages (again)
 #endif
    //myWaitForAVRdelay (160000000); //Was between S endMessagePackets and R eceiveMessagePackets...  Probably unnecessary because R eceiveMessagePackets() waits.
    Sleep(700);//600 is likely to give either "No Reply" or "Records transferred: 0" when transitting "big1000 rows of 1200 diff Patterns.csv" (after pressing Reset)
                //700 seemed ok, so tried "big2000 2200.csv", which broke it. 1700 was then ok, but 1600 was poor. Clearly the bigger file needs more time for
                // processing, but is this also dependent on how many steps are in the first row?   => Create & try "big2000 2200 w fast first row.csv"
                // This Needed Much Longer : 7000 no good 7400 OK.  This shows the AVR cannot reply when it is performing ConvertJPMessageDataToTClargeSDarray().

    //jj bWaitingForReply[nchCurrentMotorZBtfNp] = 1;//Set before calling SendMessagePackets() whenever a reply is expected
    int nReturnCodeFromSendPackets = SendMessagePackets (hDlg, nchCurrentMotorZBtfNp, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[nchCurrentMotorZBtfNp], 0);

    if(nReturnCodeFromSendPackets){
       if(nReturnCodeFromSendPackets == 5001){
          CEC.SendPktErrTFnP5001++;
          MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","TFnP: nReturnCodeFromSendPackets Error Code:",MB_OK);
         }
       else{
          char sz_aString5[11];
          CEC.SendPktErrTFnPOther++;
          _itoa_s(nReturnCodeFromSendPackets, sz_aString5, 11, 10);
          MessageBox(hDlg,sz_aString5,"TFnP: nReturnCodeFromSendPackets Error Code:",MB_OK);
         }
       return 6;
      }
    NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUSAFTERTransfer_FileAndPerform; //this should be a function of how many TCs are in the first line
    bWaitingForReply[nchCurrentMotorZBtfNp] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
    TFnP_RMP_RetCode = ReceiveMessagePackets(hDlg, nchCurrentMotorZBtfNp, MsTyp2_ZENDstatREC, 0);           //***Check the file was received OK
    NOOFTIMESTHRU_B4OBJECTING = TIMEOUT_FOR_GETSTATUS; //For getting status recs it should be safe to have short timeouts
    if (TFnP_RMP_RetCode){
       char szCode[12];
       StringCchCopy(mes,SIZMES,"TFnP:Receive Message Packets() at 7 returned code: ");
       if(TFnP_RMP_RetCode == 6666){
          CEC.RecPktErrTFnP6666++;
          StringCchCat(mes,SIZMES,"\n\nNo Reply (maybe UC3 in ConvertJPMessageDataToTClargeSDarray()?");
         }
       else{
          if(TFnP_RMP_RetCode == 123456){
             CEC.RecPktErrTFnP123456++;
             StringCchCat(mes,SIZMES,"Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
            }
          else{
             CEC.RecPktErrTFnPOther++;
             _itoa_s(TFnP_RMP_RetCode, szCode, 12, 10);
             StringCchCat(mes,SIZMES,szCode);
            }
         }
       MessageBox(hDlg,mes,NULL,MB_OK);
       return 7;
      }

    if( cMessageSeqNoSR[nchCurrentMotorZBtfNp] != chReceiveBuffer[nchCurrentMotorZBtfNp][1] ){
       MessageBoxToDisplaySeqNoError(hDlg, "At 2: ", cMessageSeqNoSR[nchCurrentMotorZBtfNp], chReceiveBuffer[nchCurrentMotorZBtfNp][1], nchCurrentMotorZBtfNp);
       return 8;
      }
    //endian_swap(*(unsigned int*)pStatRec);   //This is for the uStepsFromHome integer only  //endian_swap(*(((unsigned int*)pStatRec)+1)); //uVARa
    endian_swap(*(((unsigned int*)pStatRec)+2));   //uVARb                                    //endian_swap(*(((unsigned int*)pStatRec)+3)); //uVARc
    unsigned int uiChkSum = pStatRec->uVARb;

//  if( !b CSfromUC3isSameAsCSfromSendBuffer(hDlg, nchCurrentMotorZBtfNp) ){
//  if  (pStatRec->uVARb != (usTotalNoofPackets + 1)){ //ulLineNo in AVR program
    if( (uiTableOfCheckSums[nchCurrentMotorZBtfNp] != uiChkSum)  && (uiChkSum != 123321) ) {//123321 means dip-switch turned off checksum
#define SIZszMessage  45
       char szMessage[SIZszMessage];
       StringCchCopy(mes,SIZMES,"Motor ");
       _itoa_s(nchCurrentMotorZBtfNp+1, szMessage, SIZszMessage, 10);
       StringCchCat(mes, SIZMES,szMessage);
       StringCchCat(mes,SIZMES," - Checksum Error, UC3 said Checksum was: ");
       _ultoa_s(pStatRec->uVARb, szMessage, SIZszMessage, 10);
       StringCchCat(mes, SIZMES,szMessage);
       StringCchCopy(szMessage,SIZszMessage,"Windows PC thought it should be: ");
     //_itoa_s( (usTotalNoofPackets + 1), szMessage+33, SIZszMessage - 33, 10); //13 = 35 - 22
       _ultoa_s( uiTableOfCheckSums[nchCurrentMotorZBtfNp], szMessage+33, SIZszMessage - 33, 10); //13 = 35 - 22
       MessageBox(hDlg,szMessage,mes,MB_OK);
     //int maybeSendStopOrReset2AllMotorsHere; I think not... it's not as if we're printing
       return 8;
      }
    if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
     //if(bSTATREC_BOXisBeingDisplayed) //i.e Do we need to show the user the 'Current Status for all Motors' Dialog box, or can they already see it?
       MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","TransferFileAndPerform() Aborted before starting!",MB_OK);
     //else //as the STATREC dialogbox is not being displayed, let's show it to them:
     //  if(IDYES == MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding\n\nDo you want me to show you?","TransferFileAndPerform() Aborted before starting!",MB_YESNO))
     //    PostMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
       return 9;
      }
    return 0; //success
   }
 else{ //( nFileNameLength <= 7 )
    MessageBox (hDlg,"You need a valid filename",NULL,MB_OK);
    return 10;
   }
}

//Change of Plan. Transfer_FileAndPerform() will never be called with just a folder name, This will now be passed to a function called T ransfer_ALL_FilesAndPerform().
// T ransfer_ALL_FilesAndPerform() can be kept simpler than Transfer_FileAndPerform() by skipping all the 'status record checking' for each motor prior to sending. This
// is reasonable because T ransfer_ALL_FilesAndPerform() is called immediately after b MotorsAreInTheirStartPositionsANDhappy() returns, which confirms the status records
// are all good, just a millisecond or two earlier.

                     //This function is called only following "case I DB_VIEWHOLOGINFO_PRINT_HOLOGRAM:"  Never Fast Move, but could be bMoveNow

int Transfer_ALL_FilesAndPerform(HWND hDlg, BOOL bMoveNow, const char *pszTFnPFileName, unsigned short nNextLine2Print){ //Transfers *Movement* profiles called only when the Print button is pressed
 char szTAFnPFileNames[_MAX_DIR];                                                                                        //Static Motors & PIDs are skipped... only csb files are opened.
 int nFolderNameLength = lstrlen(pszTFnPFileName);
 int emTAFnP;
 int WarningNotWarningIfRefTableIsConfiguredToBeIgnored;//We dont want to keep telling the user that the ref beam table's motors are missing!
 int MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM; //put back NOOFMOTORS!!!!!   i.e.  Next line should be     for(emTAFnP = 0; emTAFnP < NOOFMOTORS; emTAFnP++)
 for(emTAFnP = 0; emTAFnP < 0; emTAFnP++)
   if( !PCFrec.bMotor2ExpectRepliesFrom[emTAFnP] && ((emTAFnP != 36)&&(emTAFnP != 37)) ){
      if(IDNO == MessageBox (hDlg,"At least one of the motors is configured to be ignored.\n\nDo you still want to continue?\n\nYou MUST press No if a holgraphic plate is present!","Warning Continuing may DESTROY the holographic Plate",MB_YESNO|MB_DEFBUTTON2))
        return 0;
      emTAFnP = NOOFMOTORS;
     }
 //Depending on the number of different motors we are sending data to, it may be necessary to insert a delay between sending packets to allow AVR time
 // to consume them.  If sending to lots of motors, the delay may be unnecessary because the multiple calls to SendMessagePackets (between sending each
 // single packet number to each AVR) will introduce its own delay.  I calculate, here, how many motor we are concurrently sending data to, but need
 // to wait until sufficient hardware is available, to do trial and error testing.  Note that send() in SendMessagePackets is unlikely to block so, if
 // the Windows PC is really fast, the delay arising from multiple calls to SendMessagePackets may be insufficient.

//int nInterPacketDelay = 160000; //This variable may go if sending to many different motors (it needed to be 540000 when RS485 was enabled and outputting messages acknowledging type 1 messages
 int nInterPacketDelayMilliSecs;
 int nNoofConcurrentDestinations = 0;
 for(int EacMot = 0; EacMot < NOOFMOTORS; EacMot++){
    if( PCFrec.bMotor2ExpectRepliesFrom[EacMot] && !bListOfStaticMotors[EacMot]  && !PCFrec.bMotorIS_A_PID[EacMot])
      nNoofConcurrentDestinations++;
    uiTableOfCheckSums[EacMot] = 0;//Clear the Table here, and the call to myOpen_File, below, will repopulate it. Initialisation here is unnecessary,
   }                               // (because of the repopulation) but will make debugging easier.

 if(nNoofConcurrentDestinations > 26)
   nNoofConcurrentDestinations = 26;
 //nInterPacketDelay -= (nInterPacketDelay / 25) * (nNoofConcurrentDestinations - 1); //reduce nInterPacketDelay by 1/25 for each additional destination
 //if(nInterPacketDelay < 10)
 //  nInterPacketDelay = 10;
 nInterPacketDelayMilliSecs = LONG_1ST_PACKET_DELAY500; //Make the first delay long because the 1st packet of MPOS data gets AVR to clear the 8.8 Mbytes of memory space all the packets will be read into. Having the
                                                        // delay avoids the loss of subsequent packets.

 if( nFolderNameLength > 9 ){ //e.g. "C:\H\CSVs\"
    if ( !lstrcmp(&pszTFnPFileName[nFolderNameLength - 6],"\\CSVs\\") ){ //Load 20 Dynamic Motor Position Files into 20 buffers.  The 14 Static Motors are skipped, and now the 6 PIDs are skipped too
                           //  (& they don't need the line number either, as Windows PC handles them [& is collected from Altera])
       //Before sending the motor position data, send the line number
       int IshouldWARNifANYMotorsARENTenabled; //other than ref table motors. Maybe the warning should be here... or maybe elsewhere [too]?

       //Although a bit ugly, I'm going to add the two bits of additional data required for M39's smooth acceleration and backlash algorithm into the MsTypE_SET_NEXTLINE2PRINT message (communicating
       // nPixAtwhichAccelStops to SendMessagePackets() globally).  This avoids having to make serious changes to the message sending generally.  We rely on the fact that MsTypE_SET_NEXTLINE2PRINT
       // is always sent before sending the motor position data, so we can be fairly confident that M39 will receive this data, valid & up-to-date, before decoding the motor position data.
       for(char nchEachMot = 6; nchEachMot < NOOFMOTORS; nchEachMot++){
          //i.e. don't SendMessagePackets if we're not expecting a reply (In Development, may not have hardware, bMotor2ExpectRepliesFrom[] should always be true for live system)
          if( !bListOfStaticMotors[nchEachMot] && !PCFrec.bMotorIS_A_PID[nchEachMot] && PCFrec.bMotor2ExpectRepliesFrom[nchEachMot] )
            SendMessagePackets(hDlg, nchEachMot, MsTypE_SET_NEXTLINE2PRINT, HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH, nNextLine2Print);//**** NOTE **** HCFrec.nPixelAtwhichAccelStops is a global parameter TOO!!!!
         }                                                                                      //nM39_XAXISBACKLASH will be treated as unsigned short by AVRs
       Sleep(10);
       StringCchCopy(szTAFnPFileNames, _MAX_DIR, pszTFnPFileName);
       int nMotorBuffNo = 6;

       int nMoveNowANDdoCheckSumsANDisAfastMove = 0;
       if(bMoveNow)
         nMoveNowANDdoCheckSumsANDisAfastMove = 1;
       if(bDoProperCheckSumChecks_MposFiles) //All files sent from T ransfer_ALL_FilesAndPerform() are treated as MPOS files.
         nMoveNowANDdoCheckSumsANDisAfastMove |= 2;


       StringCchCat(szTAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo]); //FILENAMEStab[0] is NOT opened. M1 is a static motor.
       char szCopyOfFirstFilesCSBheader[SIZ3LINES_OF_CSB_HEADER];//This is 3 * 42... for storing the first 3 lines of the header. We use this to ensure all csb files have identical headers (apart from the checksums).
       if( !myOpen_File(hDlg, "csb", szTAFnPFileNames, nMotorBuffNo) ){ //Effectively this is a "read ahead" to get the first files' header.


//The code below makes clear which bits are which (and what they do), but as nMoveNowANDdoCheckSumsANDisAfastMove **Never** has FASTMOVE here, commenting out the first test is more efficient:
        //if(nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BTHIS_IS_FASTMOVE)
        //  *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x40; //This  Sets  Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
        //else
          *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
            *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
          else
            *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BMOVE_NOW) //for uchMessType 1 messages, this parameter is bMoveNow...
            //*((pMxPacketBuffAr[nMotorBuffNo]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
            *((pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag)
          else
            //*((pMxPacketBuffAr[nMotorBuffNo]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
            *((pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message

          memcpy(szCopyOfFirstFilesCSBheader,chCSVHeaderLine,SIZ3LINES_OF_CSB_HEADER);
          uiTableOfCheckSums[nMotorBuffNo] += nNextLine2Print;
          szTAFnPFileNames[nFolderNameLength] = 0;
          StringCchCat(szTAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo+1]);
         }
       else
         return 0;// <-- The function failed
       while( (++nMotorBuffNo < NOOFMOTORS) && !myOpen_File(hDlg, "csb", szTAFnPFileNames, nMotorBuffNo) ){

//The code below makes clear which bits are which (and what they do), but as nMoveNowANDdoCheckSumsANDisAfastMove **Never** has FASTMOVE here, commenting out the first test is more efficient:
        //if(nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BTHIS_IS_FASTMOVE)
        //  *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x40; //This  Sets  Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
        //else
          *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
            *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
          else
            *((char*)(pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BMOVE_NOW) //for uchMessType 1 messages, this parameter is bMoveNow...
            //*((pMxPacketBuffAr[nMotorBuffNo]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
            *((pMxPacketBuffAr[nMotorBuffNo])+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag)
          else
            //*((pMxPacketBuffAr[nMotorBuffNo]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
            *((pMxPacketBuffAr[nMotorBuffNo])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message

          if(memcmp(szCopyOfFirstFilesCSBheader,chCSVHeaderLine,SIZ3LINES_OF_CSB_HEADER)){
             MessageBox (hDlg,"Inconsistent set of dynamic motor position files!\n\nMotor 7's header (first 3 lines) was not identical to the header in the above named file!",szTAFnPFileNames,MB_OK);
             return 0;// <-- The function failed
            }
  //      if(nNextLine2Print){//Need to recalculate the checksum to include the lineNo we're starting from. This should ensure not only that AVR has received 
             //the file correctly, but also that it is going to start printing from the correct line number. If we wanted to save time here, we could calculate
             // all the different checksums for each line in batch mode.
//           crcgen();
             //unsigned int nCheckSum = C alcCRC4Table(8800000, CopyUC3Buff, (void *) &nNextLine2Print);
//unsigned short sDummy = nNextLine2Print;
//unsigned short sDummy = 0;
//short_endian_swap(sDummy);
//           unsigned int nCheckSum = CalcCRC4Table(8800000, pMxPacketBuffAr[nMotorBuffNo-1], (void *) &sDummy);
//           uiTableOfCheckSums[nMotorBuffNo-1] = nCheckSum;
  //        }
          uiTableOfCheckSums[nMotorBuffNo] += nNextLine2Print; //Change of plan! Simply adding the line number to the checksum is effectively the same as incorporating it in the checksum
          while((++nMotorBuffNo < NOOFMOTORS) && (bListOfStaticMotors[nMotorBuffNo] || PCFrec.bMotorIS_A_PID[nMotorBuffNo])) //Skip trying to read files for static motors (and PIDs)
            ;//nMotorBuffNo is incremented by the above statement
          if(nMotorBuffNo < NOOFMOTORS){
             szTAFnPFileNames[nFolderNameLength] = 0;
             StringCchCat(szTAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo]);
             nMotorBuffNo--;
            }
          else
            nMotorBuffNo--;
         }


       if( nMotorBuffNo == NOOFMOTORS){ //The files (either "csb" or "csv") opened Ok. NOPE, T ransfer_ALL_FilesAndPerform() opens only csb files.  This means I can (& have) brought the following test
          //forward so it is done before sending the Packets.  It is probably almost redundent, having checked the first 3 lines of the header above, but is a final check to make certain that all dynamic
          //motors files have the same number of packets in.
//Check all buffers have the same number of packets to send. UC3 can cope with different numbers of packets (even for different lines), but the send
// loop below (sending MsTyp1_MOTOR_POS_DATA) relies on all motors having exactly the same number of packets in each MPOS file for a hologram
//Next Line shouldn't be current motor... it should be the first dynamic motor (and now is)
          unsigned short us_noofPacketsForAllMotors = *(short*)(pMxPacketBuffAr[38]+6); //Be aware that this number is endian swapped.
          for(int EaMot = 6; EaMot < NOOFMOTORS; EaMot++){//Starts from 6 because first 6 motors are static (of PIDs)
            if( PCFrec.bMotor2ExpectRepliesFrom[EaMot] && !bListOfStaticMotors[EaMot]  && !PCFrec.bMotorIS_A_PID[EaMot])//if we're not expecting a reply
              if( us_noofPacketsForAllMotors != *(unsigned short*)(pMxPacketBuffAr[EaMot]+6) ){
                 char szCode[12];
                 StringCchCopy(mes,SIZMES,"Motor 39s short_endian_swapped us_noofPacketsForAllMotors: ");
               //_itoa_s(nchCurrentMotorZB,mes+17,SIZMES-17,10);
               //StringCchCat(mes,SIZMES,"\nshort_endian_swapped us_noofPacketsForAllMotors: ");//Although called All motors, this is the buffer for nchCurrentMotorZB
                 short_endian_swap(us_noofPacketsForAllMotors);                                 // (which should be the same for all motors)
                 _itoa_s(nchCurrentMotorZB,mes+59,SIZMES-59,10);
               //_itoa_s(us_noofPacketsForAllMotors,szCode,12,10);
               //StringCchCat(mes,SIZMES,szCode);
                 StringCchCat(mes,SIZMES,"\nMotor No. that differed: ");
                 _itoa_s(EaMot+1,szCode,12,10);
                 StringCchCat(mes,SIZMES,szCode);
                 StringCchCat(mes,SIZMES,"\nIts endian_swapped noofPackets was: ");
                 us_noofPacketsForAllMotors = *(unsigned short*)(pMxPacketBuffAr[EaMot]+6); //Although called All motors, this is now the buffer for EaMot
                 short_endian_swap(us_noofPacketsForAllMotors);                             // (which should have been the same)
                 _itoa_s(us_noofPacketsForAllMotors,szCode,12,10);
                 StringCchCat(mes,SIZMES,szCode);
                 MessageBox (hDlg,mes,"Not all CSB files had the same numbers of Packets to send as Motor 39!",MB_OK);
                 return 0;// <-- The function failed
                }
            }
          unsigned short usTotalNoofPackets = *(short*)(pMxPacketBuffAr[38]+6); //6 is the offset for the Total Number of Packets//I'll assume noofPackets will be the same for all Motors
          short_endian_swap(usTotalNoofPackets);//as myOpen_File() opened the file, usTotalNoofPackets now holds the total number of packets to be sent (minus 1).

          for (unsigned int nPacketNumber = 0; nPacketNumber <= usTotalNoofPackets; nPacketNumber++){
             for(char nchEaMot = 1; nchEaMot < NOOFMOTORS; nchEaMot++)//i.e. don't SendMessagePackets if we're not expecting a reply (In Development, may not have hardware, bMotor2ExpectRepliesFrom[] should always be true for live system)
               if( PCFrec.bMotor2ExpectRepliesFrom[nchEaMot] && !bListOfStaticMotors[nchEaMot]  && !PCFrec.bMotorIS_A_PID[nchEaMot])
               //SendMessagePackets(hDlg, nchEaMot, MsTyp1_MOTOR_POS_DATA, nMoveNowANDdoCheckSumsANDisAfastMove, nPacketNumber);   //0x01 Motor-Position data Message - (Packet x)
                 SendMessagePackets(hDlg, nchEaMot, MsTyp1_MOTOR_POS_DATA, NULL, nPacketNumber); //4th Param above is not used, so no point in showing in the parameter list. The bits are now inserted more directly into the packets (above).
           //myWaitForAVRdelay(nInterPacketDelay);//Delay sending the next packet, briefly to allow AVR time to receive last one. Unnecessary if comms sent in parallel
             myWaitForAVRdelay(nInterPacketDelayMilliSecs);//Delay sending the next packet, briefly to allow AVR time to receive last one. May be unnecessary if comms sent in parallel
             nInterPacketDelayMilliSecs = SHORT_SUBSEQUENT_PACKETS_DELAY1; //remaining packets can be processed far quicker than the 1st
            }

          return 1;// <-- The function succeeded
         }
      }
    else
      MessageBox (hDlg,"The Hologram's folder name for the Hologram's CSVs folder must end with  \\CSVs\\  !",NULL,MB_OK);
   }
 else //( nFolderNameLength <= 8 )
   MessageBox (hDlg,"The Hologram's folder name for the Hologram's CSVs folder is too short",NULL,MB_OK);
 return 0;// <-- The function failed
}  //End if T ransfer_ALL_FilesAndPerform()


//T ransfer_ALL_AccProfsAndPerform is very similar to T ransfer_ALL_FilesAndPerform, but is required to send AccProfile files. I kept the name "AndPerform" to highlight the similarity, but AccProf files are
// never automatically 'performed'... the AVRs wait for a MsTyp3_HOME message.
//Also, This function may send either csb or csv files, whereas Transfer_FileAndPerform operates only on csb files.

       //This function is called only by M oveToStartPositions(), but that function is called in 2 places: One in from the ViewHoloGramInfoForPrinting dialog, and the other is the Main Screen, if 'Home All'
       // is pressed.  If 'Home All' is pressed, M oveToStartPositions() has a parameter set to stops it after homing (i.e. before it then moves the motos on to their start positions).
       //   **NEVER**   FASTMOVE  or  MOVE_NOW

int  Transfer_ALL_AccProfsAndPerform(HWND hDlg, BOOL bWANT_CSB_ENDING){ //, const char *pszTFnPFileName){
 //char sz TAFnPFileNames[_MAX_DIR];
 //int nFolderNameLength = lstrlen(pszTFnPFileName);
 int emTAFnP;
 int WarningNotWarningIfRefTableIsConfiguredToBeIgnored;//We dont want to keep telling the user that the ref beam table's motors are missing!
 int anotherMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM; //put back NOOFMOTORS!!!!!   i.e.  Next line should be     for(emTAFnP = 0; emTAFnP < NOOFMOTORS; emTAFnP++)
 for(emTAFnP = 0; emTAFnP < 0; emTAFnP++)
   if( !PCFrec.bMotor2ExpectRepliesFrom[emTAFnP] && ((emTAFnP != 36)&&(emTAFnP != 37)) ){
      if(IDNO == MessageBox (hDlg,"At least one of the motors is configured to be ignored.\n\nDo you still want to continue?","Warning",MB_YESNO|MB_DEFBUTTON2))
        return 0;
      emTAFnP = NOOFMOTORS;
     }
 //Depending on the number of different motors we are sending data to, it may be necessary to insert a delay between sending packets to allow AVR time
 // to consume them.  If sending to lots of motors, the delay may be unnecessary because the multiple calls to SendMessagePackets (between sending each
 // single packet number to each AVR) will introduce its own delay.  I calculate, here, how many motor we are concurrently sending data to, but need
 // to wait until sufficient hardware is available, to do trial and error testing.  Note that send() in SendMessagePackets is unlikely to block so, if
 // the Windows PC is really fast, the delay arising from multiple calls to SendMessagePackets may be insufficient.

//int nInterPacketDelay = 160000; //This variable may go if sending to many different motors (it needed to be 540000 when RS485 was enabled and outputting messages acknowledging type 1 messages
 int nInterPacketDelayMilliSecs;              //nInterPacketDelayMilliSecs is NOT REQUIRED for AccProf files... These files are always just one packet long. Perhaps I should wait 250 milliseconds anyway after sending
 //int nNoofConcurrentDestinations = 0;         // all of the files, to ensure all the AVRs are ready to accept the home instruction
 //for(int EacMot = 0; EacMot < NOOFMOTORS; EacMot++){
 //   if( PCFrec.bMotor2ExpectRepliesFrom[EacMot] && !bListOfStaticMotors[EacMot]  && !PCFrec.bMotorIS_A_PID[EacMot])
 //     nNoofConcurrentDestinations++;
 //   uiTableOfCheckSums[EacMot] = 0;//Clear the Table here, and the call to myOpen_File, below, will repopulate it. Initialisation here is unnecessary,
 //  }                               // (because of the repopulation) but will make debugging easier.

 //if(nNoofConcurrentDestinations > 26)
 //  nNoofConcurrentDestinations = 26;
 ////nInterPacketDelay -= (nInterPacketDelay / 25) * (nNoofConcurrentDestinations - 1); //reduce nInterPacketDelay by 1/25 for each additional destination
 ////if(nInterPacketDelay < 10)
 ////  nInterPacketDelay = 10;
 nInterPacketDelayMilliSecs = 1; //Make the first delay long because the 1st packet of MPOS data gets AVR to clear the 8.8 Mbytes of memory space all the packets will be read into. THIS DOESN'T APPLY HERE: In
                                 // the case of AccProf files each motor is sent only 1 packet.  This is a Single delay after all motors have had their packets dispatched (& probably doesn't need to be anything).

//if( nFolderNameLength > 9 ){ //e.g. "C:\H\CSVs\"
  //if ( !lstrcmp(&pszTFnPFileName[nFolderNameLength - 6],"\\CSVs\\") ){ //Load 20 Dynamic Motor Position Files into 20 buffers.  The 14 Static Motors are skipped, and now the 6 PIDs are skipped too
                           //  (& they don't need the line number either, as Windows PC handles them [& is collected from Altera])
       //Before sending the motor position data, send the line number
       //int IshouldWARNifANYMotorsARENTenabled; //other than ref table motors. Maybe the warning should be here... or maybe elsewhere [too]?
       //for(char nchEachMot = 6; nchEachMot < NOOFMOTORS; nchEachMot++)
       //  //i.e. don't SendMessagePackets if we're not expecting a reply (In Development, may not have hardware, bMotor2ExpectRepliesFrom[] should always be true for live system)
       //  if( !bListOfStaticMotors[nchEachMot] && !PCFrec.bMotorIS_A_PID[nchEachMot] && PCFrec.bMotor2ExpectRepliesFrom[nchEachMot] )
       //    SendMessagePackets(hDlg, nchEachMot, MsTypE_SET_NEXTLINE2PRINT, 0, nNextLine2Print); 
       //Sleep(10);
       //StringCchCopy(sz TAFnPFileNames, _MAX_DIR, pszTFnPFileName);
       int nMotorBuffNo = 0;
       //StringCchCat(sz TAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo]); //FILENAMEStab[0] is NOT opened. M1 is a static motor.
       if(ConstructedFileName(hDlg, szAccelFileNamesDir, nMotorBuffNo + 1, PCFrec.chNoofUsteps4EachMotorTab[nMotorBuffNo], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bWANT_CSB_ENDING))
         return 0;// <-- The function failed

       char szCopyOfFirstFilesCSBheader[SIZ3LINES_OF_CSB_HEADER];//This is 3 * 42... for storing the first 3 lines of the header. We use this to ensure all csb files have identical headers (apart from the checksums).
       nchCurrentMotorZB = (char)nMotorBuffNo;
       memset(pMxPacketBuffAr[nMotorBuffNo],0,SIZ_8832000);
//     bReallyThisIsAnAccelProfile = 1;

       int nMoveNowANDdoCheckSumsANDisAfastMove = 0;
       if(bDoProperCheckSumChecks_AccelProfs) //All files sent from T ransfer_ALL_AccProfsAndPerform() should be treated as AccProf files. AVR should treat these as always having usLineNum2Convert set to 0, call
         nMoveNowANDdoCheckSumsANDisAfastMove = 2;          // CheatFill_raArray() and not look for LineStart after calling ConvertJPMessageDataToTClargeSDarray

       char szType2OpenCSB_or_CSV[4] = "csb";
       if(!bWANT_CSB_ENDING)
         szType2OpenCSB_or_CSV[2] = 'v';
       if( !myOpen_File(hDlg, szType2OpenCSB_or_CSV, szAccelFileNamesDir, nMotorBuffNo) ){ //Effectively this is a "read ahead" to get the first file's header.
//        bReallyThisIsAnAccelProfile = 0;

        //if(nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BTHIS_IS_FASTMOVE)
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x40; //This  Sets  Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
        //else
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)

        //if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
        //else
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

        //if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BMOVE_NOW) //for uchMessType 1 messages, this parameter is bMoveNow...
        //  //*((pMxPacketBuffAr[nchCurrentMotorZB]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag)
        //else
        //  //*((pMxPacketBuffAr[nchCurrentMotorZB]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
        //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
//The above code made clear which bits are which (and what they do), but as nMoveNowANDdoCheckSumsANDisAfastMove can be one of only two values here (0, or 2 if doing ProperCheckSumChecks), the following is more efficient:

        //In other words Transfer_ALL_AccProfsAndPerform is **NEVER**   FASTMOVE  or  MOVE_NOW

          *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
          *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
          if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
            *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
          else
            *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.


          memcpy(szCopyOfFirstFilesCSBheader,chCSVHeaderLine,SIZ3LINES_OF_CSB_HEADER);
        //uiTableOfCheckSums[nMotorBuffNo] += nNextLine2Print;
        //sz TAFnPFileNames[nFolderNameLength] = 0;
        //StringCchCat(sz TAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo+1]);

        //memset(pMxPacketBuffAr[nMotorBuffNo+1],0,SIZ_8832000);
        //memset(pMxPacketBuffAr[nMotorBuffNo+2],0,SIZ_8832000);
          nMotorBuffNo += 2; //Skip the two PID motors M2 & M3... We never home these.

          if(ConstructedFileName(hDlg, szAccelFileNamesDir, nMotorBuffNo + 2, PCFrec.chNoofUsteps4EachMotorTab[nMotorBuffNo], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bWANT_CSB_ENDING))
            return 0;// <-- The function failed
         }
       else{
//        bReallyThisIsAnAccelProfile = 0;
          return 0;// <-- The function failed
         }
     //while( (++nMotorBuffNo < NOOFMOTORS) && !myOpen_File(hDlg, "csb", szAccelFileNamesDir, nMotorBuffNo) ){
       while(++nMotorBuffNo < NOOFMOTORS){
          nchCurrentMotorZB = (char)nMotorBuffNo;
          memset(pMxPacketBuffAr[nMotorBuffNo],0,SIZ_8832000);
//        bReallyThisIsAnAccelProfile = 1;
          if(!myOpen_File(hDlg, szType2OpenCSB_or_CSV, szAccelFileNamesDir, nMotorBuffNo) ){
//           bReallyThisIsAnAccelProfile = 0;

           //if(nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BTHIS_IS_FASTMOVE)
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x40; //This  Sets  Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
           //else
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
           //                                                     //If it's not a fast movements, it's a home.
           //if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
           //else
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

           //if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BMOVE_NOW) //for uchMessType 1 messages, this parameter is bMoveNow...
           //  //*((pMxPacketBuffAr[nchCurrentMotorZB]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag)
           //else
           //  //*((pMxPacketBuffAr[nchCurrentMotorZB]+(PACKETLENGTH * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
           //  *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
//The above code made clear which bits are which (and what they do), but as nMoveNowANDdoCheckSumsANDisAfastMove can be one of only two values here (0, or 2 ir doing ProperCheckSumChecks), the following is more efficient:

           //In other words Transfer_ALL_AccProfsAndPerform is **NEVER**   FASTMOVE  or  MOVE_NOW

             *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xbf; //This Clears Byte 1 bit 6 in 1st packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!! (i.e. here)
             *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xfe; //This Clears Byte 1 bit 0 in the first packet only (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message
             if (nMoveNowANDdoCheckSumsANDisAfastMove & TFnP_BPROPER_CSUM_CHKS) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
               *((pMxPacketBuffAr[nchCurrentMotorZB])+1) |= 0x20; //This  Sets  Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
             else
               *((pMxPacketBuffAr[nchCurrentMotorZB])+1) &= 0xdf; //This Clears Byte 1 bit 5 in 1st packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.



           //if(memcmp(szCopyOfFirstFilesCSBheader,chCSVHeaderLine,SIZ3LINES_OF_CSB_HEADER)){
           //   MessageBox (hDlg,"Inconsistent set of dynamic motor position files!\n\nMotor 1's header (first 3 lines) was not identical to the header in the above named file!",szAccelFileNamesDir,MB_OK);
           //   return 0;// <-- The function failed
           //  }
  //      if(nNextLine2Print){//Need to recalculate the checksum to include the lineNo we're starting from. This should ensure not only that AVR has received
             //the file correctly, but also that it is going to start printing from the correct line number. If we wanted to save time here, we could calculate
             // all the different checksums for each line in batch mode.
//           crcgen();
             //unsigned int nCheckSum = C alcCRC4Table(8800000, CopyUC3Buff, (void *) &nNextLine2Print);
//unsigned short sDummy = nNextLine2Print;
//unsigned short sDummy = 0;
//short_endian_swap(sDummy);
//           unsigned int nCheckSum = CalcCRC4Table(8800000, pMxPacketBuffAr[nMotorBuffNo-1], (void *) &sDummy);
//           uiTableOfCheckSums[nMotorBuffNo-1] = nCheckSum;
  //        }
       //uiTableOfCheckSums[nMotorBuffNo] += nNextLine2Print; //Change of plan! Simply adding the line number to the checksum is effectively the same as incorporating it in the checksum
           //while((++nMotorBuffNo < NOOFMOTORS) && (bListOfStaticMotors[nMotorBuffNo] || PCFrec.bMotorIS_A_PID[nMotorBuffNo])) //Skip trying to read files for static motors (and PIDs)
           //while(++nMotorBuffNo < NOOFMOTORS)                                                                                 //NO!!! For AccProfs, all motors must be sent the files
             while( (++nMotorBuffNo < NOOFMOTORS) && (PCFrec.bMotorIS_A_PID[nMotorBuffNo]) )   //Change of plan again!!! Forget homing the PIDs
               ;//nMotorBuffNo is incremented by the above statement
             if(nMotorBuffNo < NOOFMOTORS){
                //sz TAFnPFileNames[nFolderNameLength] = 0;
                //StringCchCat(sz TAFnPFileNames, _MAX_DIR, FILENAMEStab[nMotorBuffNo]);
                if(ConstructedFileName(hDlg, szAccelFileNamesDir, nMotorBuffNo + 1, PCFrec.chNoofUsteps4EachMotorTab[nMotorBuffNo], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bWANT_CSB_ENDING))
                  return 0;// <-- The function failed
                nMotorBuffNo--;
               }
             else
               nMotorBuffNo--;
            }
          else{
//           bReallyThisIsAnAccelProfile = 0;
             return 0;// <-- The function failed
            }
         }

       if( nMotorBuffNo == NOOFMOTORS){ //The files (either "csb" or "csv") opened Ok. NOPE, T ransfer_ALL_FilesAndPerform() opens only csb files.  This means I can (& have) brought the following test
          //forward so it is done before sending the Packets.  It is probably almost redundent, having checked the first 3 lines of the header above, but is a final check to make certain that all dynamic
          //motors files have the same number of packets in.
//Check all buffers have the same number of packets to send. UC3 can cope with different numbers of packets (even for different lines), but the send
// loop below (sending MsTyp1_MOTOR_POS_DATA) relies on all motors having exactly the same number of packets in each MPOS file for a hologram
//Next Line shouldn't be current motor... it should be the first dynamic motor (and now is)
          unsigned short us_noofPacketsForAllMotors = *(short*)(pMxPacketBuffAr[38]+6); //Be aware that this number is endian swapped.
        //for(int EaMot = 6; EaMot < NOOFMOTORS; EaMot++){//Starts from 6 because first 6 motors are static (of PIDs)
        //  if( PCFrec.bMotor2ExpectRepliesFrom[EaMot] && !bListOfStaticMotors[EaMot]  && !PCFrec.bMotorIS_A_PID[EaMot])//if we're not expecting a reply
          for(int EaMot = 0; EaMot < NOOFMOTORS; EaMot++){ //Starts from 6 because first 6 motors are static (of PIDs). Not anymore... Static motors are sent homing profiles too (but not the PIDs)
            if( PCFrec.bMotor2ExpectRepliesFrom[EaMot] && !PCFrec.bMotorIS_A_PID[EaMot])//if we're not expecting a reply
              if( us_noofPacketsForAllMotors != *(unsigned short*)(pMxPacketBuffAr[EaMot]+6) ){ //6 is the offset for the Total Number of Packets
                 char szCode[12];
                 StringCchCopy(mes,SIZMES,"Motor 39s short_endian_swapped us_noofPacketsForAllMotors: ");
               //_itoa_s(nchCurrentMotorZB,mes+17,SIZMES-17,10);
               //StringCchCat(mes,SIZMES,"\nshort_endian_swapped us_noofPacketsForAllMotors: ");//Although called All motors, this is the buffer for nchCurrentMotorZB
                 short_endian_swap(us_noofPacketsForAllMotors);                                 // (which should be the same for all motors)
                 _itoa_s(nchCurrentMotorZB,mes+59,SIZMES-59,10);
               //_itoa_s(us_noofPacketsForAllMotors,szCode,12,10);
               //StringCchCat(mes,SIZMES,szCode);
                 StringCchCat(mes,SIZMES,"\nMotor No. that differed: ");
                 _itoa_s(EaMot+1,szCode,12,10);
                 StringCchCat(mes,SIZMES,szCode);
                 StringCchCat(mes,SIZMES,"\nIts endian_swapped noofPackets was: ");
                 us_noofPacketsForAllMotors = *(unsigned short*)(pMxPacketBuffAr[EaMot]+6); //Although called All motors, this is now the buffer for EaMot
                 short_endian_swap(us_noofPacketsForAllMotors);                             // (which should have been the same)
                 _itoa_s(us_noofPacketsForAllMotors,szCode,12,10);
                 StringCchCat(mes,SIZMES,szCode);
                 MessageBox (hDlg,mes,"In Transfer_ALL_AccProfsAndPerform() Not all CSB files had the same numbers of Packets to send as Motor 39!",MB_OK);
                 return 0;// <-- The function failed
                }
            }
          unsigned short usTotalNoofPackets = *(short*)(pMxPacketBuffAr[38]+6); //6 is the offset for the Total Number of Packets//I'll assume noofPackets will be the same for all Motors
          short_endian_swap(usTotalNoofPackets);//as myOpen_File() opened the file, usTotalNoofPackets now holds the total number of packets to be sent (minus 1).

//Remember T ransfer_ALL_AccProfsAndPerform sends AccProfile files. AccProf files are never automatically 'performed'... the AVRs wait for a MsTyp3_HOME message.

        //for (unsigned int nPacketNumber = 0; nPacketNumber <= usTotalNoofPackets; nPacketNumber++){
          for(char nchEaMot = 0; nchEaMot < NOOFMOTORS; nchEaMot++)//i.e. don't SendMessagePackets if we're not expecting a reply (In Development, may not have hardware, bMotor2ExpectRepliesFrom[] should always be true for live system)
            if( PCFrec.bMotor2ExpectRepliesFrom[nchEaMot]   && !PCFrec.bMotorIS_A_PID[nchEaMot] )
            //SendMessagePackets(hDlg, nchEaMot, MsTyp1_MOTOR_POS_DATA, nMoveNowANDdoCheckSumsANDisAfastMove, 0);   //0x01 Motor-Position data Message - (Packet 0) //AccProfs require only 1 packet
              SendMessagePackets(hDlg, nchEaMot, MsTyp1_MOTOR_POS_DATA, NULL, 0); //4th Param above is not used, so no point in showing in the parameter list. The bits are now inserted more directly into the packets (above).
          myWaitForAVRdelay(nInterPacketDelayMilliSecs);//Delay sending anything, briefly, to allow AVR time to receive last one. Unnecessary if comms sent in parallel
           //nInterPacketDelayMilliSecs = SHORT_SUBSEQUENT_PACKETS_DELAY1; //remaining packets can be processed far quicker than the 1st  Doesn't apply... AccProfile files are one packet.
         }

 return 1;// <-- The function succeeded
}
    //  }
    //else
    //  MessageBox (hDlg,"The Hologram's folder name for the Hologram's CSVs folder must end with  \\CSVs\\  !",NULL,MB_OK);
//   }
// else //( nFolderNameLength <= 8 )
//   MessageBox (hDlg,"The Hologram's folder name for the Hologram's CSVs folder is too short",NULL,MB_OK);
//return 0;// <-- The function failed
//}  //End if T ransfer_ALL_FilesAndPerform()

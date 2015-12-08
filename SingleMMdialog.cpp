#include "stdafx.h"

#define NOOFDISTITEMS     17
#define FASTEST2OFFER     35  //The List of Speed Available will range from 34 to 4000 (plus the manually added 8K 16K 32K and 64K). If the  
#define SLOWEST2OFFERp1 4001  // user selects one of the last four items on this list, use these values not the selected row number + 34.
#define SIZSPINTEXT 10

//extern int bSTATREC_BOXisBeingDisplayed;
extern HWND hGLWnd;
extern char nchCurrentMotorZB;
//extern int  nArFastSpeeds4HomingDeptOn_uSteps[];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
extern PCFRECORD PCFrec;
extern HCFRECORD HCFrec;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;
//extern char szAccelFileNamesDir[_MAX_DIR];         //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//extern int  nLenszAccelFileNamesDir;               // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.

INT_PTR CALLBACK SingleMM_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 HWND hWndSpin, hWndSpinEdit;
 char szSpinText[SIZSPINTEXT];
 static char szDialogTitle[35] = "Single Move Dialog - Motor ";
 static HWND hSINGLEMM_SPEED;
 static BOOL bOnTheFlySpeed = FALSE;
 static BOOL bReverseMove = FALSE;
 static int nSingleMM_SpeedRowChosen = 0;
 int  nSpeeds;
//int distanceTable[NOOFDISTITEMS] = {-25600,-12800,-9600,-6400,-4800,-3200,-1600,1600,3200,4800,6400,9600,12800,25600};
 int distanceTable[NOOFDISTITEMS] = {4,800,1600,3200,4800,6400,9600,12800,19200,25600,32000,25600,51200,102400,204800,409600,627200};
 static int nInfinitePulsesPosOrNeg; //When the User changes Speed On the Fly, Change to Infinate Pulses.  As the number of pulses they
        // originally chose contains direction (ie is +ve or -ve) we need to remember this otherwise when they change speed this is
        // likely to cause a direction change.
 switch (message){
    case WM_INITDIALOG:{
//IDC_SPINJOGFAST2  IDC_SPINCONTROL_EDITF2
//IDC_SPINJOGMEDM2  IDC_SPINCONTROL_EDITM2
//IDC_SPINJOGSLOW2  IDC_SPINCONTROL_EDITS2
//IDC_SPINPULSELEN  IDC_SPINCONTROL_EDITP
       hWndSpin = GetDlgItem(hDlg, IDC_SPINJOGFAST2);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITF2);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINJOGFAST2, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       hWndSpin = GetDlgItem(hDlg, IDC_SPINJOGMEDM2);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITM2);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINJOGMEDM2, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       hWndSpin = GetDlgItem(hDlg, IDC_SPINJOGSLOW2);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITS2);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINJOGSLOW2, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       hWndSpin = GetDlgItem(hDlg, IDC_SPINPULSELEN);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITP);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINPULSELEN, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       hSINGLEMM_SPEED = GetDlgItem (hDlg, IDC_SINGLEMM_SPEED);
       #define SIZszDumSpeedsNdistances 8
       char szDumSpeedsNdistances[SIZszDumSpeedsNdistances];
       for(nSpeeds = FASTEST2OFFER; nSpeeds < SLOWEST2OFFERp1; nSpeeds++ ){
          _itoa_s(nSpeeds,szDumSpeedsNdistances,SIZszDumSpeedsNdistances,10);
          SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)szDumSpeedsNdistances);
         }
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"4800");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"8000");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"12000");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"16000");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"24000");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"32000");
       SendDlgItemMessage(hDlg,IDC_SINGLEMM_SPEED,CB_ADDSTRING,0,(LPARAM)"50000");
       if (nSingleMM_SpeedRowChosen > 0)
         SendDlgItemMessage (hDlg, IDC_SINGLEMM_SPEED, CB_SETCURSEL, nSingleMM_SpeedRowChosen, 0L);
       else
         SendDlgItemMessage (hDlg, IDC_SINGLEMM_SPEED, CB_SETCURSEL, 3972, 0L);//odd number because the list starts at 35 (& we've also added 7 more strings)
     //SendDlgItemMessage (hDlg, IDC_SINGLEMM_SPEED, CB_SETCURSEL, SLOWEST2OFFERp1 - FASTEST2OFFER + 4, 0L);       //Set Default speed to 50000
       for(int ii = 0; ii < NOOFDISTITEMS; ii++){
          _itoa_s(distanceTable[ii],szDumSpeedsNdistances,SIZszDumSpeedsNdistances,10);
          SendDlgItemMessage(hDlg,IDC_SINGLEMM_NOOFPULSES,CB_ADDSTRING,0,(LPARAM)szDumSpeedsNdistances);
         }
     //SendDlgItemMessage (hDlg, IDC_SINGLEMM_NOOFPULSES, CB_SETCURSEL, 0, 0L);                       //Set Default distance to 4 pulses
       SendDlgItemMessage (hDlg, IDC_SINGLEMM_NOOFPULSES, CB_SETCURSEL, NOOFDISTITEMS - 11, 0L);         //Set Default distance to 627200 pulses
       if( bOnTheFlySpeed )
         CheckRadioButton(hDlg, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO);
       else
         CheckRadioButton(hDlg, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,FALSE);
       RECT ParentRc;           //Display dialog positioned to cover the bitmap on the menu bar
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, 3, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

     //StringCchCopy(szDialogTitle,35,"Single Move Dialog - Motor ");
       //nchMno = nchCurrentMotorZB + 1;
       //_itoa_s(nchMno,szDialogTitle+27,6,10);

       _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
       SetWindowText(hDlg,szDialogTitle);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDC_SPINCONTROL_EDITF2:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITF2, szSpinText, SIZSPINTEXT);
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
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITF2, "0");
           break;
         case IDC_SPINCONTROL_EDITM2:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITM2, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, (UINT)JOGMEDMNOOFSTEPS_M, TC_CNT); //Jog Minus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, JOGMEDMNOOFSTEPS_P, TC_CNT); //Jog  Plus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITM2, "0");
           break;
         case IDC_SPINCONTROL_EDITS2:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS2, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, (UINT)JOGSLOWNOOFSTEPS_M, TC_CNT); //Jog Minus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, JOGSLOWNOOFSTEPS_P, TC_CNT); //Jog  Plus Fast
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS2, "0");
           break;
         case IDC_SPINCONTROL_EDITP:
           GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITP, szSpinText, SIZSPINTEXT);
           if (szSpinText[0] == '-'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp8_SET_PULSE_LENGTH, 1, 0);   //0x08   Increase The Pulse Length
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '1'){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp8_SET_PULSE_LENGTH, 0, 0);   //0x08   Decrease The Pulse Length
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           else if (szSpinText[0] == '0')
             break;
           SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITP, "0");
           break;
         case IDC_SINGLEMM_SPEED:
           if( bOnTheFlySpeed ){
              if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
                 nSingleMM_SpeedRowChosen = SendMessage(hSINGLEMM_SPEED, CB_GETCURSEL,0,0);
                 if(nSingleMM_SpeedRowChosen == CB_ERR)
                   MessageBox(hDlg,"You may need to clear 'Enable On the Fly'\n\nIf you typed something in the box you need\nto clear this by selecting a row with your mouse","Error reading your Speed selection",MB_OK);
                 else{
                    int noofAutoGenItemsOnSpeedList = SLOWEST2OFFERp1 - FASTEST2OFFER - 1; //(minus 1 because the row is zero based)
                    int nHowFarPastEndOfAutomaticallyGeneratedList = nSingleMM_SpeedRowChosen - noofAutoGenItemsOnSpeedList;
                    if(nHowFarPastEndOfAutomaticallyGeneratedList > 0){
                       switch (nHowFarPastEndOfAutomaticallyGeneratedList){
                          case  1:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 4800); break;
                          case  2:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 8000); break;
                          case  3:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 12000); break;
                          case  4:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 16000); break;
                          case  5:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 24000); break;
                          case  6:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 32000); break;
                          default:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, 50000); break;
                         }
                      }
                    else
                      SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp7_SING_MM_OTF, nInfinitePulsesPosOrNeg, nSingleMM_SpeedRowChosen + FASTEST2OFFER); //(SingleMM message)
                   }
                }
              else
                MessageBoxWarnNotConnected(hDlg);
             }
           break;
         case IDB_SINGLEMM_PREVMOTOR: 
           nchCurrentMotorZB = nchCurrentMotorZB - 1;
           if(-1 == nchCurrentMotorZB)
             nchCurrentMotorZB = 39;
           //nchMno = nchCurrentMotorZB + 1;
           //_itoa_s(nchMno,szDialogTitle+27,6,10);
           _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
           SetWindowText(hDlg,szDialogTitle);
           break;
         case IDB_SINGLEMM_NEXTMOTOR:
           nchCurrentMotorZB = nchCurrentMotorZB + 1;
           if(NOOFMOTORS == nchCurrentMotorZB)
             nchCurrentMotorZB = 0;
           //nchMno = nchCurrentMotorZB + 1;
           //_itoa_s(nchMno,szDialogTitle+27,6,10);
           _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
           SetWindowText(hDlg,szDialogTitle);
           break;
         case IDB_SINGLEMM_PREV12MOTOR:
           if(nchCurrentMotorZB < 36){
              if(nchCurrentMotorZB > 11)
                nchCurrentMotorZB = nchCurrentMotorZB - 12;
              else
                nchCurrentMotorZB = nchCurrentMotorZB + 24;
              //nchMno = nchCurrentMotorZB + 1;
              //_itoa_s(nchMno,szDialogTitle+27,6,10);
              _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
              SetWindowText(hDlg,szDialogTitle);
             }
           else
             MessageBox(hDlg,"You can change colour only if the current motor is 1-12, or 13-24, or 25-36","Cannot Change Motor",MB_OK);
           break;
         case IDB_SINGLEMM_NEXT12MOTOR:
           if(nchCurrentMotorZB < 36){
              if(nchCurrentMotorZB < 24)
                nchCurrentMotorZB = nchCurrentMotorZB + 12;
              else
                nchCurrentMotorZB = nchCurrentMotorZB - 24;
              //nchMno = nchCurrentMotorZB + 1;
              //_itoa_s(nchMno,szDialogTitle+27,6,10);
              _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
              SetWindowText(hDlg,szDialogTitle);
             }
           else
             MessageBox(hDlg,"You can change colour only if the current motor is 1-12, or 13-24, or 25-36","Cannot Change Motor",MB_OK);
           break;
//       case IDM_STARTSTOPTOGGLE:   //Was 0x02 (Start/Stop Message), 98 in nPulsesOrSubTypOrFOPP parameter is instruction to Toggle. 98 NOT USED ANYMORE.  NOW JUST TOGGLE
//         SendMessagePackets(hDlg, (nchCurrentMotorZB, MsTypB_STARTSTOP_TOGGLE, 98, 0); 
//         break;
         case IDB_SINGLEMM_RESET:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
             SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp4_RESET,        0,       0);  //0x04 (RESET Message)
           else
             MessageBoxWarnNotConnected(hDlg);
           break;
//       case IDB_SINGLEMM_SHOWSTATUS:
         //if(!bSTATREC_BOXisBeingDisplayed)
//           SendMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
         //BringWindowToTop(hDlg);
//         break;
         case IDB_SINGLEMM_HOME:     /*
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

///*
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
           //  MessageBoxWarnNotConnected(hDlg); */
           HomeSingleMotor(hDlg, nchCurrentMotorZB);
           break;
       //case IDB_SINGLEMM_HOMEFAST://Same as above, but fast speed is made odd to tell UC3 to use accelerating homing (will cause slipage, when motor tries stopping abbruptly)
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
         case IDB_SINGLEMM_REVERSEMOVENOW:
            bReverseMove = TRUE; //Note no break here!
         case IDB_SINGLEMM_MOVENOW:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              BOOL bTranslatedOK;
              char szDum_mn[11];
              char *pStrCannotProceed = "Cannot Proceed!";
              UINT uiChars = GetDlgItemText(hDlg,IDC_SINGLEMM_NOOFPULSES,szDum_mn,sizeof(szDum_mn));
              if(!uiChars){
                 MessageBox(hDlg,"Couldn't Read the Number of Steps field",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              else if (uiChars > 9){
                 MessageBox(hDlg,"> 10 characters in the Number of Pulses field",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              UINT uiEachChar;
              for (uiEachChar=0; uiEachChar < uiChars; uiEachChar++)
                if( ((szDum_mn[uiEachChar] >'9') || (szDum_mn[uiEachChar] <'0'))  && (szDum_mn[uiEachChar] != '-') )
                  uiEachChar = 1000000;
              if (uiEachChar > 999){
                 MessageBox(hDlg,"Only characters '0' to '9', allowed in the Number of Steps field\nAs well as an optional leading minus sign [-]",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              int nSingleMM_noofPulses = (int) GetDlgItemInt(hDlg,IDC_SINGLEMM_NOOFPULSES,&bTranslatedOK,TRUE);
              if(nSingleMM_noofPulses < 0)
                nSingleMM_noofPulses--;
              else
                nSingleMM_noofPulses++;
              //I don't quite remember why I did the above, but it means nSingleMM_noofPulses is never zero, & is always 1 greater than what the user really
              // wants.  (I). This is corrected for when calling SendMessagePackets(... M sTyp5_SING_MM, ...) by subracting one
  //The following code is now defunct - 139 is no longer used as a sentinel value.
    //        if(nSingleMM_noofPulses == 139) { //Special number (138 as far as user is concerned)
      //         MessageBox(hDlg,"The Number 138 is reserved.",pStrCannotProceed,MB_OK);
        //       bReverseMove = FALSE;
          //     break;
            //  }
              if ((nSingleMM_noofPulses > 100000001)
                ||(nSingleMM_noofPulses < -100000001)
                ||!bTranslatedOK){
                 MessageBox(hDlg,"Number of Pulses must be between -100,000,000 and +100,000,000 ",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              else{
                 nSingleMM_SpeedRowChosen = SendMessage(hSINGLEMM_SPEED, CB_GETCURSEL,0,0);
                 if(nSingleMM_SpeedRowChosen == CB_ERR)
                   MessageBox(hDlg,"You may need to clear 'Enable On the Fly'\n\nIf you typed something in the box you need\nto clear this by selecting a row with your mouse","Error reading your Speed selection",MB_OK);
                 else{
                    if (bReverseMove)
                      nSingleMM_noofPulses *= -1;
    //The following is broken by the decision to create TCs for each and every point!
                 /* if(nSingleMM_noofPulses < 0)
                      nInfinitePulsesPosOrNeg = -2111000999;
                    else
                      nInfinitePulsesPosOrNeg =  2111000999; */
                    if(nSingleMM_noofPulses < 0){
                       nInfinitePulsesPosOrNeg = -20000;       //And this just isn't the same!
                       nSingleMM_noofPulses += 1;
                      }
                    else{
                       nInfinitePulsesPosOrNeg =  20000;
                       nSingleMM_noofPulses -= 1;
                      }
                    MakeWarningSound();
                    if(!nSingleMM_noofPulses){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
                       bReverseMove = FALSE;
                       break;
                      }
                    int noofAutoGenItemsOnSpeedList = SLOWEST2OFFERp1 - FASTEST2OFFER - 1; //(minus 1 because the row is zero based)
                    int nHowFarPastEndOfAutomaticallyGeneratedList = nSingleMM_SpeedRowChosen - noofAutoGenItemsOnSpeedList;
                    if(nHowFarPastEndOfAutomaticallyGeneratedList > 0){
                       switch (nHowFarPastEndOfAutomaticallyGeneratedList){
                          case  1:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses,  4800); break; //retained to allow either Constant Move or Fast Move
                          case  2:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses,  8000); break; //retained to allow either Constant Move or Fast Move
                          case  3:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, 12000); break; //retained to allow either Constant Move or Fast Move
                          case  4:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, 16000); break; //retained to allow either Constant Move or Fast Move
                          case  5:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, 24000); break; //retained to allow either Constant Move or Fast Move
                          case  6:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, 32000); break; //retained to allow either Constant Move or Fast Move
                          default:
                            SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, 50000); break; //retained to allow either Constant Move or Fast Move
                         }
                      }
                    else
                      SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp5_SING_MM, nSingleMM_noofPulses, nSingleMM_SpeedRowChosen + FASTEST2OFFER);
                   }
                 bReverseMove = FALSE;
                }
             }
           else{
              bReverseMove = FALSE;
              MessageBoxWarnNotConnected(hDlg);
             }
           break;




         case IDB_FASTSINGLEMM_REVERSEMOVENOW:
            bReverseMove = TRUE; //Note no break here!
         case IDB_FASTSINGLEMM_MOVENOW:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] ){
              BOOL bTranslatedOK;
              char szDum_mn[11];
              char *pStrCannotProceed = "Cannot Proceed!";
              UINT uiChars = GetDlgItemText(hDlg,IDC_SINGLEMM_NOOFPULSES,szDum_mn,sizeof(szDum_mn));
              if(!uiChars){
                 MessageBox(hDlg,"Couldn't Read the Number of Steps field",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              else if (uiChars > 9){
                 MessageBox(hDlg,"> 10 characters in the Number of Pulses field",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              UINT uiEachChar;
              for (uiEachChar=0; uiEachChar < uiChars; uiEachChar++)
                if( ((szDum_mn[uiEachChar] >'9') || (szDum_mn[uiEachChar] <'0'))  && (szDum_mn[uiEachChar] != '-') )
                  uiEachChar = 1000000;
              if (uiEachChar > 999){
                 MessageBox(hDlg,"Only characters '0' to '9', allowed in the Number of Steps field\nAs well as an optional leading minus sign [-]",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              int nSingleMM_noofPulses = (int) GetDlgItemInt(hDlg,IDC_SINGLEMM_NOOFPULSES,&bTranslatedOK,TRUE);
              if(nSingleMM_noofPulses < 0)
                nSingleMM_noofPulses--;
              else
                nSingleMM_noofPulses++;
              //I don't quite remember why I did the above, but it means nSingleMM_noofPulses is never zero, & is always 1 gretaer than what the user really
              // wants.  (I). This is corrected for when calling SendMessagePackets(... M sTyp5_SING_MM, ...) by subracting one
  //The following code is now defunct - 139 is no longer used as a sentinel value.
    //        if(nSingleMM_noofPulses == 139) { //Special number (138 as far as user is concerned)
      //         MessageBox(hDlg,"The Number 138 is reserved.",pStrCannotProceed,MB_OK);
        //       bReverseMove = FALSE;
          //     break;
            //  }
              if ((nSingleMM_noofPulses > 100000001)
                ||(nSingleMM_noofPulses < -100000001)
                ||!bTranslatedOK){
                 MessageBox(hDlg,"Number of Pulses must be between -100,000,000 and +100,000,000 ",pStrCannotProceed,MB_OK);
                 bReverseMove = FALSE;
                 break;
                }
              else{
                 nSingleMM_SpeedRowChosen = SendMessage(hSINGLEMM_SPEED, CB_GETCURSEL,0,0);
                 if(nSingleMM_SpeedRowChosen == CB_ERR)
                   MessageBox(hDlg,"You may need to clear 'Enable On the Fly'\n\nIf you typed something in the box you need\nto clear this by selecting a row with your mouse","Error reading your Speed selection",MB_OK);
                 else{
                    if (bReverseMove)
                      nSingleMM_noofPulses *= -1;
    //The following is broken by the decision to create TCs for each and every point!
                 /* if(nSingleMM_noofPulses < 0)
                      nInfinitePulsesPosOrNeg = -2111000999;
                    else
                      nInfinitePulsesPosOrNeg =  2111000999; */
                    if(nSingleMM_noofPulses < 0){
                       nInfinitePulsesPosOrNeg = -20000;       //And this just isn't the same!
                       nSingleMM_noofPulses += 1;
                      }
                    else{
                       nInfinitePulsesPosOrNeg =  20000;
                       nSingleMM_noofPulses -= 1;
                      }
                    MakeWarningSound();
                    if(!nSingleMM_noofPulses){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
                       bReverseMove = FALSE;
                       break;
                      }
                    FastMoveSingleMotor(hDlg, nchCurrentMotorZB, nSingleMM_noofPulses);
                    }
                 bReverseMove = FALSE;
                }
             }
           else{
              bReverseMove = FALSE;
              MessageBoxWarnNotConnected(hDlg);
             }
           break;





         case IDC_SINGLEMM_ENABLEONTHEFLY_RADIO:
           if( bOnTheFlySpeed ){
              bOnTheFlySpeed = FALSE;
              CheckRadioButton(hDlg, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,FALSE);
             }
           else{
              bOnTheFlySpeed = TRUE;
              CheckRadioButton(hDlg, IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,IDC_SINGLEMM_ENABLEONTHEFLY_RADIO,IDC_SINGLEMM_ENABLEONTHEFLY_RADIO);
             }
           break;
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}
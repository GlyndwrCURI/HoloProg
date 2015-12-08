#include "stdafx.h"

//extern double SetPoint;// = 7.89;
//extern double SetPointWP2red; //SetPoints are no longer looked at
//extern double SetPointWP3red;
//extern double SetPointWP2gre; //SetPoints are no longer looked at
//extern double SetPointWP3gre;
//extern double SetPointWP2blu; //SetPoints are no longer looked at
//extern double SetPointWP3blu;

extern char nchCurrentMotorZB;
//extern int  nArFastSpeeds4HomingDeptOn_uSteps[];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
extern PCFRECORD PCFrec;
extern HCFRECORD HCFrec;
extern BOOL bRunningManually;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;
//extern char szAccelFileNamesDir[_MAX_DIR];         //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//extern int  nLenszAccelFileNamesDir;               // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.

#define SIZSPINTEXT 10
INT_PTR CALLBACK SingleMM_PID_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 HWND hWndSpin, hWndSpinEdit;
//static HWND  hWndPIDnoofPulses;
 char szSpinText[SIZSPINTEXT];
 static char szDialogTitle[35] = "PID Move Test Dialog4Motor ";
 switch (message){
    case WM_INITDIALOG:{
//IDC_SPINJOG_PID  IDC_SPINCONTROL_EDITS_PID
       //hWndPIDnoofPulses = GetDlgItem(hDlg, IDC_SINGLEMMPID_NOOFPULSES);
       hWndSpin = GetDlgItem(hDlg, IDC_SPINJOG_PID);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITS_PID);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINJOG_PID, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       hWndSpin = GetDlgItem(hDlg, IDC_SPINPULSELEN);
       hWndSpinEdit = GetDlgItem(hDlg, IDC_SPINCONTROL_EDITP);
       SendMessage(hWndSpin, UDM_SETBUDDY, (WPARAM)hWndSpinEdit, 0L);
       SendDlgItemMessage(hDlg, IDC_SPINPULSELEN, UDM_SETRANGE, 0, MAKELPARAM(1,-1));

       if( bRunningManually )
         CheckRadioButton(hDlg, IDC_SINGLEMMPID_RUNMANUALLY, IDC_SINGLEMMPID_RUNMANUALLY, IDC_SINGLEMMPID_RUNMANUALLY);
       else
         CheckRadioButton(hDlg, IDC_SINGLEMMPID_RUNMANUALLY,IDC_SINGLEMMPID_RUNMANUALLY,FALSE);

       #define SIZszDumSpeedsNdistances 8
       //char szDumSpeedsNdistances[SIZszDumSpeedsNdistances];
       /*
       for(int ii = 1; ii < 301; ii++){
          _itoa_s(ii,szDumSpeedsNdistances,SIZszDumSpeedsNdistances,10);
          SendDlgItemMessage(hDlg,IDC_SINGLEMMPID_NOOFPULSES,CB_ADDSTRING,0,(LPARAM)szDumSpeedsNdistances);
         }
       SendDlgItemMessage (hDlg, IDC_SINGLEMMPID_NOOFPULSES, CB_SETCURSEL, 31, 0L);         //Set Default distance to 32  pulses
       */ //Instead of the above:
       SendDlgItemMessage(hDlg,IDC_SINGLEMMPID_NOOFPULSES,CB_ADDSTRING,0,(LPARAM)"Not currently used!");
       SendDlgItemMessage (hDlg, IDC_SINGLEMMPID_NOOFPULSES, CB_SETCURSEL, 0, 0L);         //Set Default distance to 32  pulses

       RECT ParentRc;           //Display dialog positioned to cover the bitmap on the menu bar
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, 3, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
       _itoa_s(nchCurrentMotorZB + 1, szDialogTitle+27, 6, 10);
       SetWindowText(hDlg,szDialogTitle);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_PRINTCONFIG_UR_APID: //Send the message to ALL pid controllers (if they're connected)
           if( PCFrec.bMotor2ExpectRepliesFrom[1] )
             SendMessagePackets(hDlg, 1, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           if( PCFrec.bMotor2ExpectRepliesFrom[2] )
             SendMessagePackets(hDlg, 2, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           if( PCFrec.bMotor2ExpectRepliesFrom[13] )
             SendMessagePackets(hDlg,13, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           if( PCFrec.bMotor2ExpectRepliesFrom[14] )
             SendMessagePackets(hDlg,14, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           if( PCFrec.bMotor2ExpectRepliesFrom[25] )
             SendMessagePackets(hDlg,25, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           if( PCFrec.bMotor2ExpectRepliesFrom[26] )
             SendMessagePackets(hDlg,26, MsTypF_YOU_ARE_A_PID, TRUE, TRUE);
           break;
         case IDB_PRINTCONFIG_UR_NOTAPID: //Send the message to ALL pid controllers (if they're connected)
           if( PCFrec.bMotor2ExpectRepliesFrom[1] )
             SendMessagePackets(hDlg, 1, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           if( PCFrec.bMotor2ExpectRepliesFrom[2] )
             SendMessagePackets(hDlg, 2, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           if( PCFrec.bMotor2ExpectRepliesFrom[13] )
             SendMessagePackets(hDlg,13, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           if( PCFrec.bMotor2ExpectRepliesFrom[14] )
             SendMessagePackets(hDlg,14, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           if( PCFrec.bMotor2ExpectRepliesFrom[25] )
             SendMessagePackets(hDlg,25, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           if( PCFrec.bMotor2ExpectRepliesFrom[26] )
             SendMessagePackets(hDlg,26, MsTypF_YOU_ARE_A_PID, 0, FALSE);
           break;
         case IDC_SPINCONTROL_EDITP: //Alter Pulse Length
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
         case IDC_SPINCONTROL_EDITS_PID:{ //Jog the motor
          //int nSingleMM_SpeedRowChosen = SendMessage(hWndPIDnoofPulses, CB_GETCURSEL,0,0);
          //if(nSingleMM_SpeedRowChosen == CB_ERR){
          //   MessageBox(hDlg,"If you typed something in the box you need\nto clear this by selecting a row with your mouse","Error reading your Selected Number of Steps",MB_OK);
          //   break;
          //  }
          //else{
          //   nSingleMM_SpeedRowChosen++;
               GetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS_PID, szSpinText, SIZSPINTEXT);
               if (szSpinText[0] == '-'){      //Move a negative number of steps
                  if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                    //SendMessagePackets(hDlg, nchCurrentMotorZB, MsTypB_SING_MM_PID, 0, nSingleMM_SpeedRowChosen * -1); //Jog Minus
                    SendMessagePackets(hDlg, nchCurrentMotorZB, MsTypB_SING_MM_PID, REVERSE_DIR, 60000); //Jog Minus
                  else
                    MessageBoxWarnNotConnected(hDlg);
                 }
               else if (szSpinText[0] == '1'){ //Move a positive number of steps
                  if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
                    //SendMessagePackets(hDlg, nchCurrentMotorZB, MsTypB_SING_MM_PID, 0, nSingleMM_SpeedRowChosen); //Jog  Plus
                    SendMessagePackets(hDlg, nchCurrentMotorZB, MsTypB_SING_MM_PID, FORWARD_DIR, 60000); //Jog  Plus
                  else
                    MessageBoxWarnNotConnected(hDlg);
                 }
               else if (szSpinText[0] == '0')
                 break;
            //}
            SetDlgItemText(hDlg, IDC_SPINCONTROL_EDITS_PID, "0");
           }
           break;
         case IDB_SINGLEMMPID_RESET:
           if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrentMotorZB] )
             SendMessagePackets(hDlg, nchCurrentMotorZB, MsTyp4_RESET,        0,       0);  //0x04 (RESET Message)
           else
             MessageBoxWarnNotConnected(hDlg);
           break;
         case IDB_SINGLEMMPID_HOME:     /*
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
         case IDC_SINGLEMMPID_ADDp1ToSETPOINT:
           MessageBox(hDlg,"SetPoints are no longer looked at","Disabled",MB_OK);
         /*if(nchCurrentMotorZB == 1)
             SetPointWP2red += 0.2;
           else if (nchCurrentMotorZB == 2)
             ;//SetPointWP3red += 0.2; broken 
           else if (nchCurrentMotorZB == 13)
             SetPointWP2gre += 0.2;
           else if (nchCurrentMotorZB == 14)
             ;//SetPointWP3gre += 0.2;
           else if (nchCurrentMotorZB == 25)
             SetPointWP2blu += 0.2;
           else
             ;//SetPointWP3blu += 0.2; */
           break;
         case IDC_SINGLEMMPID_SUBp1FromSETPOINT:
           MessageBox(hDlg,"SetPoints are no longer looked at","Disabled",MB_OK);
         /*if(nchCurrentMotorZB == 1)
             SetPointWP2red -= 0.2;
           else if (nchCurrentMotorZB == 2)
             ;//SetPointWP3red -= 0.2;
           else if (nchCurrentMotorZB == 13)
             SetPointWP2gre -= 0.2;
           else if (nchCurrentMotorZB == 14)
             ;//SetPointWP3gre -= 0.2;
           else if (nchCurrentMotorZB == 25)
             SetPointWP2blu -= 0.2;
           else
             ;//SetPointWP3blu -= 0.2; */
           break;
         case IDC_SINGLEMMPID_ADDp5ToSETPOINT:
           MessageBox(hDlg,"SetPoints are no longer looked at","Disabled",MB_OK);
         /*if(nchCurrentMotorZB == 1)
             SetPointWP2red += 1.0;
           else if (nchCurrentMotorZB == 2)
             ;//SetPointWP3red += 1.0;
           else if (nchCurrentMotorZB == 13)
             SetPointWP2gre += 1.0;
           else if (nchCurrentMotorZB == 14)
             ;//SetPointWP3gre += 1.0;
           else if (nchCurrentMotorZB == 25)
             SetPointWP2blu += 1.0;
           else
             ;//SetPointWP3blu += 1.0; */
           break;
         case IDC_SINGLEMMPID_SUBp5FromSETPOINT:
           MessageBox(hDlg,"SetPoints are no longer looked at","Disabled",MB_OK);
         /*if(nchCurrentMotorZB == 1)
             SetPointWP2red -= 1.0;
           else if (nchCurrentMotorZB == 2)
             ;//SetPointWP3red -= 1.0;
           else if (nchCurrentMotorZB == 13)
             SetPointWP2gre -= 1.0;
           else if (nchCurrentMotorZB == 14)
             ;//SetPointWP3gre -= 1.0;
           else if (nchCurrentMotorZB == 25)
             SetPointWP2blu -= 1.0;
           else
             ;//SetPointWP3blu -= 1.0; */
           break;
         case IDC_SINGLEMMPID_RUNMANUALLY:
           if( bRunningManually ){
              bRunningManually = FALSE;
              CheckRadioButton(hDlg, IDC_SINGLEMMPID_RUNMANUALLY,IDC_SINGLEMMPID_RUNMANUALLY,FALSE);
             }
           else{
              bRunningManually = TRUE;
              CheckRadioButton(hDlg, IDC_SINGLEMMPID_RUNMANUALLY,IDC_SINGLEMMPID_RUNMANUALLY,IDC_SINGLEMMPID_RUNMANUALLY);
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
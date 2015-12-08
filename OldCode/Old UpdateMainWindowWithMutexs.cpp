#include "stdafx.h"
extern HINSTANCE hInst;
extern HWND hGLWnd;
unsigned char cMessageSeqNoSR[NOOFMOTORS];

#define NOOFMOTORS2DISPLAY 40

//extern HANDLE ghMutex;
COMMS_ERROR_COUNT  CEC; //Structure containing counts of the various comms errors that occur in different places

extern int bWaitingForReply[];
extern BOOL bTimerHasBeenKilled; //This file only Kills the timer permanently - It is never temporary (i.e. after an error has occurred)

extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
//extern int  bMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec
//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
extern PCFRECORD PCFrec;
STATUSRECORD *pStatRec;
extern int  NOOFTIMESTHRU_B4OBJECTING;
//int  bSTATREC_BOXisBeingDisplayed;

//extern BYTE RevBits[256]; //= {0x00,0x80,0x40,0xC0,0x20 ... etc};
//The RevBits table is used as a fast way to reverse (i.e. mirror) the bits in a byte. The bits are in reverse order for the UC3's status record.
//0000 0000 -> 0000 0000
//0000 0001 -> 1000 0000
//0000 0010 -> 0100 0000
//0000 0011 -> 1100 0000
//We store the pattern windows expects in an array and use the value received to index into the array.
#define SIZ_SERIOUS_WARNS_TABLE 22
char SeriousWarningsStrings[SIZ_SERIOUS_WARNS_TABLE][46] = {//These are text descriptions (for the CPC to display) for status codes reported by the UC3 in it's status record
  "ok", //Error zero is No Error
  "E1: Limit switch 1 hit",
  "E2: Limit switch 2 hit",
  "E3: nTransitionNo >= MAXNOOFTRANSITIONALLOWED", //Error 3
  "E4: PixPrintSignal is Stopped",
  "E5: Unrecognised Sub-Type on Type 1 mess",
  "E6: BAD UDP CHECKSUM",             //Error 6
  "E7: 1514 byte Packet, unrecog Type",
  "E8: Critical Error!",
  "E9: Pulse Time Out of Bounds: ",   //Error 9
  "E10: Stored Error from previous Session:",  //Error was stored in flash ram - perhaps couldn't initialise PHY, or some other comms error (unimplemented)
  "E11: Unexpected Line No",
  //Don't change errors 12,13,14 without considering them being cleared in tc_irqWhenHoming()
  "E12: Slow Homing TC Not 60 or 61K",   //When homing out (slower) TC count must be 60K or 61K(non-pulsing)
  "E13: Error Homing-LONG Pulse fnd!",   //When homing in (faster) TC count must be no higher than 59999
  "E14: Homing-Run OUT, No Home Fund!",  //More Serious than not finding it on the way in (that could just be the stage being too far away)
  "E15: NoofUstepsPerStep : Out of Bounds",
  "E16: Error: U32Pulses2Go Set!",
  "E17: usLineNum2Convert not zero b4 conversion",
  "E18: 32 Mbyte SDRAM test Failed!",
  "E19: 4 Mbyte SDRAM test Failed!",
  "E20: Invalid Fill_raArray arg",
  "E21: pusEndOfConvertedTCdata 0, AccProf Sent?"
};

#define SIZ_WARNS2REPORT_TABLE 19
char Warnings2ReportStrings[SIZ_WARNS2REPORT_TABLE][40] = {//These are text descriptions (for the CPC to display) for status codes reported by the UC3 in it's status record
  "ok", //Warning zero is No Warning
  "W1: Invalid Message Param6",
  "W2: Invalid Message Param2",
  "W3: 64 byte Packet, unrecog Type", //Warning 3
  "W4: Invalid Message Param9",
  "W5: Dest Port Unreachable",
  "W6: Unimplemented ICMP packet",    //Warning 6
  "W7: MyIPfromDIPswitches > 39",     //Note: Any board whose IP > 39 will be set to IP 202. Just because motor2 shows the error does not mean it is motor 2 that has the problem! In anycase
  "W8: Unexpected Seq No",            // this will all probably not work because there will likely be a conflict with motor 2! The problem is best resolved by looking at RS485 debug output.
  "W9: RX SingMM_PID when running-IGNORED",  //Warning 9
  "W10: Homing-Run IN: Home not Found",      //Don't change W10 without considering it being cleared in tc_irqWhenHoming()
  "W11: Received RX SingMM while running",  
  "W12: RX SingMM_OTF when running-IGNORED", //increase message length up from 40 if extending this or any other message
  "W13: RX MPos Data when running-IGNORED",
  "W14: RX Home when running-IGNORED",
  "W15: LS Hi when looking for it (Line1)",
  "W16: LS Hi when looking for it (Main)",
  "W17: Update_raArray Out of Bounds",
  "W18: Update_raArray PixSignal Stopped"
};

#define SIZSTRING_SSM 80   //SSM==ShowStatusMultiple

#if 1
int UpdateMainWindowsControlsM(void){ //returns zero if it Succeeds
 char szStringSSM[SIZSTRING_SSM];
 int  nReturnCodeFromReceivePktsAr[NOOFMOTORS];
 int  nReturnCodeFromSendPkts;
 char nchMotorNo;
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(!PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo]){
      SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, "OK No Reply expected");
      SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + nchMotorNo, " ");
      ClearMostFields(nchMotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
     }
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo]){
      nReturnCodeFromSendPkts = SendMessagePackets(NULL, nchMotorNo, MsTyp2_ZENDstatREC, ++cMessageSeqNoSR[nchMotorNo], 0); //As the comms are repeatedly called by a timer it is 
      Sleep(2);
      if(nReturnCodeFromSendPkts){
         bTimerHasBeenKilled = TRUE;
         KillTimer(hGLWnd, IDT_TIMER_WNDPROC);//A "S endMessagePackets" Error in U pdateMainWindowsControlsM() kills the timer permanently
         ClearMostFields(nchMotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
         if(nReturnCodeFromSendPkts == 5001){
            CEC.SendPktErrStatScr5001++;
            SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, "Send() function took > 0.2 seconds");
            SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + nchMotorNo, " ");
           }
         else{
            CEC.SendPktErrStatScrOther++;
            SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, "COMMS Send Problem");
            StringCchCopy(szStringSSM,SIZSTRING_SSM, "Error Code: ");
            _itoa_s(nReturnCodeFromSendPkts, szStringSSM + 12, SIZSTRING_SSM - 12, 10);
            SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + nchMotorNo, szStringSSM);
           }
         MessageBox(hGLWnd,"Stopped 4 debugger (uncommon)!","nReturnCodeFromSendPkts Error",MB_OK);
         return 1; //return failure
        }
     }
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo]){
      bWaitingForReply[nchMotorNo] = 1;
      nReturnCodeFromReceivePktsAr[nchMotorNo] = ReceiveMessagePackets(NULL, nchMotorNo, MsTyp2_ZENDstatREC, 0);      // best to avoid MessageBoxes (hence use of NULLs)
     }
 //This next if Statement is ALSO VERY doubtful...
 //This next if Statement is VERY doubtful... but will help, during debugging, to show if we have not waited long enough for the reply.
 /*
 //if(nReturnCodeFromReceivePktsAr[nchMotorNo] == 6666){
 //   bTimerHasBeenKilled = TRUE;
 //   KillTimer(hGLWnd, IDT_TIMER_WNDPROC);
 //   int j;
 //   for(j = 0; j<5; j++){
 //      bWaitingForReply[nchMotorNo] = 1;//set again here because the reply failed
 //      nReturnCodeFromReceivePktsAr[nchMotorNo] = R eceiveMessagePackets(NULL, nchMotorNo, MsTyp2_ZENDstatREC, 0); //will have cleared this.
 //      if(!nReturnCodeFromReceivePktsAr[nchMotorNo]){
 //         MessageBox(hGLWnd,"Is now OK!\nWe are in U pdateMainWindowsControlsM()","nReturnCodeFromSendPackets Error",MB_OK);
 //         j = 1000;
 //        }
 //      if(j > 200)
 //        if(!SetTimer(hGLWnd, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
 //          MessageBox(hGLWnd,"STATREC_AUTO_TIMER reset problem!",NULL,MB_OK);
 //        else
 //          bTimerHasBeenKilled = FALSE;
 //     }
 //  }
 */
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo])
     if(nReturnCodeFromReceivePktsAr[nchMotorNo]){ //i.e. != 0

//int THIS__HAS___TO__GO;
//int SecondAttemptAtReceiving = ReceiveMessagePackets(NULL, 0, MsTyp2_ZENDstatREC, 0);      // best to avoid MessageBoxes (hence use of NULLs)
//if(SecondAttemptAtReceiving != 6666)
//  MessageBox(hGLWnd,"SecondAttemptAtReceiving was different!!!",NULL,MB_OK);

        bTimerHasBeenKilled = TRUE;
        KillTimer(hGLWnd, IDT_TIMER_WNDPROC);//A "R eceiveMessagePackets" Error in U pdateMainWindowsControlsM() kills the timer permanently
        ClearMostFields(nchMotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
        SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + nchMotorNo, "COMMS Problem");
        if(nReturnCodeFromReceivePktsAr[nchMotorNo] == 6666){
           CEC.RecPktErrStatScr6666++;
           SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, "Error Code: 6666 - No Reply");
          }
        else{
           if(nReturnCodeFromReceivePktsAr[nchMotorNo] == 123456){
              CEC.RecPktErrStatScr123456++;
              StringCchCopy(szStringSSM,SIZSTRING_SSM,"Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
             }
           else{
              CEC.RecPktErrStatScrOther++;
              StringCchCopy(szStringSSM,SIZSTRING_SSM,"Error Code: ");
              _itoa_s(nReturnCodeFromReceivePktsAr[nchMotorNo], szStringSSM + 12, SIZSTRING_SSM - 12, 10);
             }
           SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, szStringSSM);
          }//\n\nYou may need to wait a few seconds for\nsystem response to  be restored
        if(TIMEOUT_FOR_STARTUP == NOOFTIMESTHRU_B4OBJECTING)
          MessageBox(hGLWnd,"The Update-Screen Timer has been stopped because of this error.\n\nPress 'Auto - Refresh' button if you want to start the Timer again, but\nyou may need to resolve the error first.\n\nIf the AVRs are rebooting because you just restarted Windows,\nWait until they're finished then press 'Auto - Refresh'","U_pdateStatusDialogsControlsM: Didn't get reply in alloted time @1.",MB_OK);
        else
          MessageBox(hGLWnd,"The Update-Screen Timer has been stopped because of this error.\n\nPress 'Auto - Refresh' button if you want to start the Timer again, but\nyou may need to resolve the error first.","U_pdateStatusDialogsControlsM: Didn't get reply in alloted time @2.",MB_OK);
        return 1; //return failure
       }
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo])
     if(chReceiveBuffer[nchMotorNo][1] != cMessageSeqNoSR[nchMotorNo]){
        CEC.RecPktErrStatScrSeqNo++;
        bTimerHasBeenKilled = TRUE;
        KillTimer(hGLWnd, IDT_TIMER_WNDPROC);//An unmatched "Sequence number" Error in U pdateMainWindowsControlsM() kills the timer permanently
        ClearMostFields(nchMotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
        #define SIZszSeriousText 24
        #define SIZszWarnText    28
        char szSeriousText[SIZszSeriousText];
        char szWarnText[SIZszWarnText];
        StringCchCopy(szSeriousText, SIZszSeriousText, "Message seqNo Sent:    ");
        _itoa_s(cMessageSeqNoSR[nchMotorNo], szSeriousText+20, SIZszSeriousText - 20, 10);
        SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + nchMotorNo, szSeriousText);
        StringCchCopy(szWarnText, SIZszWarnText, "Message seqNo Received:    ");
        _itoa_s(chReceiveBuffer[nchMotorNo][1], szWarnText+24, SIZszWarnText - 24, 10);
        SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + nchMotorNo, szWarnText);
        //MessageBox(hGLWnd,"I stopped so you can see(3)!","Seqno Error",MB_OK);
        MessageBoxToDisplaySeqNoError(hGLWnd, "At 44: ", cMessageSeqNoSR[nchMotorNo], chReceiveBuffer[nchMotorNo][1], nchMotorNo);
        return 1; //return failure
       }
 for(nchMotorNo = 0; nchMotorNo < NOOFMOTORS; nchMotorNo++)
   if(PCFrec.bMotor2ExpectRepliesFrom[nchMotorNo]){
      pStatRec = (STATUSRECORD*)&chReceiveBuffer[nchMotorNo][2]; //+2 because the reply from AVR contains a message-type in the first 2 bytes
      UseRecvStatRecToUpdateTheFields(nchMotorNo, pStatRec);
     }
 //pStatRec->Length = 0x3f;
 return 0; //Success
}

#else  //This was the old attempt, with mutex's, and when the controls were in a dialogbox rather than the main Window.

//void U pdateMainWindowsControlsM(HWND hDlg, int MotorNo){
// char szStringSSM[SIZSTRING_SSM];
// //int ItemporarilyOverRodeMotorNo; //Because there is only One board to talk to at the moment
// //MotorNo = 18;
// int nReturnCodeFromReceivePackets;
// int nReturnCodeFromSendPackets;
//
// if(!PCFrec.bMotor2ExpectReplievsFrom[MotorNo]){
//    SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "OK No Reply expected");
//    SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, " ");
//    ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
//    return;
//   }
//
// DWORD dwWaitResult = WaitForSingleObject( ghMutex, INFINITE);  // no time-out interval
// switch (dwWaitResult) {  // The thread got ownership of the mutex
//    case WAIT_OBJECT_0:
//      __try {
//         //jj bWaitingForReply[MotorNo] = 1;//Set before calling SendMessagePackets() whenever a reply is expected
//         nReturnCodeFromSendPackets = SendMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, ++cMessageSeqNoSR[MotorNo], 0); //As the comms are repeatedly called by a timer it is 
//
//         if(nReturnCodeFromSendPackets){
//            bTimerHasBeenKilled = TRUE;
//            KillTimer(hDlg, IDT_TIMER_WNDPROC);
//            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
//            if(nReturnCodeFromSendPackets == 5001){
//               CEC.SendPktErrStatScr5001++;
//               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "Send() function took > 0.2 seconds");
//               SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, " ");
//              }
//            else{
//               CEC.SendPktErrStatScrOther++;
//               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "COMMS Send Problem");
//               StringCchCopy(szStringSSM,SIZSTRING_SSM, "Error Code: ");
//               _itoa_s(nReturnCodeFromSendPackets, szStringSSM + 12, SIZSTRING_SSM - 12, 10);
//               SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, szStringSSM);
//              }
//            MessageBox(hDlg,"Stopped 4 debugger (uncommon)!","nReturnCodeFromSendPackets Error",MB_OK);
//            return;
//           }
//         bWaitingForReply[MotorNo] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
//         nReturnCodeFromReceivePackets = R eceiveMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, 0);      // best to avoid MessageBoxes (hence use of NULLs)
////This next if Statement is ALSO VERY doubtful...
////This next if Statement is VERY doubtful... but will help, during debugging, to show if we have not waited long enough for the reply.
///*
//         if(nReturnCodeFromReceivePackets == 6666){
//            bTimerHasBeenKilled = TRUE;
//            K illTimer(hDlg, IDT_TIMER_WNDPROC);
//            int j;
//            for(j = 0; j<5; j++){
//               bWaitingForReply[MotorNo] = 1;//set again here because the reply failed
//               nReturnCodeFromReceivePackets = R eceiveMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, 0); //will have cleared this.
//               if(!nReturnCodeFromReceivePackets){
//                  MessageBox(hDlg,"Is now OK!\nWe are in U pdateMainWindowsControlsM()","nReturnCodeFromSendPackets Error",MB_OK);
//                  j = 1000;
//                 }
//               if(j > 200)
//                 if(!SetTimer(hDlg, IDT_TIMER_WNDPROC, AUTOREFRESHRATE, (TIMERPROC) NULL))
//                   MessageBox(hDlg,"STATREC_AUTO_TIMER reset problem!",NULL,MB_OK);
//                 else
//                   bTimerHasBeenKilled = FALSE;
//              }
//           }
//*/
//
//         if(nReturnCodeFromReceivePackets){ //i.e. != 0
//            bTimerHasBeenKilled = TRUE;
//            KillTimer(hDlg, IDT_TIMER_WNDPROC);
//            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
//            SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, "COMMS Problem");
//            if(nReturnCodeFromReceivePackets == 6666){
//               CEC.RecPktErrStatScr6666++;
//               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "Error Code: 6666 - No Reply");
//              }
//            else{
//               if(nReturnCodeFromReceivePackets == 123456){
//                  CEC.RecPktErrStatScr123456++;
//                  StringCchCopy(szStringSSM,SIZSTRING_SSM,"Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
//                 }
//               else{
//                  CEC.RecPktErrStatScrOther++;
//                  StringCchCopy(szStringSSM,SIZSTRING_SSM,"Error Code: ");
//                  _itoa_s(nReturnCodeFromReceivePackets, szStringSSM + 12, SIZSTRING_SSM - 12, 10);
//                 }
//               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szStringSSM);
//              }//\n\nYou may need to wait a few seconds for\nsystem response to  be restored
//            MessageBox(hDlg,"The Update-Screen Timer has been stopped because of this error.\n\nPress 'Auto - Refresh' button if you want to start the Timer again, but\nyou may need to resolve the error first.","nReturnCodeFromReceivePackets Error",MB_OK);
//            return;
//           }
//         if(c hReceiveBuffer[MotorNo][1] != c MessageSeqNoSR[MotorNo]){
//            CEC.RecPktErrStatScrSeqNo++;
//            bTimerHasBeenKilled = TRUE;
//            KillTimer(hDlg, IDT_TIMER_WNDPROC);
//            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
//            #define SIZszSeriousText 24
//            #define SIZszWarnText    28
//            char szSeriousText[SIZszSeriousText];
//            char szWarnText[SIZszWarnText];
//            StringCchCopy(szSeriousText, SIZszSeriousText, "Message seqNo Sent:    ");
//            _itoa_s(cMessageSeqNoSR[MotorNo], szSeriousText+20, SIZszSeriousText - 20, 10);
//            SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szSeriousText);
//            StringCchCopy(szWarnText, SIZszWarnText, "Message seqNo Received:    ");
//            _itoa_s(chReceiveBuffer[MotorNo][1], szWarnText+24, SIZszWarnText - 24, 10);
//            SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, szWarnText);
//            MessageBox(hDlg,"I stopped so you can see(3)!","S eqno Error",MB_OK);  //in old attempt, with mutex's
//            return;
//           }
//         pStatRec = (STATUSRECORD*)&chReceiveBuffer[MotorNo][2]; //+2 because the reply from AVR contains a message-type in the first 2 bytes
//         UseRecvStatRecToUpdateTheFields(hDlg, MotorNo, pStatRec);
//
//         //pStatRec->Length = 0x3f;
//        }
//      __finally {  // Release ownership of the mutex object
//         if (! ReleaseMutex(ghMutex)) 
//           MessageBox(hDlg,"ReleaseMutex(ghMutex) Failed 7!",NULL,MB_OK);
//        }
//      break;
//    case WAIT_ABANDONED:  // The thread got ownership of an abandoned mutex
//      MessageBox(hDlg,"Got ownership of an abandoned mutex@7!",NULL,MB_OK);
//      break;
//   }
//}

#endif //#if 1's #else

//void ClearMostFields(HWND hDlg, int nnMotorNo){
void ClearMostFields(int nnMotorNo){ //stick with nnMotorNo being an int here: there's no danger from signed/unsigned conversions / comparisons etc
 SetDlgItemText(hGLWnd, IDB_STATREC_POS         + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_HOME_STATE  + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWa   + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWb   + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_HOME_SW     + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_DRIVER_ENAB + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_RS485_ENAB  + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_IWANTLow    + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_IT_IS_Low   + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_REDYTPAL    + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_VARa        + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_VARb        + nnMotorNo, "");
 SetDlgItemText(hGLWnd, IDB_STATREC_VARc        + nnMotorNo, "");
}

//void UseRecvStatRecToUpdateTheFields(HWND hDlg, int n_MotorNo, STATUSRECORD *pStatRec){
void UseRecvStatRecToUpdateTheFields(int n_MotorNo, STATUSRECORD *pStatRec){
 char szuStepsFromHome[15];
 char szVARa[15];
 char szVARb[15];
 char szVARc[15];
 char szStringSSM[SIZSTRING_SSM];

 endian_swap(*(unsigned int*)pStatRec);       //This is for the uStepsFromHome integer only
 endian_swap(*(((unsigned int*)pStatRec)+1)); //uVARa
 endian_swap(*(((unsigned int*)pStatRec)+2)); //uVARb
 endian_swap(*(((unsigned int*)pStatRec)+3)); //uVARc

 _itoa_s(pStatRec->uStepsFromHome, szuStepsFromHome, 15, 10);
 if(pStatRec->usHomingState) //usHomingState should be zero, if homing has occurred (& so position is known)
   StringCchCat(szuStepsFromHome,15," UH");

 SetDlgItemText(hGLWnd, IDB_STATREC_POS + n_MotorNo, szuStepsFromHome);

 _itoa_s(pStatRec->uVARa, szVARa, 15, 10);
 SetDlgItemText(hGLWnd, IDB_STATREC_VARa + n_MotorNo, szVARa);
 _ultoa_s(pStatRec->uVARb, szVARb, 15, 10);
 SetDlgItemText(hGLWnd, IDB_STATREC_VARb + n_MotorNo, szVARb);
 _itoa_s(pStatRec->uVARc, szVARc, 15, 10);
 SetDlgItemText(hGLWnd, IDB_STATREC_VARc + n_MotorNo, szVARc);

                                ///1st 4b
 //EVK sent     0000 1011
 //PC received 1101 0000, so the bits are completely reversed in order.
 //To check this applies also to the 2-bit usHomingState field,
 //EVK sent     0000 1001
 //PC received 1001 0000 (so it does)
 //The first byte sent is the first byte received, so to correct the Endianess (or bit ordering?), we need a function that processes bytes
 if(pStatRec->Limit1Switch)
   SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWa + n_MotorNo, "X");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWa + n_MotorNo, "");
 if(pStatRec->Limit2Switch)
   SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWb + n_MotorNo, "X");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_LIMIT_SWb + n_MotorNo, "");
 if(pStatRec->HomeSwitch)
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_SW + n_MotorNo, "X");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_SW + n_MotorNo, "");

 if(pStatRec->ImReadyTPAL)
   SetDlgItemText(hGLWnd, IDB_STATREC_REDYTPAL + n_MotorNo, "Y");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_REDYTPAL + n_MotorNo, "N");

                                ///2nd 4b
 if(pStatRec->bDriverEnabled)
   SetDlgItemText(hGLWnd, IDB_STATREC_DRIVER_ENAB + n_MotorNo, "X");
 else 
   SetDlgItemText(hGLWnd, IDB_STATREC_DRIVER_ENAB + n_MotorNo, "");
 if(pStatRec->bRS485Enabled)
   SetDlgItemText(hGLWnd, IDB_STATREC_RS485_ENAB + n_MotorNo, "X");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_RS485_ENAB + n_MotorNo, "");

 if(!pStatRec->usHomingState)     //I think I just have to accept the bit reversing means HomingState 1 is shown
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_STATE + n_MotorNo, "H");
 else if(pStatRec->usHomingState == 1)
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_STATE + n_MotorNo, "U");
 else if(pStatRec->usHomingState == 2)
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_STATE + n_MotorNo, "I");//Currently homing In  (towards Home / motor)
 else if(pStatRec->usHomingState == 3)
   SetDlgItemText(hGLWnd, IDB_STATREC_HOME_STATE + n_MotorNo, "O");//Currently homing Out (Away from Home / motor)
                                ///BYTE
 if(pStatRec->SeriousWarnings > SIZ_SERIOUS_WARNS_TABLE)
   StringCchCopy(szStringSSM, SIZSTRING_SSM, "Need2updateHoloProg Lookup SW" );
 else
   StringCchCopy(szStringSSM, SIZSTRING_SSM, *(SeriousWarningsStrings + pStatRec->SeriousWarnings) );
 SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + n_MotorNo, szStringSSM);

                                ///BYTE
 /*if(pStatRec->Warnings2Report){
  _itoa_s(pStatRec->Warnings2Report, szStringSSM, SIZSTRING_SSM, 10);
    SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + n_MotorNo, szStringSSM);
   }
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + n_MotorNo, "ok"); */
 if(pStatRec->Warnings2Report > SIZ_WARNS2REPORT_TABLE)
   StringCchCopy(szStringSSM, SIZSTRING_SSM, "Need2updateW2RLookup (HoloProg)" );
 else
   StringCchCopy(szStringSSM, SIZSTRING_SSM, *(Warnings2ReportStrings + pStatRec->Warnings2Report) );
 SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + n_MotorNo, szStringSSM);

                                ///4th BYTE
 if(pStatRec->IWantStatusOutLineLow)
   SetDlgItemText(hGLWnd, IDB_STATREC_IWANTLow + n_MotorNo, "L");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_IWANTLow + n_MotorNo, "H");
 if(pStatRec->StatusOutLineIsHi)
   SetDlgItemText(hGLWnd, IDB_STATREC_IT_IS_Low + n_MotorNo, "H");
 else
   SetDlgItemText(hGLWnd, IDB_STATREC_IT_IS_Low + n_MotorNo, "L");


 //RECT rClient;
 //redraw(hGLWnd, &rClient);
 redraw();
 
 //UpdateWindow(hGLWnd);
}
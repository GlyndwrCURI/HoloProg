//                           MoveRefTableToLoadPosition.cpp
//MoveRefTableToLoadPosition() is very similar M oveToStartPositions()
#include "stdafx.h"
extern unsigned char cMessageSeqNoSR[NOOFMOTORS];
extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern int bWaitingForReply[];
extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern char mes[];

//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
extern PCFRECORD PCFrec;

extern int nArMaxSpeedsDependentOn_uSteps[65];
extern STATUSRECORD *pStatRec;
extern HWND hGLWnd;


void MoveREF_BEAM_TABLEtoLoadPosition(HWND hDlg, int nDesiredRefTablePosM1, int nDesiredRefTablePosM2, BOOL bMoveRefTableFast){
 int nSleepDelay = 8;
 int nReturnCodeFromSendPacketsM2LP;
 int M2SP_RMP_RetCode;
 int nCurrent_uStepsForM37 = PCFrec.chNoofUsteps4EachMotorTab[36];
 int nCurrent_uStepsForM38 = PCFrec.chNoofUsteps4EachMotorTab[37];

 int RBTbMotorHomed[2] = {0,0};    //This doesn't mean 'At Home' (i.e. position 0), but just that the position is valid
 int RBTbMotorPosition[2];         // ... The position being stored here
 MSG nullMSG;

 if( !((PCFrec.bMotor2ExpectRepliesFrom[36]) && (PCFrec.bMotor2ExpectRepliesFrom[37])) ){
    MessageBox(hDlg,"MoveRefTable2LoadPos operates only when the Printer Configuration shows both motors as being present!",NULL,MB_OK);
    return;
   }

 for (char emMRT2LP = 36; emMRT2LP < 38; emMRT2LP++) //Don't change these numbers without looking below at [emMRT2LP - 36]  & probably [emMRT2LP2 - 36]
   if(PCFrec.bMotor2ExpectRepliesFrom[emMRT2LP]){
     nReturnCodeFromSendPacketsM2LP = SendMessagePackets (hDlg, emMRT2LP, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[emMRT2LP], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)
     if(nReturnCodeFromSendPacketsM2LP){
        if(nReturnCodeFromSendPacketsM2LP == 5001){
           CEC.SendPktErrTFnP5001++;
           MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveRefTable2LoadPos: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
          }
        else{
           char sz_aString[11];
           CEC.SendPktErrTFnPOther++;
           _itoa_s(nReturnCodeFromSendPacketsM2LP, sz_aString, 11, 10);
           MessageBox(hDlg,sz_aString,"MoveRefTable2LoadPos: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
          }
        return;
       }

     bWaitingForReply[emMRT2LP] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
     M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, emMRT2LP, MsTyp2_ZENDstatREC, 0);
     if (M2SP_RMP_RetCode){
        char szCode[12];
        StringCchCopy(mes,SIZMES,"MoveRefTable2LoadPos:Receive Message Packets() at 1 returned code: ");//ViewHologInfo_Dialog
        if(M2SP_RMP_RetCode == 6666){
           CEC.RecPktErrTFnP6666++;
           StringCchCat(mes,SIZMES,"\n\nNo Reply");
          }
        else{
           if(M2SP_RMP_RetCode == 123456){
              CEC.RecPktErrTFnP123456++;
              StringCchCat(mes,SIZMES,"MoveRefTable2LoadPos:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
             }
           else{
              CEC.RecPktErrTFnPOther++;
              _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
              StringCchCat(mes,SIZMES,szCode);
             }
          }
        
        StringCchCat(mes,SIZMES,  " for Motor: ");
        DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, emMRT2LP, mes, "Error");
        return;
       }
     if( cMessageSeqNoSR[emMRT2LP] != chReceiveBuffer[emMRT2LP][1] ){
        MessageBoxToDisplaySeqNoError(hDlg, "At 43: ", cMessageSeqNoSR[emMRT2LP], chReceiveBuffer[emMRT2LP][1], emMRT2LP);
        return;
       }
    //StatusRec is now received - let's look at it.
     pStatRec = (STATUSRECORD*)&chReceiveBuffer[emMRT2LP][2]; //[2] because the reply from AVR contains a message-type & SeqNoSR in the first 2 bytes
     if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
      //if(bSTATREC_BOXisBeingDisplayed) //i.e Do we need to show the user the 'Current Status for all Motors' Dialog box, or can they already see it?
          MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","'Move To Load Positions' was NOT started!",MB_OK);
      //else //as the STATREC dialogbox is not being displayed, let's show it to them:
      //  if(IDOK == MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding\n\nHere, let me show you...                     Press Cancel if you don't want me to","The HOMING part of 'MoveRefTable2LoadPos' was NOT started!",MB_OKCANCEL))
      //    PostMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
        return;  //Initial Status SeriousWarnings or Warnings from UC3 via R eceiveMessagePackets() stops us from proceding
       }
     if(!pStatRec->bDriverEnabled){
        MessageBox(hDlg,"This needs to be enabled before proceeding","Motor Driver is not enabled!",MB_OK);
        return;  //Initial Status stops us from proceding
       }

     if(pStatRec->bRS485Enabled){
        int nReplyButtonPressed = MessageBox(hDlg,"Do you want to proceed (at a slower rate) with debug messages pouring out?\n\nPress No to Switch off RS485 (but still proceed)\n\nPress Cancel if you don't want to Move the motors","Warning RS485 is enabled!",MB_YESNOCANCEL|MB_DEFBUTTON2);
        if(IDCANCEL == nReplyButtonPressed){
           MessageBox (hDlg,"'MoveRefTable2LoadPos' was aborted","Message",MB_OK);
           return;  //Initial Status stops us from proceding
          }
        else if (IDNO == nReplyButtonPressed){//User wants to switch off RS485, then proceed
          nReturnCodeFromSendPacketsM2LP = SendMessagePackets (hDlg, emMRT2LP, MsTyp6_ENAB_DISAB,  DISAB_485, 0);
          if(nReturnCodeFromSendPacketsM2LP){
             if(nReturnCodeFromSendPacketsM2LP == 5001){
                CEC.SendPktErrTFnP5001++;
                MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveRefTable2LoadPos@6: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
               }
             else{
                char sz_aString2[11];
                CEC.SendPktErrTFnPOther++;
                _itoa_s(nReturnCodeFromSendPacketsM2LP, sz_aString2, 11, 10);
                MessageBox(hDlg,sz_aString2,"MoveRefTable2LoadPos@6: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
               }
             MessageBox (hDlg,"'MoveRefTable2LoadPos' was aborted","Message",MB_OK);
             return;  //Initial Status stops us from proceding
            }
          }
        else
          nSleepDelay = 20;
       }
     if (!pStatRec->usHomingState){
        endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
        RBTbMotorHomed[emMRT2LP - 36] = 1;
        RBTbMotorPosition[emMRT2LP - 36] = pStatRec->uStepsFromHome;
       }
   }
          //If Either Motor is not homed, Home both of them, then move to Load Position. If they are both homed, calculate
          // the move that needs to be made and move straight there
//int WARNNGNExtlineisWRONG_PROB_REMOVE_NOW; //should be !RBTbMotorHomed[1] (when the hardware is available)
 if (!RBTbMotorHomed[0] || !RBTbMotorHomed[1]){ //Homing needs to be done, so issue Homing Instruction. No reply is expected, so will be ok even in a 1 motor development environment
    char emot;
    for(emot = 37; emot > 35; emot--){
     //int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[emot];
     //int                                                        This_probably_needs_to_be_replaced;
     //SendMessagePackets(hDlg,
     //                     emot,
     //            MsTyp3_HOME, //******* ******* ******* *******This probably needs to be replaced by a call to HomeSingleMotor(hDlg, nchCurrentMotorZB); ******* ******* **************
     //        nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
     //     nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant Fast speed to move at, when homing in
       HomeSingleMotor(hDlg, emot);
                     //The limits on 'how many steps to move' are still determined by UC3 hardcode
       Sleep(nSleepDelay); //8 or 20, depending on if RS485 is enabled. It's questionable if any delay is necessary anymore.
      }
//Now need to keep checking the status recs until all motors are homed...
    int ArHomingStates[2] = {1,1};
  //int nHomingAllMotorsTimeout = 4;       //Allow up to 8 secs for the motors to home. 
    int nHomingAllMotorsTimeout = 6000000; //Allow forever secs for the motors to home (but finish loop earlier if all homed)

    while(nHomingAllMotorsTimeout-- >= 0){
       Sleep(1000); //At least give them a chance to get started
#define REWRITTEN_THIS
#ifdef REWRITTEN_THIS
       if( PeekMessage(&nullMSG, hGLWnd, WM_STOPHOMINGMESS_FROM_HELPERPROG, WM_STOPHOMINGMESS_FROM_HELPERPROG, PM_REMOVE | PM_NOYIELD) ){//If HoloProgHelper sends an ABORT_HOMING_MESSAGE we should stop HoloProg waiting for homing to occur
          UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
          return;
         }
       if(UpdateMainWindowsControlsM()){
          UpdateWindow(hGLWnd);           //If UpdateMainWindowsControlsM() failed, it will have put out a message and killed the timer
          return;
         }
       UpdateWindow(hGLWnd);
       for (char emMRT2LP2 = 36;emMRT2LP2 < 38; emMRT2LP2++)
         if(PCFrec.bMotor2ExpectRepliesFrom[emMRT2LP2]){ //These two motors cannot be pids, so no need to test, here, if they are
            pStatRec = (STATUSRECORD*)&chReceiveBuffer[emMRT2LP2][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
            if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){ //<- Could Abort //WARNINGS APPEARED
             //if(bSTATREC_BOXisBeingDisplayed)
               //if(bStopWhenHomed)
                 MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of 'MoveRefTable2LoadPos' Started... BUT",MB_OK);
               //else
                 //MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","HOMING Started... BUT",MB_OK);
               restartPIDs();
               return; //bAbortMOVE2START = TRUE;
              }
            if( (!pStatRec->usHomingState) && !pStatRec->uStepsFromHome) //If these are both true, the motor is homed
              ArHomingStates[emMRT2LP2 - 36] = 0;//Set to zero when this motor is homed at position zero.
            else
              ArHomingStates[emMRT2LP2 - 36] = 1;
           }
#else
       for (char emMRT2LP2 = 36;emMRT2LP2 < 38; emMRT2LP2++){
        //if( (!PCFrec.bMotor2ExpectRepliesFrom[emMRT2LP2]) || (!ArHomingStates[emMRT2LP2 - 36]) )
          if( !ArHomingStates[emMRT2LP2 - 36] )
            continue; //i.e. don't ask for status recs after we've aborted, or if we're not expecting a reply, or if the motor is already homed
          nReturnCodeFromSendPacketsM2LP = SendMessagePackets (hDlg, emMRT2LP2, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[emMRT2LP2], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)
          if(nReturnCodeFromSendPacketsM2LP){ /*<- Could Abort*/
             if(nReturnCodeFromSendPacketsM2LP == 5001){
                CEC.SendPktErrTFnP5001++;
                MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveRefTable2LoadPosH: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
               }
             else{
                char sz_aString3[11];
                CEC.SendPktErrTFnPOther++;
                _itoa_s(nReturnCodeFromSendPacketsM2LP, sz_aString3, 11, 10);
                MessageBox(hDlg,sz_aString3,"MoveRefTable2LoadPosH: nReturnCodeFromSendPacketsM2LP Error Code:",MB_OK);
               }
             return;
            }

          bWaitingForReply[emMRT2LP2] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
          M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, emMRT2LP2, MsTyp2_ZENDstatREC, 0);
          if (M2SP_RMP_RetCode){              /*<- Could Abort*/
             char szCode[12];
             StringCchCopy(mes,SIZMES,"MoveRefTable2LoadPosH:Receive Message Packets() at 2 returned code: "); //ViewHologInfo_Dialog
             if(M2SP_RMP_RetCode == 6666){
                CEC.RecPktErrTFnP6666++;
                StringCchCat(mes,SIZMES,"\n\nNo Reply");
               }
             else{
                if(M2SP_RMP_RetCode == 123456){
                   CEC.RecPktErrTFnP123456++;
                   StringCchCat(mes,SIZMES,"MoveRefTable2LoadPosH:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
                  }
                else{
                   CEC.RecPktErrTFnPOther++;
                   _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
                   StringCchCat(mes,SIZMES,szCode);
                  }
               }
             MessageBox(hDlg,mes,NULL,MB_OK);
             return;
            }
          if( cMessageSeqNoSR[emMRT2LP2] != chReceiveBuffer[emMRT2LP2][1] ){  //SEQNO ERROR    /*<- Could Abort*/
             MessageBoxToDisplaySeqNoError(hDlg, "At 54: ", cMessageSeqNoSR[emMRT2LP2], chReceiveBuffer[emMRT2LP2][1], emMRT2LP2);
             return;
            }
          pStatRec = (STATUSRECORD*)&chReceiveBuffer[emMRT2LP2][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
          if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){           //WARNINGS APPEARED              /*<- Could Abort*/
             MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of MoveREF_BEAM_TABLEtoLoadPosition() Started... BUT",MB_OK);
             return;
            }
          endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
        //endian_swap(*(((unsigned int*)pStatRec)+1));
        //endian_swap(*(((unsigned int*)pStatRec)+2));
        //endian_swap(*(((unsigned int*)pStatRec)+3));
          if( (!pStatRec->usHomingState) && !pStatRec->uStepsFromHome) //If these are both true, the motor is homed
            ArHomingStates[emMRT2LP2 - 36] = 0;//Set to zero when this motor is homed at position zero.
          else
            ArHomingStates[emMRT2LP2 - 36] = 1;
         }
#endif

       if( !(ArHomingStates[0]+ArHomingStates[1]) )
         nHomingAllMotorsTimeout = -100;  //break out of the loop now that we're homed properly on all motors
      }//while(nHomingAllMotorsTimeout)

    if(nHomingAllMotorsTimeout > -5){ //we timed out
       MessageBox (hDlg,"At least 1 Motor failed to Home (timed out)... \n\nYou may need simply to try again (if the cause of the problem was\nthat the motor had a very long way to travel)","In MoveREF_BEAM_TABLEtoLoadPosition()... ",MB_OK);
       return;
      }
    if(nDesiredRefTablePosM1){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveRefTableFast)
         FastMoveSingleMotor(hDlg, 36, nDesiredRefTablePosM1);
       else
         SendMessagePackets(hDlg, 36, MsTyp5_SING_MM, nDesiredRefTablePosM1, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM37] ); //Motor 37 RefBeamTable M1
      }

    if(nDesiredRefTablePosM2){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveRefTableFast)
         FastMoveSingleMotor(hDlg, 37, nDesiredRefTablePosM2);
       else
         SendMessagePackets(hDlg, 37, MsTyp5_SING_MM, nDesiredRefTablePosM2, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM38] );//Motor 38 RefBeamTable M2
      }

   }// if (!RBTbMotorHomed[0] || !RBTbMotorHomed[1])  i.e. we needed to do homing
 else{
    //The Motors were homed, so we can rely on their position, so calc the move that needs to be made based on that
    int nStepsM37NeedsToMove = nDesiredRefTablePosM1 - RBTbMotorPosition[0];
    int nStepsM38NeedsToMove = nDesiredRefTablePosM2 - RBTbMotorPosition[1];

    if(nStepsM37NeedsToMove){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveRefTableFast)
         FastMoveSingleMotor(hDlg, 36, nStepsM37NeedsToMove);
       else
         SendMessagePackets(hDlg, 36, MsTyp5_SING_MM, nStepsM37NeedsToMove, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM37] );//Motor 37 RefBeamTable M1
      }
    if(nStepsM38NeedsToMove){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveRefTableFast)
         FastMoveSingleMotor(hDlg, 37, nStepsM38NeedsToMove);
       else
         SendMessagePackets(hDlg, 37, MsTyp5_SING_MM, nStepsM38NeedsToMove, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM38] );//Motor 38 RefBeamTable M2
      }
   }
}

/************************************************************************************************************************************************/
/************************************************************************************************************************************************/
/************************************************************************************************************************************************/
/*************************************************** SIMILAR TO ABOVE, BUT JUST ONE MOTOR *******************************************************/
/************************************************************************************************************************************************/
/************************************************************************************************************************************************/
/************************************************************************************************************************************************/
void MoveMotor2AbsolutePosition(HWND hDlg, char nMotorZB, int nDesiredPos, BOOL bMoveMotorFast){ //nDesiredPos
 int nSleepDelay = 8;
 int nReturnCodeFromSendPacketsM2AP;
 int M2SP_RMP_RetCode;
 int nCurrent_uStepsForSpecifiedMotor = PCFrec.chNoofUsteps4EachMotorTab[nMotorZB];

 int bSpecifiedMotorHomed = 0; //This doesn't mean 'At Home' (i.e. position 0), but just that the position is valid
 int bSpecifedMotorPosition;   // ... The position being stored here
 MSG nullMSG;

 if( !PCFrec.bMotor2ExpectRepliesFrom[nMotorZB] ){
    MessageBox(hDlg,"MoveMotor2AbsolutePosition operates only when the Printer Configuration shows the motor as being present!",NULL,MB_OK);
    return;
   }
 int SurelyCanFunctionalizeTheFollowing;//I Think its used all over the place:
 nReturnCodeFromSendPacketsM2AP = SendMessagePackets (hDlg, nMotorZB, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[nMotorZB], 0);
 if(nReturnCodeFromSendPacketsM2AP){
    if(nReturnCodeFromSendPacketsM2AP == 5001){
       CEC.SendPktErrTFnP5001++;
       MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveMotor2AbsolutePosition: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
      }
    else{
       char sz_aString[11];
       CEC.SendPktErrTFnPOther++;
       _itoa_s(nReturnCodeFromSendPacketsM2AP, sz_aString, 11, 10);
       MessageBox(hDlg,sz_aString,"MoveMotor2AbsolutePosition: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
      }
    return;
   }

 bWaitingForReply[nMotorZB] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
 M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, nMotorZB, MsTyp2_ZENDstatREC, 0);
 if (M2SP_RMP_RetCode){
    char szCode[12];
    StringCchCopy(mes,SIZMES,"MoveMotor2AbsolutePosition:Receive Message Packets() at 1 returned code: ");//ViewHologInfo_Dialog
    if(M2SP_RMP_RetCode == 6666){
       CEC.RecPktErrTFnP6666++;
       StringCchCat(mes,SIZMES,"\n\nNo Reply");
      }
    else{
       if(M2SP_RMP_RetCode == 123456){
          CEC.RecPktErrTFnP123456++;
          StringCchCat(mes,SIZMES,"MoveMotor2AbsolutePosition:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
         }
       else{
          CEC.RecPktErrTFnPOther++;
          _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
          StringCchCat(mes,SIZMES,szCode);
         }
      }
    
    StringCchCat(mes,SIZMES,  " for Motor: ");
    DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, nMotorZB, mes, "Error");
    return;
   }
 if( cMessageSeqNoSR[nMotorZB] != chReceiveBuffer[nMotorZB][1] ){
    MessageBoxToDisplaySeqNoError(hDlg, "At 43: ", cMessageSeqNoSR[nMotorZB], chReceiveBuffer[nMotorZB][1], nMotorZB );
    return;
   }
 //StatusRec is now received - let's look at it.
 pStatRec = (STATUSRECORD*)&chReceiveBuffer[nMotorZB][2]; //[2] because the reply from AVR contains a message-type & SeqNoSR in the first 2 bytes
 if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
  //if(bSTATREC_BOXisBeingDisplayed) //i.e Do we need to show the user the 'Current Status for all Motors' Dialog box, or can they already see it?
      MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","'MoveMotor2AbsolutePosition' was NOT started!",MB_OK);
  //else //as the STATREC dialogbox is not being displayed, let's show it to them:
  //  if(IDOK == MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding\n\nHere, let me show you...                     Press Cancel if you don't want me to","The HOMING part of 'MoveMotor2AbsolutePosition' was NOT started!",MB_OKCANCEL))
  //    PostMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
    return;  //Initial Status SeriousWarnings or Warnings from UC3 via R eceiveMessagePackets() stops us from proceding
   }
 if(!pStatRec->bDriverEnabled){
    MessageBox(hDlg,"This needs to be enabled before proceeding","Motor Driver is not enabled!",MB_OK);
    return;  //Initial Status stops us from proceding
   }

 if(pStatRec->bRS485Enabled){
    int nReplyButtonPressed = MessageBox(hDlg,"Do you want to proceed (at a slower rate) with debug messages pouring out?\n\nPress No to Switch off RS485 (but still proceed)\n\nPress Cancel if you don't want to Move the motors","Warning RS485 is enabled!",MB_YESNOCANCEL|MB_DEFBUTTON2);
    if(IDCANCEL == nReplyButtonPressed){
       MessageBox (hDlg,"'MoveMotor2AbsolutePosition' was aborted","Message",MB_OK);
       return;  //Initial Status stops us from proceding
      }
    else if (IDNO == nReplyButtonPressed){//User wants to switch off RS485, then proceed
      nReturnCodeFromSendPacketsM2AP = SendMessagePackets (hDlg, nMotorZB, MsTyp6_ENAB_DISAB,  DISAB_485, 0);
      if(nReturnCodeFromSendPacketsM2AP){
         if(nReturnCodeFromSendPacketsM2AP == 5001){
            CEC.SendPktErrTFnP5001++;
            MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveMotor2AbsolutePosition@6: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
           }
         else{
            char sz_aString2[11];
            CEC.SendPktErrTFnPOther++;
            _itoa_s(nReturnCodeFromSendPacketsM2AP, sz_aString2, 11, 10);
            MessageBox(hDlg,sz_aString2,"MoveMotor2AbsolutePosition@6: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
           }
         MessageBox (hDlg,"'MoveMotor2AbsolutePosition' was aborted","Message",MB_OK);
         return;  //Initial Status stops us from proceding
        }
      }
    else
      nSleepDelay = 20;
   }
 if (!pStatRec->usHomingState){
    endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
    bSpecifiedMotorHomed = 1;
    bSpecifedMotorPosition = pStatRec->uStepsFromHome;
   }
//If Motor is not homed, Home it, then move to desired Position. If homed, calculate the move that needs to be made and move straight there.
 if(!bSpecifiedMotorHomed /*[0] || !bSpecifiedMotorHomed[1]*/){ //Homing needs to be done, so issue Homing Instruction. No reply is expected, so will be ok even in a 1 motor development environment
  //int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nMotorZB];
  //int                                                       This_probably_needs_to_be_replaced2;
  //SendMessagePackets(hDlg,
  //               nMotorZB,
  //            MsTyp3_HOME, //******* ******* ******* *******This probably needs to be replaced by a call to HomeSingleMotor(hDlg, nchCurrentMotorZB); ******* ******* **************
  //        nCurrent_uStepsForSpecifiedMotor, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
  //     nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uStepsForSpecifiedMotor] ); //The constant Fast speed to move at, when homing in
    HomeSingleMotor(hDlg, nMotorZB);
                  //The limits on 'how many steps to move' are still determined by UC3 hardcode
    Sleep(nSleepDelay); //8 or 20, depending on if RS485 is enabled. It's questionable if any delay is necessary anymore.
//Now need to keep checking the status recs until all motors are homed...
    int ArHomingStates = 1; //It's not an array here, but I'll retain the name to draw attention to other parts of the program using the same logic
  //int nHomingAllMotorsTimeout = 4;       //Allow up to 8 secs for the motors to home. 
    int nHomingAllMotorsTimeout = 6000000; //Allow forever secs for the motors to home (but finish loop earlier if all homed)

    while(nHomingAllMotorsTimeout-- >= 0){
       Sleep(1000); //At least give it a chance to get started
     //for (int emSR2 = 36;emSR2 < 38; emSR2++){
        //if( (!PCFrec.bMotor2ExpectRepliesFrom[emSR2]) || (!ArHomingStates[emSR2 - 36]) )

//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS//Added THIS
       if( PeekMessage(&nullMSG, hGLWnd, WM_STOPHOMINGMESS_FROM_HELPERPROG, WM_STOPHOMINGMESS_FROM_HELPERPROG, PM_REMOVE | PM_NOYIELD) ){//If HoloProgHelper sends an ABORT_HOMING_MESSAGE we should stop HoloProg waiting for homing to occur
          UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
          return;
         }
       if(UpdateMainWindowsControlsM()){//This call does the above (but for all motors), & updates the controls. FROM WITHIN A LOOP UNTIL ALL HOMED it gets status records [ during M oveToStartPositions() ]
          UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
          return;
         }
       UpdateWindow(hGLWnd);
//Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added ABOVE/Added THIS
          //if( !ArHomingStates )
            //continue; //i.e. don't ask for status recs after we've aborted, or if we're not expecting a reply, or if the motor is already homed
          nReturnCodeFromSendPacketsM2AP = SendMessagePackets (hDlg, nMotorZB, MsTyp2_ZENDstatREC,  ++cMessageSeqNoSR[nMotorZB], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)
          if(nReturnCodeFromSendPacketsM2AP){ /*<- Could Abort*/
             if(nReturnCodeFromSendPacketsM2AP == 5001){
                CEC.SendPktErrTFnP5001++;
                MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","MoveMotor2AbsolutePosition: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
               }
             else{
                char sz_aString3[11];
                CEC.SendPktErrTFnPOther++;
                _itoa_s(nReturnCodeFromSendPacketsM2AP, sz_aString3, 11, 10);
                MessageBox(hDlg,sz_aString3,"MoveMotor2AbsolutePositionH: nReturnCodeFromSendPacketsM2AP Error Code:",MB_OK);
               }
             return;
            }

          bWaitingForReply[nMotorZB] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
          M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, nMotorZB, MsTyp2_ZENDstatREC, 0);
          if (M2SP_RMP_RetCode){              /*<- Could Abort*/
             char szCode[12];
             StringCchCopy(mes,SIZMES,"MoveMotor2AbsolutePositionH:Receive Message Packets() at 2 returned code: "); //ViewHologInfo_Dialog
             if(M2SP_RMP_RetCode == 6666){
                CEC.RecPktErrTFnP6666++;
                StringCchCat(mes,SIZMES,"\n\nNo Reply");
               }
             else{
                if(M2SP_RMP_RetCode == 123456){
                   CEC.RecPktErrTFnP123456++;
                   StringCchCat(mes,SIZMES,"MoveMotor2AbsolutePositionH:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
                  }
                else{
                   CEC.RecPktErrTFnPOther++;
                   _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
                   StringCchCat(mes,SIZMES,szCode);
                  }
               }
             MessageBox(hDlg,mes,NULL,MB_OK);
             return;
            }
          if( cMessageSeqNoSR[nMotorZB] != chReceiveBuffer[nMotorZB][1] ){  //SEQNO ERROR    /*<- Could Abort*/
             MessageBoxToDisplaySeqNoError(hDlg, "At 54a: ", cMessageSeqNoSR[nMotorZB], chReceiveBuffer[nMotorZB][1], nMotorZB);
             return;
            }
          pStatRec = (STATUSRECORD*)&chReceiveBuffer[nMotorZB][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
          if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){           //WARNINGS APPEARED              /*<- Could Abort*/
             MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of MoveMotor2AbsolutePosition() Started... BUT",MB_OK);
             return;
            }
          endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
        //endian_swap(*(((unsigned int*)pStatRec)+1));
        //endian_swap(*(((unsigned int*)pStatRec)+2));
        //endian_swap(*(((unsigned int*)pStatRec)+3));
          if( (!pStatRec->usHomingState) && !pStatRec->uStepsFromHome) //If these are both true, the motor is homed
            ArHomingStates = 0;//Set to zero when this motor is homed at position zero.
          else
            ArHomingStates = 1;
       //}// for (int emSR2 = 36;emSR2 < 38; emSR2++){
       if( !ArHomingStates )
         nHomingAllMotorsTimeout = -100;  //break out of the loop now that we're homed properly on all motors
      }//while(nHomingAllMotorsTimeout)

    if(nHomingAllMotorsTimeout > -5){ //we timed out
       MessageBox (hDlg,"At least 1 Motor failed to Home (timed out)... \n\nYou may need simply to try again (if the cause of the problem was\nthat the motor had a very long way to travel)","In MoveMotor2AbsolutePosition()... ",MB_OK);
       return;
      }

    if(nDesiredPos){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveMotorFast)
         FastMoveSingleMotor(hDlg, nMotorZB, nDesiredPos);
       else
         SendMessagePackets(hDlg, nMotorZB, MsTyp5_SING_MM, nDesiredPos, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForSpecifiedMotor] );//Motor 38 RefBeamTable M2
      }

   }// if (!bSpecifiedMotorHomed)  i.e. we needed to do homing
 else{
    //The Motor was homed, so we can rely on its position... Calc the move that needs to be made based on that
    int nStepsSpecifiedMotorNeedsToMove = nDesiredPos - bSpecifedMotorPosition;

    if(nStepsSpecifiedMotorNeedsToMove){ //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
       if(bMoveMotorFast)
         FastMoveSingleMotor(hDlg, nMotorZB, nStepsSpecifiedMotorNeedsToMove);
       else
         SendMessagePackets(hDlg, nMotorZB, MsTyp5_SING_MM, nStepsSpecifiedMotorNeedsToMove, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForSpecifiedMotor] );//Motor 38 RefBeamTable M2
      }
   }
}

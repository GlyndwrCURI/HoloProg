//                           M oveToStartPositions.cpp
//M oveToStartPositions() was a message handler for ViewHologInfo_Dialog() - case IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS:  and still is, really.
//It was turned into a function called by case IDB_VIEWHOLOGINFO_MOVE2STARTPOSITIONS: to avoid Compiler Error C2712, which is:
// "When you use /EHsc, a function with structured exception handling cannot have objects that require unwinding (destruction)."
//This appears to mean we cannot have __try ... __finally code in the same function as contains the registry accessing code
// lstrcpy( filename, stdstring(LastHGramFileOpened).c_str() )
//but putting __try ... __finally code in a seperate function and calling it from a function that contains the registry accessing code
//seems to be Ok.

//All the above is now irrelvant because the mutex's are now removed, so the __try ... __finally code has been removed as well.
//However, I'll still keep M oveToStartPositions() as a called function because it is convenient to use a short version of it for
// homing all the motors (by setting the parameter bStopWhenHomed).

#include "stdafx.h"

extern CRegStdWORD   nOverRideM39HomingOutSpeedIfNotZero;
extern CRegStdWORD   nOverRideM40HomingOutSpeedIfNotZero;
extern CRegStdWORD   bUseCSBs4AccProfs;

//extern int  bMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec
extern unsigned char cMessageSeqNoSR[NOOFMOTORS];
//extern unsigned char cMessageSeqNoCR[NOOFMOTORS];
extern unsigned int uiTableOfCheckSums[NOOFMOTORS];

extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern int bWaitingForReply[];
//extern int bSTATREC_BOXisBeingDisplayed;
//extern char s zGlobalFileName[];
extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern char mes[];
//extern BOOL bMoveNowRadioButPressed;
//extern CRegStdString LastHGramFileOpened;

//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
extern int nArSlowSpeeds4HomingDeptOn_uSteps[]; //instead of the above

extern int nArMaxSpeedsDependentOn_uSteps[65];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS]; //part of the pcf file
//extern int nNoofUsteps4EachApertureTab[9];         //part of the pcf file
//extern int nNoofUsteps4EachFocusTab[3];            //part of the pcf file
//extern int nNoofUsteps4RefBeamTablePrintUnload[4]; //part of the pcf file
extern PCFRECORD PCFrec;

extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];

extern STATUSRECORD *pStatRec;
extern HWND hGLWnd;
//extern HINSTANCE hInst;
//extern char szGLWndTitle[]; // [_MAX_DIR + 11] Eg: HoloProg: <filename>
//extern BOOL nShowHologInfoBecauseFileOpenedOK;

//extern HCFRECORDsCURRENTfolders HCFcfs;

//Fields shared with ChoosDir.cpp:
//extern char szDirName[_MAX_PATH];
extern HCFRECORD HCFrec;

//If bSTOP_WHEN_HOMED is set this function would be better named MoveToHOMPositions! (because it stops 1/2 way thru)
int MoveToStartPositions(HWND hDlg, /*BOOL bMove2SPsFast,*/ BOOL bSeeMessages, BOOL bMove2SPsHomeRegardless, BOOL bStopWhenHomed){ //returns 1 if the function should be re-started, else zero (even if there's an error)
 int nSleepDelay = 8;
 int nReturnCodeFromSendPacketsM2SP;
 if(bSeeMessages)
   MessageBox (hDlg,"Please press OK   -  THEN WAIT\n (while the motors are homed)","Instruction :",MB_OK);
 char emSR, emSR2;
 MSG nullMSG;
 //for (emSR = 0;emSR < NOOFMOTORS; emSR++)
 //  if(!PCFrec.bMotor2ExpectRepliesFrom[emSR]){
 //     SetDlgItemText(hGLWnd, IDB_STATREC_GENERALWARN + emSR, "OK No Reply expected");
 //     SetDlgItemText(hGLWnd, IDB_STATREC_SERIOUSWARN + emSR, " ");
 //     ClearMostFields(emSR); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
 //    }

 //                   nReturnCodeFromSendPacketsM2SP = SendMessagePackets (hDlg, emSR, M sTyp2_ZENDstatREC,  ++cMessageSeqNoSR[emSR], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)
 //                   if(nReturnCodeFromSendPacketsM2SP){
 //                      if(nReturnCodeFromSendPacketsM2SP == 5001){
 //                         CEC.SendPktErrTFnP5001++;
 //                         MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","VHIdg: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
 //                        }
 //                      else{
 //                         char sz_aString[11];
 //                         CEC.SendPktErrTFnPOther++;
 //                         _itoa_s(nReturnCodeFromSendPacketsM2SP, sz_aString, 11, 10);
 //                         MessageBox(hDlg,sz_aString,"VHIdg: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
 //                        }
 //                      return;
 //                     }

 //                   bWaitingForReply[emSR] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
 //                   M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, emSR, M sTyp2_ZENDstatREC, 0);
 //                   if (M2SP_RMP_RetCode){
 //                      char szCode[12];
 //                      StringCchCopy(mes,SIZMES,"VHIdg:Receive Message Packets() at 3 returned code: ");//ViewHologInfo_Dialog
 //                      if(M2SP_RMP_RetCode == 6666){
 //                         CEC.RecPktErrTFnP6666++;
 //                         StringCchCat(mes,SIZMES,"\n\nNo Reply");
 //                        }
 //                      else{
 //                         if(M2SP_RMP_RetCode == 123456){
 //                            CEC.RecPktErrTFnP123456++;
 //                            StringCchCat(mes,SIZMES,"VHIdg:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
 //                           }
 //                         else{
 //                            CEC.RecPktErrTFnPOther++;
 //                            _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
 //                            StringCchCat(mes,SIZMES,szCode);
 //                           }
 //                        }
 //                      StringCchCat(mes,SIZMES,  " for Motor: ");
 //                      DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, emSR, mes, "Error");
 //                      return;
 //                     }
 //                   if( cMessageSeqNoSR[emSR] != chReceiveBuffer[emSR][1] ){
 //                      MessageBoxToDisplaySeqNoError(hDlg, "At 4: ", cMessageSeqNoSR[emSR], chReceiveBuffer[emSR][1], emSR);
 //                      return;
 //                     }
 if(UpdateMainWindowsControlsM()){//This call does the above (but for all motors), plus updates the controls. It basically gets a status record. [ at the start of M oveToStartPositions() ]
    UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
    return 0;
   }
 UpdateWindow(hGLWnd); //sends a WM_PAINT message directly to the window procedure of the specified window, bypassing the application queue.
 //b PIDMovementStoppedForHoming = TRUE;
 //S endMessagePackets(hDlg, 1, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(hDlg, 2, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(hDlg,13, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(hDlg,14, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(hDlg,25, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(hDlg,26, MsTypB_SING_MM_PID,FORWARD_DIR,60001 ); //This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 StopPIDMovementWhileHomingOrFileTransfering(); //does the above.   Not anymore! it now sets bStopPIDsendingMessages TRUE, & does  "SendMessage(hGLWnd, WM_COMMAND, IDM_MM_PID_STOPMOVES, 0)" (which does the above)
//M oveToStartPositions() INITIAL StatusRecs are now received - let's look at them. Note this function now has to completely ignore the PID motors because homing
// is meaningless/pointless for them, & they're probably running anyway (& would treat messages as "received while runnning - i.e. as errors) 
 for (emSR = 0;emSR < NOOFMOTORS; emSR++)
   if((PCFrec.bMotor2ExpectRepliesFrom[emSR]) && (!PCFrec.bMotorIS_A_PID[emSR]) ){
      pStatRec = (STATUSRECORD*)&chReceiveBuffer[emSR][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
      if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
       //if(bSTATREC_BOXisBeingDisplayed) //i.e Do we need to show the user the 'Current Status for all Motors' Dialog box, or can they already see it?
         if(bStopWhenHomed)
           MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","HOMING was NOT started @1 for at least one motor!",MB_OK);
         else
           MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of 'Move To Start Positions' @1 was NOT started!",MB_OK);
       //else //as the STATREC dialogbox is not being displayed, let's show it to them:
       //  if(IDOK == MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding\n\nHere, let me show you...                     Press Cancel if you don't want me to","The HOMING part of 'Move To Start Positions' was NOT started!",MB_OKCANCEL))
       //    PostMessage(hGLWnd, WM_COMMAND, IDM_ZEND_STATUSREC, 0);
         restartPIDs();
         return 0;  //Initial Status SeriousWarnings or Warnings from UC3 via R eceiveMessagePackets() stops us from proceding
        }
      if(!pStatRec->bDriverEnabled){
         DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, emSR, "All drivers need to be enabled before proceeding - e.g. M", "Motor Driver is not enabled!");
         restartPIDs();
         return 0;  //Initial Status stops us from proceding
        }

      if(pStatRec->bRS485Enabled){ //"Warning RS485 is enabled!"
         int nReplyButtonPressed = MessageBox(hDlg,"Do you want to proceed (at a slower rate) with debug messages pouring out?\n\nPress No to Switch off RS485 (but still proceed)\n\nPress Cancel if you don't want to Move the motors","Warning RS485 is enabled!",MB_YESNOCANCEL|MB_DEFBUTTON2);
         if(IDCANCEL == nReplyButtonPressed){
            restartPIDs();
            return 0;  //Initial Status stops us from proceding
           }
         else if (IDNO == nReplyButtonPressed){ //User wants to switch off RS485, then proceed
            nReturnCodeFromSendPacketsM2SP = 987654321;  //Without this, compiler warns of potentially uninitialised local variable ''nReturnCodeFromSendPacketsM2SP' being used. I think the compiler's wrong!!!
            for(emSR2 = 0; emSR2 < NOOFMOTORS; emSR2++){
               nReturnCodeFromSendPacketsM2SP = SendMessagePackets (hDlg, emSR2, MsTyp6_ENAB_DISAB,  DISAB_485, 0);
               if(nReturnCodeFromSendPacketsM2SP)
                 emSR2 = 126;
               else
                 Sleep(2);
              }
            if(emSR2 > 125){
               if(nReturnCodeFromSendPacketsM2SP == 5001){
                  CEC.SendPktErrTFnP5001++;
                  MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","VHIdg@6: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
                 }
               else{
                  char sz_aString2[11];
                  CEC.SendPktErrTFnPOther++;
                  _itoa_s(nReturnCodeFromSendPacketsM2SP, sz_aString2, 11, 10);
                  MessageBox(hDlg,sz_aString2,"VHIdg@6: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
                 }
               restartPIDs();
               return 0;//Error stops us from proceding
              }
            else
              return 1; //Start this function again, now DISAB_485 messages have been sent
           }
         else
           nSleepDelay = 20;
        }
      //if(bMove2SPsHomeRegardless && !pStatRec->usHomingState){ //The motor says it is homed (so should not be homed again if user chose this)
      //    bMove2SPsHomeRegardless = !bMove2SPsHomeRegardless;  //I could do this test after T ransfer_ALL_AccProfsAndPerform() or, if I keep it here, I could avoid even sending
      //    bMove2SPsHomeRegardless = !bMove2SPsHomeRegardless;  // the profiles - by passing a table of these results to that function.
      //  }
     }
 //StatusRec OK so issue Homing Instruction.  I'll do this for every motor apart from PIDs... No reply is expected, so this should be ok even in a 1 motor development environment

 SendMessage(hGLWnd,WM_COMMAND,IDM_EXEC_HOLOPROGHELPER,0);

//Now that we need to use AccelProfiles when homing, these have to be sent before the MsTyp3_HOME command.  We need something similar to T ransfer_ALL_FilesAndPerform(), but it must send
// the appropriate (Homing) acceleration files instead.


//int PerhapsNEEDbMotorsAreHappy; //To replace the following function...  Then again, doesn't the above do that?
//            if( b MotorsAreInTheirStartPositionsANDhappy(hDlg, ArrayMotorIsInRightPlace, HCFrec.nNextLineToPrint) ){ //and happy!
//             StopPIDMovementWhileHomingOrFileTransfering();

               //bTimerHasBeenKilled = TRUE; //I think setting this is sufficient - actually killing the timer is unnecessary, just so long as it is ignored
Sleep(100);
               if( Transfer_ALL_AccProfsAndPerform(hDlg, bUseCSBs4AccProfs) ){ //Transfers *Homing* accelerations.  These go to all motors that are connected apart from the PIDs (including the ref beam table).
//Sleep(6000);
                  restartPIDs();
                  int nCSretCode = bCheckAlltheChecksumsAreOK(hDlg, CHECK_STATIC_MOTOR_CSUMS_AS_WELL);
                  //bTimerHasBeenKilled = FALSE;
                  if(nCSretCode){
                     if(nCSretCode < 1000){
                        char szWindowsCS[12];
                        StringCchCopy(mes, SIZMES, "Motor: ");
                        _itoa_s(nCSretCode, mes+7,_MAX_DIR - 7,10);
                        StringCchCat(mes, SIZMES, " with Windows Checksum: ");
                        _ultoa_s(uiTableOfCheckSums[nCSretCode-1], szWindowsCS, 12, 10);
                        StringCchCat(mes, SIZMES, szWindowsCS);
                        MessageBox (hDlg,"@2 After Transfer_ALL_AccProfsAndPerform() returned OK\n\n  bCheckAlltheChecksumsAreOK() found a mismatch!\n\nCHECK you haven't also got any other warnings as well\n(as these probably indicate the cause)",mes,MB_OK);
                       }
                     else{
                        StringCchCopy(mes, SIZMES, "Error: whilst getting checksum for Motor: ");
                        _itoa_s(nCSretCode - 1000, mes+42,_MAX_DIR - 42,10);
                        MessageBox (hDlg,"CheckAlltheChecksumsAreOK() got no reply within the time allowed @ 2.",mes,MB_OK);
                       }
                     return 0;
                    }
                  else{ //T ransfer_ALL_FilesAndPerform succeeded() & b CheckAlltheChecksumsAreOK() did too, so carry on
                     ;
                    }
                 }
               else{ //T ransfer_ALL_FilesAndPerform() failed
                  //bTimerHasBeenKilled = FALSE;
                  restartPIDs();
                  return 0;
                 }
//              }

//Check again that the status records are OK.  It is possible that Messages sent in Transfer_ALL_AccProfsAndPerform() caused an error on the AVRs (e.g. pulse time out of bounds). We needn't send a
// status request because b CheckAlltheChecksumsAreOK() just did that (althought it was actually a MsTypD_SENDchecksumREC message: more or less the same thing)
 for (emSR = 0;emSR < NOOFMOTORS; emSR++)
   if((PCFrec.bMotor2ExpectRepliesFrom[emSR]) && (!PCFrec.bMotorIS_A_PID[emSR]) ){
      pStatRec = (STATUSRECORD*)&chReceiveBuffer[emSR][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
      if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){
         if(bStopWhenHomed)
           MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","HOMING was NOT started @2 for at least one motor!",MB_OK);
         else
           MessageBox(hDlg,"There are Warnings or Serious warnings in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of 'Move To Start Positions' @2 was NOT started!",MB_OK);
         return 0;  //Initial Status SeriousWarnings or Warnings from UC3 via R eceiveMessagePackets() stops us from proceding
        }
     }

 int ArHomingStates[NOOFMOTORS];
 memset(&ArHomingStates,0,NOOFMOTORS*sizeof(int));
 char emMTSP;
 BOOL bAtLeastOneNeedsHoming = 0;
//for(emMTSP = 0; emMTSP < NOOFMOTORS; emMTSP++){
 for(emMTSP = NOOFMOTORSm1; emMTSP > -1; emMTSP--){
    if( (PCFrec.bMotor2ExpectRepliesFrom[emMTSP]) && (!(PCFrec.bMotorIS_A_PID[emMTSP])) ){
       //int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[emMTSP];
       pStatRec = (STATUSRECORD*)&chReceiveBuffer[emMTSP][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
       if((emMTSP != 1) && (emMTSP != 2) && (emMTSP != 13) && (emMTSP != 14) && (emMTSP != 25) && (emMTSP != 26) ){
          if((pStatRec->usHomingState) || bMove2SPsHomeRegardless){ //Send the Home command if the Motor is not homed, or if bMove2SPsHomeRegardless is set. In other words, home only if
             SendMsTyp3_HOMEmessage(hDlg, emMTSP);                  // necessary (& it may be necessary just because the user has specified that homing should always happen)
             ArHomingStates[emMTSP] = 1;
             bAtLeastOneNeedsHoming = 1;
            }
         }
       //if(emMTSP < 38){ //We're currently taking M39 and M40's HO speeds from the registry if nOverRideM39HomingOutSpeedIfNotZero or nOverRideM40HomingOutSpeedIfNotZero is set.
       //  if((emMTSP != 1) && (emMTSP != 2) && (emMTSP != 13) && (emMTSP != 14) && (emMTSP != 25) && (emMTSP != 26) )
       //     SendMessagePackets(hDlg,
       //                      emMTSP,
       //                 MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
       //             nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //         nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
       //           //The limits on 'how many steps to move' are still determined by UC3 hardcode
       //  }
       //else{//Motor 39 or 40
       //   if(emMTSP == 39){//Motor 40
       //      if(nOverRideM40HomingOutSpeedIfNotZero)
       //        SendMessagePackets(hDlg,
       //                         emMTSP,
       //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
       //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //          nOverRideM40HomingOutSpeedIfNotZero); //The constant Fast speed to move at, when homing in
       //      else
       //        SendMessagePackets(hDlg,
       //                         emMTSP,
       //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
       //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //          nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
       //     }
       //   else{//Motor 39
       //      if(nOverRideM39HomingOutSpeedIfNotZero)
       //        SendMessagePackets(hDlg,
       //                         emMTSP,
       //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
       //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //          nOverRideM39HomingOutSpeedIfNotZero ); //The constant Fast speed to move at, when homing in
       //      else
       //        SendMessagePackets(hDlg,
       //                         emMTSP,
       //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
       //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
       //          nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
       //     }
       //  }
       Sleep(nSleepDelay); //8 or 20, depending on if RS485 is enabled
      }
   }

//Now need to keep checking the status recs until all motors are homed...
//int ArHomingStates[NOOFMOTORS];
 //for(emMTSP = 0; emMTSP < NOOFMOTORS; emMTSP++)
 //  ArHomingStates[emMTSP] = 1;
 //int WARNINGdevelopmentCode4SingleMotor; //Next 3 lines ARE FOR DEVELOPMENT.. the previous two lines should be all that's needed.  It look as though this variable
 // should be named WARNINGdevelopmentCode4FewMotors. (Perhaps, now we have the facility for the ref beam table, the following is better than the above).
 //for(emMTSP = 0; emMTSP < NOOFMOTORS; emMTSP++)
 //  if((PCFrec.bMotorIS_A_PID[emMTSP]))  //fool the homing part of this routine into thinking PIDs are homed
 //    ArHomingStates[emMTSP] = 0;
 //  else{
 //     if(PCFrec.bMotor2ExpectRepliesFrom[emMTSP]){
 //        pStatRec = (STATUSRECORD*)&chReceiveBuffer[emSR][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes
 //        if((pStatRec->usHomingState) || bMove2SPsHomeRegardless)
 //          ArHomingStates[emMTSP] = 1;
 //        else
 //          ArHomingStates[emMTSP] = 0;
 //       }
 //     else
 //       ArHomingStates[emMTSP] = 0;
 //    }

//int ForTestingUSING_VERY_LONG_HomingTime;
//int n HomingAllMotorsTimeout  = 4;      //Allow up to 8 secs for the motors to home.
//int nHomingAllMotorsTimeout  = 60;      //Allow up to 30 secs for the motors to home (but finish loop earlier if all homed)
 int nHomingAllMotorsTimeout = 6000000;     //Effectively this stops the Windows PC from ever timing out while waiting for homing to complete.  I think this time-out was only there, really, to allow the Windows 
                                         // program to become responsive (eventually) in the event that homing was not completed.  HoloProgHelper now comes to the rescue by posting a WM_STOPHOMINGMESS_FROM_HELPERPROG
                                         // message if & when the user chooses, so the original timeout is no longer required.
 if(!bAtLeastOneNeedsHoming)
   nHomingAllMotorsTimeout = -100; //i.e. skip the next loop if no motors need homing

 while(nHomingAllMotorsTimeout-- >= 0){ //If all motors home properly, this is set to -100
    Sleep(1000); //At least give them a chance to get started
    //for (int emSR2 = 0; emSR2 < NOOFMOTORS; emSR2++){
    //   if( !PCFrec.bMotor2ExpectRepliesFrom[emSR2] || !ArHomingStates[emSR2] )
    //     continue; //i.e. don't ask for status recs after we've aborted, or if we're not expecting a reply, or if the motor is already homed
    //                   nReturnCodeFromSendPacketsM2SP = SendMessagePackets (hDlg, emSR2, M sTyp2_ZENDstatREC,  ++cMessageSeqNoSR[emSR2], 0); //BUT MUST CHANGE EVK. 139 must be killed off (& is now)
    //                   if(nReturnCodeFromSendPacketsM2SP){ //<- Could Abort
    //                      if(nReturnCodeFromSendPacketsM2SP == 5001){
    //                         CEC.SendPktErrTFnP5001++;
    //                         MessageBox(hDlg,"5001 - Send() function took > 0.3 seconds","VHIdg2: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
    //                        }
    //                      else{
    //                         char sz_aString3[11];
    //                         CEC.SendPktErrTFnPOther++;
    //                         _itoa_s(nReturnCodeFromSendPacketsM2SP, sz_aString3, 11, 10);
    //                         MessageBox(hDlg,sz_aString3,"VHIdg2: nReturnCodeFromSendPacketsM2SP Error Code:",MB_OK);
    //                        }
    //                      return 0; //bAbortMOVE2START = TRUE;
    //                     }
    //                   bWaitingForReply[emSR2] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
    //                   M2SP_RMP_RetCode =  ReceiveMessagePackets(hDlg, emSR2, M sTyp2_ZENDstatREC, 0);
    //                   if (M2SP_RMP_RetCode){              //<- Could Abort
    //                      char szCode[12];
    //                      StringCchCopy(mes,SIZMES,"VHIdg2:Receive Message Packets() at 4 returned code: "); //ViewHologInfo_Dialog
    //                      if(M2SP_RMP_RetCode == 6666){
    //                         CEC.RecPktErrTFnP6666++;
    //                         StringCchCat(mes,SIZMES,"\n\nNo Reply");
    //                        }
    //                      else{
    //                         if(M2SP_RMP_RetCode == 123456){
    //                            CEC.RecPktErrTFnP123456++;
    //                            StringCchCat(mes,SIZMES,"VHIdg2:Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
    //                           }
    //                         else{
    //                            CEC.RecPktErrTFnPOther++;
    //                            _itoa_s(M2SP_RMP_RetCode, szCode, 12, 10);
    //                            StringCchCat(mes,SIZMES,szCode);
    //                           }
    //                        }
    //                      MessageBox(hDlg,mes,NULL,MB_OK);
    //                      return 0; //bAbortMOVE2START = TRUE;
    //                     }
    //                   if( cMessageSeqNoSR[emSR2] != chReceiveBuffer[emSR2][1] ){  //<- Could Abort //SEQNO ERROR
    //                      MessageBoxToDisplaySeqNoError(hDlg, "At 5: ", cMessageSeqNoSR[emSR2], chReceiveBuffer[emSR2][1], emSR2);
    //                      return 0; //bAbortMOVE2START = TRUE;
    //                     }
    if( PeekMessage(&nullMSG, hGLWnd, WM_STOPHOMINGMESS_FROM_HELPERPROG, WM_STOPHOMINGMESS_FROM_HELPERPROG, PM_REMOVE | PM_NOYIELD) ){//If HoloProgHelper sends an ABORT_HOMING_MESSAGE we should stop HoloProg waiting for homing to occur
       UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
       return 0;
      }
    if(UpdateMainWindowsControlsM()){//This call does the above (but for all motors), & updates the controls. FROM WITHIN A LOOP UNTIL ALL HOMED it gets status records [ during M oveToStartPositions() ]
       UpdateWindow(hGLWnd);           //If it failed, it will have put out a message and killed the timer
       return 0;
      }
    UpdateWindow(hGLWnd);
//M oveToStartPositions() INITIAL StatusRecs are now received - let's look at them.
    for (emSR2 = 0;emSR2 < NOOFMOTORS; emSR2++)
      if((PCFrec.bMotor2ExpectRepliesFrom[emSR2]) && (!PCFrec.bMotorIS_A_PID[emSR2]) ){
         pStatRec = (STATUSRECORD*)&chReceiveBuffer[emSR2][2]; //[2] because the reply from AVR contains a message-type in the first 2 bytes (or message-type and chUC3cMessageSeqNoSR?)
         if(pStatRec->SeriousWarnings || pStatRec->Warnings2Report){ //<- Could Abort //WARNINGS APPEARED
          //if(bSTATREC_BOXisBeingDisplayed)
            if(bStopWhenHomed)
              MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","The HOMING part of 'Move To Start Positions' Started... BUT",MB_OK);
            else
              MessageBox(hDlg,"Warnings or Serious warnings then appeared in the status records.\nThese need to be manually cleared before proceeding","HOMING Started... BUT",MB_OK);
            restartPIDs();
            return 0; //bAbortMOVE2START = TRUE;
           }
       //endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
       //endian_swap(*(((unsigned int*)pStatRec)+1));
       //endian_swap(*(((unsigned int*)pStatRec)+2));
       //endian_swap(*(((unsigned int*)pStatRec)+3));
         if( (!pStatRec->usHomingState) && !pStatRec->uStepsFromHome) //If these are both true, the motor is homed
           ArHomingStates[emSR2] = 0;//Set to zero when this motor is homed at position zero.
         else
           ArHomingStates[emSR2] = 1;
        }
    if(!(ArHomingStates[ 0]+ArHomingStates[ 1]+ArHomingStates[ 2]+ArHomingStates[ 3]+ArHomingStates[ 4]+ArHomingStates[ 5]+ArHomingStates[ 6]+ArHomingStates[ 7]+
         ArHomingStates[ 8]+ArHomingStates[ 9]+ArHomingStates[10]+ArHomingStates[11]+ArHomingStates[12]+ArHomingStates[13]+ArHomingStates[14]+ArHomingStates[15]+
         ArHomingStates[16]+ArHomingStates[17]+ArHomingStates[18]+ArHomingStates[19]+ArHomingStates[20]+ArHomingStates[21]+ArHomingStates[22]+ArHomingStates[23]+
         ArHomingStates[24]+ArHomingStates[25]+ArHomingStates[26]+ArHomingStates[27]+ArHomingStates[28]+ArHomingStates[29]+ArHomingStates[30]+ArHomingStates[31]+
         ArHomingStates[32]+ArHomingStates[33]+ArHomingStates[34]+ArHomingStates[35]+ArHomingStates[36]+ArHomingStates[37]+ArHomingStates[38]+ArHomingStates[39]))
      nHomingAllMotorsTimeout = -100;  //break out of the loop now that we're homed properly on all motors
   }//while(n HomingAllMotorsTimeout)

 if(nHomingAllMotorsTimeout > -5){ //we timed out
    MessageBox (hDlg,"At least 1 Motor failed to Home (timed out)... \n\nYou may need simply to try again (if the cause of the problem was\nthat the motor had a very long way to travel)","In Move_To_Start_Positions()... ",MB_OK);
    restartPIDs();
    return 0;
   }
 else //nHomingAllMotorsTimeout must be -100 or -101 => ArHomingStates[n] == 0 for all motors => All motor are homed
   if(bSeeMessages){  //OK, so all motors are homed and happy
      MessageBox (hDlg,"The Motors are all snuggly cuddly and at Home...","Update",MB_OK);
      MessageBox (hDlg,"Please press OK to move from Home to the start position","Move Motor(s) to the  correct start position for this line",MB_OK);
      BringWindowToTop(hDlg);
     }

 if(bStopWhenHomed){ //bStopWhenHomed allows this routine to be called just for homing - without then moving to the start positions
    //MessageBox (hDlg,"'Move To Start Positions' was aborted","Message",MB_OK);
    restartPIDs();
    return 0;
   }


 //int nCurrent_uStepsForM1; //eg 32, 64 etc - WavePlate1 (beam split ratio)
 //int nCurrent_uStepsForM13;
 //int nCurrent_uStepsForM25;
 //int nCurrent_uStepsForM4; //eg 32, 64 etc - Length of Object Beam
 //int nCurrent_uStepsForM16;
 //int nCurrent_uStepsForM28;
 //int nCurrent_uStepsForM5; //eg 32, 64 etc - Aperture Size
 //int nCurrent_uStepsForM17;
 //int nCurrent_uStepsForM29;
 //int nCurrent_uStepsForM6; //eg 32, 64 etc - LCOS
 //int nCurrent_uStepsForM18;
 //int nCurrent_uStepsForM30;
 //int nCurrent_uStepsForM37;//              - RefBeamTable
 //int nCurrent_uStepsForM38;
 //int nCurrent_uStepsForM39;//              - XY stage
 //int nCurrent_uStepsForM40;
 //int nCurrent_uStepsForOM;//Other Motors

//Now the 3 WavePlate1 (beam split ratio) Motors.  These are Motors M1, M13 and M25 (rgb)
 //nCurrent_uStepsForM1  = PCFrec.chNoofUsteps4EachMotorTab[0];
 //if(GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1) && PCFrec.bMotor2ExpectRepliesFrom[0]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 0, GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1), (STATUSRECORD*)&chReceiveBuffer[0][2]);
 //else
 //  SendMessagePackets(hDlg, 0, //Motor 1 RED
 //          MsTyp5_SING_MM, GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1), nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM1] );

 //nCurrent_uStepsForM13 = PCFrec.chNoofUsteps4EachMotorTab[12];
 //if(GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2) && PCFrec.bMotor2ExpectRepliesFrom[12]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 12, GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2), (STATUSRECORD*)&chReceiveBuffer[12][2]);
 //else
 //  SendMessagePackets(hDlg, 12,//Motor 13 GREEN
 //          MsTyp5_SING_MM, GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2), nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM13] );

 //nCurrent_uStepsForM25 = PCFrec.chNoofUsteps4EachMotorTab[24];
 //if(GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3) && PCFrec.bMotor2ExpectRepliesFrom[24]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 24, GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3), (STATUSRECORD*)&chReceiveBuffer[24][2]);
 //else
 //  SendMessagePackets(hDlg, 24,//Motor 25 BLUE
 //          MsTyp5_SING_MM, GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3), nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM25] );
//Motor 2 and 3 (rgb) are PID motors and don't have a start postion as such.  See restartPIDs() below.

//Now the 3 Length Of Object Beam Motors.          These are Motors M4, M16 and M28 (rgb)
 //nCurrent_uStepsForM4  = PCFrec.chNoofUsteps4EachMotorTab[3];
 //if((PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0] +  nLineXstartPositionsFEM[3][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[3]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 3, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0] + nLineXstartPositionsFEM[3][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[3][2]);
 //else
 //  SendMessagePackets(hDlg, 3, //Motor 4 RED
           //MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0] + HCFrec.ObjBeamLengthR, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM4] );
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0] + nLineXstartPositionsFEM[3][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM4] );

 //nCurrent_uStepsForM16 = PCFrec.chNoofUsteps4EachMotorTab[15];
 //if((PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1] + nLineXstartPositionsFEM[15][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[15]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 15, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1] + nLineXstartPositionsFEM[15][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[15][2]);
 //else
 //  SendMessagePackets(hDlg, 15,//Motor 16 GREEN
           //MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1] + HCFrec.ObjBeamLengthG, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM16] );
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1] + nLineXstartPositionsFEM[15][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM16] );

 //nCurrent_uStepsForM28 = PCFrec.chNoofUsteps4EachMotorTab[27];
 //if((PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2] + nLineXstartPositionsFEM[27][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[27]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 27, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2] + nLineXstartPositionsFEM[27][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[27][2]);
 //else
 //  SendMessagePackets(hDlg, 27,//Motor 28 BLUE
           //MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2] + HCFrec.ObjBeamLengthB, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM28] );
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2] + nLineXstartPositionsFEM[27][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM28] );

//Move the three aperture motors to the correct position.  These are Motors M5, M17 and M29 (rgb)
  //For   Red, this will be 0 or 1 but not 2 (RedNo1,RedNo2,RedPin)... Holograms will not be printed with the Pin Aperture
  //For Green, this will be 3 or 4 but not 5 (GreNo1,GreNo2,GrePin)... Holograms will not be printed with the Pin Aperture
  //For  Blue, this will be 6 or 7 but not 8 (BluNo1,BluNo2,BluPin)... Holograms will not be printed with the Pin Aperture
  //I shall store the Aperture for only the Red beam in the hcf file.  (Green and blue must surely be the same). However, I shall store different
  // offsets in the pcf file as there will be mechanical differences that will determine how many steps the motor controlling that aperture should move.
//This really needs to be a field in the hcf file:
 int RED0p8or1p6NotPin = HCFrec.nApertureSize;  //See comments below:
 //nCurrent_uStepsForM5 = PCFrec.chNoofUsteps4EachMotorTab[4];
 //if(PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin] && PCFrec.bMotor2ExpectRepliesFrom[4]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 4, PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin], (STATUSRECORD*)&chReceiveBuffer[4][2]);
 //else
 //  SendMessagePackets(hDlg,
 //                 4,         //Motor 5 RED
 //                 MsTyp5_SING_MM,
 //                 PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin],   //How many steps to move (depends if this Hologram is 0.8 or 1.6 aperture)
 //                 nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM5] );  //The constant speed to move at.

 int GRE0p8or1p6NotPin = RED0p8or1p6NotPin + 3;
 //nCurrent_uStepsForM17 = PCFrec.chNoofUsteps4EachMotorTab[16];
 //if(PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin] && PCFrec.bMotor2ExpectRepliesFrom[16]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 16, PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin], (STATUSRECORD*)&chReceiveBuffer[16][2]);
 //else
 //  SendMessagePackets(hDlg, 16,//Motor 17 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM17] );

 int BLU0p8or1p6NotPin = RED0p8or1p6NotPin + 6;
 //nCurrent_uStepsForM29 = PCFrec.chNoofUsteps4EachMotorTab[28];
 //if(PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin] && PCFrec.bMotor2ExpectRepliesFrom[28]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 28, PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin], (STATUSRECORD*)&chReceiveBuffer[28][2]);
 //else
 //  SendMessagePackets(hDlg, 28,//Motor 29 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM29] );

//Now the 3 LCOS Focus Motors                            These are Motors M6, M18 and M30 (rgb)
 //nCurrent_uStepsForM6  = PCFrec.chNoofUsteps4EachMotorTab[5];
 //if(PCFrec.nNoofUsteps4EachFocusTab[0] && PCFrec.bMotor2ExpectRepliesFrom[5]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 5, PCFrec.nNoofUsteps4EachFocusTab[0], (STATUSRECORD*)&chReceiveBuffer[5][2]);
 //else
 //  SendMessagePackets(hDlg, 5, //Motor 6 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4EachFocusTab[0], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM6] );

 //nCurrent_uStepsForM18 = PCFrec.chNoofUsteps4EachMotorTab[17];
 //if(PCFrec.nNoofUsteps4EachFocusTab[1] && PCFrec.bMotor2ExpectRepliesFrom[17]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 17, PCFrec.nNoofUsteps4EachFocusTab[1], (STATUSRECORD*)&chReceiveBuffer[17][2]);
 //else
 //  SendMessagePackets(hDlg, 17,//Motor 18 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4EachFocusTab[1], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM18] );

 //nCurrent_uStepsForM30 = PCFrec.chNoofUsteps4EachMotorTab[29];
 //if(PCFrec.nNoofUsteps4EachFocusTab[2] && PCFrec.bMotor2ExpectRepliesFrom[29]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 29, PCFrec.nNoofUsteps4EachFocusTab[2], (STATUSRECORD*)&chReceiveBuffer[29][2]);
 //else
 //  SendMessagePackets(hDlg, 29,//Motor 30 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4EachFocusTab[2], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM30] );

//Now the 12 Variable Aperture Motors                    These are Motors M7, M8, M9 and M10 (rgb)
 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[ 6];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointT[0] + nLineXstartPositionsFEM[ 6][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[6])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 6, PCFrec.nNoofUsteps_Hm2CenterPointT[0] + nLineXstartPositionsFEM[ 6][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[6][2]);
 //else
 //  SendMessagePackets(hDlg,  6, //Motor 7 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointT[0] + nLineXstartPositionsFEM[ 6][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[18];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointT[1] + nLineXstartPositionsFEM[18][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[18]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 18, PCFrec.nNoofUsteps_Hm2CenterPointT[1] + nLineXstartPositionsFEM[18][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[18][2]);
 //else
 //  SendMessagePackets(hDlg, 18, //Motor 19 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointT[1] + nLineXstartPositionsFEM[18][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[30];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointT[2] + nLineXstartPositionsFEM[30][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[30]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 30, PCFrec.nNoofUsteps_Hm2CenterPointT[2] + nLineXstartPositionsFEM[30][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[30][2]);
 //else
 //  SendMessagePackets(hDlg, 30, //Motor 31 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointT[2] + nLineXstartPositionsFEM[30][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );


 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[ 7];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointB[0] + nLineXstartPositionsFEM[ 7][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[7])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 7, PCFrec.nNoofUsteps_Hm2CenterPointB[0] + nLineXstartPositionsFEM[ 7][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[7][2]);
 //else
 //  SendMessagePackets(hDlg,  7, //Motor 8 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointB[0] + nLineXstartPositionsFEM[ 7][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[19];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointB[1] + nLineXstartPositionsFEM[19][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[19]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 19, PCFrec.nNoofUsteps_Hm2CenterPointB[1] + nLineXstartPositionsFEM[19][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[19][2]);
 //else
 //  SendMessagePackets(hDlg, 19, //Motor 20 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointB[1] + nLineXstartPositionsFEM[19][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[31];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointB[2] + nLineXstartPositionsFEM[31][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[31]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 31, PCFrec.nNoofUsteps_Hm2CenterPointB[2] + nLineXstartPositionsFEM[31][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[31][2]);
 //else
 //  SendMessagePackets(hDlg, 31, //Motor 32 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointB[2] + nLineXstartPositionsFEM[31][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );


 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[ 8];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointL[0] + nLineXstartPositionsFEM[ 8][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[8])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 8, PCFrec.nNoofUsteps_Hm2CenterPointL[0] + nLineXstartPositionsFEM[ 8][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[8][2]);
 //else
 //  SendMessagePackets(hDlg,  8, //Motor 9 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointL[0] + nLineXstartPositionsFEM[ 8][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[20];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointL[1] + nLineXstartPositionsFEM[20][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[20]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 20, PCFrec.nNoofUsteps_Hm2CenterPointL[1] + nLineXstartPositionsFEM[20][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[20][2]);
 //else
 //  SendMessagePackets(hDlg, 20, //Motor 21 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointL[1] + nLineXstartPositionsFEM[20][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[32];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointL[2] + nLineXstartPositionsFEM[32][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[32]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 32, PCFrec.nNoofUsteps_Hm2CenterPointL[2] + nLineXstartPositionsFEM[32][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[32][2]);
 //else
 //  SendMessagePackets(hDlg, 32, //Motor 33 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointL[2] + nLineXstartPositionsFEM[32][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );


 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[ 9];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointR[0] + nLineXstartPositionsFEM[ 9][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[9])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 9, PCFrec.nNoofUsteps_Hm2CenterPointR[0] + nLineXstartPositionsFEM[ 9][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[9][2]);
 //else
 //  SendMessagePackets(hDlg,  9, //Motor 10 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointR[0] + nLineXstartPositionsFEM[ 9][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[21];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointR[1] + nLineXstartPositionsFEM[21][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[21]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 21, PCFrec.nNoofUsteps_Hm2CenterPointR[1] + nLineXstartPositionsFEM[21][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[21][2]);
 //else
 //  SendMessagePackets(hDlg, 21, //Motor 22 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointR[1] + nLineXstartPositionsFEM[21][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[33];
 //if((PCFrec.nNoofUsteps_Hm2CenterPointR[2] + nLineXstartPositionsFEM[33][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[33]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 33, PCFrec.nNoofUsteps_Hm2CenterPointR[2] + nLineXstartPositionsFEM[33][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[33][2]);
 //else
 //  SendMessagePackets(hDlg, 33, //Motor 34 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2CenterPointR[2] + nLineXstartPositionsFEM[33][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

//Now the 6 Goniometer Motors                            These are Motors M11, M12 (rgb)
 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[10];
 //if((PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0] + nLineXstartPositionsFEM[10][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[10])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 10, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0] + nLineXstartPositionsFEM[10][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[10][2]);
 //else
 //  SendMessagePackets(hDlg, 10, //Motor 11 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0] + nLineXstartPositionsFEM[10][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[22];
 //if((PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1] + nLineXstartPositionsFEM[22][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[22]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 22, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1] + nLineXstartPositionsFEM[22][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[22][2]);
 //else
 //  SendMessagePackets(hDlg, 22,  //Motor 23 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1] + nLineXstartPositionsFEM[22][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[34];
 //if((PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2] + nLineXstartPositionsFEM[34][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[34]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 34, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2] + nLineXstartPositionsFEM[34][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[34][2]);
 //else
 //  SendMessagePackets(hDlg, 34,  //Motor 35 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2] + nLineXstartPositionsFEM[34][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );
 

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[11];
 //if((PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0] + nLineXstartPositionsFEM[11][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[11])  //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 11, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0] + nLineXstartPositionsFEM[11][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[11][2]);
 //else
 //  SendMessagePackets(hDlg,  11, //Motor 12 RED
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0] + nLineXstartPositionsFEM[11][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[23];
 //if((PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1] + nLineXstartPositionsFEM[23][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[23]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 23, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1] + nLineXstartPositionsFEM[23][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[23][2]);
 //else
 //  SendMessagePackets(hDlg, 23,  //Motor 24 GREEN
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1] + nLineXstartPositionsFEM[23][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );

 //nCurrent_uStepsForOM  = PCFrec.chNoofUsteps4EachMotorTab[35];
 //if((PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2] + nLineXstartPositionsFEM[35][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[35]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
     FastMoveSingleMotorABS(hDlg, 35, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2] + nLineXstartPositionsFEM[35][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[35][2]);
 //else
 //  SendMessagePackets(hDlg, 35,  //Motor 36 BLUE
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2] + nLineXstartPositionsFEM[35][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );


//Now the 2 RefBeamTable Motors                          These are Motors M37, M38
 //nCurrent_uStepsForM37 = PCFrec.chNoofUsteps4EachMotorTab[36];
 //if(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0] && PCFrec.bMotor2ExpectRepliesFrom[36]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 36, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0], (STATUSRECORD*)&chReceiveBuffer[36][2]);
 //else
 //  SendMessagePackets(hDlg, 36,//Motor 37 RefBeamTable M1
 //        MsTyp5_SING_MM, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM37] );

 //nCurrent_uStepsForM38 = PCFrec.chNoofUsteps4EachMotorTab[37];
 //if(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1] && PCFrec.bMotor2ExpectRepliesFrom[37]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXtttttt
     FastMoveSingleMotorABS(hDlg, 37, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1], (STATUSRECORD*)&chReceiveBuffer[37][2]);
 //else
 //  SendMessagePackets(hDlg, 37,//Motor 38 RefBeamTable M2
 //          MsTyp5_SING_MM, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM38] );


//int IshouldREMOVEnOverRideMov2StartPosM39SpeedIfNotZero;//and for motor 40 too

//Now the 2 XYstage Motors                               These are Motors M39, M40
 //nCurrent_uStepsForM39 = PCFrec.chNoofUsteps4EachMotorTab[38];            
//if((PCFrec.nNoofUsteps_XYstageHm2Origin[0] + HCFrec.XposOfHologsOriginOnPlateMM) && PCFrec.bMotor2ExpectRepliesFrom[38]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if((PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nLineXstartPositionsFEM[38][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[38]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   FastMoveSingleMotorABS(hDlg, 38, PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nLineXstartPositionsFEM[38][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[38][2]);
 //else{
 //   if(nOverRideM39HomingOutSpeedIfNotZero)
 //     SendMessagePackets(hDlg, 38,//Motor 39   //USING OVERRIDDEN SPEED FROM REGISTRY!
            //MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[0] + HCFrec.XposOfHologsOriginOnPlateMM, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM39] );
 //           MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nLineXstartPositionsFEM[38][HCFrec.nNextLineToPrint], nOverRideM39HomingOutSpeedIfNotZero );
 //   else
 //     SendMessagePackets(hDlg, 38,//Motor 39 
 //         //MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[0] + HCFrec.XposOfHologsOriginOnPlateMM, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM39] );
 //           MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nLineXstartPositionsFEM[38][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM39] );
 //  }

 //nCurrent_uStepsForM40 = PCFrec.chNoofUsteps4EachMotorTab[39];
//if((PCFrec.nNoofUsteps_XYstageHm2Origin[1] + HCFrec.YposOfHologsOriginOnPlateMM) && PCFrec.bMotor2ExpectRepliesFrom[39]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if((PCFrec.nNoofUsteps_XYstageHm2Origin[1] + nLineXstartPositionsFEM[39][HCFrec.nNextLineToPrint]) && PCFrec.bMotor2ExpectRepliesFrom[39]) //DO NOT SEND M sTyp5_SING_MM messages with noofSteps set to zero (would crash AVR)
 //if(bMove2SPsFast) //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   FastMoveSingleMotorABS(hDlg, 39, PCFrec.nNoofUsteps_XYstageHm2Origin[1] + nLineXstartPositionsFEM[39][HCFrec.nNextLineToPrint], (STATUSRECORD*)&chReceiveBuffer[39][2]);
 //else{
 //   if(nOverRideM40HomingOutSpeedIfNotZero)
 //     SendMessagePackets(hDlg, 39,//Motor 40
 //         //MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[1] + HCFrec.YposOfHologsOriginOnPlateMM, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM40] );
 //             MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[1] + nLineXstartPositionsFEM[39][HCFrec.nNextLineToPrint], nOverRideM40HomingOutSpeedIfNotZero );
 //   else
 //     SendMessagePackets(hDlg, 39,//Motor 40
            //MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[1] + HCFrec.YposOfHologsOriginOnPlateMM, nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM40] );
 //           MsTyp5_SING_MM, PCFrec.nNoofUsteps_XYstageHm2Origin[1] + nLineXstartPositionsFEM[39][HCFrec.nNextLineToPrint], nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForM40] );
 //  }

 //StringCchCopy(s zGlobalFileName,_MAX_DIR,HCFcfs.szCSVDirName4Reading);
 //StringCchCat(s zGlobalFileName,_MAX_DIR,FILENAMEStab[0]);
 //T ransfer_FileAndPerform(hDlg, FALSE); //Maybe have another function called Transfer_AllFilesAndPerform as well?  T ransfer_FileAndPerform requires s zGlobalFileName to be set
//Move the remaining 29 motors to the correct position.  i.e Not The aperture motors M5, M17 and M29 (rgb), LCOS Focus Motors, Beam split ratio Motors, Ref table Motors
// (now the 6 PIDs are excluded, this should be 23  motors)
 /*
 for(char nRemaining29Motors = 1; nRemaining29Motors < NOOFMOTORS; nRemaining29Motors++) //No point starting from 0 (it's filtered out below anyway)
   if((nRemaining29Motors !=  0) && (nRemaining29Motors != 12) && (nRemaining29Motors != 24) &&  //Beam split ratio Motors
      (nRemaining29Motors !=  1) && (nRemaining29Motors != 13) && (nRemaining29Motors != 25) &&  //PID motors
      (nRemaining29Motors !=  2) && (nRemaining29Motors != 14) && (nRemaining29Motors != 26) &&  //PID motors
      (nRemaining29Motors !=  3) && (nRemaining29Motors != 15) && (nRemaining29Motors != 27) &&  //Length Of Object Beam Motors
      (nRemaining29Motors !=  4) && (nRemaining29Motors != 16) && (nRemaining29Motors != 28) &&  //Aperture motors
      (nRemaining29Motors !=  5) && (nRemaining29Motors != 17) && (nRemaining29Motors != 29) &&  //LCOS Focus Motors
      (nRemaining29Motors != 36) && (nRemaining29Motors != 37) &&								//Ref table Motors
      (nRemaining29Motors != 38) && (nRemaining29Motors != 39))									//XY stage Motors
     if(nLineXstartPositionsFEM[nRemaining29Motors][HCFrec.nNextLineToPrint] && PCFrec.bMotor2ExpectRepliesFrom[nRemaining29Motors]){ //DO NOT SEND M sTyp5_SING_MM messages if noofSteps set to zero (would crash AVR)
        Sleep(nSleepDelay);
        nCurrent_uStepsForOM = PCFrec.chNoofUsteps4EachMotorTab[nRemaining29Motors];
        SendMessagePackets(hDlg,
                  nRemaining29Motors,
                  M sTyp5_SING_MM,
                  nLineXstartPositionsFEM[nRemaining29Motors][HCFrec.nNextLineToPrint], //How many steps to move
                  nArMaxSpeedsDependentOn_uSteps[nCurrent_uStepsForOM] );//The constant speed to move at.
       }
 */
 //Start the PIDs again
 restartPIDs();
 return 0;
}
//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()
//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()
//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()
//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()//END OF M oveToStartPositions()

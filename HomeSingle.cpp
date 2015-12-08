#include <stdafx.h>
//Home Single Motor is a button on different dialog boxes (S ingleMM_Dialog, S ingleMM_PID_Dialog, T ransBlock_Dialog)
extern CRegStdWORD   bUseCSBs4AccProfs;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;
extern PCFRECORD PCFrec;
extern HCFRECORD HCFrec;
extern CRegStdWORD   nOverRideM39HomingOutSpeedIfNotZero;
extern CRegStdWORD   nOverRideM40HomingOutSpeedIfNotZero;
//extern int nArFastSpeeds4HomingDeptOn_uSteps[];
extern int nArSlowSpeeds4HomingDeptOn_uSteps[]; //instead of the above
extern char szAccelFileNamesDir[_MAX_DIR];         //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
                                                   // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.

//#define TFnP_BMOVE_NOW         0x01
//#define TFnP_BPROPER_CSUM_CHKS 0x02
//#define TFnP_BTHIS_IS_FASTMOVE 0x04

void FastMoveSingleMotor(HWND hDlg, char nchCurrMotrZB, int nReqDistance){
 if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrMotrZB] ){
    if(!ConstructedFileName(hDlg, szAccelFileNamesDir, nchCurrMotrZB + 1, PCFrec.chNoofUsteps4EachMotorTab[nchCurrMotrZB], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bUseCSBs4AccProfs)){
       //int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrMotrZB];
       int nMoveNowANDdoCheckSumsANDisAfastMove;// = 0;
       //if(bMoveNow)         Always False Here
       //  nMoveNowANDdoCheckSumsANDisAfastMove = 1;
       if(bDoProperCheckSumChecks_AccelProfs)
         nMoveNowANDdoCheckSumsANDisAfastMove = 6; //bit 1 (of nMoveNowANDdoCheckSumsANDisAfastMove) is TFnP_BPROPER_CSUM_CHKS (0x02)
       else
         nMoveNowANDdoCheckSumsANDisAfastMove = 4; //bit 2 (of nMoveNowANDdoCheckSumsANDisAfastMove) is TFnP_BTHIS_IS_FASTMOVE (0x04)
       endian_swap(*(unsigned int*)&nReqDistance);
                                                 //never MoveNow, maybe Checksums, always FastMove
       if( !Transfer_FileAndPerform(hDlg, nchCurrMotrZB, nMoveNowANDdoCheckSumsANDisAfastMove, szAccelFileNamesDir, nReqDistance,BDONT_WARNIfITSAnACCPROFILE) ) //FALSE was "Don't move now", the parameter is now nMoveNowANDdoCheckSumsANDisAfastMove - three bit-fields.
            ;  //nReqDistance is valid for T ransfer_FileAndPerform() only when TFnP_BTHIS_IS_FASTMOVE bit is set
//         SendMessagePackets(hDlg,                                                        // szAccelFileNamesDir has something like "c:\MyProjects\HoloProg\Debug\AccProfM07-15Hz-32.csb" or csv in it
//               nchCurrMotrZB,
//                     MsTyp3_HOME,
//                 nCurrent_uSteps,   //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
//                 NULL );
       else
         MessageBox(hDlg,"Unable to start moving FAST as an error occurred\nwhen transferring the Acceleration Profile",NULL,MB_OK);
      }
   }
 else
   MessageBoxWarnNotConnected(hDlg);
}

//The ABS (Absolute) version of this function is very similar to the above version, but takes the required position and current position as parameters. The need to test if a move is necessary is shifted from
// everywhere the function is called from to here (so the test is performed in only one place).
void FastMoveSingleMotorABS(HWND hDlg, char nchCurrMotrZB, int nReqPos, STATUSRECORD *ptr2SR4nCurrentPos){
 if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrMotrZB] ){
    if(!ConstructedFileName(hDlg, szAccelFileNamesDir, nchCurrMotrZB + 1, PCFrec.chNoofUsteps4EachMotorTab[nchCurrMotrZB], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bUseCSBs4AccProfs)){
       int nMoveNowANDdoCheckSumsANDisAfastMove;// = 0;
       if(bDoProperCheckSumChecks_AccelProfs)
         nMoveNowANDdoCheckSumsANDisAfastMove = 6; //bit 1 (of nMoveNowANDdoCheckSumsANDisAfastMove) is TFnP_BPROPER_CSUM_CHKS (0x02)       if(bMoveNow) Always False Here
       else
         nMoveNowANDdoCheckSumsANDisAfastMove = 4; //bit 2 (of nMoveNowANDdoCheckSumsANDisAfastMove) is TFnP_BTHIS_IS_FASTMOVE (0x04)       if(bMoveNow) Always False Here
       int nCurrentPos = *(unsigned int*)ptr2SR4nCurrentPos; //endian_swap will have been done on ptr2SR4nCurrentPos when the packet was received????????????????????????
       endian_swap(*(unsigned int*)&nCurrentPos);
       int nSteps2Move = (nReqPos -  nCurrentPos);
       endian_swap(*(unsigned int*)&nSteps2Move);
                                                 //never MoveNow, maybe Checksums, always FastMove
       if( Transfer_FileAndPerform(hDlg, nchCurrMotrZB, nMoveNowANDdoCheckSumsANDisAfastMove, szAccelFileNamesDir, nSteps2Move, BDONT_WARNIfITSAnACCPROFILE) )
         MessageBox(hDlg,"Unable to start moving FAST as an error occurred\nwhen transferring the Acceleration Profile",NULL,MB_OK);
      }
   }
//else
//  MessageBoxWarnNotConnected(hDlg);
}

void HomeSingleMotor(HWND hDlg, char nchCurrMotrZB){
 if( PCFrec.bMotor2ExpectRepliesFrom[nchCurrMotrZB] ){
    if(!ConstructedFileName(hDlg, szAccelFileNamesDir, nchCurrMotrZB + 1, PCFrec.chNoofUsteps4EachMotorTab[nchCurrMotrZB], HCFrec.uiFrequencyOfPixelPrintsTimes1M / 1000000, bUseCSBs4AccProfs)){
       //int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nchCurrMotrZB];
       int nMoveNowANDdoCheckSumsANDisAfastMove = 0;
       //if(bMoveNow)         Always False Here
       //  nMoveNowANDdoCheckSumsANDisAfastMove = 1;
       if(bDoProperCheckSumChecks_AccelProfs)
         nMoveNowANDdoCheckSumsANDisAfastMove = 2; //bit 1 is TFnP_BPROPER_CSUM_CHKS (0x02)
                                                 //never MoveNow, maybe Checksums, never FastMove   (rather like when T ransfer_ALL_AccProfsAndPerform() is called)
       if( !Transfer_FileAndPerform(hDlg, nchCurrMotrZB, nMoveNowANDdoCheckSumsANDisAfastMove, szAccelFileNamesDir,0,BDONT_WARNIfITSAnACCPROFILE) ) //FALSE was "Don't move now", the parameter is now
                // nMoveNowANDdoCheckSumsANDisAfastMove - three bit-fields.  szAccelFileNamesDir has something like "c:\MyProjects\HoloProg\Debug\AccProfM07-15Hz-32.csb" or csv in it
         SendMsTyp3_HOMEmessage(hDlg, nchCurrMotrZB);
         //if(nchCurrMotrZB < 38){ //We're currently taking M39 and M40's HO speeds from the registry if nOverRideM39HomingOutSpeedIfNotZero or nOverRideM40HomingOutSpeedIfNotZero is set.
         //   SendMessagePackets(hDlg,
         //             nchCurrMotrZB,
         //               MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
         //           nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
         //       nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
         //         //The limits on 'how many steps to move' are still determined by UC3 hardcode
         //  }
         //else{//Motor 39 or 40
         //   if(nchCurrMotrZB == 39){//Motor 40
         //      if(nOverRideM40HomingOutSpeedIfNotZero)
         //        SendMessagePackets(hDlg,
         //                  nchCurrMotrZB,
         //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
         //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
         //          nOverRideM40HomingOutSpeedIfNotZero); //The constant Fast speed to move at, when homing in
         //      else
         //        SendMessagePackets(hDlg,
         //                  nchCurrMotrZB,
         //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
         //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
         //          nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
         //     }
         //   else{//Motor 39
         //      if(nOverRideM39HomingOutSpeedIfNotZero)
         //        SendMessagePackets(hDlg,
         //                  nchCurrMotrZB,
         //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
         //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
         //          nOverRideM39HomingOutSpeedIfNotZero ); //The constant Fast speed to move at, when homing in
         //      else
         //        SendMessagePackets(hDlg,
         //                  nchCurrMotrZB,
         //                    MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
         //                nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
         //          nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
         //     }
         //  }
       else
         MessageBox(hDlg,"Unable to start homing as an error occurred\nwhen transferring the Acceleration Profile",NULL,MB_OK);
      }
   }
 else
   MessageBoxWarnNotConnected(hDlg);
}

void SendMsTyp3_HOMEmessage(HWND hDlg, char emMTSP){
 int nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[emMTSP];
 if(emMTSP < 38){ //We're currently taking M39 and M40's HO speeds from the registry if nOverRideM39HomingOutSpeedIfNotZero or nOverRideM40HomingOutSpeedIfNotZero is set.
    SendMessagePackets(hDlg,
                     emMTSP,
                MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
            nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
        nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
          //The limits on 'how many steps to move' are still determined by UC3 hardcode
   }
 else{//Motor 39 or 40
    if(emMTSP == 39){//Motor 40
       if(nOverRideM40HomingOutSpeedIfNotZero)
         SendMessagePackets(hDlg,
                          emMTSP,
                     MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
                 nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
                 nOverRideM40HomingOutSpeedIfNotZero); //The constant Fast speed to move at, when homing in
       else
         SendMessagePackets(hDlg,
                          emMTSP,
                     MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
                 nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
                 nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
      }
    else{//Motor 39
       if(nOverRideM39HomingOutSpeedIfNotZero)
         SendMessagePackets(hDlg,
                          emMTSP,
                     MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
                 nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
                 nOverRideM39HomingOutSpeedIfNotZero ); //The constant Fast speed to move at, when homing in
       else
         SendMessagePackets(hDlg,
                          emMTSP,
                     MsTyp3_HOME, //This is OK... we transferred the *Homing* accelerations above
                 nCurrent_uSteps, //Controls the constant Slow speed to move at, when homing out (how many Non-Pulsing_Interrupts to use)
                 nArSlowSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ); //The constant slow speed to move at, when homing OUT
      }
   }
}
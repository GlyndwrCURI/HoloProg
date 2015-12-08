#include "stdafx.h"

//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP//TEMP
//double dblHISTORYErrorWP2greSumdAndAveraged[66000];
//short  nHIST = 0;
extern HFONT newFont;
extern BOOL bDisplayPIDdebugMsgs;
unsigned short usXcoordFromEMrec;
unsigned short usYcoordFromEMrec;

double dblErrorWP2redDisplay;

double dblErrorWP3redDisplay;
double dblErrorWP2greDisplay;
double dblErrorWP3greDisplay;
double dblErrorWP2bluDisplay;
double dblErrorWP3bluDisplay;

double PidOutputToActOn[6];
extern int nNOOFreadsToAVERAGE;

//double SetPointWP2red; //SetPoints are no longer looked at
//double SetPointWP3red;
//double SetPointWP2gre; //SetPoints are no longer looked at
//double SetPointWP3gre;
//double SetPointWP2blu; //SetPoints are no longer looked at
//double SetPointWP3blu;
//int nJUSTTRYINGSPEED = 64000;
int nNoofCatastrophys;
BOOL bFirstTime;               //Using to set the
SPid PidStructVariableWP2Red;  //  initial postion in the PID routine
volatile BOOL bStopPIDsendingMessages;  //    - ALSO when reenabling the motors!
//BOOL bPIDMovementStoppedForHoming;
SPid PidStructVariableWP3Red;
SPid PidStructVariableWP2Gre;
SPid PidStructVariableWP3Gre;
SPid PidStructVariableWP2Blu;
SPid PidStructVariableWP3Blu;

//extern PIDdebugStruct dbPID[6][6000];//big enough to store 200 seconds
/*
extern int nWavPlate2RealEnergyAr[4096];
extern int nWavPlate3RealEnergyAr[4096];
extern int nWavPlate14RealEnergyAr[4096];
extern int nWavPlate15RealEnergyAr[4096];
extern int nWavPlate26RealEnergyAr[4096];
extern int nWavPlate27RealEnergyAr[4096];
*/

extern double nWavPlate2RealEnergyAr[4096];
extern double nWavPlate3RealEnergyAr[4096];
extern double nWavPlate14RealEnergyAr[4096];
extern double nWavPlate15RealEnergyAr[4096];
extern double nWavPlate26RealEnergyAr[4096];
extern double nWavPlate27RealEnergyAr[4096];

static double dblErrorWP2redSumdAndAveraged = 0;
static double dblErrorWP3redSumdAndAveraged = 0;
static double dblErrorWP2greSumdAndAveraged = 0;
static double dblErrorWP3greSumdAndAveraged = 0;
static double dblErrorWP2bluSumdAndAveraged = 0;
static double dblErrorWP3bluSumdAndAveraged = 0;
static int nIsFifthMessage = 0;


extern int bWaitingForReply[];
extern BOOL bRunningManually;
extern HWND hGLWnd;

extern double dblArDesiredEnergyWP3r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];

extern double dblArDesiredEnergyWP2r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];


int NOOFTIMESTHRU_B4OBJECTING;//5 or 60?  NEVER set NOOFTIMESTHRU_B4OBJECTING >= 999,999,997 : that will break the hard-coded use of such numbers
int com_thr_CAstop; //Stop UDP thread  CA == Check All
static int comUSB_thrstop; //Stop USB thread

extern int nGlobalLastUSBrecNoRecvd;// = -1; This is an index into the table:
extern ENERGYREC sArrEnergRecs[]; // = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)

extern HCFRECORD HCFrec;

int nThreadErrorCode;
HMODULE     m_hmodule;
DLLFuncs    DLLf;
static FT_HANDLE ftHandleEM; //internal linkage, EM == Energy Meters
FT_HANDLE   ftHandleCB;      //external linkage, CB == Contorl Box

LPTSTR   GetStringRes (int id); //Function Prototype

int Create_CheckAll_MonitoringThread(HWND hWnd){ //This now creates only 1 thread (at start of program)
 com_thr_CAstop=0;
 DWORD tid;                                      //** Open thread now
 HANDLE hThrCA=CreateThread(0,0,(LPTHREAD_START_ROUTINE)com_Thr_CheckAllWaiting4reply,0,0,&tid);
 if( hThrCA==NULL ){
    char szDum[12];
    int ee=GetLastError();
    _itoa_s(ee, szDum, sizeof(szDum), 10);
    MessageBox(hWnd,"Create Thread(Check All) failed, Code:",szDum,MB_OK);
    com_Exit();
    return -1; //This will cause WndProc to terminate the program
   }
 return 0;
}

void com_Exit(void){    //CALL com_Exit ONCE at the end of main program - It stops the thread running.
 com_thr_CAstop=1;           //Uses com_thr_CAstop to stop com_Thr_CheckAllWaiting4reply()
 Sleep(150);                 // & waits till its done
}

//com_Thr_CheckAllWaiting4reply() Maintains counters (i.e. monitors the time) for all motors from whom the main thread is waiting for a UDP reply.
//MainThread calls R eceiveMessagePackets() which immediately creates R eceive_Thr() or R eceive8000_Thr() which will block on recv(). Meantime MainThread
//will continue waiting in  R eceiveMessagePackets() for either the reply or for the counters maintained here to indicate a timeout (in which case
//R eceive_Thr() or R eceive8000_Thr() will be terminated) and MainThread can continue to run normally.
static DWORD WINAPI com_Thr_CheckAllWaiting4reply(void){
 while ( !com_thr_CAstop ){
    for(int emThreadCheck = 0; emThreadCheck < NOOFMOTORS; emThreadCheck++)
      if(bWaitingForReply[emThreadCheck]){  //See Comments in the initial section - i.e. near to:     if(bWaitingForReply[18]){  
         if(bWaitingForReply[emThreadCheck]++ > NOOFTIMESTHRU_B4OBJECTING){
            if( (bWaitingForReply[emThreadCheck] > 999999999) && (bWaitingForReply[emThreadCheck] < 1010000003) ) //i.e. 999,999,999 & 1,010,000,003
              ;  //This means recv returned... we can leave bWaitingForReply[x], as it is and let the main program process the reply normally 
            else // recv did not return.  THIS function has incremented bWaitingForReply[x] until it shows we've timed out (probably a value of about 10 - 100 i.e. NOOFTIMESTHRU_B4OBJECTING)
              bWaitingForReply[emThreadCheck] = 2000000000;  //Main program to be responsible for killing the thread. //i.e.  2,000,000,000
           }
        }
    Sleep(100); // could also add extra user operations here
   }      //while ( !com_thr_CAstop )
 return 0;//** End of thread CA (Check All)
}

//The above functions all handle the UDP comms with all the motor controllers, the following functions handle the USB comms with the
// Energy controller (as a second, seperate, thread).

// Uses comUSB_thrstop
int  comUSB_Exit(HWND hWnd){             //************   CALL comUSB_Exit ONCE at the end of the program
 int comUSB_ExtRetCode;
 if(ftHandleEM==0) return 0; //** If already closed, ignore the call
 comUSB_thrstop=1; //** If thread is running, attempt to close it
 Sleep(200);    //**  -(& wait till its done it)
 comUSB_ExtRetCode = DLLf.m_pClose(ftHandleEM);
 if(FT_OK != comUSB_ExtRetCode){
    MessageBox(hWnd,"Couldn't close USB port",NULL,MB_OK);
    nThreadErrorCode = 8;
   }
 ftHandleEM=NULL;
 if(!FreeLibrary(m_hmodule)){
    MessageBox(hWnd,"Couldn't Free FTDI Library",NULL,MB_OK);
    nThreadErrorCode = 9;
   }
 m_hmodule = NULL;
 return 0;
}

//**   - The main running thread. This is called by the Init function, and
//        made to stop by the Exit function. User does not call this.
//     - Uses NON_OVERLAPPED mode with simple sleep delay for simplicity
static DWORD WINAPI comUSB_Thr_EM(void){ //Thread for the Energy Meters runs continously until the end of the program
 int nRecvingRecNo = 0; //We could kill and restart this thread for each Print (& perhaps need to if nRecvingRecNo is initialised here). See also nGlobalLastUSBrecNoRecvd
 DWORD ntr;
 DWORD nNumBytesRead;
 int   comUSB_ThRetCodeEM;
 BYTE  *pOneByteRead;
 BYTE  NewBuff[32000];
 BYTE  byteDuffBuff[_MAX_DIR];

 bFirstTime = 1; //Using to set the initial postion
 double dlbPositionWP2red;
 double dlbPositionWP3red;
 double dlbPositionWP2gre;
 double dlbPositionWP3gre;
 double dlbPositionWP2blu;
 double dlbPositionWP3blu;
 double dblErrorWP2red;
 double dblErrorWP3red;
 double dblErrorWP2gre;
 double dblErrorWP3gre;
 double dblErrorWP2blu;
 double dblErrorWP3blu;
 
//double dblErrorWP2redARRAY[nNOOFreadsToAVERAGE];
//double dblErrorWP3redARRAY[nNOOFreadsToAVERAGE];
//double dblErrorWP2greARRAY[nNOOFreadsToAVERAGE];
//double dblErrorWP3greARRAY[nNOOFreadsToAVERAGE];
//double dblErrorWP2bluARRAY[nNOOFreadsToAVERAGE];
//double dblErrorWP3bluARRAY[nNOOFreadsToAVERAGE];

 //double intValue;
 for(PidStructVariableWP2Red.ptr = 0; PidStructVariableWP2Red.ptr < 16; PidStructVariableWP2Red.ptr++){
    PidStructVariableWP2Red.Tab[PidStructVariableWP2Red.ptr] = 0;
    PidStructVariableWP3Red.Tab[PidStructVariableWP2Red.ptr] = 0;
    PidStructVariableWP2Gre.Tab[PidStructVariableWP2Red.ptr] = 0;
    PidStructVariableWP3Gre.Tab[PidStructVariableWP2Red.ptr] = 0;
    PidStructVariableWP2Blu.Tab[PidStructVariableWP2Red.ptr] = 0;
    PidStructVariableWP3Blu.Tab[PidStructVariableWP2Red.ptr] = 0;
   }

 PidStructVariableWP2Red.pidID = 0;
 PidStructVariableWP3Red.pidID = 1;
 PidStructVariableWP2Gre.pidID = 2;
 PidStructVariableWP3Gre.pidID = 3;
 PidStructVariableWP2Blu.pidID = 4;
 PidStructVariableWP3Blu.pidID = 5;

 PidStructVariableWP2Red.ptr = 0;
 PidStructVariableWP3Red.ptr = 0;
 PidStructVariableWP2Gre.ptr = 0;
 PidStructVariableWP3Gre.ptr = 0;
 PidStructVariableWP2Blu.ptr = 0;
 PidStructVariableWP3Blu.ptr = 0;
#if 0
 PidStructVariableWP2Red.pGain = 8000;
 PidStructVariableWP2Red.dGain = 10000;
 PidStructVariableWP2Red.iGain = 23;

 PidStructVariableWP2Red.pGain = 15;
 PidStructVariableWP2Red.dGain = 90;
 PidStructVariableWP2Red.iGain = 0.1;

 PidStructVariableWP2Red.pGain = 15;   // A bit slow 
 PidStructVariableWP2Red.dGain = 9;    //  settling, but
 PidStructVariableWP2Red.iGain = 0.1;  //   relatively smooth!
/*
 PidStructVariableWP2Red.pGain = 45;   // A bit slow 
 PidStructVariableWP2Red.dGain = 29;    //  settling, but
 PidStructVariableWP2Red.iGain = 0.1;  //   relatively smooth!

 PidStructVariableWP2Red.pGain = 45;   // A bit slow 
 PidStructVariableWP2Red.dGain = 29;    //  settling, but
 PidStructVariableWP2Red.iGain = 3;  //   relatively smooth!

 PidStructVariableWP2Red.pGain = 200;   // A bit slow 
 PidStructVariableWP2Red.dGain = 200;    //  settling, but
 PidStructVariableWP2Red.iGain = 9;  //   relatively smooth!
*/
 PidStructVariableWP2Red.pGain = 400;
 PidStructVariableWP2Red.dGain = 300;
 PidStructVariableWP2Red.iGain = 1;

 PidStructVariableWP2Red.pGain = 15;
 PidStructVariableWP2Red.dGain = 0;
 //PidStructVariableWP2Red.iGain = 0.03;
 PidStructVariableWP2Red.iGain = 0;
 PidStructVariableWP2Red.pGain = 120;
 PidStructVariableWP2Red.pGain = 290;
 PidStructVariableWP2Red.dGain = 0;
 //PidStructVariableWP2Red.iGain = 0.03;
 PidStructVariableWP2Red.iGain = 2;

#endif

//When TESTING GREEN
 PidStructVariableWP2Gre.pGain = 0.15;
 PidStructVariableWP2Gre.dGain = 0;
 //PidStructVariableWP2Red.iGain = 0.03;
 PidStructVariableWP2Gre.iGain = 0.002;

//When TESTING GREEN DYNAMIC
 PidStructVariableWP3Gre.pGain = 0.15;
 PidStructVariableWP3Gre.dGain = 0;
 //PidStructVariableWP2Red.iGain = 0.03;
 PidStructVariableWP3Gre.iGain = 0.002;
 
//BLUE
 PidStructVariableWP2Blu.pGain = PidStructVariableWP2Gre.pGain;
 PidStructVariableWP2Blu.dGain = PidStructVariableWP2Gre.dGain;
 PidStructVariableWP2Blu.iGain = PidStructVariableWP2Gre.iGain;

//BLUE DYNAMIC
 PidStructVariableWP3Blu.pGain = PidStructVariableWP3Gre.pGain;
 PidStructVariableWP3Blu.dGain = PidStructVariableWP3Gre.dGain;
 PidStructVariableWP3Blu.iGain = PidStructVariableWP3Gre.iGain;
 
//RED
 PidStructVariableWP2Red.pGain = PidStructVariableWP2Gre.pGain;
 PidStructVariableWP2Red.dGain = PidStructVariableWP2Gre.dGain;
 PidStructVariableWP2Red.iGain = PidStructVariableWP2Gre.iGain;
//RED DYNAMIC
 PidStructVariableWP3Red.pGain = PidStructVariableWP3Gre.pGain;
 PidStructVariableWP3Red.dGain = PidStructVariableWP3Gre.dGain;
 PidStructVariableWP3Red.iGain = PidStructVariableWP3Gre.iGain;

 //SetPoint = 5.39; //This was volts
 //SetPointWP2red = (double)HCFrec.WP2rObjEnergyTimes1000/1000; //SetPoints are no longer looked at
 //SetPointWP2gre = (double)HCFrec.WP2gObjEnergyTimes1000/1000; //SetPoints are no longer looked at
 //SetPointWP2blu = (double)HCFrec.WP2bObjEnergyTimes1000/1000; //SetPoints are no longer looked at

 //int WRONG_3sSHOULDcomeFromFile; //Perhaps the StartPositions File??
 //SetPointWP3red = (double)HCFrec.WP2rObjEnergyTimes1000/1000;
 //SetPointWP3gre = (double)HCFrec.WP2gObjEnergyTimes1000/1000;
 //SetPointWP3blu = (double)HCFrec.WP2bObjEnergyTimes1000/1000;

int _Is_this_where_I_should_be_getting_the_value_from_File;
int _Should_I_Also_be_Setting_up_dblArDesiredEnergyWP2r_g_b_here_also;
 //dblArDesiredEnergyWP3r[0][0] = (double)HCFrec.WP2rObjEnergyTimes1000/1000;
 //dblArDesiredEnergyWP3g[0][0] = (double)HCFrec.WP2gObjEnergyTimes1000/1000;
 //dblArDesiredEnergyWP3b[0][0] = (double)HCFrec.WP2bObjEnergyTimes1000/1000;
 //
 //dblArDesiredEnergyWP2r[0][0] = (double)HCFrec.WP2rObjEnergyTimes1000/1000;
 //dblArDesiredEnergyWP2g[0][0] = (double)HCFrec.WP2gObjEnergyTimes1000/1000;
 //dblArDesiredEnergyWP2b[0][0] = (double)HCFrec.WP2bObjEnergyTimes1000/1000;

 //SetPoint = 944;  //These were
 //SetPoint = 1544; // ADC codes

 /*
 PidStructVariableWP2Red.iMax=100;
 PidStructVariableWP2Red.iMin=-100;
//The following initialises all the controllers to be indentical to WP2Red controller
 PidStructVariableWP3Red.iMax = PidStructVariableWP2Red.iMax;
 PidStructVariableWP3Red.iMin = PidStructVariableWP2Red.iMin;

 PidStructVariableWP2Gre.iMax = PidStructVariableWP2Red.iMax;
 PidStructVariableWP2Gre.iMin = PidStructVariableWP2Red.iMin;

 PidStructVariableWP3Gre.iMax = PidStructVariableWP2Red.iMax;
 PidStructVariableWP3Gre.iMin = PidStructVariableWP2Red.iMin;

 PidStructVariableWP2Blu.iMax = PidStructVariableWP2Red.iMax;
 PidStructVariableWP2Blu.iMin = PidStructVariableWP2Red.iMin;

 PidStructVariableWP3Blu.iMax = PidStructVariableWP2Red.iMax;
 PidStructVariableWP3Blu.iMin = PidStructVariableWP2Red.iMin;
 */
 /*
 PidStructVariableWP3Red.pGain = PidStructVariableWP2Red.pGain;
 PidStructVariableWP3Red.dGain = PidStructVariableWP2Red.dGain;
 PidStructVariableWP3Red.iGain = PidStructVariableWP2Red.iGain;

 PidStructVariableWP2Gre.pGain = PidStructVariableWP2Red.pGain;
 PidStructVariableWP2Gre.dGain = PidStructVariableWP2Red.dGain;
 PidStructVariableWP2Gre.iGain = PidStructVariableWP2Red.iGain;

 PidStructVariableWP3Gre.pGain = PidStructVariableWP2Red.pGain;
 PidStructVariableWP3Gre.dGain = PidStructVariableWP2Red.dGain;
 PidStructVariableWP3Gre.iGain = PidStructVariableWP2Red.iGain;
                                                        
 PidStructVariableWP2Blu.pGain = PidStructVariableWP2Red.pGain;
 PidStructVariableWP2Blu.dGain = PidStructVariableWP2Red.dGain;
 PidStructVariableWP2Blu.iGain = PidStructVariableWP2Red.iGain;

 PidStructVariableWP3Blu.pGain = PidStructVariableWP2Red.pGain;
 PidStructVariableWP3Blu.dGain = PidStructVariableWP2Red.dGain;
 PidStructVariableWP3Blu.iGain = PidStructVariableWP2Red.iGain;
*/
 nNoofCatastrophys = 0;
 do{
    comUSB_ThRetCodeEM = DLLf.m_pStopInTask(ftHandleEM);
    if(comUSB_ThRetCodeEM != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCodeEM != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
       nThreadErrorCode = 1;
      }
   } while (comUSB_ThRetCodeEM != FT_OK);
 do{
    comUSB_ThRetCodeEM = DLLf.m_pPurge(ftHandleEM, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
    if(comUSB_ThRetCodeEM != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCodeEM != FT_OK Comm Thread (Purge)",NULL,MB_OK);
       nThreadErrorCode = 2;
      }
   } while (comUSB_ThRetCodeEM != FT_OK);
 int cnt = 0;
 do{
    comUSB_ThRetCodeEM = DLLf.m_pRestartInTask(ftHandleEM);
    cnt++;
   } while ((comUSB_ThRetCodeEM != FT_OK) && (cnt < 50010));
 if(cnt > 50000) //It's actually about 1400!
   MessageBox(NULL,"count >50000 for RestartInTask",NULL,MB_OK);
 int NoofBytesDiscarded = 0;
 int nIsDataBeingCollected = 0;
/**************************************************************************************************/
/*********************                                           **********************************/
/********************* INITIAL LOOP THAT THROWS AWAY A FEW BYTES **********************************/
/*********************                                           **********************************/
/**************************************************************************************************/
 while ( (comUSB_thrstop==0) && (ftHandleEM!=NULL) &&(m_hmodule!=NULL) && (NoofBytesDiscarded < 500) ){

#ifndef B_ALLOW_USBCOMMS_ERRORS
    if(nIsDataBeingCollected++ > 60){ //was 1960
       MessageBox(NULL,GetStringRes(STR_IS_ALTERA_EM_PROCESSOR_RUNNING), "USB device found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA (energy meters) processor running?"
       nThreadErrorCode = 3;
                                // NoofBytesDiscarded = 4000;
      }
#endif
//* Get nr of bytes in rx queue to read
    //int WARNINGweDontUSeFollowingRetCode;//What I mean is, "we don't inspect the return code" (also search below for WARNINGweDontUSeFollowingRetCodeEITHER)
    comUSB_ThRetCodeEM = DLLf.m_pGetQueueStatus(ftHandleEM, &ntr );//ntr was dwAmountInRxQueue - gives the Q size
    if (comUSB_ThRetCodeEM!=FT_OK){    //* Handle errors, if any{    //* Handle errors, if any
       MessageBox(NULL,"m_pGetQueueStatus: bad retcode in Comm Thread",NULL,MB_OK);
       nThreadErrorCode = 12;
       return 0;
      }
    if (ntr>0){ //* align for wraparound in circular buffer
       nIsDataBeingCollected = 0;
       if (ntr > _MAX_DIR)
         ntr = _MAX_DIR;
       //* Read the bytes to our buffer
//ntr (number to receive) will have been set to less than what's in the Q, if the buffer didnt have enough room between
// com_bindx and the end of the buffer.  The stuff left in the Q will be read next time.
       comUSB_ThRetCodeEM = DLLf.m_pRead(ftHandleEM, byteDuffBuff, ntr, &nNumBytesRead);
       if (comUSB_ThRetCodeEM!=FT_OK){    //* Handle errors, if any
          MessageBox(NULL,"Communication Read Error in Comm Thread",NULL,MB_OK);
          nThreadErrorCode = 4;
         }
       else {
          NoofBytesDiscarded += nNumBytesRead;
          unsigned char myVisableBuff[260];
          nIsDataBeingCollected = 0;
          if(nNumBytesRead < _MAX_DIR + 4)
            memcpy(myVisableBuff,byteDuffBuff,nNumBytesRead);
          else{
             memcpy(myVisableBuff,byteDuffBuff,_MAX_DIR + 4);//should be impossible if _MAX_DIR < 260
             MessageBox(NULL,"Program Maintenance needed - (nNumBytesRead >= _MAX_DIR + 4) was thought impossible",NULL,MB_OK);//Is FTDI's DLLf.m_pRead function knackered?
            }
          nIsDataBeingCollected = nIsDataBeingCollected;//for breakpoint
         }
      }
    else     //* If nothing was read from line, block thread to allow buffer to fill.
      Sleep(10);         // could also add extra user operations here
  //com_bindx = 0;//Keep writting to the beginning of the buffer
   }//while
 nIsDataBeingCollected = 0;
//We've thrown away the first few K of data, now lets start the data-collection proper (com_bindx will be zero)

/***************************************************************************************************/
/***************************************************************************************************/
/******************************************             ********************************************/
/******************************************  MAIN LOOP  ********************************************/
/******************************************             ********************************************/
/******************************************-------------********************************************/
//********* Do the forever loop until “stop” flag is set (either externally or internally) *********/
/***************************************************************************************************/
 nNumBytesRead = 0;
 //nRecvingRecNo = 0; Done above.  It needs to stay set at nGlobalLastUSBrecNoRecvd + 1
 char *pch_sArrEnergRecs = (char*)sArrEnergRecs; //here, this is same as pch_sArrEnergRecs = &sArrEnergRecs[nRecvingRecNo];

//The following [commented] code tests all is ok if sArrEnergRecs fills up completely (as could happen after 20 Hrs printing @ 60Hz)
//It demonstrates that Windows doen't crash, but it is unlikely at the moment that proper (or even any) control of Energy would remain if the table did fill
//nRecvingRecNo = NOOF_ENERGYRECS - 500;
//pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];
//nGlobalLastUSBrecNoRecvd = nRecvingRecNo - 1;

#define AWAITINGSTART 0
#define AWAITINGADDR  1
#define AWAITING_F    2
#define AWAITING_N    3
#define AWAITING_D0   4
#define AWAITING_D1   5
#define AWAITING_D2   6
#define AWAITING_D3   7
#define AWAITING_D4   8
#define AWAITING_D5   9
#define AWAITING_D6  10
#define AWAITING_D7  11
#define AWAITING_D8  12
#define AWAITING_D9  13
#define AWAITING_D10 14
#define AWAITING_D11 15
#define AWAITING_D12 16
#define AWAITING_D13 17
#define AWAITING_D14 18
#define AWAITING_D15 19
#define AWAITING_D16 20
#define AWAITING_D17 21
#define AWAITING_D18 22
#define AWAITING_D19 23
#define AWAITING_D20 24
#define AWAITING_D21 25
#define AWAITING_D22 26
#define AWAITING_D23 27
#define AWAITING_D24 28
#define AWAITING_D25 29
#define AWAITING_D26 30
#define AWAITING_CRC 31
#define AWAITINGSTOP 32
#define AWAITINGSPC1 33
#define AWAITINGSPC2 34
#define XTRA1        35
#define XTRA2        36
#define XTRA3        37
#define XTRA4        38

 int nState = AWAITINGSTART;
 static char crc;

 int nNoofConsecutiveNonStartChars = 0;

//int hNTRidx = 0, HistoryOfNTR[100000];
 while ( !comUSB_thrstop && (ftHandleEM!=NULL) && (m_hmodule!=NULL) ){
    comUSB_ThRetCodeEM = DLLf.m_pGetQueueStatus(ftHandleEM, &ntr );//* Get noof bytes in rx queue available to read
    if (comUSB_ThRetCodeEM!=FT_OK){    //* Handle errors, if any{    //* Handle errors, if any
       MessageBox(NULL,"m_pGetQueueStatus: bad retcode in Comm Thread2",NULL,MB_OK);
       nThreadErrorCode = 13;
       return 0;
      }
    //HistoryOfNTR[hNTRidx++] = ntr;
    if(ntr > 32000){
     //MessageBox(NULL, "\t\tCATASTROPHY!! (it's not really!)\n\nToo much data was available in ftdi buffers collecting data from the energy meters","Exiting Thread. Holoprog needs restarting",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
     //nThreadErrorCode = 10;
       SetRedWarning();
       nNoofCatastrophys++;
     //return 0;
       while(ntr > 0){   //this just clears and discards the buffer
          if(ntr > 32000){
             DLLf.m_pRead(ftHandleEM, NewBuff, 32000, &nNumBytesRead);
             ntr -= 32000;
            }
          else{
             DLLf.m_pRead(ftHandleEM, NewBuff, ntr, &nNumBytesRead);
             ntr = 0;
            }
         }
       continue;
      }
    comUSB_ThRetCodeEM = DLLf.m_pRead(ftHandleEM, NewBuff, ntr, &nNumBytesRead);
    if (comUSB_ThRetCodeEM!=FT_OK){    //Handle errors, if any
       MessageBox(NULL,"Communication Read Error1 in Comm Thread",NULL,MB_OK);
       nThreadErrorCode = 11;
       return 0;
      }
    pOneByteRead = &NewBuff[0];
    if(!ntr)           //If nothing was read from line, block thread to allow buffer to fill.
      Sleep(10);
    else{
       while (ntr-- > 0){
          //comUSB_ThRetCodeEM = DLLf.m_pRead(ftHandleEM, OneByteRead, 1, &nNumBytesRead);
          //if (comUSB_ThRetCodeEM!=FT_OK){    //Handle errors, if any
          //  MessageBox(NULL,"Communication Read Error1 in Comm Thread",NULL,MB_OK);  nThreadErrorCode = 5;   }
          //else{
             nIsDataBeingCollected = 0;
             switch (nState){
                case AWAITINGSTART:
                  if(*pOneByteRead++ == 0x3A){
                     nState = AWAITINGADDR;//1st byte recvd
                     nNoofConsecutiveNonStartChars = 0;
                    }
                  else{
                     //nState = AWAITINGSTART; unnecessary - we are in this state already
                     nNoofConsecutiveNonStartChars++;
                     continue; //Drop this character, go on to 'while (ntr-- > 0)' and read the next one
                    }
                  break;
                case AWAITINGADDR:
                  if(*pOneByteRead++ == 0x01){
                   //crc = *OneByteRead;
                     crc = 0x01;
                     nState = AWAITING_F;  //2nd byte
                    }
                  else{
                     nState = AWAITINGSTART;
                     continue; //Abandon this message, go on to 'while (ntr-- > 0)' and await the start of the next one
                    }
                  break;
                case AWAITING_F:
                  if(*pOneByteRead++ == 0x45){
                   //crc += *OneByteRead;
                     crc += 0x45;
                     nState = AWAITING_N;  //3rd byte recvd
                    }
                  else{
                     nState = AWAITINGSTART;
                     continue;
                    }
                  break;
                  /*original
                case AWAITING_N:
                  if(*OneByteRead == 27)  //3*9
                    nState = AWAITING_D0; //4th byte recvd
                  else{
                     nState = AWAITINGSTART;
                     continue;
                    }
                  break;
                  */
                case AWAITING_N:
                  if(*pOneByteRead++ == 31){ //3*9 + the 4 extra bytes ()
                     crc += 31;
                     nState = XTRA1; //4th byte recvd
                    }
                  else{
                     nState = AWAITINGSTART;
                     continue;
                    }
                  break;
                //These are the coordinate fields Jerzy has added in to the Records from the Energy Meters
                case XTRA1:
                  nState = XTRA2;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case XTRA2:
                  nState = XTRA3;
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case XTRA3:
                  nState = XTRA4;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case XTRA4:
                  nState = AWAITING_D0;
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;

//1st Meter              Motor 1, 13, 25, 2, 14, 26, 3 ,15, 27
                case AWAITING_D0:
                  nState = AWAITING_D1;     //5th byte recvd - D0 (LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                //crc = 97 + *OneByteRead;  //73 is Addr(0x01) + Func ('E', 0x45, 69) + N (9*3).  Start is not included.
                  crc += *pOneByteRead++;  //73 is Addr(0x01) + Func ('E', 0x45, 69) + N (9*3).  Start is not included.
                  break;
                case AWAITING_D1:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D2;  //10th byte recvd - D1 (MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D2:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D3;  //11th byte recvd - D2 (Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//2nd Meter
                case AWAITING_D3:
                  nState = AWAITING_D4;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D4:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D5;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit) //13th byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D5:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D6;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//14th byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//3rd Meter
                case AWAITING_D6:
                  nState = AWAITING_D7;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D7:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D8;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit) //16th byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D8:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D9;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1! //17th byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//4th Meter
                case AWAITING_D9:
                  nState = AWAITING_D10;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D10:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D11;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)//19th byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D11:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D12;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//20th byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//5th Meter
                case AWAITING_D12:
                  nState = AWAITING_D13;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D13:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D14;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)//22nd byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D14:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D15;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//23rd byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//6th Meter
                case AWAITING_D15:
                  nState = AWAITING_D16;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D16:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D17;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)//25th byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D17:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D18;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//26th byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//7th Meter
                case AWAITING_D18:
                  nState = AWAITING_D19;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D19:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D20;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)//28th byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D20:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D21;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//29th byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//8th Meter
                case AWAITING_D21:
                  nState = AWAITING_D22;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead;  //30th byte recvd
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D22:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D23;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)//31st byte recvd
                     *pch_sArrEnergRecs++ = *pOneByteRead;
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D23:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_D24;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!//32nd byte recvd
                     crc += *pOneByteRead;
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//9th Meter
                case AWAITING_D24:
                  nState = AWAITING_D25;     //(LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *pOneByteRead; //33rd byte recvd
                  crc += *pOneByteRead++;
                  break;
                case AWAITING_D25:
                  if(*pOneByteRead < 0x10){
                     nState = AWAITING_D26;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *pOneByteRead; //34th byte recvd
                     crc += *pOneByteRead++;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D26:
                  if(*pOneByteRead < 2){
                     nState = AWAITING_CRC;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *pOneByteRead;   //35th byte recvd
                     if(*pOneByteRead++ < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0; //= 0x7e41;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//CRC
                case AWAITING_CRC:
                //if(*OneByteRead == CalcCRC() ){ //Actually, the crc's been calculated as each character was read...
                  if((char)*pOneByteRead++ == crc){
                     nState = AWAITINGSTOP;  //(CRC OK).    //36th byte recvd
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//Stop
                case AWAITINGSTOP:
                  if(*pOneByteRead++ == 0xaa){
                     nState = AWAITINGSPC1;  //(Stop byte OK).    //37th byte recvd
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//1stZero  //Jerzy's SendPCRec actually sends a zero to start the Rec and a zero at end of Rec. As this is a continuous stream of data, this receiving code can view
           // both zeros as being at the end. If Jerzy removed these zeros, this code could have the following two case statements removed too.
                case AWAITINGSPC1:
                  if(*pOneByteRead++ == 0){
                     nState = AWAITINGSPC2;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!    //38th byte recvd
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//2ndZero
                case AWAITINGSPC2:
                  if(*pOneByteRead++ == 0){
                     nState = AWAITINGSTART;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     if(++nRecvingRecNo == NOOF_ENERGYRECS)//The table is full. Stop Incrementing the record numbers and pointers. Leave nGlobalLastUSBrecNoRecvd set at last record fully received,
                          // and let pch_sArrEnergRecs keep overwriting the current line.  Don't show a messagebox here - the thread shouldn't stop.
                       pch_sArrEnergRecs = (char*)&sArrEnergRecs[--nRecvingRecNo];    //39th byte recvd
                     else {//The table is not full



//The non-indented code her must go... It is just for collecting data from Picoscope, but arrowed lines must remain (and be uncommented)
#if 0
 short smallADC200buffer[2];
 adc200_get_single(smallADC200buffer);
 if( adc200_get_overflow(0) ){
    MakeWarningSound();
    MessageBox(NULL,"PICOSCOPE overflow > 10Volts!",NULL,MB_OK);
   }
//sArrEnergRecs[nRecvingRecNo-1].nEnergyRead[3] = smallADC200buffer[0]; //To show ADC code when Updating the Windows Pc screen
 *(short*)(pch_sArrEnergRecs - 16) = smallADC200buffer[0]; //To show ADC code when Updating the Windows Pc screen (equivalent to the above line)
 dlbPositionWP2red = nWavPlate2RealEnergyAr[ smallADC200buffer[0] ]; //dlbPosition is Now Real Voltage, or Real Laser Energy, owing to the calibration lookup
#endif
                        nGlobalLastUSBrecNoRecvd++; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//int VERYWRONG; //next line should be - 16 NOT - 18

/*
                        dlbPositionWP2red = nWavPlate2RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 16) ]; //  <<<<<<<<<<<<<<<<<<<<<<< //-18 is WP1 red
                             //dlbPositionWP2red is Now Real Laser Energy, owing to the calibration lookup <<<<<<<<<<<<<<<<<<<<<<<
                             // the index "pch_sArrEnergRecs - 16" points to the ADC reading for channel 0 just received from the Altera
                        dlbPositionWP3red = nWavPlate3RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 14) ];
                        dlbPositionWP2gre = nWavPlate14RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 10) ];//-12 is WP1 gre
                        dlbPositionWP3gre = nWavPlate15RealEnergyAr[ *(short*)(pch_sArrEnergRecs -  8) ];
                        dlbPositionWP2blu = nWavPlate26RealEnergyAr[ *(short*)(pch_sArrEnergRecs -  4) ];//-6 is WP1 blu
                        dlbPositionWP3blu = nWavPlate27RealEnergyAr[ *(short*)(pch_sArrEnergRecs -  2) ];
*/
 //The Energy Record From the Energy meters, after the coords, consist of:
 // firstly, the three Laser Energy fields (these are not acted on... They should just be recorded in a file)
 // secondly, three Object Energy fields,
 // and, lastly, three Reference Energy fields
 //Here, we must extract the Object Energy fields...
                       
                        dlbPositionWP2red = nWavPlate2RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 12) ];
                        dlbPositionWP2gre = nWavPlate14RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 10) ];
                        dlbPositionWP2blu = nWavPlate26RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 8) ];
 //... then extract the Reference Energy fields...
                        dlbPositionWP3red = nWavPlate3RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 6) ];
                        dlbPositionWP3gre = nWavPlate15RealEnergyAr[ *(short*)(pch_sArrEnergRecs -  4) ];
                        dlbPositionWP3blu = nWavPlate27RealEnergyAr[ *(short*)(pch_sArrEnergRecs -  2) ];

//Note pch_sArrEnergRecs, after getting the coordinates, picked up just the readings from the meters (i.e. not the 3rd bytes that say how many pulses are received).

                        usXcoordFromEMrec = *(unsigned short*)(pch_sArrEnergRecs - 22);
                        usYcoordFromEMrec = *(unsigned short*)(pch_sArrEnergRecs - 20);
int I_DONT_LIKE_THIS_INEFFICIENCY___OK_NOW; //(calculating the following value each time an Energy Record Arrives)     - I have now put this right by increasing HCFrec size and putting nLeadInForM39 in there:
//short n LeadInForM39_2 = (short)((HCFrec.nLengthOfM39rowsWhenGenerated_inPixels + 1 - HCFrec.uiNoofCols) / 2);
//                      usXcoordFromEMrec -= n LeadInForM39_2;
                      //usXcoordFromEMrec -= HCFrec.nLeadInForM39; No! - It's better to generate set-positions for lead-in, lead-out (in batch mode) and not make confusing adjustments to Altera's coords.
                        if(usYcoordFromEMrec){//(Remember, if it equals zero, here, the print hasn't started).
                           usYcoordFromEMrec--;
                           //We are now using the True Ycoord (i.e. have compensated for Altera adding one to it). The X-coord may indicate we are past the End of Line, or it may be -ve*** (which
                           // is probably the same thing due to overflow)... Either way, we should move the set-point on to the start of the next line.  If printer is stopped for long enough at
                           // the end of a line, even the overflow will overflow and become valid again... There's not much I can do about that & the PIDs will probably end up following the current
                           // line again.
                           //Although the above happens at the end of every line, it should be most common that the coords are valid bcause most time it spent printing lines.

                           //***Not any more... I've made X-coord unsigned. Now The X-coord is either valid or too big.

                           //if(usXcoordFromEMrec < 0)
                           //  usXcoordFromEMrec = 0;
                           //else if(usXcoordFromEMrec >= (int)HCFrec.uiNoofCols){
                           //   usXcoordFromEMrec = 0;
                           //   usYcoordFromEMrec++;
                           //  }
                           if( usXcoordFromEMrec > HCFrec.nLengthOfM39rowsWhenGenerated_inPixels ){ //If Xcoord is past end of line, keep it at the end of the line
                              usXcoordFromEMrec = (unsigned short)HCFrec.nLengthOfM39rowsWhenGenerated_inPixels;
                            //usYcoordFromEMrec++; //Y is being incremented on to the next line only when X says we're past the end of this line.
                              //No!- This is a confusing adjustment to Altera's coords. If we want the PID motor to be put in the correct position for the start the next line, these positions
                             }// should be put in the Desired Energies table in batch mode when we're loading the tables.
                         //else if (usXcoordFromEMrec < 0)
                         //  usXcoordFromEMrec = 0;
   //Depending on whether or not the control box increments the y coord at the end of the last line, the following message may never be displayed.
   //If it isn't displayed, the condition should be removed.
                           if(usYcoordFromEMrec >= HCFrec.nNoofLinesIncludingBetweemTheBeams){//The line above (usYcoordFromEMrec++) could put usYcoordFromEMrec out of bounds
                                                                                              // if print has 2000 lines or if Altera supplied an excessive number.
                              //MessageBox(NULL, "\t\tPrint Aborted!!\n\nAltera supplied a negative Y-coordinate!","Exiting Thread. Holoprog needs restarting",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
                              char szmes[117];
                            //sprintf_s(szmes,110,"Adjusted Coordinates from Altera (%d , %d) indicate we are past end of print. Coordinates (0,0) will be used", usXcoordFromEMrec, usYcoordFromEMrec);
                              sprintf_s(szmes,117,"Y Coordinate from Altera (%d) says we are past end of print. Coordinates (0,0) will be used", usYcoordFromEMrec);
                              HDC hDC = GetDC(hGLWnd); //Update Screen with what was sent and received
                              TextOut(hDC,140,1096,szmes,lstrlen(szmes));
                            //TextOut(hDC,100,1056,"Y Coordinate from Altera (%d) says we are past end of print. Coordinates (0,0) will be used",87);
                              ReleaseDC(hGLWnd,hDC);
                              usXcoordFromEMrec = 0; //This should hold the PIDs at the first pixel on the first line, while the Ycoord From Altera shows the print hasn't started.
                              usYcoordFromEMrec = 0; //(and also if Altera supplied a negative Y-coordinate)
                             }
                          }
                        else{//The Y coord Altera supplied was zero.
                           //if(usYcoordFromEMrec != -1){//Altera supplied a -ve number. Perhaps this is sufficient reason to abort the print?
                           //   //MessageBox(NULL, "\t\tPrint Aborted!!\n\nAltera supplied a negative Y-coordinate!","Exiting Thread. Holoprog needs restarting",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
                           //   HDC hDC = GetDC(hGLWnd); //Update Screen with what was sent and received
                           //   TextOut(hDC,300,1076,"Print Went wrong!! Altera supplied a negative Y-coordinate",58);
                           //   ReleaseDC(hGLWnd,hDC);
                           //  }
                           usXcoordFromEMrec = 0; //This should hold the PIDs at the first pixel on the first line, while the Ycoord From Altera shows the print hasn't started.
                           //usYcoordFromEMrec = 0; //(and also if Altera supplied a negative Y-coordinate)
                          }

//                      dblErrorWP2red =   dlbPositionWP2red - SetPointWP2red;
                        dblErrorWP2red =   dlbPositionWP2red - dblArDesiredEnergyWP2r[usYcoordFromEMrec][usXcoordFromEMrec]; //Note Array's subscripts are [Rows][Cols]
                        dblErrorWP2redSumdAndAveraged += dblErrorWP2red;
                      //dblErrorWP3red =   dlbPositionWP3red - SetPointWP3red;
                        dblErrorWP3red =   dlbPositionWP3red - dblArDesiredEnergyWP3r[usYcoordFromEMrec][usXcoordFromEMrec];
                        dblErrorWP3redSumdAndAveraged += dblErrorWP3red;

//                      dblErrorWP2gre =   dlbPositionWP2gre - SetPointWP2gre;
                        dblErrorWP2gre =   dlbPositionWP2gre - dblArDesiredEnergyWP2g[usYcoordFromEMrec][usXcoordFromEMrec];
                        dblErrorWP2greSumdAndAveraged += dblErrorWP2gre;
// dblHISTORYErrorWP2greSumdAndAveraged[nHIST++]= dblErrorWP2greSumdAndAveraged;
                      //dblErrorWP3gre =   dlbPositionWP3gre - SetPointWP3gre;
                        dblErrorWP3gre =   dlbPositionWP3gre - dblArDesiredEnergyWP3g[usYcoordFromEMrec][usXcoordFromEMrec];
                        dblErrorWP3greSumdAndAveraged += dblErrorWP3gre;

//                      dblErrorWP2blu =   dlbPositionWP2blu - SetPointWP2blu;
                        dblErrorWP2blu =   dlbPositionWP2blu - dblArDesiredEnergyWP2b[usYcoordFromEMrec][usXcoordFromEMrec];
                        dblErrorWP2bluSumdAndAveraged += dblErrorWP2blu;
                      //dblErrorWP3blu =   dlbPositionWP3blu - SetPointWP3blu;
                        dblErrorWP3blu =   dlbPositionWP3blu - dblArDesiredEnergyWP3b[usYcoordFromEMrec][usXcoordFromEMrec];
                        dblErrorWP3bluSumdAndAveraged += dblErrorWP3blu;

                        if( (!bRunningManually) && (++nIsFifthMessage >= nNOOFreadsToAVERAGE) ){
                         //dblErrorWP2redDisplay = (dblErrorWP2redARRAY[0] +  dblErrorWP2redARRAY[1] +  dblErrorWP2redARRAY[2] +  dblErrorWP2redARRAY[3] +  dblErrorWP2redARRAY[4]) / nNOOFreadsToAVERAGE;
                           dblErrorWP2redDisplay = dblErrorWP2redSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP3redDisplay = dblErrorWP3redSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP2greDisplay = dblErrorWP2greSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP3greDisplay = dblErrorWP3greSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP2bluDisplay = dblErrorWP2bluSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP3bluDisplay = dblErrorWP3bluSumdAndAveraged / nNOOFreadsToAVERAGE;
                           dblErrorWP2redSumdAndAveraged = 0;
                           dblErrorWP3redSumdAndAveraged = 0;
                           dblErrorWP2greSumdAndAveraged = 0;
                           dblErrorWP3greSumdAndAveraged = 0;
                           dblErrorWP2bluSumdAndAveraged = 0;
                           dblErrorWP3bluSumdAndAveraged = 0;
                           nIsFifthMessage = 0;
                           if(bFirstTime){
                              PidStructVariableWP2Red.dState = dblErrorWP2redDisplay; //dlbPositionWP2red;
                              PidStructVariableWP2Red.iState = 0;
                              PidStructVariableWP3Red.dState = dblErrorWP3redDisplay; //dlbPositionWP3red;
                              PidStructVariableWP3Red.iState = 0;
                              PidStructVariableWP2Gre.dState = dblErrorWP2greDisplay; //dlbPositionWP2gre;
                              PidStructVariableWP2Gre.iState = 0;
                              PidStructVariableWP3Gre.dState = dblErrorWP3greDisplay; //dlbPositionWP3gre;
                              PidStructVariableWP3Gre.iState = 0;
                              PidStructVariableWP2Blu.dState = dblErrorWP2bluDisplay; //dlbPositionWP2blu;
                              PidStructVariableWP2Blu.iState = 0;
                              PidStructVariableWP3Blu.dState = dblErrorWP3bluDisplay; //dlbPositionWP3blu;
                              PidStructVariableWP3Blu.iState = 0;

                              PidOutputToActOn[0] = UpdatePID(&PidStructVariableWP2Red, dblErrorWP2redDisplay, dlbPositionWP2red);
                              PidOutputToActOn[1] = UpdatePID(&PidStructVariableWP3Red, dblErrorWP3redDisplay, dlbPositionWP3red);
                              PidOutputToActOn[2] = UpdatePID(&PidStructVariableWP2Gre, dblErrorWP2greDisplay, dlbPositionWP2gre);
                              PidOutputToActOn[3] = UpdatePID(&PidStructVariableWP3Gre, dblErrorWP3greDisplay, dlbPositionWP3gre);
                              PidOutputToActOn[4] = UpdatePID(&PidStructVariableWP2Blu, dblErrorWP2bluDisplay, dlbPositionWP2blu);
                              PidOutputToActOn[5] = UpdatePID(&PidStructVariableWP3Blu, dblErrorWP3bluDisplay, dlbPositionWP3blu);

                              bFirstTime = 0;
                             }
                           else{
                              PidOutputToActOn[0] = UpdatePID(&PidStructVariableWP2Red, dblErrorWP2redDisplay, dlbPositionWP2red);
                              PidOutputToActOn[1] = UpdatePID(&PidStructVariableWP3Red, dblErrorWP3redDisplay, dlbPositionWP3red);
                              PidOutputToActOn[2] = UpdatePID(&PidStructVariableWP2Gre, dblErrorWP2greDisplay, dlbPositionWP2gre);
                              PidOutputToActOn[3] = UpdatePID(&PidStructVariableWP3Gre, dblErrorWP3greDisplay, dlbPositionWP3gre);
                              PidOutputToActOn[4] = UpdatePID(&PidStructVariableWP2Blu, dblErrorWP2bluDisplay, dlbPositionWP2blu);
                              PidOutputToActOn[5] = UpdatePID(&PidStructVariableWP3Blu, dblErrorWP3bluDisplay, dlbPositionWP3blu);
                              if(!bStopPIDsendingMessages)
                                SendMessage(hGLWnd, WM_COMMAND, IDM_MM_PID_MOVE, 0); //Really, message is an OTF2 message containing direction and speed (expressed as a TC)
                              /*else{
 //S endMessagePackets(NULL,1, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(NULL,2, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(NULL,13, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(NULL,14, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(NULL,25, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 //S endMessagePackets(NULL,26, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
                                }*/
                             }
                           //dblErrorWP2redARRAY[0] = dblErrorWP2redARRAY[1] = dblErrorWP2redARRAY[2] = dblErrorWP2redARRAY[3] = dblErrorWP2redARRAY[4] =  0;
                           if(bDisplayPIDdebugMsgs){
                              HDC hDC = GetDC(hGLWnd); //Update Screen with what was sent and received
                              HGDIOBJ oldFont = SelectObject(hDC,(HFONT)newFont);
                              char szDbMes[185];
                              //int recX = 128;
                              //int recY = 991;
                              //int wid = 675 + recX;
                              //int hit = 101 + recY;
                              //Rectangle(hDC,recX,recY,wid,hit);
                            //Rectangle(hDC,128,991,633,1092);//Don't amend without seraching for 128,991,633,1092
                              Rectangle(hDC, PID_RECT_L, PID_RECT_R, PID_RECT_T, PID_RECT_B);
                              sprintf_s(szDbMes,185,"X= %d, Y= %d,   M2-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f        ", usXcoordFromEMrec, usYcoordFromEMrec, 
                                  dblArDesiredEnergyWP2r[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP2red, dblErrorWP2redDisplay, PidOutputToActOn[0],  PidStructVariableWP2Red.iState);
                              TextOut(hDC,135,994,szDbMes,lstrlen(szDbMes));

                              sprintf_s(szDbMes,185,"M3-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f       ",
                                  dblArDesiredEnergyWP3r[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP3red, dblErrorWP3redDisplay, PidOutputToActOn[1], PidStructVariableWP3Red.iState);
                              TextOut(hDC,197,1010,szDbMes,lstrlen(szDbMes));
                         
                              sprintf_s(szDbMes,185,"M14-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f        ",
                                  dblArDesiredEnergyWP2g[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP2gre, dblErrorWP2greDisplay, PidOutputToActOn[2],  PidStructVariableWP2Gre.iState);
                              TextOut(hDC,191,1026,szDbMes,lstrlen(szDbMes));

                              sprintf_s(szDbMes,185,"M15-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f       ",
                                  dblArDesiredEnergyWP3g[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP3gre, dblErrorWP3greDisplay, PidOutputToActOn[3], PidStructVariableWP3Gre.iState);
                              TextOut(hDC,191,1042,szDbMes,lstrlen(szDbMes));

                              sprintf_s(szDbMes,185,"M26-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f        ",
                                  dblArDesiredEnergyWP2b[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP2blu, dblErrorWP2bluDisplay, PidOutputToActOn[4],  PidStructVariableWP2Blu.iState);
                              TextOut(hDC,191,1058,szDbMes,lstrlen(szDbMes));

                              sprintf_s(szDbMes,185,"M27-> Set=%8.2f  Act=%8.2f  Err=%8.2f  PidOut=%8.2f  iState=%10.5f       ",
                                  dblArDesiredEnergyWP3b[usYcoordFromEMrec][usXcoordFromEMrec], dlbPositionWP3blu, dblErrorWP3bluDisplay, PidOutputToActOn[5], PidStructVariableWP3Blu.iState);
                              TextOut(hDC,191,1074,szDbMes,lstrlen(szDbMes));

                              SelectObject(hDC,(HFONT)oldFont);
                              ReleaseDC(hGLWnd,hDC);
                             }
                          }
                       }//The table is not full
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
               }
         } //while (ntr-- > 0)
      } //else (ntr)
#if 0 //ifndef B_ALLOW_USBCOMMS_ERRORS
       if(nIsDataBeingCollected++ == 600){
          MessageBox(NULL,GetStringRes(STR_IS_ALTERA_EM_PROCESSOR_RUNNING), "USB device @2 found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA (energy meters) processor running?"
          nThreadErrorCode = 6;
         }
       if(nNoofConsecutiveNonStartChars > 800){
          MessageBox(NULL,"nNoofConsecutiveNonStartChars > 800!\n\nYou can still interact with the main program but the USB thread\nis not receiving good data from the energy meters!\n\nEnsure 'Load VCP' is not checked in the Advanced Tab of\n'USB Serial Converter A' (in Device Manager)", NULL,MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
          nNoofConsecutiveNonStartChars = 0;
         }
#endif
   }//while the thread is running
 //MessageBox(NULL,"USB Comm Thread Exiting!",NULL,MB_OK); 
 return 0;// comUSB_thrstop=0;    //** End of thread
}

void StopPIDMovementWhileHomingOrFileTransfering(void){
 //bPIDMovementStoppedForHoming = TRUE;
 bStopPIDsendingMessages = TRUE;
 /*
 M akeWarningSound();
 S leep(200);
 M akeErrorSound();
 S leep(200);
 M akeAcknowledgeSound();
 S leep(200);
 S endMessagePackets(hDlg,1, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 S leep(5);
 S endMessagePackets(hDlg,2, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 S leep(5);
 S endMessagePackets(hDlg,13, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 S leep(5);
 S endMessagePackets(hDlg,14, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 S leep(5);
 S endMessagePackets(hDlg,25, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 S leep(5);
 S endMessagePackets(hDlg,26, MsTypB_SING_MM_PID,FORWARD_DIR,60001 );//This stops the motor from moving. (Interrupts will still occur, but will be non-pulsing)
 */
//The problem with the above code is that the PID controllers (AVRs) don't *always* stop moving. This is possibly (probably?) due to the Status Request messages
// being sent 'at about the same time' from the main thread, as the result of a timer event.  To overcome this scheduling problem, Send this message to the main 
// thread to get it to send the MsTypB_SING_MM_PID messages.
 SendMessage(hGLWnd, WM_COMMAND, IDM_MM_PID_STOPMOVES, 0); 
//The above hypothesis is incorrect! Further testing showed that after the above messages to stop moving were sent and (acted on) subsequent messages to move
// the PIDs (IDM_MM_PID_MOVE) were sent.  This suggests EITHER that bStopPIDsendingMessages was not seen as TRUE within the USB comms thread (see 
// where the following 2 lines occur above:)

//  if(!bStopPIDsendingMessages)
//    SendMessage(hGLWnd, WM_COMMAND, IDM_MM_PID_MOVE, 0); //Really, message is an OTF2 message containing direction and speed (expressed as a TC)

//OR that somehow the message sent still occasionally got through late - perhaps as a result of the USB thread yielding to the main thread after
// bStopPIDsendingMessages was inspected, but before the "SendMessage(hGLWnd, WM_COMMAND, IDM_MM_PID_MOVE, 0);" got processed.
// The solution appears to be to put another "if(!bStopPIDsendingMessages)" test in WndProc surrounding the code that handles "case IDM_MM_PID_MOVE:"
}

void restartPIDs(void){
 PidStructVariableWP2Red.iState = 0;
 PidStructVariableWP3Red.iState = 0;
 PidStructVariableWP2Gre.iState = 0;
 PidStructVariableWP3Gre.iState = 0;
 PidStructVariableWP2Blu.iState = 0;
 PidStructVariableWP3Blu.iState = 0;
 for(int x = 0; x <16; x++){
    PidStructVariableWP2Red.Tab[x] = 0;
    PidStructVariableWP3Red.Tab[x] = 0;
    PidStructVariableWP2Gre.Tab[x] = 0;
    PidStructVariableWP3Gre.Tab[x] = 0;
    PidStructVariableWP2Blu.Tab[x] = 0;
    PidStructVariableWP3Blu.Tab[x] = 0;
   }

 nIsFifthMessage = 0;
 //dblErrorWP2redARRAY[0] = dblErrorWP2redARRAY[1] = dblErrorWP2redARRAY[2] = dblErrorWP2redARRAY[3] = dblErrorWP2redARRAY[4] =  0;
 dblErrorWP2redSumdAndAveraged = 0;
 dblErrorWP3redSumdAndAveraged = 0;
 dblErrorWP2greSumdAndAveraged = 0;
 dblErrorWP3greSumdAndAveraged = 0;
 dblErrorWP2bluSumdAndAveraged = 0;
 dblErrorWP3bluSumdAndAveraged = 0;

 bStopPIDsendingMessages = FALSE; //Here [i.e. in restartPIDs()] is the only place bStopPIDsendingMessages is set FALSE, apart from in WM_CREATE

 bFirstTime = 1;
 //bPIDMovementStoppedForHoming = FALSE;
}

//**************************************************************************
//**    Initialise. Called once at the start of the program
//      port A is for the Energy meters, port B for the Control Box
//      port A remains open for the duration of the program, but port B may be opened and closed many times to allow c# program access through VCP
//int comUSB_Init(const char *port, int bufsize, HWND hWnd){
int comUSB_Init(const char *portA, /*const char *portB,*/ HWND hWnd){
 int comUSBInit_RetCodePA/*, comUSBInit_RetCodePB*/;
 FT_STATUS ftStatus;
 char *BufPtrs[6];//pointer to array of 4 pointers
 char Buffer1[64];//buffer for product description of first device found
 char Buffer2[64];//buffer for product description of second device
 char Buffer3[64];
 char Buffer4[64];
 char Buffer5[64];
 DWORD numDevs;
 char szLocalmes[SIZMES];
 if(ftHandleEM!=0)  //** Validate
   return 0;        // If already opened, ignore the call
 if(LoadDLL(hWnd))
   return -1;//no drivers installed
//** Open the comms ports
       //This sample shows how to get the product descriptions of all the devices currently connected.
       //initialize the array of pointers
       BufPtrs[0] = Buffer1;
       BufPtrs[1] = Buffer2;
       BufPtrs[2] = Buffer3;
       BufPtrs[3] = Buffer4;
       BufPtrs[4] = Buffer5;
       BufPtrs[5] = NULL;//last entry should be NULL
     //ftStatus   = DLLf.m_pListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION); //If one board is present, This fills buffers with "USB mj A" and "USB mj B"
       ftStatus   = DLLf.m_pListDevices(BufPtrs,&numDevs,FT_LIST_ALL); //If one board is present, This fills buffers with "USB mj A" and "USB mj B"
     //ftStatus   = DLLf.m_pListDevices(&numDevs,NULL,FT_LIST_NUMBER_ONLY);                   //This sets numDevs to 2 if one board is present
     //ftStatus   = DLLf.m_pListDevices(&numDevs,&numDevs,FT_LIST_NUMBER_ONLY);

       if (FT_OK != ftStatus){ //if FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2,and numDevs contains the number of devices connected
#ifdef B_ALLOW_USBCOMMS_ERRORS
          MessageBox(hWnd,"You will be unable to take readings over the USB.\n\nThis version of the program allows you to continue despite this.","Altera not found on USB port",MB_OK);
#else
        //MessageBox(hWnd,"Couldn't find any FTDI USB devices on the USB ports.\nPlease check the Alteras are plugged in and powered up.","FT_ListDevices failed",MB_OK);
          MessageBox(hWnd,"This may be because a wrong VCP port is already opened?","FT_ListDevices failed",MB_OK);
#endif
        //return -1;
         }
       if(!lstrcmp(portA,"FIRSTDEVICE") )
         comUSBInit_RetCodePA = DLLf.m_pOpenEx(BufPtrs[0],FT_OPEN_BY_DESCRIPTION,&ftHandleEM);
       else
       //comUSBInit_RetCodePA = DLLf.m_pOpenEx((PVOID)portA,FT_OPEN_BY_DESCRIPTION,&ftHandleEM);//port defaults to "Dual RS232 A",
         comUSBInit_RetCodePA = DLLf.m_pOpenEx((PVOID)"USB mj B",FT_OPEN_BY_DESCRIPTION,&ftHandleEM);//port defaults to "Dual RS232 A", < - this now works
       //comUSBInit_RetCodePA = DLLf.m_pOpenEx("FTT3R5ZBB",FT_OPEN_BY_SERIAL_NUMBER,&ftHandleEM);//port defaults to "Dual RS232 A",
       //comUSBInit_RetCodePA = DLLf.m_pOpenEx((PVOID)portA,FT_OPEN_BY_SERIAL_NUMBER,&ftHandleEM);//port defaults to "Dual RS232 A",
       //comUSBInit_RetCodePA = DLLf.m_pOpenEx((PVOID)"mjT3R2UA",FT_OPEN_BY_SERIAL_NUMBER,&ftHandleEM);//port defaults to "Dual RS232 A",
       if(FT_OK != comUSBInit_RetCodePA){                                                   //  but can be changed by user
          StringCchCopy(szLocalmes,SIZMES,"Couldnt open USB port - USBportToOpenForEnergyMeters\nCheck the registry contains the correct FTDI Serial Number for\nthe device");
        //StringCchCat(szLocalmes,SIZMES,portA);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pOpenEx failed",MB_OK);
          return -1;
         }
       else
         ;//MessageBox(hWnd,"This Version opens the first enumerated device!","Not Hardcoded",MB_OK);
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
       if(!lstrcmp(portB,"SECONDDEVICE") )
         comUSBInit_RetCodePB = DLLf.m_pOpenEx(BufPtrs[1],FT_OPEN_BY_DESCRIPTION,&ftHandleCB);
       else
         comUSBInit_RetCodePB = DLLf.m_pOpenEx((PVOID)portB,FT_OPEN_BY_DESCRIPTION,&ftHandleCB);//port defaults to "Dual RS232 A",
       if(FT_OK != comUSBInit_RetCodePB){                                                   //  but can be changed by user
          StringCchCopy(szLocalmes,SIZMES,"Couldnt open USB portB: ");
          StringCchCat(szLocalmes,SIZMES,portB);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pOpenEx failed",MB_OK);
          return -1;
         }
       else
         ;//MessageBox(hWnd,"This Version opens the first enumerated device!","Not Hardcoded",MB_OK);
*/
     //comUSBInit_RetCodePA = DLLf.m_pSetBaudRate( ftHandleEM,230400);
       comUSBInit_RetCodePA = DLLf.m_pSetBaudRate( ftHandleEM,115200); //change baudrate in message below, if changing this
       if(FT_OK != comUSBInit_RetCodePA){
          StringCchCopy(szLocalmes,SIZMES,"Baudrate NOT set to 115200!!! portA: ");
          StringCchCat(szLocalmes,SIZMES,portA);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetBaudRate failed",MB_OK);
          return -1;
         }
//     else
//       MessageBox(hWnd,"Baudrate set to 230400","Message",MB_OK);
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
       comUSBInit_RetCodePB = DLLf.m_pSetBaudRate( ftHandleCB,230400);
       if(FT_OK != comUSBInit_RetCodePB){
          StringCchCopy(szLocalmes,SIZMES,"Baudrate NOT set to 230400!!! portB: ");
          StringCchCat(szLocalmes,SIZMES,portB);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetBaudRate failed",MB_OK);
          return -1;
         }
//     else
//       MessageBox(hWnd,"Baudrate set to 230400","Message",MB_OK);
*/
       comUSBInit_RetCodePA = DLLf.m_pSetFlowControl( ftHandleEM,FT_FLOW_NONE, NULL, NULL);
       if(FT_OK != comUSBInit_RetCodePA){
          StringCchCopy(szLocalmes,SIZMES,"FlowControl NOT set! portA: ");
          StringCchCat(szLocalmes,SIZMES,portA);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetFlowControl failed",MB_OK);
          return -1;
         }
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
       comUSBInit_RetCodePB = DLLf.m_pSetFlowControl( ftHandleCB,FT_FLOW_NONE, NULL, NULL);
       if(FT_OK != comUSBInit_RetCodePB){
          StringCchCopy(szLocalmes,SIZMES,"FlowControl NOT set! portB: ");
          StringCchCat(szLocalmes,SIZMES,portB);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetFlowControl failed",MB_OK);
          return -1;
         }
*/
       comUSBInit_RetCodePA = DLLf.m_pSetDataCharacteristics( ftHandleEM,FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
       if(FT_OK != comUSBInit_RetCodePA){
          StringCchCopy(szLocalmes,SIZMES,"DataCharacteristics NOT set! portA: ");
          StringCchCat(szLocalmes,SIZMES,portA);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetDataCharacteristics failed",MB_OK);
          return -1;
         }
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
       comUSBInit_RetCodePB = DLLf.m_pSetDataCharacteristics( ftHandleCB,FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
       if(FT_OK != comUSBInit_RetCodePB){
          StringCchCopy(szLocalmes,SIZMES,"DataCharacteristics NOT set! portB: ");
          StringCchCat(szLocalmes,SIZMES,portB);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetDataCharacteristics failed",MB_OK);
          return -1;
         }
*/
do{
   comUSBInit_RetCodePA = DLLf.m_pStopInTask(ftHandleEM);
   if(comUSBInit_RetCodePA != FT_OK){
      MessageBox(NULL,"comUSBInit_RetCodePA != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
      nThreadErrorCode = 7;//Not strictly a Thread error, but we might as well see it. If its always there, delete this line
     }
  } while (comUSBInit_RetCodePA != FT_OK);
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
do{
   comUSBInit_RetCodePB = DLLf.m_pStopInTask(ftHandleCB);
   if(comUSBInit_RetCodePB != FT_OK){
      MessageBox(NULL,"comUSBInit_RetCodePB != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
      nThreadErrorCode = 7;//Not strictly a Thread error, but we might as well see it. If its always there, delete this line
     }
  } while (comUSBInit_RetCodePB != FT_OK);
*/

       comUSBInit_RetCodePA = DLLf.m_pPurge(ftHandleEM, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCodePA){
          MessageBox(hWnd,"Couldnt Purge Buffers after opening USB portA","DLLf.m_pPurge failed",MB_OK);
          return -5;
         }
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
       comUSBInit_RetCodePB = DLLf.m_pPurge(ftHandleCB, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCodePB){
          MessageBox(hWnd,"Couldnt Purge Buffers after opening USB portB","DLLf.m_pPurge failed",MB_OK);
          return -5;
         }
*/

//int cnt = 0;
do{
   comUSBInit_RetCodePA = DLLf.m_pRestartInTask(ftHandleEM);
   // cnt++;
  } while (comUSBInit_RetCodePA != FT_OK);

       comUSBInit_RetCodePA = DLLf.m_pResetDevice(ftHandleEM);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCodePA){
          MessageBox(hWnd,"Couldnt ResetDevice after opening USB portA","DLLf.m_pResetDevice failed",MB_OK);
          return -6;
         }
//if(cnt > 1) Its actually about 1400!
  // MessageBox(NULL,"count >1 for RestartInTask",NULL,MB_OK);
       comUSBInit_RetCodePA = DLLf.m_pSetTimeouts(ftHandleEM, 1000, 1000);
       if(FT_OK != comUSBInit_RetCodePA){
          MessageBox(hWnd,"Couldnt SetTimeouts after opening USB portA","DLLf.m_pSetTimeouts failed",MB_OK);
          return -7;
         }
/*
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
do{
   comUSBInit_RetCodePB = DLLf.m_pRestartInTask(ftHandleCB);
   // cnt++;
  } while (comUSBInit_RetCodePB != FT_OK);

       comUSBInit_RetCodePB = DLLf.m_pResetDevice(ftHandleCB);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCodePB){
          MessageBox(hWnd,"Couldnt ResetDevice after opening USB portB","DLLf.m_pResetDevice failed",MB_OK);
          return -6;
         }
//if(cnt > 1) Its actually about 1400!
  // MessageBox(NULL,"count >1 for RestartInTask",NULL,MB_OK);
       comUSBInit_RetCodePB = DLLf.m_pSetTimeouts(ftHandleCB, 1000, 1000);
       if(FT_OK != comUSBInit_RetCodePB){
          MessageBox(hWnd,"Couldnt SetTimeouts after opening USB portB","DLLf.m_pSetTimeouts failed",MB_OK);
          return -7;
         }
*/



 comUSB_thrstop=0;
 DWORD tid;                                       //** Open thread now

#if 1  //change this to 1 when the energy meters are connected
 HANDLE hThr=CreateThread(0,0,(LPTHREAD_START_ROUTINE)comUSB_Thr_EM,0,0,&tid);
  if( hThr==NULL ){
    char szDum[12];
    int ee=GetLastError();
    _itoa_s(ee, szDum, sizeof(szDum), 10);
    MessageBox(hWnd,"Create Thread() USB_EM failed, Code:",szDum,MB_OK);
    com_Exit();
    return -1; //This will cause WndProc to terminate the program
   }
#else //no energy meters are a connected... close the port
  comUSBInit_RetCodePA = DLLf.m_pClose(ftHandleEM);
  comUSBInit_RetCodePB = DLLf.m_pClose(ftHandleCB);
#endif
//HANDLE hThr=CreateThread(0,0,(LPTHREAD_START_ROUTINE)comUSB_Thr_CB,0,0,&tid);
  /* 
  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be collected from along with the energy records.
  hThr=CreateThread(0,0,(LPTHREAD_START_ROUTINE)comUSB_Thr_CB,0,0,&tid);
  if( hThr==NULL ){
    char szDum[12];
    int ee=GetLastError();
    _itoa_s(ee, szDum, sizeof(szDum), 10);
    MessageBox(hWnd,"Create Thread() USB_CB failed, Code:",szDum,MB_OK);
    com_Exit();
    return -1; //This will cause WndProc to terminate the program
   }
  */

 return 0;
}

//---------------------------------------------------------------------------
// FUNCTION:    GetStringRes (int id INPUT ONLY)
//
// COMMENTS:    Load the resource string with the ID given, and return a
//              pointer to it.  Notice that the buffer is common memory so
//              the string must be used before this call is made a second time.
//---------------------------------------------------------------------------
LPTSTR   GetStringRes (int id){
 static TCHAR TCHbuffer[MAX_PATH];
 TCHbuffer[0]=0;
 LoadString (GetModuleHandle (NULL), id, TCHbuffer, MAX_PATH);
 return TCHbuffer;
}
#include "stdafx.h"
#if 0

  This was for when I was trying to collect Coordinates from the Control Box ... an action that is unnecessary if the coordinates
  are to be present in the the energy records.

int comUSB_Exit_ControlBox(HWND);       //ProtoType Currently not called anywhere!!

extern HWND hGLWnd;

int comUSB_thrstopCB;                   //Stop USB thread for the CONTROL BOX

extern int nGlobalLastUSBrecNoRecvd;// = -1; This is an index into the table:
extern ENERGYREC sArrEnergRecs[]; // = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)

int nThread_CB_ErrorCode;
extern HMODULE m_hmodule;         //comUSB_Init
extern DLLFuncs    DLLf;
extern FT_HANDLE   ftHandleCB;

LPTSTR   GetStringRes_CB (int id); //Local Function Prototype

//The above functions all handle the UDP comms with all the motor controllers, the following functions handle the USB comms with the
// Control Box controller (as a second, seperate, thread).

//**   - The main running thread. This is called by the Init function, and
//        made to stop by the Exit function. User does not call this.
//     - Uses NON_OVERLAPPED mode with simple sleep delay for simplicity

DWORD WINAPI comUSB_Thr_CB(void){
 int nRecvingRecNo = 0; //We could kill and restart this thread for each Print (& perhaps need to if nRecvingRecNo is initialised here). See also nGlobalLastUSBrecNoRecvd
 DWORD ntr;
 DWORD nNumBytesRead;
 int   comUSB_ThRetCodeCB;
 BYTE  *pOneByteRead;
 BYTE  NewBuff[32000];
 BYTE  byteDuffBuff[_MAX_DIR];

 //PidStructVariableWP3Blu.pGain = PidStructVariableWP2Red.pGain;
 //PidStructVariableWP3Blu.dGain = PidStructVariableWP2Red.dGain;
 //PidStructVariableWP3Blu.iGain = PidStructVariableWP2Red.iGain;

 do{
    comUSB_ThRetCodeCB = DLLf.m_pStopInTask(ftHandleCB);
    if(comUSB_ThRetCodeCB != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCodeCB != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
       nThread_CB_ErrorCode = 1;
      }
   } while (comUSB_ThRetCodeCB != FT_OK);
 do{
    comUSB_ThRetCodeCB = DLLf.m_pPurge(ftHandleCB, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
    if(comUSB_ThRetCodeCB != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCodeCB != FT_OK Comm Thread (Purge)",NULL,MB_OK);
       nThread_CB_ErrorCode = 2;
      }
   } while (comUSB_ThRetCodeCB != FT_OK);
 int cnt = 0;
 do{
    comUSB_ThRetCodeCB = DLLf.m_pRestartInTask(ftHandleCB);
    cnt++;
   } while ((comUSB_ThRetCodeCB != FT_OK) && (cnt < 50010));
 if(cnt > 50000) //It's actually about 1400!
   MessageBox(NULL,"count >50000 for RestartInTask",NULL,MB_OK);
 int NoofBytesDiscarded = 0;
 int nIsDataBeingCollected = 0;
/**************************************************************************************************/
/*********************                                           **********************************/
/********************* INITIAL LOOP THAT THROWS AWAY A FEW BYTES **********************************/
/*********************                                           **********************************/
/**************************************************************************************************/
 while ( (comUSB_thrstopCB==0) && (ftHandleCB!=NULL) &&(m_hmodule!=NULL) && (NoofBytesDiscarded < 4000) ){

#ifndef B_ALLOW_USBCOMMS_ERRORS
    if(nIsDataBeingCollected++ > 60000000){ //was 1960
       MessageBox(NULL,GetStringRes_CB(STR_IS_ALTERA_CB_PROCESSOR_RUNNING), "USB device found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA (Control Box) processor running?"
       nThread_CB_ErrorCode = 3;
                                // NoofBytesDiscarded = 4000;
      }
#endif
//* Get nr of bytes in rx queue to read
    //int WARNINGweDontUSeFollowingRetCode;//What I mean is, "we don't inspect the return code" (also search below for WARNINGweDontUSeFollowingRetCodeEITHER)
    comUSB_ThRetCodeCB = DLLf.m_pGetQueueStatus(ftHandleCB, &ntr );//ntr was dwAmountInRxQueue - gives the Q size
    if (comUSB_ThRetCodeCB!=FT_OK){    //* Handle errors, if any{    //* Handle errors, if any
       MessageBox(NULL,"m_pGetQueueStatus: bad retcode in Comm Thread",NULL,MB_OK);
       nThread_CB_ErrorCode = 12;
       return 0;
      }
    if (ntr>0){ //* align for wraparound in circular buffer
       nIsDataBeingCollected = 0;
       if ( ntr > _MAX_DIR)
         ntr = _MAX_DIR;
       //* Read the bytes to our buffer
//ntr (number to receive) will have been set to less than what's in the Q, if the buffer didnt have enough room between
// com_bindx and the end of the buffer.  The stuff left in the Q will be read next time.
       comUSB_ThRetCodeCB = DLLf.m_pRead(ftHandleCB, byteDuffBuff, ntr, &nNumBytesRead);
       if (comUSB_ThRetCodeCB!=FT_OK){    //* Handle errors, if any
          MessageBox(NULL,"Communication Read Error in Comm Thread",NULL,MB_OK);
          nThread_CB_ErrorCode = 4;
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
 while ( !comUSB_thrstopCB && (ftHandleCB!=NULL) && (m_hmodule!=NULL) ){
    comUSB_ThRetCodeCB = DLLf.m_pGetQueueStatus(ftHandleCB, &ntr );//* Get noof bytes in rx queue available to read
    if (comUSB_ThRetCodeCB!=FT_OK){    //* Handle errors, if any{    //* Handle errors, if any
       MessageBox(NULL,"m_pGetQueueStatus: bad retcode in Comm Thread2",NULL,MB_OK);
       nThread_CB_ErrorCode = 13;
       return 0;
      }
    //HistoryOfNTR[hNTRidx++] = ntr;
    if(ntr > 32000){
       MessageBox(NULL, "\t\tCATASTROPHY!! (it's not really!)\n\nToo much data was available in ftdi buffers that collect data from the energy meters","Exiting Thread. Holoprog needs restarting",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
       nThread_CB_ErrorCode = 10;
       SetRedWarning();
       return 0;
      }
    comUSB_ThRetCodeCB = DLLf.m_pRead(ftHandleCB, NewBuff, ntr, &nNumBytesRead);
    if (comUSB_ThRetCodeCB!=FT_OK){    //Handle errors, if any
       MessageBox(NULL,"Communication Read Error1 in Comm Thread",NULL,MB_OK);
       nThread_CB_ErrorCode = 11;
       return 0;
      }
    pOneByteRead = &NewBuff[0];
    if(!ntr)           //If nothing was read from line, block thread to allow buffer to fill.
      Sleep(10);
    else{
       while (ntr-- > 0){
          //comUSB_ThRetCodeCB = DLLf.m_pRead(ftHandleCB, OneByteRead, 1, &nNumBytesRead);
          //if (comUSB_ThRetCodeCB!=FT_OK){    //Handle errors, if any
          //  MessageBox(NULL,"Communication Read Error1 in Comm Thread",NULL,MB_OK);  nThread_CB_ErrorCode = 5;   }
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
                     nState = AWAITINGSPC2;  //2nd byte
                    }
                  else{
                     nState = AWAITINGSTART;
                     continue; //Abandon this message, go on to 'while (ntr-- > 0)' and await the start of the next one
                    }
                  break;
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
#endif
                        nGlobalLastUSBrecNoRecvd++; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//int VERYWRONG; //next line should be - 16 NOT - 18
                        //dlbPositionWP2red = nWavPlate2RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 16) ]; //  <<<<<<<<<<<<<<<<<<<<<<< //-18 is WP1 red
                        //     //dlbPositionWP2red is Now Real Laser Energy, owing to the calibration lookup <<<<<<<<<<<<<<<<<<<<<<<
                        //     // the index "pch_sArrEnergRecs - 16" points to the ADC reading for channel 0 just received from the Altera
                        //dlbPositionWP3red = nWavPlate2RealEnergyAr[ *(short*)(pch_sArrEnergRecs - 14) ];
                            //dblErrorWP2redARRAY[0] = dblErrorWP2redARRAY[1] = dblErrorWP2redARRAY[2] = dblErrorWP2redARRAY[3] = dblErrorWP2redARRAY[4] =  0;
                       }//The table is not full
                    }
                  else{
                     nState = AWAITINGSTART;
                     //pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
               }
         } //while (ntr-- > 0)
      } //else (ntr)
#if 0 //ifndef B_ALLOW_USBCOMMS_ERRORS
       if(nIsDataBeingCollected++ == 600){
          MessageBox(NULL,GetStringRes_CB(STR_IS_ALTERA_CB_PROCESSOR_RUNNING), "USB device @2 found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA (Control Box) processor running?"
          nThread_CB_ErrorCode = 6;
         }
       if(nNoofConsecutiveNonStartChars > 800){
          MessageBox(NULL,"nNoofConsecutiveNonStartChars > 800!\n\nYou can still interact with the main program but the USB thread\nis not receiving good data from the energy meters!\n\nEnsure 'Load VCP' is not checked in the Advanced Tab of\n'USB Serial Converter A' (in Device Manager)", NULL,MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
          nNoofConsecutiveNonStartChars = 0;
         }
#endif
   }//while the thread is running
 //MessageBox(NULL,"USB Comm Thread Exiting!",NULL,MB_OK); 
 return 0;// comUSB_thrstopCB=0;    //** End of thread
}


#if 0

//**************************************************************************
//**    Initialise. Called once at the start of the program
//      ‘port’  is the wanted port number, e.g. 3  (now its "DLP-USB245M")
//      ‘bsize’ is the wanted buffer size, must be a power of 2, e.g. 32768
int comUSB_EM_Init(const char *port, HWND hWnd){
 int comUSBInit_RetCode;
 FT_STATUS ftStatus;
 char *BufPtrs[4];//pointer to array of 4 pointers
 char Buffer1[64];//buffer for product description of first device found
 char Buffer2[64];//buffer for product description of second device
 char Buffer3[64];
 DWORD numDevs;
 char szLocalmes[SIZMES];
 //TEMPTIMESthru = 0;
 //TEMPmaxInQQQue = 0;
 //nIndx2StopAt = 0;
 //com_bsize=bufsize;             // save buffersize, must be 2^n
 //com_bmask=bufsize-1;           // set mask e.g. 0x7FFF

//** Validate
 if(ftHandleCB!=0)
   return 0;    // If already opened, ignore the call

//** Allocate a dynamic byte buffer for storing incoming data
     /*
 if(com_bdata==NULL){//This added line (mj) ensures the memory allocation takes place only once during the program's
    com_bdata= new BYTE[com_bsize];//lifetime - even though com_Init & com_Exit maybe called many times
    if(com_bdata==NULL){
       wsprintf(Buffer1,"Couldnt allocate %d Bytes for storing incoming USB data",com_bsize);//REUSE of Buffer1
       MessageBox(hWnd,Buffer1,"Try restarting Windows?",MB_OK);//Insufficient memory
       com_Exit();
       return -3;
      }
   }
//** Allocate a dynamic byte buffer for storing incoming data
 if(com_dbdata==NULL){//This added line (mj) ensures the memory allocation takes place only once during the program's
    com_dbdata= new BYTE[BUFSIZE_4MEG];//lifetime - even though com_Init & com_Exit maybe called many times
    if(com_dbdata==NULL){
       wsprintf(Buffer1,"Couldnt allocate %d Bytes for storing incoming USB data",BUFSIZE_4MEG);//REUSE of Buffer1
       MessageBox(hWnd,Buffer1,"Try restarting Windows?",MB_OK);//Insufficient memory
       com_Exit();
       return -4;
      }
   } */

//pReading = (short*) (com_dbdata + n30SECS_BUFFERLENGTH);//Stick with this one, it allows the use of a negative index. When checking
//pReading = (short*) (com_dbdata);                     // this very first reading for the trigger condition, I stuff dummy data into
//last_dbd = 0;                                          // the previous (-ve) reading.
 //nchanLastReadingNo = 0;
 //pReadingFrom_dbdata = (short*) (com_dbdata);

 if(LoadDLL(hWnd))
   return -1;//no drivers installed

//FROM WM_CREATE
       //This sample shows how to get the product descriptions of all the devices currently connected.

       //initialize the array of pointers
       BufPtrs[0] = Buffer1;
       BufPtrs[1] = Buffer2;
       BufPtrs[2] = Buffer3;
       BufPtrs[3] = NULL;//last entry should be NULL
       ftStatus   = DLLf.m_pListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
       if (FT_SUCCESS(ftStatus)){
       //FT_ListDevices OK,product descriptions are in Buffer1 and Buffer2,and
       //numDevs contains the number of devices connected
          BufPtrs[2] = BufPtrs[2];
         }
       else {
#ifdef B_ALLOW_USBCOMMS_ERRORS
          MessageBox(hWnd,"You will be unable to take readings over the USB.\n\nThis version of the program allows you to continue despite this.","Altera not found on USB port",MB_OK);
#else
          MessageBox(hWnd,"Couldn't find any FTDI USB devices on the USB port.\nPlease check the Altera is plugged in and powered up.","FT_ListDevices failed",MB_OK);
#endif
          return -1;
         }
       if(!lstrcmp(port,"FIRSTDEVICE") )
         comUSBInit_RetCode = DLLf.m_pOpenEx(BufPtrs[0],FT_OPEN_BY_DESCRIPTION,&ftHandleCB);
       else
         comUSBInit_RetCode = DLLf.m_pOpenEx((PVOID)port,FT_OPEN_BY_DESCRIPTION,&ftHandleCB);//port defaults to "DLP-USB245M",
       if(FT_OK != comUSBInit_RetCode){                                                   //  but can be changed by user
          StringCchCopy(szLocalmes,SIZMES,"Couldnt open USB port: ");
          StringCchCat(szLocalmes,SIZMES,port);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pOpenEx failed",MB_OK);
          return -1;
         }
       else
         ;//MessageBox(hWnd,"This Version opens the first enumerated device!","Not Hardcoded",MB_OK);

       comUSBInit_RetCode = DLLf.m_pSetBaudRate( ftHandleCB,230400);
       if(FT_OK != comUSBInit_RetCode){
          StringCchCopy(szLocalmes,SIZMES,"Baudrate NOT set to 230400!!! port: ");
          StringCchCat(szLocalmes,SIZMES,port);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetBaudRate failed",MB_OK);
          return -1;
         }
//     else
//       MessageBox(hWnd,"Baudrate set to 230400","Message",MB_OK);
       comUSBInit_RetCode = DLLf.m_pSetFlowControl( ftHandleCB,FT_FLOW_NONE, NULL, NULL);
       if(FT_OK != comUSBInit_RetCode){
          StringCchCopy(szLocalmes,SIZMES,"FlowControl NOT set! port: ");
          StringCchCat(szLocalmes,SIZMES,port);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetFlowControl failed",MB_OK);
          return -1;
         }
       comUSBInit_RetCode = DLLf.m_pSetDataCharacteristics( ftHandleCB,FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
       if(FT_OK != comUSBInit_RetCode){
          StringCchCopy(szLocalmes,SIZMES,"DataCharacteristics NOT set! port: ");
          StringCchCat(szLocalmes,SIZMES,port);
          MessageBox(hWnd,szLocalmes,"DLLf.m_pSetDataCharacteristics failed",MB_OK);
          return -1;
         }
do{
   comUSBInit_RetCode = DLLf.m_pStopInTask(ftHandleCB);
   if(comUSBInit_RetCode != FT_OK){
      MessageBox(NULL,"comUSBInit_RetCode != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
      nThread_CB_ErrorCode = 7;//Not strictly a Thread error, but we might as well see it. If its always there, delete this line
     }
  } while (comUSBInit_RetCode != FT_OK);

       comUSBInit_RetCode = DLLf.m_pPurge(ftHandleCB, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt Purge Buffers after opening USB port","DLLf.m_pPurge failed",MB_OK);
          return -5;
         }

//int cnt = 0;
do{
   comUSBInit_RetCode = DLLf.m_pRestartInTask(ftHandleCB);
   // cnt++;
  } while (comUSBInit_RetCode != FT_OK);

       comUSBInit_RetCode = DLLf.m_pResetDevice(ftHandleCB);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt ResetDevice after opening USB port","DLLf.m_pResetDevice failed",MB_OK);
          return -6;
         }
//if(cnt > 1) Its actually about 1400!
  // MessageBox(NULL,"count >1 for RestartInTask",NULL,MB_OK);
       comUSBInit_RetCode = DLLf.m_pSetTimeouts(ftHandleCB, 1000, 1000);
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt SetTimeouts after opening USB port","DLLf.m_pSetTimeouts failed",MB_OK);
          return -7;
         }

 comUSB_thrstopCB=0;
 DWORD tid;                                       //** Open thread now

 HANDLE hThr=CreateThread(0,0,(LPTHREAD_START_ROUTINE)comUSB_Thr_CB,0,0,&tid);
 if(hThr==NULL)
  if( hThr==NULL ){
    char szDum[12];
    int ee=GetLastError();
    _itoa_s(ee, szDum, sizeof(szDum), 10);
    MessageBox(hWnd,"Create Thread() USB_CB failed, Code:",szDum,MB_OK);
    com_Exit();
    return -1; //This will cause WndProc to terminate the program
   }
 return 0;
}
#endif

// Uses comUSB_thrstopCB
int  comUSB_Exit_ControlBox(HWND hWnd){             //************ comUSB_Exit() is called ONCE at the end of the program... but comUSB_Exit_ControlBox() may be called many times????????????
 int comUSB_ExtRetCode;
 if(ftHandleCB==0) return 0; //** If already closed, ignore the call
 comUSB_thrstopCB=1; //** If thread is running, attempt to close it
 Sleep(200);    //**  -(& wait till its done it)
 comUSB_ExtRetCode = DLLf.m_pClose(ftHandleCB);
 if(FT_OK != comUSB_ExtRetCode){
    MessageBox(hWnd,"Couldnt close port - in comUSB_Exit_ControlBox()",NULL,MB_OK);
    nThread_CB_ErrorCode = 8;
   }
 ftHandleCB=NULL;
 /*
 if(!FreeLibrary(m_hmodule)){
    MessageBox(hWnd,"Couldnt Free Library",NULL,MB_OK);
    nThread_CB_ErrorCode = 9;
   }
 m_hmodule = NULL; */
 return 0;
}

//---------------------------------------------------------------------------
// FUNCTION:    GetStringRes_CB (int id INPUT ONLY)
//
// COMMENTS:    Load the resource string with the ID given, and return a
//              pointer to it.  Notice that the buffer is common memory so
//              the string must be used before this call is made a second time.
//---------------------------------------------------------------------------
LPTSTR   GetStringRes_CB (int id){ //identical to GetStringRes() but using a seperate buffer (each function will be called in a seperate thread)
 static TCHAR TCHbuffer[MAX_PATH];
 TCHbuffer[0]=0;
 LoadString (GetModuleHandle (NULL), id, TCHbuffer, MAX_PATH);
 return TCHbuffer;
}

#endif
#include "stdafx.h"
extern int bWaitingForReply[];

int NOOFTIMESTHRU_B4OBJECTING;//5 or 60?  NEVER set NOOFTIMESTHRU_B4OBJECTING >= 999,999,997 : that will break the hard-coded use of such numbers
int com_thr_CAstop; //Stop UDP thread
int comUSB_thrstop; //Stop USB thread

extern int nGlobalLastUSBrecNoRecvd;// = -1; This is an index into the table:
extern ENERGYREC sArrEnergRecs[]; // = 20hrs * 3600(secs per hour) * 60 Hz.  Table is 4,320,000 * 18 bytes size (== 77,760,000 bytes)

extern char mes[];
int nThreadErrorCode;
HMODULE     m_hmodule;
DLLFuncs    DLLf;
FT_HANDLE   ftHandle;

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
    com_Exit(hWnd);
    return -1; //This will cause WndProc to terminate the program
   }
 return 0;
}

void com_Exit(HWND hWnd){    //CALL com_Exit ONCE at the end of main program - It stops the thread running.
 com_thr_CAstop=1;           //Uses com_thr_CAstop to stop com_Thr_CheckAllWaiting4reply()
 Sleep(150);                 // & waits till its done
}

//com_Thr_CheckAllWaiting4reply() Maintains counters (i.e. monitors the time) for all motors from whom the main thread is waiting for a UDP reply.
//MainThread calls R eceiveMessagePackets() which immediately creates R eceive_Thr() or R eceive8000_Thr() which will block on recv(). Meantime MainThread
//will continue waiting in  R eceiveMessagePackets() for either the reply or for the counters maintained here to indicate a timeout (in which case
//R eceive_Thr() or R eceive8000_Thr() will be terminated) and MainThread can continue to run normally.
static DWORD WINAPI com_Thr_CheckAllWaiting4reply(LPVOID p){
 while ( !com_thr_CAstop ){
    for(int em = 0; em < NOOFMOTORS; em++)
      if(bWaitingForReply[em]){  //See Comments in the initial section - i.e. near to:     if(bWaitingForReply[18]){  
         if(bWaitingForReply[em]++ > NOOFTIMESTHRU_B4OBJECTING){
            if( (bWaitingForReply[em] > 999999999) && (bWaitingForReply[em] < 1010000003) ) //i.e. 999,999,999 & 1,010,000,003
              ;  //This means recv returned... we can leave bWaitingForReply[x], as it is and let the main program process the reply normally 
            else // recv did not return.  THIS function has incremented bWaitingForReply[x] until it shows we've timed out (probably a value of about 10 - 100 i.e. NOOFTIMESTHRU_B4OBJECTING)
              bWaitingForReply[em] = 2000000000;  //Main program to be responsible for killing the thread. //i.e.  2,000,000,000
           }
        }
    Sleep(100); // could also add extra user operations here
   }      //while ( !com_thr_CAstop )
 return 0;//** End of thread CA (Check All)
}

//The above functions all handle the UDP comms with all the motor controllers, the following functions handle the USB comms with the
// Energy controller (as a second, seperate, thread).

//**   - The main running thread. This is called by the Init function, and
//        made to stop by the Exit function. User does not call this.
//     - Uses NON_OVERLAPPED mode with simple sleep delay for simplicity
static DWORD WINAPI comUSB_Thr(LPVOID p){
 int nRecvingRecNo = 0; //We could kill and restart this thread for each Print (& perhaps need to if nRecvingRecNo is initialised here). See also nGlobalLastUSBrecNoRecvd
 DWORD ntr;
 DWORD nNumBytesRead;
 int   comUSB_ThRetCode;
 BYTE  OneByteRead[2];
 BYTE  byteDuffBuff[_MAX_DIR];
 //com_bindx=  0;
 do{
    comUSB_ThRetCode = DLLf.m_pStopInTask(ftHandle);
    if(comUSB_ThRetCode != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCode != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
       nThreadErrorCode = 1;
      }
   } while (comUSB_ThRetCode != FT_OK);
 do{
    comUSB_ThRetCode = DLLf.m_pPurge(ftHandle, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
    if(comUSB_ThRetCode != FT_OK){
       MessageBox(NULL,"comUSB_ThRetCode != FT_OK Comm Thread (Purge)",NULL,MB_OK);
       nThreadErrorCode = 2;
      }
   } while (comUSB_ThRetCode != FT_OK);
 int cnt = 0;
 do{
    comUSB_ThRetCode = DLLf.m_pRestartInTask(ftHandle);
    cnt++;
   } while ((comUSB_ThRetCode != FT_OK) && (cnt < 50010));
 if(cnt > 50000) //It's actually about 1400!
   MessageBox(NULL,"count >50000 for RestartInTask",NULL,MB_OK);
 int NoofBytesDiscarded = 0;
 int nIsDataBeingCollected = 0;
/**************************************************************************************************/
/*********************                                           **********************************/
/********************* INITIAL LOOP THAT THROWS AWAY A FEW BYTES **********************************/
/*********************                                           **********************************/
/**************************************************************************************************/
 while ( (comUSB_thrstop==0) && (ftHandle!=NULL) &&(m_hmodule!=NULL) && (NoofBytesDiscarded < 4000) ){

#ifndef B_ALLOW_USBCOMMS_ERRORS
    if(nIsDataBeingCollected++ > 60){ //was 1960
       MessageBox(NULL,GetStringRes(STR_IS_ALTERA_RUNNING), "USB device found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
       nThreadErrorCode = 3;
                                // NoofBytesDiscarded = 4000;
      }
#endif
//* Get nr of bytes in rx queue to read
    int WARNINGweDontUSeFollowingRetCode;//What I mean is, "we don't inspect the return code" (also search below for WARNINGweDontUSeFollowingRetCodeEITHER)
    comUSB_ThRetCode = DLLf.m_pGetQueueStatus(ftHandle, &ntr );//ntr was dwAmountInRxQueue - gives the Q size
    if (ntr>0){ //* align for wraparound in circular buffer
       nIsDataBeingCollected = 0;
       if ( ntr > _MAX_DIR)
         ntr = _MAX_DIR;
       //* Read the bytes to our buffer
//     int bNoError=ReadFile(com_handle,ptr,ntr,&nNumBytesRead,NULL);
//ntr (number to receive) will have been set to less than what's in the Q, if the buffer didnt have enough room between
// com_bindx and the end of the buffer.  The stuff left in the Q will be read next time.
       comUSB_ThRetCode = DLLf.m_pRead(ftHandle, byteDuffBuff, ntr, &nNumBytesRead);
       if (comUSB_ThRetCode!=FT_OK){    //* Handle errors, if any
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

 int nState = AWAITINGSTART;
 static char crc;
 int nNoofConsecutiveNonStartChars = 0;
 while ( !comUSB_thrstop && (ftHandle!=NULL) && (m_hmodule!=NULL) ){
    //int WARNINGweDontUSeFollowingRetCodeEITHER;
    comUSB_ThRetCode = DLLf.m_pGetQueueStatus(ftHandle, &ntr );//* Get noof bytes in rx queue available to read
    if(ntr){
       while (ntr-- > 0){
          comUSB_ThRetCode = DLLf.m_pRead(ftHandle, OneByteRead, 1, &nNumBytesRead);
          if (comUSB_ThRetCode!=FT_OK){    //Handle errors, if any
            MessageBox(NULL,"Communication Read Error1 in Comm Thread",NULL,MB_OK);  nThreadErrorCode = 5;   }
          else{
             nIsDataBeingCollected = 0;
             switch (nState){
                case AWAITINGSTART:
                  if(*OneByteRead == 0x3A){
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
                  if(*OneByteRead == 0x01)
                    nState = AWAITING_F;  //2nd byte
                  else{
                     nState = AWAITINGSTART;
                     continue; //Abandon this message, go on to 'while (ntr-- > 0)' and await the start of the next one
                    }
                  break;
                case AWAITING_F:
                  if(*OneByteRead == 0x45)
                    nState = AWAITING_N;  //3rd byte recvd
                  else{
                     nState = AWAITINGSTART;
                     continue;
                    }
                  break;
                case AWAITING_N:
                  if(*OneByteRead == 27)//
                    nState = AWAITING_D0; //4th byte recvd
                  else{
                     nState = AWAITINGSTART;
                     continue;
                    }
                  break;
//1st Meter
                case AWAITING_D0:
                  nState = AWAITING_D1;     //5th byte recvd - D0 (LSB). The endianess is correct (little endian), so store this whatever the value. (LSB cannot be range checked)
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc = 97 + *OneByteRead;  //73 is Addr(0x01) + Func ('E', 0x45, 69) + N (9*3).  Start is not included.
                  break;
                case AWAITING_D1:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D2;  //6th byte recvd - D1 (MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D2:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D3;  //7th byte recvd - D2 (Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D4:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D5;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D5:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D6;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D7:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D8;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D8:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D9;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D10:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D11;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D11:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D12;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D13:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D14;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D14:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D15;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D16:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D17;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D17:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D18;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D19:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D20;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D20:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D21;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D22:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D23;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D23:
                  if(*OneByteRead < 2){
                     nState = AWAITING_D24;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  *pch_sArrEnergRecs++ = *OneByteRead;
                  crc += *OneByteRead;
                  break;
                case AWAITING_D25:
                  if(*OneByteRead < 0x10){
                     nState = AWAITING_D26;  //(MSB). Provided MSB < 16 store it. (Data from the meters is supposed to be 12-bit)
                     *pch_sArrEnergRecs++ = *OneByteRead;
                     crc += *OneByteRead;
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
                case AWAITING_D26:
                  if(*OneByteRead < 2){
                     nState = AWAITING_CRC;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     crc += *OneByteRead;
                     if(*OneByteRead < 1)
                       *(short*)(pch_sArrEnergRecs - 2) = 0x7e41;
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
                  if((char)*OneByteRead == crc){
                     nState = AWAITINGSTOP;  //(CRC OK).
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//Stop
                case AWAITINGSTOP:
                  if(*OneByteRead == 0xaa){
                     nState = AWAITINGSPC1;  //(Stop byte OK).
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
                  if(*OneByteRead == 0){
                     nState = AWAITINGSPC2;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
//2ndZero
                case AWAITINGSPC2:
                  if(*OneByteRead == 0){
                     nState = AWAITINGSTART;  //(Status OK). At present I'm not saving anything, but should really note if it is 0 or 1!
                     if(++nRecvingRecNo != NOOF_ENERGYRECS)
                        nGlobalLastUSBrecNoRecvd++;
                     else{ //The table is full. Stop Incrementing the record numbers and pointers. Leave nGlobalLastUSBrecNoRecvd set at last record fully received,
                           // and let pch_sArrEnergRecs keep overwriting the current line.  Don't show a messagebox here - the thread shouldn't stop.
                        pch_sArrEnergRecs = (char*)&sArrEnergRecs[--nRecvingRecNo];
                       }
                    }
                  else{
                     nState = AWAITINGSTART;
                     pch_sArrEnergRecs = (char*)&sArrEnergRecs[nRecvingRecNo];//Must reset pointer to undo increments made while starting to save this 'now abandoned' record
                     continue;
                    }
                  break;
               }
            }       
         }
      }
    else{           //If nothing was read from line, block thread to allow buffer to fill.
       Sleep(10);
#ifndef B_ALLOW_USBCOMMS_ERRORS
       if(nIsDataBeingCollected++ == 60){
          MessageBox(NULL,GetStringRes(STR_IS_ALTERA_RUNNING), "USB device @2 found, BUT...",MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
          nThreadErrorCode = 6;
         }
       if(nNoofConsecutiveNonStartChars > 800){
          MessageBox(NULL,"nNoofConsecutiveNonStartChars > 800!\n\nYou can still interact with the main program but the USB thread\nis not receiving good data from the energy meters!\n\nEnsure 'Load VCP' is not checked in the Advanced Tab of\n'USB Serial Converter A' (in Device Manager)", NULL,MB_OK);//"Data is not being received through the USB port. Is the ALTERA processor running?"
          nNoofConsecutiveNonStartChars = 0;
         }
#endif
      }
   }//while
 //MessageBox(NULL,"USB Comm Thread Exiting!",NULL,MB_OK); 
 return 0;// comUSB_thrstop=0;    //** End of thread
}

//**************************************************************************
//**    Initialise. Called once at the start of the program
//      ‘port’  is the wanted port number, e.g. 3  (now its "DLP-USB245M")
//      ‘bsize’ is the wanted buffer size, must be a power of 2, e.g. 32768
int comUSB_Init(const char *port, int bufsize, HWND hWnd){
 int comUSBInit_RetCode;
 FT_STATUS ftStatus;
 char *BufPtrs[4];//pointer to array of 4 pointers
 char Buffer1[64];//buffer for product description of first device found
 char Buffer2[64];//buffer for product description of second device
 char Buffer3[64];
 DWORD numDevs;
 //TEMPTIMESthru = 0;
 //TEMPmaxInQQQue = 0;
 //nIndx2StopAt = 0;
 //com_bsize=bufsize;             // save buffersize, must be 2^n
 //com_bmask=bufsize-1;           // set mask e.g. 0x7FFF

//** Validate
 if(ftHandle!=0)
   return 0;    // If already opened, ignore the call

//** Allocate a dynamic byte buffer for storing incoming data
     /*
 if(com_bdata==NULL){//This added line (mj) ensures the memory allocation takes place only once during the program's
    com_bdata= new BYTE[com_bsize];//lifetime - even though com_Init & com_Exit maybe called many times
    if(com_bdata==NULL){
       wsprintf(Buffer1,"Couldnt allocate %d Bytes for storing incoming USB data",com_bsize);//REUSE of Buffer1
       MessageBox(hWnd,Buffer1,"Try restarting Windows?",MB_OK);//Insufficient memory
       com_Exit(hWnd);
       return -3;
      }
   }
//** Allocate a dynamic byte buffer for storing incoming data
 if(com_dbdata==NULL){//This added line (mj) ensures the memory allocation takes place only once during the program's
    com_dbdata= new BYTE[BUFSIZE_4MEG];//lifetime - even though com_Init & com_Exit maybe called many times
    if(com_dbdata==NULL){
       wsprintf(Buffer1,"Couldnt allocate %d Bytes for storing incoming USB data",BUFSIZE_4MEG);//REUSE of Buffer1
       MessageBox(hWnd,Buffer1,"Try restarting Windows?",MB_OK);//Insufficient memory
       com_Exit(hWnd);
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

//** Open the comms port
/*
 char zp[8];
 sprintf(zp,"COM%d",port);
 com_handle=CreateFile( zp,                            // "COMn"
                        GENERIC_READ|GENERIC_WRITE,    //
                        0,                             // no share (only option)
                        NULL,                          // Security attribs
                        OPEN_EXISTING,                 // must be this for comms
                        0,                             // non FLAG_OVERLAPPED
                        NULL                           // must be NULL for comms
                      );
 if(com_handle==INVALID_HANDLE_VALUE){    //cannot open com port?
    int ee=GetLastError();
    sprintf (com_zerror,"COM%d error %d",port,ee);
    com_handle=NULL;
    return -1;
   } */
//FROM WM_CREATE
       //This sample shows how to get the product descriptions of all the devices currently connected.

       //initialize the array of pointers
       BufPtrs [0 ] = Buffer1;
       BufPtrs [1 ] = Buffer2;
       BufPtrs [2 ] = Buffer3;
       BufPtrs [3 ] = NULL;//last entry should be NULL
       ftStatus =DLLf.m_pListDevices(BufPtrs,&numDevs,FT_LIST_ALL|FT_OPEN_BY_DESCRIPTION);
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
         comUSBInit_RetCode = DLLf.m_pOpenEx(BufPtrs[0],FT_OPEN_BY_DESCRIPTION,&ftHandle);
       else
         comUSBInit_RetCode = DLLf.m_pOpenEx((PVOID)port,FT_OPEN_BY_DESCRIPTION,&ftHandle);//port defaults to "DLP-USB245M",
       if(FT_OK != comUSBInit_RetCode){                                                   //  but can be changed by user
          StringCchCopy(mes,SIZMES,"Couldnt open USB port: ");
          StringCchCat(mes,SIZMES,port);
          MessageBox(hWnd,mes,"DLLf.m_pOpenEx failed",MB_OK);
          return -1;
         }
       else
         ;//MessageBox(hWnd,"This Version opens the first enumerated device!","Not Hardcoded",MB_OK);


       comUSBInit_RetCode = DLLf.m_pSetBaudRate( ftHandle,230400);
//     comUSBInit_RetCode = DLLf.m_pSetBaudRate( ftHandle,115200);
       if(FT_OK != comUSBInit_RetCode){
          StringCchCopy(mes,SIZMES,"Baudrate NOT set to 230400!!! port: ");
          StringCchCat(mes,SIZMES,port);
          MessageBox(hWnd,mes,"DLLf.m_pSetBaudRate failed",MB_OK);
          return -1;
         }
//     else
//       MessageBox(hWnd,"Baudrate set to 230400","Message",MB_OK);

do{
   comUSBInit_RetCode = DLLf.m_pStopInTask(ftHandle);
   if(comUSBInit_RetCode != FT_OK){
      MessageBox(NULL,"comUSBInit_RetCode != FT_OK Comm Thread (StopInTask)",NULL,MB_OK);
      nThreadErrorCode = 7;//Not strictly a Thread error, but we might as well see it. If its always there, delete this line
     }
  } while (comUSBInit_RetCode != FT_OK);

       comUSBInit_RetCode = DLLf.m_pPurge(ftHandle, FT_PURGE_TX | FT_PURGE_RX);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt Purge Buffers after opening USB port","DLLf.m_pPurge failed",MB_OK);
          return -5;
         }

//int cnt = 0;
do{
   comUSBInit_RetCode = DLLf.m_pRestartInTask(ftHandle);
   // cnt++;
  } while (comUSBInit_RetCode != FT_OK);

       comUSBInit_RetCode = DLLf.m_pResetDevice(ftHandle);   //Purge Buffers
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt ResetDevice after opening USB port","DLLf.m_pResetDevice failed",MB_OK);
          return -6;
         }
//if(cnt > 1) Its actually about 1400!
  // MessageBox(NULL,"count >1 for RestartInTask",NULL,MB_OK);
       comUSBInit_RetCode = DLLf.m_pSetTimeouts(ftHandle, 1000, 1000);
       if(FT_OK != comUSBInit_RetCode){
          MessageBox(hWnd,"Couldnt SetTimeouts after opening USB port","DLLf.m_pSetTimeouts failed",MB_OK);
          return -7;
         }


 comUSB_thrstop=0;
 DWORD tid;                                       //** Open thread now

 HANDLE hThr=CreateThread(0,0,(LPTHREAD_START_ROUTINE)comUSB_Thr,0,0,&tid);
 if(hThr==NULL)
  if( hThr==NULL ){
    char szDum[12];
    int ee=GetLastError();
    _itoa_s(ee, szDum, sizeof(szDum), 10);
    MessageBox(hWnd,"Create Thread() USB failed, Code:",szDum,MB_OK);
    com_Exit(hWnd);
    return -1; //This will cause WndProc to terminate the program
   }
 return 0;
}

// Uses com_handle, comUSB_thrstop
int  comUSB_Exit(HWND hWnd){             //************   CALL com_Exit ONCE at the end of the program
 int comUSB_ExtRetCode;
 if(ftHandle==0) return 0; //** If already closed, ignore the call
 comUSB_thrstop=1; //** If thread is running, attempt to close it
 Sleep(100);    //**  -(& wait till its done it)
 comUSB_ExtRetCode = DLLf.m_pClose(ftHandle);
 if(FT_OK != comUSB_ExtRetCode){
    MessageBox(hWnd,"Couldnt close port",NULL,MB_OK);
    nThreadErrorCode = 8;
   }
 ftHandle=NULL;
 if(!FreeLibrary(m_hmodule)){
    MessageBox(hWnd,"Couldnt Free Library",NULL,MB_OK);
    nThreadErrorCode = 9;
   }
 m_hmodule = NULL;
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
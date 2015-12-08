//In Windows Local Area Connection dialog box, I recommend turning off:
// - "Network monitor Driver",
// - "File and Printer sharing",
// - "Client for Microsoft Networks", etc. (But not TCP/IP!)
//This will help minimise unnecessary network traffic.

// CONTAINS:
//S endMessagePackets()
//R eceive_Thr()
//R eceive8000_Thr()
//R eceiveMessagePackets()
//S afelyDisplayErrorAndCleanUp()
//I nitialiseAllSockets()
//D estroyAllSockets()
//I nitialiseAllReceiveBuffers()
//D umpReceiveBuffers2File()

#include <stdafx.h>
extern HINSTANCE hInst;
//extern int CountStatRecsRecvd_TimesPlaySoundCalled;
//extern CRegStdWORD   bDoProperCheckSumChecks_MposFiles;

int nRECVsRetVal[NOOFMOTORS];   //R eceive_Thr() sets this and R eceiveMessagePackets() inspects it to see if there's been a SOCKET_ERROR

//extern HANDLE ghMutex;

//#define USE_SILLYBUF          //Enable Debugger to view contents of packets by copying them to another buffer
unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];

int   nPixPositionsAr[2200];
int   nSizPixPositionsAr;
short sPixDiffsAr[4400002]; //2200 * MAX_NOOFLINES short ints, + 2 bytes that get used by the last packet received by Receive8000_Thr() [but 4
                            // if there's an error with recv] (seqno + message type) 8,800,004 bytes total
int   nPixFreqsAr[NOOFMOTORS * 4]; //There are 4 readings for each motor
int   nSizPixDiffsAr;

static int nActualMessTypeRecvdIn8000;
static int nActualSeqnoRecvdIn8000;
static int nActualSeqnoExpctdIn8000;

int globDumpBuff;

SOCKET  conn_socket[NOOFMOTORS];
//char szCSVDirName4Writing[_MAX_DIR];
extern HCFRECORDsCURRENTfolders   HCFcfs;
extern HCFRECORD HCFrec;

int  bWaitingForReply[NOOFMOTORS];

extern BOOL bMakeSounds;
extern BOOL bSaveDiffFileAsBinary;

#define DEFAULT_PORT 27015   //This is 0x6987
//#define DEFAULT_PORT 8738  //This is 0x2222
//#define DEFAULT_PORT 5001  //This is 0x1389
//#define DEFAULT_PORT 4909  //This is 0x132D
//#define DEFAULT_PORT 1058  //This is 0x422
//#define DEFAULT_PROTO SOCK_STREAM // TCP

//extern char ATMEL_IP_ADDRESStab[NOOFMOTORS][16];
extern PCFRECORD PCFrec;

char TabDEFAULT_CSV_OUTFILENAMEPPOSLINE[NOOFMOTORS][24] ={ 
 "PixPositionsLineM01.csv","PixPositionsLineM02.csv","PixPositionsLineM03.csv","PixPositionsLineM04.csv","PixPositionsLineM05.csv","PixPositionsLineM06.csv","PixPositionsLineM07.csv","PixPositionsLineM08.csv","PixPositionsLineM09.csv","PixPositionsLineM10.csv",
 "PixPositionsLineM11.csv","PixPositionsLineM12.csv","PixPositionsLineM13.csv","PixPositionsLineM14.csv","PixPositionsLineM15.csv","PixPositionsLineM16.csv","PixPositionsLineM17.csv","PixPositionsLineM18.csv","PixPositionsLineM19.csv","PixPositionsLineM20.csv",
 "PixPositionsLineM21.csv","PixPositionsLineM22.csv","PixPositionsLineM23.csv","PixPositionsLineM24.csv","PixPositionsLineM25.csv","PixPositionsLineM26.csv","PixPositionsLineM27.csv","PixPositionsLineM28.csv","PixPositionsLineM29.csv","PixPositionsLineM30.csv",
 "PixPositionsLineM31.csv","PixPositionsLineM32.csv","PixPositionsLineM33.csv","PixPositionsLineM34.csv","PixPositionsLineM35.csv","PixPositionsLineM36.csv","PixPositionsLineM37.csv","PixPositionsLineM38.csv","PixPositionsLineM39.csv","PixPositionsLineM40.csv"};

char TabDEFAULT_CSV_OUTFILENAMEPDIFFLINE[NOOFMOTORS][20] ={
 "PixDiffsLineM01.csv","PixDiffsLineM02.csv","PixDiffsLineM03.csv","PixDiffsLineM04.csv","PixDiffsLineM05.csv","PixDiffsLineM06.csv","PixDiffsLineM07.csv","PixDiffsLineM08.csv","PixDiffsLineM09.csv","PixDiffsLineM10.csv",
 "PixDiffsLineM11.csv","PixDiffsLineM12.csv","PixDiffsLineM13.csv","PixDiffsLineM14.csv","PixDiffsLineM15.csv","PixDiffsLineM16.csv","PixDiffsLineM17.csv","PixDiffsLineM18.csv","PixDiffsLineM19.csv","PixDiffsLineM20.csv",
 "PixDiffsLineM21.csv","PixDiffsLineM22.csv","PixDiffsLineM23.csv","PixDiffsLineM24.csv","PixDiffsLineM25.csv","PixDiffsLineM26.csv","PixDiffsLineM27.csv","PixDiffsLineM28.csv","PixDiffsLineM29.csv","PixDiffsLineM30.csv",
 "PixDiffsLineM31.csv","PixDiffsLineM32.csv","PixDiffsLineM33.csv","PixDiffsLineM34.csv","PixDiffsLineM35.csv","PixDiffsLineM36.csv","PixDiffsLineM37.csv","PixDiffsLineM38.csv","PixDiffsLineM39.csv","PixDiffsLineM40.csv"};

char TabDEFAULT_CSV_OUTFILENAMEPDIFFFILE[NOOFMOTORS][20] ={
 "PixDiffsFileM01.csv","PixDiffsFileM02.csv","PixDiffsFileM03.csv","PixDiffsFileM04.csv","PixDiffsFileM05.csv","PixDiffsFileM06.csv","PixDiffsFileM07.csv","PixDiffsFileM08.csv","PixDiffsFileM09.csv","PixDiffsFileM10.csv",
 "PixDiffsFileM11.csv","PixDiffsFileM12.csv","PixDiffsFileM13.csv","PixDiffsFileM14.csv","PixDiffsFileM15.csv","PixDiffsFileM16.csv","PixDiffsFileM17.csv","PixDiffsFileM18.csv","PixDiffsFileM19.csv","PixDiffsFileM20.csv",
 "PixDiffsFileM21.csv","PixDiffsFileM22.csv","PixDiffsFileM23.csv","PixDiffsFileM24.csv","PixDiffsFileM25.csv","PixDiffsFileM26.csv","PixDiffsFileM27.csv","PixDiffsFileM28.csv","PixDiffsFileM29.csv","PixDiffsFileM30.csv",
 "PixDiffsFileM31.csv","PixDiffsFileM32.csv","PixDiffsFileM33.csv","PixDiffsFileM34.csv","PixDiffsFileM35.csv","PixDiffsFileM36.csv","PixDiffsFileM37.csv","PixDiffsFileM38.csv","PixDiffsFileM39.csv","PixDiffsFileM40.csv"};

char TabDEFAULT_BIN_OUTFILENAMEPDIFFFILE[NOOFMOTORS][20] ={
 "PixDiffsFileM01.bin","PixDiffsFileM02.bin","PixDiffsFileM03.bin","PixDiffsFileM04.bin","PixDiffsFileM05.bin","PixDiffsFileM06.bin","PixDiffsFileM07.bin","PixDiffsFileM08.bin","PixDiffsFileM09.bin","PixDiffsFileM10.bin",
 "PixDiffsFileM11.bin","PixDiffsFileM12.bin","PixDiffsFileM13.bin","PixDiffsFileM14.bin","PixDiffsFileM15.bin","PixDiffsFileM16.bin","PixDiffsFileM17.bin","PixDiffsFileM18.bin","PixDiffsFileM19.bin","PixDiffsFileM20.bin",
 "PixDiffsFileM21.bin","PixDiffsFileM22.bin","PixDiffsFileM23.bin","PixDiffsFileM24.bin","PixDiffsFileM25.bin","PixDiffsFileM26.bin","PixDiffsFileM27.bin","PixDiffsFileM28.bin","PixDiffsFileM29.bin","PixDiffsFileM30.bin",
 "PixDiffsFileM31.bin","PixDiffsFileM32.bin","PixDiffsFileM33.bin","PixDiffsFileM34.bin","PixDiffsFileM35.bin","PixDiffsFileM36.bin","PixDiffsFileM37.bin","PixDiffsFileM38.bin","PixDiffsFileM39.bin","PixDiffsFileM40.bin"};
/*
 char TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[NOOFMOTORS][20] ={
 "PixFreqsFileM01.csv","PixFreqsFileM02.csv","PixFreqsFileM03.csv","PixFreqsFileM04.csv","PixFreqsFileM05.csv","PixFreqsFileM06.csv","PixFreqsFileM07.csv","PixFreqsFileM08.csv","PixFreqsFileM09.csv","PixFreqsFileM10.csv",
 "PixFreqsFileM11.csv","PixFreqsFileM12.csv","PixFreqsFileM13.csv","PixFreqsFileM14.csv","PixFreqsFileM15.csv","PixFreqsFileM16.csv","PixFreqsFileM17.csv","PixFreqsFileM18.csv","PixFreqsFileM19.csv","PixFreqsFileM20.csv",
 "PixFreqsFileM21.csv","PixFreqsFileM22.csv","PixFreqsFileM23.csv","PixFreqsFileM24.csv","PixFreqsFileM25.csv","PixFreqsFileM26.csv","PixFreqsFileM27.csv","PixFreqsFileM28.csv","PixFreqsFileM29.csv","PixFreqsFileM30.csv",
 "PixFreqsFileM31.csv","PixFreqsFileM32.csv","PixFreqsFileM33.csv","PixFreqsFileM34.csv","PixFreqsFileM35.csv","PixFreqsFileM36.csv","PixFreqsFileM37.csv","PixFreqsFileM38.csv","PixFreqsFileM39.csv","PixFreqsFileM40.csv"};
*/
 //char TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[1][21] ={"PixFreqsFileAllM.csv"};


//#define LOCAL_IP_ADDRESS    "192.168.0.2" //The address of this Windows machine
#define LOCAL_IP_ADDRESS    "127.0.0.1" //The address of this Windows machine
//With UDP Type 1 messages, this function sends a 22 byte header plus 280 ints (22+1120 bytes).  An addtional 42 bytes is prepended by the
// Winsock "send" function, so Total real packet size is 1184 bytes (That's what Atmel receives, but the application discards the first 42 bytes.
//#define PACKETLENGTH       1472  //moved to stdafx.h
#define PACKETLENGTHtimes2 2944
//'Header' structure (for type 1-1 messages):
//uchMessType 1
//usMPDSeqNo  1
//nPulsesOrSubTypOrFOPP     4  //Not set for 
//nPacketNo_orLineSubPacketNo      4
//UnUsed      10
extern char mes[];
//Note the following comment is rather out of date (e.g only 1 packet is sent per call, & type 1 messages have a 4 byte header + 4 bytes trailing on every 3rd record):
//packetbuff contains the data to be sent.  If several packets (say 5) must be sent to the DP, packetbuff must contain all of them. If 280 ints are to be sent in each packet
//(1120 bytes) plus an [application] header of say 22 bytes, the packets actually sent will be 1142 bytes + 42 bytes appended by WinSock.  packetbuff would need to store
//5 * 1142 bytes (because it knows nothing about what WinSock does).  As our switch is limited to mini-jumbo packets of 1600 bytes, PACKETLENGTH has to be limited to 1558
//bytes, which allows 779 short ints to be sent.  As I want to send 2200 numbers, Three blocks of 734 shorts (=2202) would be sufficient.  Adding the 22 bytes, gives a 
//Packet Size of 1490 bytes (plus the 42 from WinSock) is 1532.

//char packetbuff[PACKETLENGTH * 5]; //Global... Data may be read into this by myOpen_File(), called from TransBlock_Dialog()
char packetbuff[PACKETLENGTH * 3];   //Global... Data may be read into this by myOpen_File(), called from TransBlock_Dialog()
extern char *pMxPacketBuffAr[];

//Function SendMessagePackets():
//For Message Type 1, chCardNo indexes into pMxPacketBuffAr to find the correct buffer containing the data to be sent.
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   SendMessagePackets ()                                       ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
//To stop SendMessagePackets() displaying Errors as Message Boxes (e.g. When U pdateMainWindowsControlsM() is requesting dozens of status
// records to display elsewhere) set hDlg to NULL and then look at the return code to see if any error occured.
//Return codes 6,8,87,995,996,997 & 10000 - 12000 will be WSA error codes, but have 1,000,000 2,000,000 3,000,000 etc added to them depending on where the error occurred.
//This app's codes own error codes are 5001 (Timeout AVR not responding?) and 5002 (impossible!)  Dont change any of these codes without considering how they're decoded.
int SendMessagePackets (HWND hDlg, char chCardNo, unsigned char uchMessType,  unsigned int nPulsesOrSubTypOrFOPP, unsigned int nPacketNo_orLineSubPacketNo) {//Note nPulsesOrSubTypOrFOPP
                                   // is used as a message sub-type for many messages, and to send FOPP (FrequencyOfPixPrints) for the IDM_SETpixFREQ message
 int Snaplen;
 int retval;
 //unsigned int uiMessageSubType;
 //uiMessageSubType = nPulsesOrSubTypOrFOPP;
#ifdef USE_SILLYBUF                      //Useful for debugging (Enables Debugger to view contents of packets by copying them to another buffer)
       char  SillyBuf[PACKETLENGTH * 3]; //Silly, because it's silly that I have to copy the buffer to this in order to see it!
       short nPacketData[734];
       int i;
#endif
 packetbuff[0]=uchMessType; //Message Type - 1 is Motor Position Data (up to 1400 32-bit ints, 280 ints per packet)
 if(uchMessType > 0x01){ //i.e. NOT a MsTyp1_MOTOR_POS_DATA message... Just send One short Packet, not x long Packets.
    Snaplen = 22;
    endian_swap(nPulsesOrSubTypOrFOPP);
    *(int*)&packetbuff[2] = nPulsesOrSubTypOrFOPP;
    endian_swap(nPacketNo_orLineSubPacketNo);
    *(int*)&packetbuff[6] = nPacketNo_orLineSubPacketNo;//This actually holds LineSubPacketNo for Message type 0x9 (IDM_SENDpixPOSITIONsLINE etc) 
    if((chCardNo == 38)&&(uchMessType == MsTypE_SET_NEXTLINE2PRINT)){//We need to add nM39_XAXISBACKLASH & PixelAtwhichAccelStops (using 4 of the 12 unused bytes)
       unsigned int nPixAtwhichAccelStops = HCFrec.nPixelAtwhichAccelStops;
       //unsigned int nM39_XaxisBACKLASH = HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH;
       endian_swap(nPixAtwhichAccelStops);
       *(int*)&packetbuff[10] = nPixAtwhichAccelStops;
       //endian_swap(nM39_XaxisBACKLASH);
       //*(int*)&packetbuff[14] = nM39_XaxisBACKLASH;
      }
   } //With UDP, I'm sending a 22 byte short packet (plus the 42 bytes that gets prepended). 12 of the 22 bytes are currently unused
 else{//uchMessType is 1
    Snaplen = PACKETLENGTH; //Now 1472 i.e. (734 * 2) + 4  i.e. Noof shorts (per packet) * sizeof(short) + sizeof header
                            // or 1472 i.e. (732 * 2) + 8  i.e. Noof shorts (per packet) * sizeof(short) + sizeof header (for 1st packet in each row)
//  if (nPulsesOrSubTypOrFOPP & 1) //for uchMessType 1 messages, this parameter is bMoveNow...
//    *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) |= 0x1;  //This  Sets  Byte 1 bit 0 in each packet (i.e. bMoveNow flag)
//  else
//    *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) &= 0xfe; //This Clears Byte 1 bit 0 in each packet (i.e. bMoveNow flag). We'll wait for the line start, or for the Home message

    //Historic comment (now corrected): I don't like using the GLOBAL CRegStdWORD bDoProperCheckSumChecks_MposFiles here, but don't want an extra parameter in such a commonly used function either :-(   
//  if (nPulsesOrSubTypOrFOPP & 2) //for uchMessType 1 messages, this parameter is bDoProperCheckSumChecks_MposFiles...
//    *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) |= 0x20; //This  Sets  Byte 1 bit 5 in each packet (i.e. bDoProperCheckSumChecks_MposFiles flag)
//  else
//    *((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) &= 0xdf; //This Clears Byte 1 bit 5 in each packet (i.e. bDoProperCheckSumChecks_MposFiles flag)... Do a nonexistent checksum check.

    //*((pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo))+1) |= 0x40; //This  Sets  Byte 1 bit 6 in each packet (i.e. AccProf is for Fast Movements) It must be inserted b4 SendMessagePackets!!
   }
 //ALL THE PREPARATION IS NOW DONE (i.e. the buffer is loaded with data {addresses, message types, & data) and we know if we're sending 1 short
 // packet or five long ones.  All we need to do know is get the Network Adapter to send it:
#ifdef USE_SILLYBUF
//This allows the debugger to inspect the Xth, Xth + 1, & Xth + 2 Packets sent (set the number X following Snaplen) rem 736 shorts in each buffer
 if(uchMessType == MsTyp1_MOTOR_POS_DATA){
  //memcpy(SillyBuf,pMxPacketBuffAr[chCardNo]+(Snaplen * nSpeed),PACKETLENGTH); //Snaplen is PACKETLENGTH, nSpeed is PacketNum
    memcpy(SillyBuf,pMxPacketBuffAr[chCardNo]+(Snaplen * 0),PACKETLENGTH * 3);  //Snaplen is PACKETLENGTH, nSpeed is PacketNum
    //This allows the debugger to view the payload as 734 ints
    for(i=0; i < 734; i++){
       char EndianSwap[2];
       //EndianSwap[0] = SillyBuf[23 + 2*i];
       //EndianSwap[1] = SillyBuf[22 + 2*i];
       EndianSwap[0] = SillyBuf[5 + 2*i];
       EndianSwap[1] = SillyBuf[4 + 2*i];
       nPacketData[i] = *(int*)EndianSwap;
       nPacketData[i] = nPacketData[i]; //for break-point
      }
    memcpy(SillyBuf,pMxPacketBuffAr[chCardNo]+(Snaplen * 4),PACKETLENGTH);  //View another 3 packet
    for(i=0; i < 734; i++){
       char EndianSwap[2];
       EndianSwap[0] = SillyBuf[5 + 2*i];
       EndianSwap[1] = SillyBuf[4 + 2*i];
       nPacketData[i] = *(int*)EndianSwap;
       nPacketData[i] = nPacketData[i]; //for break-point
      }
   }
#endif
 DWORD timeSendWasCalled = GetTickCount();  //Wraps if system is up for 49 days continuously. (GetTickCount64() is for Vista Only)
 if(uchMessType > 0x01) //i.e. NOT a MsTyp1_MOTOR_POS_DATA message... Just send One short Packet, not x long Packets.
   retval = send(conn_socket[chCardNo],&packetbuff[Snaplen * 0],Snaplen,0);
 else
   retval = send(conn_socket[chCardNo],pMxPacketBuffAr[chCardNo]+(Snaplen * nPacketNo_orLineSubPacketNo),Snaplen,0); //Note nPacketNo_orLineSubPacketNo is really the PacketNumber for uchMessType == 1

 DWORD timeSendReturned = GetTickCount();           //These time functions do nothing other than provide
 if( timeSendReturned > (timeSendWasCalled+300) ) { //  an early warning that AVR may be locked up.
    //closesocket(conn_socket[chCardNo]);  int ThisLineWasVeryQuestionable; (and also most other places calling closesocket())
    if (hDlg)
      DisplayErrorMessageBoxWithErrorAndMotorNo(hDlg, chCardNo, "Send() function in SMP() took > 0.3 seconds from Motor: ", "Error - AVR not responding?");
    //WSACleanup();
    return 5001;
   }
 else if (retval == SOCKET_ERROR){
    retval = WSAGetLastError();
    //closesocket(conn_socket[chCardNo]);
    if(hDlg)
      SafelyDisplayErrorAndCleanUp(hDlg,TEXT("\nsend() failed: error"),retval);  //Note - WSACleanup has been removed
    return retval + 4000000;
   }
 //for(int xx = 0; xx < 1; xx++){//Delay sending the next packet, briefly to allow AVR time to receive last one
 //  xx++;                       //Effectively I've moved this delay (search on nInterPacketDelay to see how calls 
 //  xx--;                       // to SendMessagePackets() are spaced apart.
 // }


 //WSACleanup();
/*
//This allows the debugger to inspect the 5th (i.e Final Packet that was sent)
 memcpy(SillyBuf,&packetbuff[Snaplen * 2],PACKETLENGTH);
//This allows the debugger to view the payload as 280 integers
 for(i=0; i < 734; i++){
  //char EndianSwap[4];
  //EndianSwap[0] = SillyBuf[25 + 4*i];
  //EndianSwap[1] = SillyBuf[24 + 4*i];
  //EndianSwap[2] = SillyBuf[23 + 4*i];
  //EndianSwap[3] = SillyBuf[22 + 4*i];
    char EndianSwap[2];
    EndianSwap[0] = SillyBuf[23 + 2*i];
    EndianSwap[1] = SillyBuf[22 + 2*i];
    nPacketData[i] = *(int*)EndianSwap;
   }
// */
 return 0;
}

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 ***** ***** ***** ***** *****                                      FUNCTION   Receive_Thr ()                                              ***** ***** ***** ***** *****
 ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
static DWORD WINAPI Receive_Thr(LPVOID p){ //p is a pointer to the motor number
//n RECVsRetVal = recv(conn_socket[*(int*)p], (char*)chReceiveBuffer, SIZ_REC_BUFF, 0);//If this function doesn't return within 0.4? seconds R eceiveMessagePackets()
 nRECVsRetVal[*(int*)p] = recv(conn_socket[*(int*)p], (char*) &chReceiveBuffer[*(int*)p][0], SIZ_REC_BUFF, 0);//If this function does not return within 0.4? seconds 
 bWaitingForReply[*(int*)p] = 1000000000;                                                                    // R eceiveMessagePackets() will Terminate this thread
 return 0;
}
static DWORD WINAPI Receive8000_Thr(LPVOID p){ //p is a pointer to the motor number
 unsigned char  *pchPixDiffsAr = (unsigned char*)sPixDiffsAr;
 short *pshPixDiffsAr = sPixDiffsAr;
 int  *pRetVal  = &nRECVsRetVal[*(int*)p];
 unsigned char uchSeqno = 0;
 int n8000pkts = 0;
 for( ; n8000pkts < 8000; n8000pkts++){
  //nRECVsRetVal[*(int*)p] = recv(conn_socket[*(int*)p], (char*) &chReceiveBuffer[*(int*)p][0], SIZ_REC_BUFF, 0);//Forget the ReceiveBuffers put directly to sPixDiffsAr
    *(pchPixDiffsAr + 1100) = 0x67; //Just to be doubly sure the following r ecv does overwrite the buffer with 0x66!
    *pRetVal = recv(conn_socket[*(int*)p], (char*)pchPixDiffsAr, 1104, 0);//If this function does not return within 8? seconds R eceiveMessagePackets() will 
    if(*pRetVal != 1102)                                           // Terminate this thread. 1104 allows detection of a packet that would be too long.
      n8000pkts = 9000; //Can't cope with wrong length packets - exit thread. Caller should check if(nRECVsRetVal[motor] != 1102)
    else{ //Check seqno and message type before accepting the data
       if( *(pchPixDiffsAr + 1100) != 0x66 ){
          n8000pkts = 9001; //Can't cope with wrong message types - exit thread. Caller should check if(nRECVsRetVal[motor] == 16666)
          nActualMessTypeRecvdIn8000 = *(pchPixDiffsAr + 1100);
         }
       else{
          if( *(pchPixDiffsAr + 1101) != uchSeqno ){
             n8000pkts = 9002; //Can't cope with wrong length packets - exit thread. Caller should check if(nRECVsRetVal[motor] != 1102)
             nActualSeqnoRecvdIn8000 = *(pchPixDiffsAr + 1101);
             nActualSeqnoExpctdIn8000 = uchSeqno;
            }
          else{
             for(int i = 0; i < 550; i++)
               short_endian_swap(*(unsigned short*)pshPixDiffsAr++);
             pchPixDiffsAr += 1100;
             uchSeqno++;
            }
         }
      }
   }
 nRECVsRetVal[*(int*)p] = *pRetVal;
 bWaitingForReply[*(int*)p] = 1000000000;
 return 0;
}
 //if (uiMessageSubType == 139){ //This else does not happen when we're sending most messages... Only those where we're expecting a reply
                                 //139 is a sentinel value (a 'random' special number I chose)
                                 //Insist on a reply for IDM_ZEND_STATUSREC & when downloading PixPositions
//A far neater way of doing this is to have a separate receive function 'R eceiveMessagePackets()' [outside of SendMessagePackets()], especially now that
// sockets are opened at program start up and closed when program exits. The ugly sentinel has gone, and SendMessagePackets() is kept very simple.
#define  SIZ_SZDUMM  12

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   R eceiveMessagePackets ()                                    ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
//IF hDlg is NULL no MessageBoxes are displayed.  All errors return a code... 
int ReceiveMessagePackets (HWND hDlg, char chCardNo, unsigned char uchExpectedMessType, unsigned int LineSubPacket){
 char szRMPsLocalFileName[_MAX_DIR];
 char szDumm[SIZ_SZDUMM];
 DWORD thr2id;            //** Open thread now
 int nCardNo = chCardNo;
 HANDLE hThrRec;

 if(LineSubPacket == 8000){
    nActualMessTypeRecvdIn8000 = 257;
    nActualSeqnoExpctdIn8000   = 257;
    hThrRec=CreateThread(0,0,(LPTHREAD_START_ROUTINE)Receive8000_Thr,&nCardNo,0,&thr2id);
   }
 else
   hThrRec=CreateThread(0,0,(LPTHREAD_START_ROUTINE)Receive_Thr,&nCardNo,0,&thr2id);

 if( hThrRec == NULL ){
    int ee=GetLastError();
    _itoa_s(ee, szDumm, SIZ_SZDUMM, 10);
    bWaitingForReply[chCardNo] = 0;
    if(hDlg)
      MessageBox(hDlg,"Create Thread(Receive Thr) failed, Code:",szDumm,MB_OK);
    return 5555;  //Calling Procedure should handle this (and 6666, below)
   }
 //The following line needs to be in the R eceive_Thr() procedure (allowing the thread to be killed if recv() doesn't return)
 //n RECVsRetVal = recv(conn_socket[chCardNo], (char*)chReceiveBuffer, SIZ_REC_BUFF, 0);//If this function doesn't return within 5? seconds the com_Thr_x() function
 int SoHowManyTimesDoesThisLoop = 0;

 while(bWaitingForReply[chCardNo] < 1000000000){ //1,000,000,000
    SoHowManyTimesDoesThisLoop++;
    Sleep(10); //Wait until we get a reply or until a timeout has killed the thread.   1000000000 means recv() did return in R eceive_Thr(), but BEWARE...
     // ... the com_Thr_CheckAllWaiting4reply thread could increment bWaitingForReply[chCardNo] immediately after a reply was received so ~1000000001 is a possibility too!
   }
 if(bWaitingForReply[chCardNo] > 1100000000){ //We lost contact (or, at least didn't get a reply)
    bWaitingForReply[chCardNo] = 0;
    if(!TerminateThread(hThrRec,0)){
       if(hDlg)
         MessageBox(hDlg,"Lost Contact, and even TerminateThread() failed!",NULL,MB_OK);
       return 6667;
      }
    return 6666;  //Calling Procedure should handle this. "Error Code: 6666 - No Reply"
   }
 else
   bWaitingForReply[chCardNo] = 0;

//timeSendReturned = GetTickCount(); //Use if we want to see how long the reply took.
 if ((nRECVsRetVal[chCardNo] == SOCKET_ERROR)||(nRECVsRetVal[chCardNo] == 0)){
    if(nRECVsRetVal[chCardNo] == 0){   //As next line is supposedly impossible, I'll not bother to close the socket first:
       SafelyDisplayErrorAndCleanUp(hDlg,TEXT("Server closed connection (impossible for UDP!)"),0); //A completly dummy error code
       if(hDlg == NULL)//... Also I'll force a MessageBox even if SendMessagePackets() has been told to suppress them.
         MessageBox(NULL, "Server closed connection (impossible for UDP!)", NULL, MB_OK);
       return 5002;
      }
    else{
       int nRECVsErrorCode = WSAGetLastError();
       //closesocket(conn_socket[chCardNo]);
       //fprintf(stderr,"r ecv() failed: error %d\n", retval);
       //OutputDebugString("r ecv() failed: error");
       //WSACleanup();
       return nRECVsErrorCode + 5000000;
      }
   }

 if(uchExpectedMessType == MsTyp2_ZENDstatREC){ //Now using 1 char for message type, & the 2nd char for cMessageSeqNo (checked by calling function)
  //if( (chReceiveBuffer[chCardNo][0] == 0x77) && (chReceiveBuffer[chCardNo][1] == 0x33) ){//if(!lstrcmp(chReceiveBuffer,"Ack5PacksOK")){MessageBox(hDlg,"OK","PC Got'Ack5PacksOK' from AVR",MB_OK);
    if(  chReceiveBuffer[chCardNo][0] == 0x77 ){//if(!lstrcmp(chReceiveBuffer,"Ack5PacksOK")){MessageBox(hDlg,"OK","PC Got'Ack5PacksOK' from AVR",MB_OK);
     
       if(chCardNo == 38){ //Call Playsound when receiving packets only for one motor
          if(bMakeSounds){
           //PlaySound ("Ack.wav",NULL,SND_ASYNC);
           //PlaySound ("beep_09.wav",NULL,SND_ASYNC);
             PlaySound ("IDR_WAVE1_ACK", hInst, SND_RESOURCE | SND_ASYNC); //MakeAcknowledgeSound();
             //CountStatRecsRecvd_TimesPlaySoundCalled++;
             //int CountStatRecsRecvd_TimesPlaySoundCalledNOTUSED;
            }
         }
       return 0;
      }
    else{
       if(hDlg)
         MessageBox(hDlg,(char*)&chReceiveBuffer[chCardNo][0],"Didn't receive a StatusRec: chReceiveBuffer contained:",MB_OK);
       return 123456;
      }
   }
//else if (uchExpectedMessType == (MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDDIFFSFILE)){// equivalent to the line below
 else if (LineSubPacket == 8000){ //This should be equivalent to the above line
    if((nRECVsRetVal[chCardNo] != 1102) || (nActualMessTypeRecvdIn8000 != 257) || (nActualSeqnoExpctdIn8000 != 257)){
       #define SIZszErrMess 75
       char szErrMess[SIZszErrMess];
       StringCchCopy(szErrMess, SIZszErrMess, "Error receiving Positional Differences File from motor number ");
       _itoa_s(chCardNo+1, szErrMess+62, SIZszErrMess - 62, 10);
       if(nRECVsRetVal[chCardNo] != 1102){
          StringCchCopy(mes, SIZMES, "Cannot store file on Windows PC until this is sorted out!\n\nExpected Packet with 1102 bytes, but received ");
          _itoa_s(nRECVsRetVal[chCardNo], mes+105, _MAX_DIR - 105, 10);
         }
       else if(nActualMessTypeRecvdIn8000 != 257){
          StringCchCopy(mes, SIZMES, "Cannot store file on Windows PC until this is sorted out!\n\nExpected Packet with MessType 0x66, but received type 0x");
          _itoa_s(nActualMessTypeRecvdIn8000, mes+115, _MAX_DIR - 115, 16);
         }
       else if(nActualSeqnoExpctdIn8000 != 257){
          StringCchCopy(mes, SIZMES, "Cannot store file on Windows PC until this is sorted out!\n\nExpected SeqNo ");
          _itoa_s(nActualSeqnoExpctdIn8000, mes+74, SIZMES - 74, 10);
          StringCchCat(mes, SIZMES, " but received SeqNo ");
          int nlenMes = lstrlen(mes);
          _itoa_s(nActualSeqnoRecvdIn8000, mes+nlenMes, SIZMES - nlenMes, 10);
         }
       MessageBox(hDlg,mes,szErrMess,MB_OK);
       return 1;//or a better value
      }
  //No errors, so File  received ok.  Just Write it. Is much quicker if done as binary.
  //_itoa_s(SoHowManyTimesDoesThisLoop, mes, SIZMES, 10);
  //MessageBox(hDlg,mes,"... and SoHowManyTimesDoesThisLoop was: ",MB_OK);
    nSizPixDiffsAr = 4400000; //There are 2000 rows of 2200 ints (8.8 Mbytes)  MAX_NOOFLINES *

    //StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
    StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szTimeStampedFolderName);
    if(bSaveDiffFileAsBinary){
       StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_BIN_OUTFILENAMEPDIFFFILE[chCardNo]);
       mySave_File (hDlg, 31313131, "bin",0, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
       MakeAcknowledgeSound(); //With the csv (below) it's easy to see when the file is saved because it is opened in an editor. with the bin, a sound helps
      }
    else{
       StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPDIFFFILE[chCardNo]);
       mySave_File (hDlg, 31313131, "csv",TYPE1CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
      }
    return 0;
   }
 else if(uchExpectedMessType == (MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDPOSLINE)){
    if( (chReceiveBuffer[chCardNo][0] == 0x88) && (chReceiveBuffer[chCardNo][1] == 0x22) ){
       unsigned int *pInt = (unsigned int*)&chReceiveBuffer[chCardNo][2];        //NOTE 32 bit ints are used for sending absolute positions
       MakeAcknowledgeSound();
       LineSubPacket *= 275;
       for(int j = 0; j < 275; j++){  //Currently I'll send the 2200 positions as 8 packets of 275.  For 9000 byte Jumbo Frames, I had "for(int j = 0; j < 2200; j++){" & no LineSubPacket
          endian_swap(*pInt);
          nPixPositionsAr[LineSubPacket + j] = *pInt++;
         }
       if(LineSubPacket == 1925){  //i.e. LineSubPacket was 7
          nSizPixPositionsAr = 2200;
          StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
          StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPPOSLINE[chCardNo]);
          //missing somut??? (like motor number?)
//szLocalBDMPfileName[LenStartOfFname] = '0'; //append "01", "02" etc. to the filename
//_itoa_s(globDumpBuff, szLocalBDMPfileName+LenStartOfFname+1, _MAX_DIR, 10);
/*        int LenStartOfFname = lstrlen(szRMPsLocalFileName);
          if(chCardNo < 9){
             szRMPsLocalFileName[LenStartOfFname] = '0'; //append "01", "02" etc. to the filename
             _itoa_s(chCardNo+1, szRMPsLocalFileName+LenStartOfFname+1, _MAX_DIR - LenStartOfFname - 1, 10);
            }
          else
            _itoa_s(chCardNo+1, szRMPsLocalFileName+LenStartOfFname, _MAX_DIR - LenStartOfFname, 10);    //append "10", "11" etc. to the filename  */
          mySave_File (hDlg, 100000, "csv",TYPE2CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
         }
       return 0;
      }
    else{ //It's not a status rec (uchExpectedMessType == M sTyp2_ZENDstatREC) and it's not MsTyp9_SENDpixPOS_TABLE (chReceiveBuffer[chCardNo][0] == 0x88) && (chReceiveBuffer[chCardNo][1] == 0x22) 
       if(hDlg) //Possibly an erroneous response to a status request or pixPOSITIONs request (or just noise!)
         MessageBox(hDlg,(char*)&chReceiveBuffer[chCardNo][0],"Didn't receive a pixPOSITIONsTAB+SUBTYPSENDPOSLINE: chReceiveBuffer contained:",MB_OK);
       return 12345;
      }
   }
 else if(uchExpectedMessType == (MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDDIFFSLINE)){
  //if( (chReceiveBuffer[chCardNo][0] == 0x66) && (chReceiveBuffer[chCardNo][1] == 0x44) ){
    if(  chReceiveBuffer[chCardNo][1100] == 0x66){ //&& (chReceiveBuffer[chCardNo][1] == 0x44) ){
       unsigned short *pSht = (unsigned short*)&chReceiveBuffer[chCardNo][0];
       MakeAcknowledgeSound();
       LineSubPacket *= 550;
       for(int j = 0; j < 550; j++){  //Currently I'll send the 2200 positions as 8 packets of 275.  For 9000 byte Jumbo Frames, I had "for(int j = 0; j < 2200; j++){" & no LineSubPacket
          short_endian_swap(*pSht);
          sPixDiffsAr[LineSubPacket + j] = *pSht++;
         }
       if(LineSubPacket == 1650){  //i.e. LineSubPacket was 3
          nSizPixDiffsAr = 2200;
          StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
          StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPDIFFLINE[chCardNo]);
/*        int LenStartOfFname = lstrlen(szRMPsLocalFileName);
          if(chCardNo < 9){
             szRMPsLocalFileName[LenStartOfFname] = '0'; //append "01", "02" etc. to the filename
             _itoa_s(chCardNo+1, szRMPsLocalFileName+LenStartOfFname+1, _MAX_DIR - LenStartOfFname - 1, 10);
            }
          else
            _itoa_s(chCardNo+1, szRMPsLocalFileName+LenStartOfFname, _MAX_DIR - LenStartOfFname, 10);    //append "10", "11" etc. to the filename
*/
          mySave_File (hDlg, 313131, "csv",TYPE1CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
         }
       return 0;
      }
    else{ //It's not a status rec (uchExpectedMessType == M sTyp2_ZENDstatREC) and it's not MsTyp9_SENDpixPOS_TABLE (chReceiveBuffer[chCardNo][0] == 0x88) && (chReceiveBuffer[chCardNo][1] == 0x22) 
       if(hDlg) //Possibly an erroneous response to a status request or pixPOSITIONs request (or just noise!)
         MessageBox(hDlg,(char*)&chReceiveBuffer[chCardNo][0],"Didn't receive a pixPOSITIONsTAB+SUBTYPSENDDIFFSLINE: chReceiveBuffer contained:",MB_OK);
       return 12345;
      }
   }
 else if(uchExpectedMessType == (MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDFREQTABLE)){
  //if( (chReceiveBuffer[chCardNo][0] == 0x66) && (chReceiveBuffer[chCardNo][1] == 0x44) ){
    if(  chReceiveBuffer[chCardNo][1100] == 0x33){ //&& (chReceiveBuffer[chCardNo][1] == 0x44) ){
       unsigned int *pInt = (unsigned int*)&chReceiveBuffer[chCardNo][0];
       MakeAcknowledgeSound();
     //LineSubPacket *= 550;
       int FourthReading = chCardNo*4 + 4;
       for(int j = FourthReading - 4; j < FourthReading; j++){ //There are 4 positions in each packet
          endian_swap(*pInt);
          nPixFreqsAr[j] = *pInt++;
         }
//     if(LineSubPacket == 0){  //i.e. LineSubPacket was 0
//        StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
//        StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[chCardNo]);
//        mySave_File (hDlg, 123, "csv",TYPE4CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
//       }
       return 0;
      }
/*else if(uchExpectedMessType == (MsTyp9_SENDpixPOS_TABLE+SUBTYPSENDFREQTABLE)){
//The following writes a file for each motor:
    if(  chReceiveBuffer[chCardNo][1100] == 0x33){ //&& (chReceiveBuffer[chCardNo][1] == 0x44) ){
       unsigned int *pInt = (unsigned int*)&chReceiveBuffer[chCardNo][0];
       MakeAcknowledgeSound();
     //LineSubPacket *= 550;
       for(int j = 0; j < 4; j++){  //Currently I'll send the 4 positions in 1 packet
          endian_swap(*pInt);
          nPixFreqsAr[j] = *pInt++;
         }
       if(LineSubPacket == 0){  //i.e. LineSubPacket was 0
          StringCchCopy(szRMPsLocalFileName,_MAX_DIR, HCFcfs.szCSVDirName4Writing);
          StringCchCat(szRMPsLocalFileName,_MAX_DIR,TabDEFAULT_CSV_OUTFILENAMEPIX_FREQS_FILE[chCardNo]);
          mySave_File (hDlg, 123, "csv",TYPE4CSV, szRMPsLocalFileName, MOTORNO_NOT_USED);//It should be ok to call this even if hDlg is set to NULL.  Data Collected from UC3 is being saved here.
         }
       return 0;
      }*/
    else{ //It's not a status rec (uchExpectedMessType == M sTyp2_ZENDstatREC) and it's not MsTyp9_SENDpixPOS_TABLE (chReceiveBuffer[chCardNo][0] == 0x88) && (chReceiveBuffer[chCardNo][1] == 0x22) 
       if(hDlg) //Possibly an erroneous response to a status request or pixPOSITIONs request (or just noise!)
         MessageBox(hDlg,(char*)&chReceiveBuffer[chCardNo][0],"Didn't receive a pixPOSITIONsTAB+SUBTYPSENDFREQTABLE: chReceiveBuffer contained:",MB_OK);
       return 12345;
      }
   }
 MessageBox(hDlg,"R eceiveMessagePackets() called with unexpectedMessType",NULL,MB_OK);
 return 12647;
}//End of R eceiveMessagePackets()

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   SafelyDisplayErrorAndCleanUp ()                             ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
void SafelyDisplayErrorAndCleanUp(HWND hDlg, TCHAR *pMess2Display, int retVal){ //Note the WSACleanup has now been removed (despite the name of the function!)
 if(hDlg){
    int const arraysize = 70;
    TCHAR pszDest[arraysize];
    size_t cbDest = arraysize * sizeof(TCHAR);
    LPCTSTR pszFormat = TEXT("%s %d.");
    HRESULT hr = StringCbPrintf(pszDest, cbDest, pszFormat, pMess2Display, retVal);
    if(hr == S_OK)
      MessageBox(hDlg, pszDest, NULL, MB_OK);
    else
      MessageBox(hDlg, "SafelyDisplayErrorAndCleanUp() failed when calling StringCbPrintf()!!", NULL, MB_OK);
   }
 //WSACleanup(); forget Cleanup. This is now handled when the program exits (now that Sockets are supposed to remain open for the duration).
 return;
}

#define SIZserver_name 70
char server_name[SIZserver_name]; //stores Atmel's IP address, but may have a message appended if connect() fails


/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   InitialiseAllSockets ()                                     ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
//In InitialiseAllSockets() no messages are sent. Initialisation of all the sockets should take place (they should all
//  open and connect) then the function returns.  All sockets to remain open for the duration of the program.
int InitialiseAllSockets(HWND hWnd){
 int IASretval; //InitialiseAllSocketsretval
 unsigned int addr;//
 struct sockaddr_in server;
//struct hostent *hp;
 WSADATA wsaData;
 unsigned short port = DEFAULT_PORT;  //currently DEFAULT_PORT 1058  //This is 0x422

 for(int eaMotor =0; eaMotor < NOOFMOTORS; eaMotor++)
   bWaitingForReply[eaMotor] = 0; //Initialise - was in Winproc's WM_CREATE

 if ((IASretval = WSAStartup(0x202,&wsaData)) != 0) {
    SafelyDisplayErrorAndCleanUp(hWnd,TEXT("WSAStartup failed with error"),IASretval);
    return IASretval + 1000000; //I'll stick with adding these large numbers - It will make it easier to see where any error has come from
   }
 for(int nCardNo = 0; nCardNo < NOOFMOTORS; nCardNo++){
    StringCchCopy(server_name,SIZserver_name,PCFrec.ATMEL_IP_ADDRESStab[nCardNo]);
    addr = inet_addr(server_name);
    memset(&server,0,sizeof(server));
    memcpy(&(server.sin_addr),&addr,sizeof(addr));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    conn_socket[nCardNo] = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); /* Open a socket... socket_type is just an int that has been assigned SOCK_DGRAM (so now I've put in SOCK_DGRAM)*/
    if (conn_socket[nCardNo] < 0 ) {
       IASretval = WSAGetLastError();
       SafelyDisplayErrorAndCleanUp(hWnd,TEXT("Client: Error Opening socket: Error"),IASretval);
       return IASretval + 2000000;
      }
    // Notice that nothing in this code is specific to whether we
    // are using UDP or TCP.
    // We achieve this by using a simple trick.
    //    When connect() is called on a datagram socket, it does not
    //    actually establish the connection as a stream (TCP) socket
    //    would. Instead, TCP/IP establishes the remote half of the
    //    ( LocalIPAddress, LocalPort, RemoteIP, RemotePort) mapping
    //    This enables us to use s end() and r ecv() on datagram sockets,
    //    instead of r ecvfrom() and sendto()
    //printf("Client connecting to: %s\n",server_name);
    //According to the documentation "If no error occurs, connect returns zero. Otherwise, it returns 
    //SOCKET_ERROR, and a specific error code can be retrieved by calling WSAGetLastError."
    //That gives me the choice of either
    //if (connect(conn_socket,(struct sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
    //or
    if ( connect(conn_socket[nCardNo], (struct sockaddr*)&server, sizeof(server)) ) {
       IASretval = WSAGetLastError();
       //closesocket(conn_socket[nCardNo]);
       StringCchCat(server_name,sizeof(server_name)," <- connect() to this IP failed, Return Code:");
       SafelyDisplayErrorAndCleanUp(hWnd,server_name,IASretval);
       WSACleanup();
       return IASretval + 3000000;
      }
    int iOptVal; //CAN I INCREASE THE Socket's RECEIVE BUFFER SIZE, AND  DOES IT HELP?????  Yes and Yes
    int iOptLen = sizeof(int);
    if (getsockopt(conn_socket[nCardNo], SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal, &iOptLen) == SOCKET_ERROR){
       IASretval = WSAGetLastError();
       closesocket(conn_socket[nCardNo]);
       StringCchCat(server_name,sizeof(server_name)," <- getsockopt() to this IP failed, Return Code:");
       SafelyDisplayErrorAndCleanUp(hWnd,server_name,IASretval);
       WSACleanup();
       return IASretval + 4000000;
      }
      //printf("SO_KEEPALIVE Value: %ld\n", iOptVal);
    iOptVal = 9000000;
    if (setsockopt(conn_socket[nCardNo], SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal, iOptLen) == SOCKET_ERROR){
       IASretval = WSAGetLastError();
       closesocket(conn_socket[nCardNo]);
       StringCchCat(server_name,sizeof(server_name)," <- setsockopt() to this IP failed, Return Code:");
       SafelyDisplayErrorAndCleanUp(hWnd,server_name,IASretval);
       WSACleanup();
       return IASretval + 5000000;
      }
    if (getsockopt(conn_socket[nCardNo], SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal, &iOptLen) == SOCKET_ERROR){
       IASretval = WSAGetLastError();
       closesocket(conn_socket[nCardNo]);
       StringCchCat(server_name,sizeof(server_name)," <- getsockopt() to this IP failed, Return Code:");
       SafelyDisplayErrorAndCleanUp(hWnd,server_name,IASretval);
       WSACleanup();
       return IASretval + 6000000;
      }
    if(iOptVal != 9000000){
       closesocket(conn_socket[nCardNo]);
       WSACleanup();
       MessageBox(hWnd, "setsockopt() failed to set buffer to 9,000,000 bytes for all sockets", NULL, MB_OK);
       return 1;
      }
   }
 return 0;
}

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   DestroyAllSockets ()                                        ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
int DestroyAllSockets(void){
 for(int nCardNo = 0; nCardNo < NOOFMOTORS; nCardNo++)
   closesocket(conn_socket[nCardNo]);
 WSACleanup();
 return 0;
}

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   InitialiseAllReceiveBuffers ()                              ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
void InitialiseAllReceiveBuffers(void){
//DumpReceiveBuffers2File("C:\\DumpPlace1Buff");
//chReceiveBuffer[0][0] = 'H';
//chReceiveBuffer[0][1] = 'e';
//chReceiveBuffer[0][2] = 'H';   The 3 commented out calls to 
//chReceiveBuffer[0][3] =   0;     DumpReceiveBuffers2File() in this
//chReceiveBuffer[ 1][0] = 'H';      function showed we probably don't need
//chReceiveBuffer[1][1] = 'e';       to call memset() below to initialise
//chReceiveBuffer[1][2] = 'l';        the buffers, but as it takes
//chReceiveBuffer[1][3] = 'l';         virtually no time, I will anyway.
//chReceiveBuffer[1][4] = 'o';
//chReceiveBuffer[1][5] =   0;
//chReceiveBuffer[4][1] = 'g';
//chReceiveBuffer[4][2] = 'H';
//chReceiveBuffer[4][3] =   0;
//DumpReceiveBuffers2File("C:\\DumpPlace2Buff");
 for(int recbuff = 0; recbuff < NOOFMOTORS; recbuff++)
   memset(&chReceiveBuffer[recbuff][0],0, SIZ_REC_BUFF);
 //DumpReceiveBuffers2File("C:\\DumpPlace3Buff");
}

/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
/***** ***** ***** ***** *****                                      FUNCTION   DumpReceiveBuffers2File ()                                  ***** ***** ***** ***** *****/
/***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****/
//This is really a debugging utility to see if the contents of all the 'receive buffers' are getting valid data
void DumpReceiveBuffers2File(char *Name4Files){     //e'g. "C:\\DumpPlace1Buff"  gives 40 files called "C:\\DumpPlace1Buff01.dmp" to "C:\\DumpPlace1Buff40.dmp"
 char szLocalBDMPfileName[26];
 StringCchCopy(szLocalBDMPfileName, 26, Name4Files);//& "C:\\DumpPlace2Buff"  gives 40 files called "C:\\DumpPlace2Buff01.dmp" to "C:\\DumpPlace2Buff40.dmp"
 int LenStartOfFname = lstrlen(szLocalBDMPfileName);
 for(globDumpBuff = 1; globDumpBuff <= NOOFMOTORS; globDumpBuff++){
  //szLocalBDMPfileName[LenStartOfFname] = 0; //probably pointless:
    if(globDumpBuff < 10){
       szLocalBDMPfileName[LenStartOfFname] = '0'; //append "01", "02" etc. to the filename
       _itoa_s(globDumpBuff, szLocalBDMPfileName+LenStartOfFname+1, 26 - LenStartOfFname - 1, 10);
      }
    else
      _itoa_s(globDumpBuff, szLocalBDMPfileName+LenStartOfFname, 26 - LenStartOfFname, 10);    //append "10", "11" etc. to the filename
  //StringCchCat(szLocalBDMPfileName, _MAX_DIR, ".bdmp"); //Let mySave_File() append the suffix
    if(mySave_File (NULL, 15000, "bdmp",0, szLocalBDMPfileName, MOTORNO_NOT_USED))
      return; //Something went wrong
   }
}
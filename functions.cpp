#include "StdAfx.h"
//                                         FUNTIONS.CPP
//This file contains:
//  m yOpen_File()  //Opens and Reads various file types to a buffer 
//  m ySave_File()  //Opens and Writes various file types from a buffer                   (now defined in mySaveFile.cpp)
//  m GetFile()     //Windows Dialog Box to obtain a file-name from the user              (now defined in mGetFileNhook.cpp)
//Associated functions:
//  D riveSpaceShort()  //Test that a disk drive has enough space for saving a file
//  G OFNHookFunction() //Part of Windows Dialog Box handling for obtaining a file-name   (now defined in mGetFileNhook.cpp)
//  D isplayErrorMessageBoxWithErrorAndLineNo() reports errors when reading files

//Miscellaneous functions:
//  M akeErrorSound()
//  M akeWarningSound()
//  M akeAcknowledgeSound()
//  W arnIfSoundFileIsMissing()
//  r edraw()
//  S tripTrailingSpaces()
//  m yWaitForAVRdelay()

#define SIZ_CSV_CALHEADER   168 //4 * 40 + 4 * cr/lf pairs

#define SIZ_CSV_SHORT_LINE      8 //includes the cr/lf pair
#define SIZ_CSV_LONG_LINE      13 //includes the cr/lf pair
#define SIZ_PRN_LINE           10

extern HINSTANCE hInst;      // current instance
extern CRegStdString EditorForViewing;

char chCSVHeaderLine[SIZ_CSV_HEADERp4]; //when reading the binary version of the file, a 32-bit number follows the header (describing how long the rest of the file is)
extern HCFRECORD HCFrec;

//pcf file consists of: ATMEL_IP_ADDRESStab AND chNoofUsteps4EachMotorTab AND nNoofUsteps4EachApertureTab AND nNoofUsteps4EachFocusTab AND nNoofUsteps4RefBeamTablePrintUnload
//extern char ATMEL_IP_ADDRESStab[NOOFMOTORS][16];
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS];
//extern int nNoofUsteps4EachApertureTab[9];
//extern int nNoofUsteps4EachFocusTab[3];
//extern int nNoofUsteps4RefBeamTablePrintUnload[4];
extern PCFRECORD PCFrec;
extern ADDITIONALarpsSTRUC aARPstrc;

extern BOOL bMoveNowRadioButPressed;
//extern BOOL bReallyThisIsAnAccelProfile;
extern char mes[];

extern char *pMxPacketBuffAr[];
extern char CopyUC3Buff[SIZ_8832000];
unsigned int nSeqNo;
unsigned int uiTableOfCheckSums[NOOFMOTORS];

extern int nAccProfile[];
extern CRegStdWORD   bDoProperCheckSumChecks_MposFiles;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;

extern int nWavPlate1PcentSplitRGBAr[];

extern int   nPixPositionsAr[];  //2200 (Just one Line)
extern int   nSizPixPositionsAr;
extern short sPixDiffsAr[];      //2200 * 2000
extern int   nSizPixDiffsAr;

extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];
extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

extern HWND hGLWnd;
extern BOOL bMakeSounds;
//File Scope:
static RECT rClient  = {172,941,247,981};
static RECT rPIDMsgsRect = {PID_RECT_L, PID_RECT_R, PID_RECT_T, PID_RECT_B};
static RECT rNoofReadsToAverageRECT = {787,992,822,1012};

static char *pMxPacketBuff;  //Motor X's Packet Buffer

void DisplayErrorMessageBoxWithErrorAndLineNo(HWND, bool bRangeError, int nErrorLineNum, const char *pszFile2Display, int nFileToClose); //Function Prototype
void DisplayCALErrorMessageBoxWithErrorAndLineNo(HWND, bool bRangeError, int nErrorLineNum, const char *pszFile2Display); //Function Prototype
void DisplayPRNErrorMessageBoxWithErrorAndLineNo(HWND hWnd, bool bRangeError, int nErrorLineNum, const char *pszFile2Display); //Function Prototype

//Returns 1 if couldnt open, 2 if unsupported filetype, & zero for success.
//  If the file does not get read succesfully, a message is displayed before returning
int myOpen_File (HWND hWnd, const char *fType, const char *pszFile2Open, int nOFmotorNo){ //nOFmotorNo used only for csv/csb/cse files
//FILE *open_file;
 int open_file;   // The file descriptor
 int returncode;
 int nCharsReadIn;
 errno_t err;
 returncode = 0;
 err = _sopen_s(&open_file, pszFile2Open, O_BINARY, _SH_DENYNO, _S_IREAD);
 if( err ){
    myLoadString(STR_1); //Unable to open and read file...\n\n"
    StringCchCat(mes,SIZMES,pszFile2Open);
    MessageBox (hWnd,mes,pszFile2Open,MB_OK);
    return 1;
   }
 if (_filelength(open_file) == -1){
    char MissFileMess[SIZMissFileMess];
    StringCchCopy(MissFileMess,SIZMissFileMess,"Unable to find file: ");
    StringCchCat(MissFileMess,SIZMissFileMess,pszFile2Open);
    StringCchCat(MissFileMess,SIZMissFileMess," to open it!");
    MessageBox(hWnd,MissFileMess,NULL,MB_OK);
    return 1;
   }
 if( !lstrcmp(fType,"csv") ){
//The file structure to be read by this routine requires that the file contains 2200 fixed length
// lines of 8 characters (including the cr/lf pair). Each line must contain one short integer, optionally
// preceded by a minus sign, with leading spaces. e.g.
//     8<cr><lf>
//-32000<cr><lf>
//... etc
//The minimun negative number is -32768 (was -2,147,483,648)
//The maximum positive nuber  is  32767 (was  2,147,483,647)
//The 2200 (+ 2 junk) numbers are read in as 3 seperate blocks of 734 numbers, and converted to short ints ready
// for transmission as packets on the local area network.  Each Packet contains 1468 bytes of data
// plus a 22 byte header (+ WinSock's 42 byte header).  As many packets are put together in one contiguous buffer
// (which contains many headers), each block is read in at a different offset within the multiple packet buffer.
    char chCSVline[SIZ_CSV_SHORT_LINE];
    BOOL bReallyThisIsAnAccelProfile;
    char cFlags;
    int nNoofCSVLines = 0;
    int nNoofHomingAccelPixels = 0;
    int nNoofLinesToRead;
    int   nTypeOneMessageLength;
    short sTypeOneMessageLengthOrLineNo;
    unsigned short *psInt;
    unsigned short *pUC3copy;// = (unsigned short*)CopyUC3Buff;
    errno  = 0;
    nSeqNo = 0;
    //int WarningThisHasStoppedRangeChecking;  //No check is made the the numbers read do actually fit into the 16-bit range
    pMxPacketBuff = pMxPacketBuffAr[nOFmotorNo]; //Get pointer to the buffer where the data will be stored
    memset(pMxPacketBuff,0,SIZ_8832000);
/****************************************************************************************************************************************************
*                                                                 READ header
*****************************************************************************************************************************************************
*                                                                 READ header
*****************************************************************************************************************************************************
*                                                                 READ header
*****************************************************************************************************************************************************/
    if ((nCharsReadIn = _read(open_file,chCSVHeaderLine, SIZ_CSV_HEADER)) == SIZ_CSV_HEADER ){
       nNoofCSVLines = 5; //We've consumed 5 rows of the CSV file
       nNoofLinesToRead = atoi(chCSVHeaderLine + 168);
       if (errno || (nNoofLinesToRead > MAX_NOOFLINES) || (nNoofLinesToRead < 1) ){ //report atoi error (or if sTypeOneMessageLengthOrLineNo out of bounds)
          DisplayErrorMessageBoxWithErrorAndLineNo(hWnd, errno == ERANGE? 1:0, nNoofCSVLines, pszFile2Open, open_file);//The function also closes 'open_file'
          return 1;
         }
       if((chCSVHeaderLine[42] == 'A') && (!memcmp(chCSVHeaderLine+42, "ACCELERATION PROFILE", 20))){
          bReallyThisIsAnAccelProfile = TRUE;
          if(nNoofLinesToRead != 1){
              MessageBox (hWnd, "Error: nNoofLinesToRead (at offset 168) does not equal 1 for the above-named file!\nAcceleration files must have just one line of motor positions",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
          nNoofHomingAccelPixels = atoi(chCSVHeaderLine + 105); //For normal MPOS files, the number present at this offset (after Cols: ) would be 1 greater than the number in movements to be read in
          //(see 'nTypeOneMessageLength' below).  However, As this is an Acceleration Profile (which may be used either for Homing or for Fast-Moving) we have two acceleration lengths to deal with... the full
          // 100 pixel-period accelerations we might use when moving quickly between 2 known points (e.g. during "Move to Start Positions"), and the short acceleration length when homing (required because we
          // need a short decelleration when the home switch is detected). I'll change the words Cols: in the header to HAPs (Homing Acceleration Periods [or Pixels]) and require it to be in the range 1 to 100
          if( (nNoofHomingAccelPixels < 1) ||(nNoofHomingAccelPixels > 100) ){
              MessageBox (hWnd, "Error: HAPs (at offset 105) in the above-named file must\nbe in the range 1 to 100",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
         }
       else
         bReallyThisIsAnAccelProfile = FALSE;
      }           // for each line (i.e. is in every 3rd packet [when the lines are long enough to need 3 packets / line])
    else{
       MessageBox (hWnd, "Error: Unable to read the file header for the above-named file!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
    if((chCSVHeaderLine[164] == '1') && (chCSVHeaderLine[165] == '0'))
      ;//chPixFreqFlags = B'xxxx00xx';
    else if((chCSVHeaderLine[164] == '1') && (chCSVHeaderLine[165] == '5'))
      ;//chPixFreqFlags = B'xxxx01xx';
    else if((chCSVHeaderLine[164] == '3') && (chCSVHeaderLine[165] == '0'))
      ;//chPixFreqFlags = B'xxxx10xx';
    else if((chCSVHeaderLine[164] == '6') && (chCSVHeaderLine[165] == '0'))
      ;//chPixFreqFlags = B'xxxx11xx';
    else{
       MessageBox (hWnd, "Illegal PixFreq found in last 2 characters of fourth line\nMust be 10, 15, 30 or 60 [Hz]!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }

    memset(CopyUC3Buff,0,SIZ_8832000);

  //This block starts by reading in the first record (i.e. 'nTypeOneMessageLength' - describing how many Motor Positions will follow for this line)
/****************************************************************************************************************************************************
*                                                                 START of File Body
*****************************************************************************************************************************************************
*                                                                 START of File Body
*****************************************************************************************************************************************************
*                                                                 START of File Body
*****************************************************************************************************************************************************/
    unsigned short sNoofLines, sNoofMPositionsInThisPacket; //732 for First Packet for the line, 734 for 2nd & 3rd Packets for the line
    psInt = (unsigned short *)&sTypeOneMessageLengthOrLineNo;
    for(int nnLineNo = 0; nnLineNo < nNoofLinesToRead; nnLineNo++){
       if ((nCharsReadIn = _read(open_file,chCSVline, SIZ_CSV_SHORT_LINE)) == SIZ_CSV_SHORT_LINE ){
          nNoofCSVLines++; //We've consumed one row of the CSV file
          nTypeOneMessageLength = atoi(chCSVline);
          if (errno || (nTypeOneMessageLength > MAXNOOFPIXELS_IN_ROW) || (nTypeOneMessageLength < 1) ){ //report atoi error (or if sTypeOneMessageLengthOrLineNo out of bounds)
             DisplayErrorMessageBoxWithErrorAndLineNo(hWnd, errno == ERANGE? 1:0, nNoofCSVLines, pszFile2Open, open_file);//nNoofCSVLines was nnLineNo * 2201 + 1 //The function also closes 'open_file'
             return 1;
            }
          //Put MessageLength in This Packet's bytes 4 & 5 (The first packet for a line always has a double-header [4 bytes + 4 bytes]).  Remember MessageLength 
          // is how many pixels this row has (i.e. 0 - 2200). Could use for deciding how many packets will follow (for this line): <= 732 is none, <= 1436 is 1, else 2
          sTypeOneMessageLengthOrLineNo = (short)nTypeOneMessageLength;
          short_endian_swap(*psInt); // psInt points to sTypeOneMessageLengthOrLineNo
          *(short*)&pMxPacketBuff[PACKETLENGTH*nSeqNo + 4] = *psInt;
          //Put LineNo in This Packet's bytes 6 & 7 (As this packet has the double-header)
          sTypeOneMessageLengthOrLineNo = (short)nnLineNo;
          short_endian_swap(*psInt); // psInt points to sTypeOneMessageLengthOrLineNo
          *(short*)&pMxPacketBuff[PACKETLENGTH*nSeqNo + 6] = *psInt;
         }           // for each line (i.e. is in every 3rd packet [when the lines are long enough to need 3 packets / line])
       else{
          int nLength_mes;
          StringCchCopy(mes,SIZMES,"Open ");
          StringCchCat(mes,SIZMES,pszFile2Open);
          StringCchCat(mes,SIZMES," and look at line ");
          nLength_mes = lstrlen(mes);
          _itoa_s(nNoofCSVLines, mes + nLength_mes, SIZMES - nLength_mes, 10);
          MessageBox (hWnd, mes, "Error: Unable to read the 'Noof Pixels On Row' field!",MB_OK);
          _close(open_file);
          return 1;
         }
/*******************************************************************MIDDLE***************************************************************************/
       unsigned short *pUS;
       int nNoof_Pkts_per_line;
       sNoofMPositionsInThisPacket = 732;
       if(nTypeOneMessageLength < 733)
         nNoof_Pkts_per_line = 1;
       else if(nTypeOneMessageLength < 1467)
         nNoof_Pkts_per_line = 2;
       else
         nNoof_Pkts_per_line = 3;
       for (int nPacketNoInThisLine = 0; nPacketNoInThisLine < nNoof_Pkts_per_line; nPacketNoInThisLine++){
          //This section is executed once per packet:
          sNoofLines = 0;
          pMxPacketBuff[PACKETLENGTH*nSeqNo] = 1;               //... ADD MessageType
          cFlags = (char)bMoveNowRadioButPressed;                     //... ADD Move Now Flag. At the moment this applies to the whole file (but that's down to AVR)
          if(!nPacketNoInThisLine)                              //... ADD Flag to say if this is the First Packet for the line.
            cFlags |= 2;                                        //Note bits 2, 3, 4 and 5 are set below, e.g. see:  pMxPacketBuff[1] &= 0xf3;  //clear both bits
          pMxPacketBuff[PACKETLENGTH*nSeqNo + 1] = cFlags;
          sTypeOneMessageLengthOrLineNo = (short)nSeqNo;
          short_endian_swap(*psInt); // psInt points to sTypeOneMessageLengthOrLineNo
          *(unsigned short*)&pMxPacketBuff[PACKETLENGTH*nSeqNo + 2] = sTypeOneMessageLengthOrLineNo; //... ADD nSeqNo. nSeqNo relates to packets NOT to Lines
          //Now read the each block of 732,734 & 734 numbers into the appropriate place in each packet of data (i.e at offset 8 for first pkt, and offset 4 for pkts 2&3
          if(sNoofMPositionsInThisPacket == 732){
             pUS = (unsigned short*)&pMxPacketBuff[PACKETLENGTH * nSeqNo + 8];//The first packet for every line effectively has a 4 byte header +
             if(nNoof_Pkts_per_line == 1)
               pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * nSeqNo]; //2200 * nSeqNo;
             else if(nNoof_Pkts_per_line == 2)
               pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * (nSeqNo / 2)];
             else //(nPacketNoInThisLine == 3)
               pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * (nSeqNo / 3)];
            }
          else{                                                               // another 4 byte header (containing LineNumber and sTypeOneMessageLength)
             pUS = (unsigned short*)&pMxPacketBuff[PACKETLENGTH * nSeqNo + 4];//The next 2 packets for each line have just the a 4 byte header (MessType, MoveNow, SeqNo)
             if(nNoof_Pkts_per_line == 2)
               pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * (nSeqNo / 2) + 1464];   //1464 == 732 * 2
             else //(nNoof_Pkts_per_line == 3)
               if(nPacketNoInThisLine == 1)       //2nd Packet of 3
                 pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * (nSeqNo / 3) + 1464]; //1464 == 732 * 2
               else //(nPacketNoInThisLine == 2)  //3rd Packet of 3
                 pUC3copy = (unsigned short*)&CopyUC3Buff[4400 * (nSeqNo / 3) + 2932]; //2932 == (732 * 2) + (734 * 2)
            }
          while ( (sNoofLines++ < sNoofMPositionsInThisPacket) &&
                   nTypeOneMessageLength-- &&
                 ((nCharsReadIn = _read(open_file,chCSVline, SIZ_CSV_SHORT_LINE)) == SIZ_CSV_SHORT_LINE) ){
             nNoofCSVLines++;
             *pUS = (unsigned short)atoi(chCSVline);
             short_endian_swap(*pUS);
             *pUC3copy++ = *pUS;
             pUS++;
             if (errno){ //report atoi error
                DisplayErrorMessageBoxWithErrorAndLineNo(hWnd, errno == ERANGE? 1:0, nNoofCSVLines, pszFile2Open, open_file);//The function also closes 'open_file'
                return 1;
               }
            }
          if(nCharsReadIn != SIZ_CSV_SHORT_LINE){
             _close(open_file);
             MessageBox (hWnd, "nCharsReadIn != SIZ_CSV_SHORT_LINE!\n\nPerhaps there's no CR/LF on the final line?\n\n(or just too few records)",pszFile2Open,MB_OK);
             return 1;
            }
          sNoofMPositionsInThisPacket = 734;
          nSeqNo++; //Move on to the next packet
          if(nSeqNo > 6600){
             _close(open_file);
             MessageBox (hWnd, "CSV file processing error: nSeqNo > 6600!\n\nThere should not be this many packets in the file.",pszFile2Open,MB_OK);
             return 1;
            }
         }
      } //for(int nnLineNo = 0; nnLineNo < NOOFLINES2SEND; nnLineNo++)
  //Having finished, Overwrite the very first Packet's Line-Number (bytes 6,7) with the Total Number of Packets (minus 1). (We know that packet 0 is line 0,
    sTypeOneMessageLengthOrLineNo = (short)nSeqNo - 1; // so no information is lost). It is minus 1 to allow AVR to compare with Sequence Number.
    short_endian_swap(*psInt); // psInt points to sTypeOneMessageLengthOrLineNo
    *(short*)(&pMxPacketBuff[6]) = *psInt;
  //Also Insert the Pixel Frequency into cFlags bit 2 & 3 (Only for the first packet)
    if((chCSVHeaderLine[164] == '1') && (chCSVHeaderLine[165] == '0'))
      pMxPacketBuff[1] &= 0xf3;  //clear both bits
    else if((chCSVHeaderLine[164] == '1') && (chCSVHeaderLine[165] == '5')){
       pMxPacketBuff[1] &= 0xf7; //clear bit 3
       pMxPacketBuff[1] |= 0x4;  //  set bit 2
      }
    else if((chCSVHeaderLine[164] == '3') && (chCSVHeaderLine[165] == '0')){
       pMxPacketBuff[1] |= 0x8;  //  set bit 3
       pMxPacketBuff[1] &= 0xfb; //clear bit 2
      }
    else if((chCSVHeaderLine[164] == '6') && (chCSVHeaderLine[165] == '0'))
      pMxPacketBuff[1] |= 0xc; //set both bits
    else{
       MessageBox (hWnd, "Illegal PixFreq found in last 2 characters of fourth line\nMust be 10, 15, 30 or 60 [Hz]!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
    if(bReallyThisIsAnAccelProfile){ //After receiving this dummy line of movement (which is actually the acceleration profile to use when homing, and for Moving to Start Positions), AVR should store it store it somewhere else
       pMxPacketBuff[1] |= 0x10;     //set bit 4    //or, at least, *could* store it somewhere else. JL requested the file be sent everytime - slower, but saves Windows keeping track of whether the AVR's copy is valid or not.
       //bReallyThisIsAnAccelProfile = 0; caller should do this. NOPE this now local, and set by "ACCELERATION PROFILE" in the file's header
       if(bDoProperCheckSumChecks_AccelProfs) //This is currently overridden when sending packets.
         pMxPacketBuff[1] |= 0x20;     //set bit 5

       if(sNoofLines != 101){
          MessageBox (hWnd, "It looks like this is an Acceleration Profile file\nwithout exactly 100 accelerations in!",pszFile2Open,MB_OK);
          _close(open_file);
          return 1;
         }

       //Put MessageLength in This Packet's bytes 4 & 5 (The first packet for a line always has a double-header [4 bytes + 4 bytes]).  Remember MessageLength 
       // is how many pixels this row has (i.e. 0 - 2200).
       sTypeOneMessageLengthOrLineNo = (short)nNoofHomingAccelPixels; //AVR will ignore this number and use '100' instead, if the profile is to be used for 'Fast-Moving' rather than homing
       short_endian_swap(*psInt); // psInt points to sTypeOneMessageLengthOrLineNo
       *(short*)&pMxPacketBuff[4] = *psInt;
      }
    else{
       if(bDoProperCheckSumChecks_MposFiles)
         pMxPacketBuff[1] |= 0x20;     //set bit 5
      }

    crcgen();
    //unsigned short sDummy = 4;
    //short_endian_swap(sDummy);
    //unsigned int uiCheckSum = CalcCRC4Table(8800000, CopyUC3Buff, (void *)&sDummy);
    unsigned int uiCheckSum = CalcCRC4Table(8800000, CopyUC3Buff);

  //Having calculated a checksum, put it in the Header as the first four bytes of line 4
    *(unsigned int*)(chCSVHeaderLine + 126) = uiCheckSum;
    memmove(chCSVHeaderLine + 130,"<-CheckSum: Don't Edit This file!!!!",36);
    uiTableOfCheckSums[nOFmotorNo] = uiCheckSum;

  //The above code has read the csv file and converted it to binary in the buffer. We can save this buffer as a csb file to allow it to be read in and
    char szCSBfnameToSaveCSVfileAs[_MAX_DIR];
    StringCchCopy(szCSBfnameToSaveCSVfileAs,_MAX_DIR,pszFile2Open);
    int nFileNameLengthM1 = lstrlen(szCSBfnameToSaveCSVfileAs) - 1;// sent far more quickly in future (and saved as a much smaller file on the hard drive).  Do this
    szCSBfnameToSaveCSVfileAs[nFileNameLengthM1] = 'b';            // by temporarily changing *pszFile2Open from csv to csb.
    mySave_File (hWnd, 9000000, "csb",0, szCSBfnameToSaveCSVfileAs, (char)nOFmotorNo);  //This is the only place a call to save a "csb" file occurs (i.e. at end of opening a csv file)
    //pszFile2Open[nFileNameLengthM1] = 'v';
/***************************************************************************************************************************************************
*                                                                  END (of csv)
****************************************************************************************************************************************************
*
****************************************************************************************************************************************************
*
****************************************************************************************************************************************************
*
****************************************************************************************************************************************************/
   } //end csv... File will be closed and 'returncode' returned (which will be zero).
 else if( !lstrcmp(fType,"csb") ){ //Similar to reading the csv motor-position files, but reads a binary version of them.
    if ((nCharsReadIn = _read(open_file,chCSVHeaderLine, SIZ_CSV_HEADERp4)) != SIZ_CSV_HEADERp4 ){
       MessageBox (hWnd, "Error613: Unable to read the file header for the above-named file!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
    uiTableOfCheckSums[nOFmotorNo] = *(int*)(chCSVHeaderLine + 126);

    pMxPacketBuff = pMxPacketBuffAr[nOFmotorNo]; //Get pointer to the buffer where the data will be stored
    if ((nCharsReadIn = _read(open_file,pMxPacketBuff, *(int*)&chCSVHeaderLine[SIZ_CSV_HEADER])) != *(int*)&chCSVHeaderLine[SIZ_CSV_HEADER] ){
       MessageBox (hWnd, "Error614: Unable to read the file body for the above-named file!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
    //returncode = 1;
   } //end csb

else if( !lstrcmp(fType,"prn") ){ //Note, prn files contain AccelProfiles, and LoadAndCheckAccelProfilePRNfile() must remain the ONLY place when this function called with fType set to
                                  // prn (because LoadAndCheckAccelProfilePRNfile completes the loading of the array by doubling the number of entries, multplying the second half by -1)
//The file structure to be read by this routine requires that the file contains 100 fixed length
// lines of 8 characters (including the cr/lf pair). Each line must contain one short positive integer.
// Each integer must be >= to the preceeding.
//   100<cr><lf>
//   200<cr><lf>
//   300<cr><lf>
//   300<cr><lf>
//... etc
    char chCSVline[SIZ_PRN_LINE]; //Although a PRN file, all records are SIZ_CSV_SHORT_LINE characters long
    int nNoofCSVLines = 0;
    errno  = 0;
    //nSeqNo = 0;
    for(int nnLineNo = 0; nnLineNo < NOOFaccelPROFILES; nnLineNo++){
       if ((nCharsReadIn = _read(open_file,chCSVline, SIZ_PRN_LINE)) == SIZ_PRN_LINE ){
          nNoofCSVLines++; //We've consumed one row of the CSV file
          nAccProfile[nnLineNo] = atoi(chCSVline);
          if (errno || (nAccProfile[nnLineNo] > 32767) || (nAccProfile[nnLineNo] < 1) ){ //report atoi error (or if out of bounds)
             _close(open_file);
             DisplayPRNErrorMessageBoxWithErrorAndLineNo(hWnd, errno == ERANGE? 1:0, nNoofCSVLines, pszFile2Open); //nNoofCSVLines was nnLineNo * 2201 + 1
             return 1;
            }
         }           // for each line (i.e. is in every 3rd packet [when the lines are long enough to need 3 packets / line])
       else{
          int nLength_mes;
          StringCchCopy(mes,SIZMES,"Open ");
          StringCchCat(mes,SIZMES,pszFile2Open);
          StringCchCat(mes,SIZMES," and look at line ");
          nLength_mes = lstrlen(mes);
          _itoa_s(nNoofCSVLines, mes + nLength_mes, SIZMES - nLength_mes, 10);
          MessageBox (hWnd, mes, "Error: Unable to read the 'nAccProfile[x]' element!",MB_OK);
          _close(open_file);
          return 1;
         }
      }
   } //end prn... File will be closed and 'returncode' returned (which will be zero).
 else if( !lstrcmp(fType,"hcf") ){ //Read the Hologram Config file into HCFrec
                                                                              nCharsReadIn = sizeof(HCFrec);//So we can see it when debugging.
                                                                            //nCharsReadIn = sizeof(HCFRECORD2);//So we can see it when debugging.
                                                                            //nCharsReadIn = sizeof(HCFrec3);//So we can see it when debugging.
                                                                            //nCharsReadIn = sizeof(HCFRECORD4);//So we can see it when debugging.
    if ((nCharsReadIn = _read(open_file, &HCFrec, sizeof(HCFrec)+1 )) != sizeof(HCFrec) ){
       char szDumy[12];
       _itoa_s(sizeof(HCFrec), szDumy, 12, 10);
       if ( nCharsReadIn > sizeof(HCFrec) ){
          MessageBox (hWnd, "Error: The above-named file was bigger than expected!\nIt is at least 1 byte too long.", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "Program expected to read this many bytes:", MB_OK);
         }
       else{
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes the program expected to read:", MB_OK);
          _itoa_s(nCharsReadIn,szDumy,12,10);
          MessageBox (hWnd, szDumy, "... and this is how many bytes were read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
   } //end (Reading the Hologram Config file)

 else if( !lstrcmp(fType,"cal") ){ //Read the Calibration file.
    char chCSVlline[SIZ_CSV_LONG_LINE];
    int   nUsteps4PercentSplit;
    int *pPcentSplit = nWavPlate1PcentSplitRGBAr;  //Get pointer to the buffer where the data will be stored
/****************************************************************************************************************************************************
*                                                                    READ header
****************************************************************************************************************************************************/
    if ((nCharsReadIn = _read(open_file,chCSVHeaderLine, SIZ_CSV_CALHEADER)) != SIZ_CSV_CALHEADER ){
       MessageBox (hWnd, "Error: Unable to read the file header for the above-named file!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
/*****************************************************************************************************************************************************
*                                                                    READ Body
*****************************************************************************************************************************************************/
    for(int nnLineNo = 0; nnLineNo < 303; nnLineNo++){
       if ((nCharsReadIn = _read(open_file,chCSVlline, SIZ_CSV_LONG_LINE)) == SIZ_CSV_LONG_LINE ){
          nUsteps4PercentSplit = atoi(chCSVlline);
          if (errno || (nUsteps4PercentSplit > 1280000) || (nUsteps4PercentSplit < 0) ){ //1280000: assume no position can be >100 revs from home @ 64uSteps/Step
             _close(open_file);
             DisplayCALErrorMessageBoxWithErrorAndLineNo(hWnd, errno == ERANGE? 1:0, nnLineNo+4, pszFile2Open);
             return 1;
            }
          *pPcentSplit++ = nUsteps4PercentSplit;
          //Put LineNo in This Packet's bytes 6 & 7 (As this packet has the double-header)
         }
       else{
          int nLength_mes;
          StringCchCopy(mes,SIZMES,"Open ");
          StringCchCat(mes,SIZMES,pszFile2Open);
          StringCchCat(mes,SIZMES," and look at line ");
          nLength_mes = lstrlen(mes);
          _itoa_s(nnLineNo + 5, mes + nLength_mes, SIZMES - nLength_mes, 10);
          MessageBox (hWnd, mes, "Error: Unable to read a WavPlate1PcentSplit uSteps number!",MB_OK);
          _close(open_file);
          return 1;
         }
      }
    if ((nCharsReadIn = _read(open_file,chCSVlline, 1)) == 1 ){
       MessageBox (hWnd, "Error: the calibration file WavPlate1PcentSplit.cal is too long!\n\nAt least one extra byte could be read from it!", NULL,MB_OK);
       _close(open_file);
       return 1;
      }
    if ((nWavPlate1PcentSplitRGBAr[0]) || (nWavPlate1PcentSplitRGBAr[101]) || (nWavPlate1PcentSplitRGBAr[202])){
       MessageBox (hWnd, "WavPlate1PcentSplit.cal must be zero in record 0, 101 and 202\n(probably line 5, 106 or 207, accounting for the header)!", NULL,MB_OK);
       _close(open_file);
       return 1;
      }
   }
 else if( !lstrcmp(fType,"cse") ){  //Read the "Energies to Follow" files (WP2rgb [& perhaps WP3 in future]).
    if ((nCharsReadIn = _read(open_file,chCSVHeaderLine, SIZ_CSV_HEADERm8)) != SIZ_CSV_HEADERm8 ){
       MessageBox (hWnd, "Error: Unable to read the file header for the above-named file!", pszFile2Open, MB_OK);
       _close(open_file);
       return 1;
      }
 //"                        This Is THE Time||                       The Hologram Name||Lines: xxxx   Cols: yyyy               *||Don't Edit This file!!!!               *||");
    int rows;// = *(int*)(chCSVHeaderLine  + 91);
    sscanf_s( chCSVHeaderLine + 91, "%d", &rows );
    int cols = *(int*)(chCSVHeaderLine  + 104);
    sscanf_s( chCSVHeaderLine + 104, "%d", &cols );

    pMxPacketBuff = pMxPacketBuffAr[nOFmotorNo]; //Get pointer to the buffer where the data will be stored
    if ((nCharsReadIn = _read(open_file,pMxPacketBuff, rows * cols * sizeof(double) ) ) != *(int*)&chCSVHeaderLine[SIZ_CSV_HEADER] ){
       MessageBox (hWnd, "Error2: Unable to read the file body for the above-named file!",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
   }
 else if( !lstrcmp(fType,"spf") ){  //Read the "StartPositions.spf" file.
    if ((nCharsReadIn = _read(open_file,&nLineXstartPositionsFEM[0][0], MAX_NOOFLINES * NOOFMOTORS * sizeof(int) + 1) ) != MAX_NOOFLINES * NOOFMOTORS * sizeof(int) ){
     //MessageBox (hWnd, "Error: Unable to read the above-named file (Wrong length!)",pszFile2Open,MB_OK);
       char szDumy[12];
       _itoa_s(nCharsReadIn,szDumy,12,10);
       MessageBox (hWnd, "Error: The above-named file was Wrong length!", pszFile2Open, MB_OK);
       MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
       _itoa_s(MAX_NOOFLINES * NOOFMOTORS * sizeof(int), szDumy, 12, 10);
       MessageBox (hWnd, szDumy, "... and this is how many bytes the program expected to read:", MB_OK);
       _close(open_file);
       return 1;
      }
   } //end spf
 else if( !lstrcmp(fType,"pcf") ){  //Read the "Printer Configuration" file.
    /*
    int nFilesSize = sizeof(ATMEL_IP_ADDRESStab) + sizeof(chNoofUsteps4EachMotorTab) +
                     sizeof(nNoofUsteps4EachApertureTab) + sizeof(nNoofUsteps4EachFocusTab) +
                     sizeof(nNoofUsteps4RefBeamTablePrintUnload);//So we can see it when debugging.
    char szFileSize[12];
    _itoa_s(nFilesSize, szFileSize, 12, 10);
    char szDumy[12];
    if ((nCharsReadIn = _read(open_file, ATMEL_IP_ADDRESStab, sizeof(ATMEL_IP_ADDRESStab))) != sizeof(ATMEL_IP_ADDRESStab) ){
       if ( nCharsReadIn < sizeof(ATMEL_IP_ADDRESStab) ){
          _itoa_s(nCharsReadIn,szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
    if ((nCharsReadIn = _read(open_file, chNoofUsteps4EachMotorTab, sizeof(chNoofUsteps4EachMotorTab))) != sizeof(chNoofUsteps4EachMotorTab) ){
       if ( nCharsReadIn < sizeof(chNoofUsteps4EachMotorTab) ){
          _itoa_s(nCharsReadIn + sizeof(ATMEL_IP_ADDRESStab),szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
    if ((nCharsReadIn = _read(open_file, nNoofUsteps4EachApertureTab, sizeof(nNoofUsteps4EachApertureTab))) != sizeof(nNoofUsteps4EachApertureTab) ){
       if ( nCharsReadIn < sizeof(nNoofUsteps4EachApertureTab) ){
          _itoa_s(nCharsReadIn + sizeof(ATMEL_IP_ADDRESStab) + sizeof(chNoofUsteps4EachMotorTab),szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
    if ((nCharsReadIn = _read(open_file, nNoofUsteps4EachFocusTab, sizeof(nNoofUsteps4EachFocusTab) )) != sizeof(nNoofUsteps4EachFocusTab) ){
       if ( nCharsReadIn < sizeof(nNoofUsteps4EachFocusTab) ){
          _itoa_s(nCharsReadIn + sizeof(ATMEL_IP_ADDRESStab) + sizeof(chNoofUsteps4EachMotorTab) + sizeof(nNoofUsteps4EachApertureTab),szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
    if ((nCharsReadIn = _read(open_file, nNoofUsteps4RefBeamTablePrintUnload, sizeof(nNoofUsteps4RefBeamTablePrintUnload) + 1 )) != sizeof(nNoofUsteps4RefBeamTablePrintUnload) ){
       if ( nCharsReadIn > sizeof(nNoofUsteps4RefBeamTablePrintUnload) ){
          MessageBox (hWnd, "Error: The above-named file was bigger than expected!\nIt is at least 1 byte too long.", pszFile2Open, MB_OK);
          MessageBox (hWnd, szFileSize, "Program expected to read this many bytes:", MB_OK);
         }
       else{
          _itoa_s(nCharsReadIn + sizeof(ATMEL_IP_ADDRESStab) + sizeof(chNoofUsteps4EachMotorTab) + sizeof(nNoofUsteps4EachApertureTab) + sizeof(nNoofUsteps4EachFocusTab) , szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
    */

    if ((nCharsReadIn = _read(open_file,  &PCFrec, sizeof(PCFrec)+1 )) != sizeof(PCFrec) ){
/*	   if ( nCharsReadIn > sizeof(nNoofUsteps4RefBeamTablePrintUnload) ){
          MessageBox (hWnd, "Error: The above-named file was bigger than expected!\nIt is at least 1 byte too long.", pszFile2Open, MB_OK);
          MessageBox (hWnd, szFileSize, "Program expected to read this many bytes:", MB_OK);
         }
       else{
          _itoa_s(nCharsReadIn + sizeof(ATMEL_IP_ADDRESStab) + sizeof(chNoofUsteps4EachMotorTab) + sizeof(nNoofUsteps4EachApertureTab) + sizeof(nNoofUsteps4EachFocusTab) , szDumy,12,10);
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes were read:", MB_OK);
          MessageBox (hWnd, szFileSize, "... and this is how many bytes the program expected to read:", MB_OK);
         }
       _close(open_file);
       return 1;
      } */
       char szDumy[12];
       _itoa_s(sizeof(PCFrec), szDumy, 12, 10);
       if ( nCharsReadIn > sizeof(PCFrec) ){
          MessageBox (hWnd, "Error: The above-named file was bigger than expected!\nIt is at least 1 byte too long.", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "Program expected to read this many bytes:", MB_OK);
         }
       else{
          MessageBox (hWnd, "Error: The above-named file was smaller than expected!", pszFile2Open, MB_OK);
          MessageBox (hWnd, szDumy, "This is how many bytes the program expected to read:", MB_OK);
          _itoa_s(nCharsReadIn, szDumy, 12, 10);
          MessageBox (hWnd, szDumy, "... and this is how many bytes were read:", MB_OK);
         }
       _close(open_file);
       return 1;
      }
   }  //end pcf... File will be closed and 'returncode' returned (which will be zero).

 else if( !lstrcmp(fType,"arp") ){  //Read the StaticARPs.arp file.
    char ch107CharLine[35]; //Although the lines are 107 chars long, we never read these, they're normally skipped. In fact we never read > 35 chars in any one read
    nCharsReadIn = _lseek(open_file, 1309, SEEK_SET); //skip the first 12 lines of 107 chars + 25 char "[First Interface Index]"
    if(nCharsReadIn != 1309){
       MessageBox (hWnd, "Error: Unable to skip the first 13 lines of above-named file (1309 chars)",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
    if (((nCharsReadIn = _read(open_file,ch107CharLine, 10)) == 10 ) && (!memcmp(ch107CharLine, "0x", 2)) ){ //read "[First Interface Index]\r\n"
       ch107CharLine[10] = 0;
       aARPstrc.dwFirstInterfaceIndex = strtol(ch107CharLine,NULL,16);
       _lseek(open_file, 109, SEEK_CUR);  //skip the cr/lf and the whole of the next line
       _read(open_file,ch107CharLine, 1);
       if((ch107CharLine[0] != 'Y') && (ch107CharLine[0] != 'N')){
          MessageBox (hWnd, "The field 'Should Holoprog Automatically Generate entries for the AVRs on the First Interface' was neither Y or N in above-named file ",pszFile2Open,MB_OK);
          _close(open_file);
          return 1;
         }
       else
         aARPstrc.bFirstInterfaceHasAVRs = ('Y' == ch107CharLine[0]);
       _read(open_file,ch107CharLine, 27);
       if((ch107CharLine[0] != 13)||(ch107CharLine[19] != 'w')||(ch107CharLine[20] != ']')){
          MessageBox (hWnd, "Error: Couldn't read lines '[Records To Follow]' properly in the First Interface section of above-named file",pszFile2Open,MB_OK);
          _close(open_file);
          return 1;
         }
       else{
          ch107CharLine[25] = 0;
          aARPstrc.nFirstInterfaceNoofRecs2Add = atoi(&ch107CharLine[23]);
          if( (aARPstrc.nFirstInterfaceNoofRecs2Add > 40) || (aARPstrc.nFirstInterfaceNoofRecs2Add < 0) ){
             MessageBox (hWnd, "Error: '[Records To Follow]' in the First Interface section was > 40 or < 0 in the above-named file",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
         }
       int eachRecToRead = aARPstrc.nFirstInterfaceNoofRecs2Add;
       while(eachRecToRead--){
          _read(open_file,ch107CharLine, 35);
          if( (ch107CharLine[15] != ' ') || (ch107CharLine[33] != 13) ){ //wont bother checking the linefeed, just the carriage return
             MessageBox (hWnd, "Error: In the above-named file (in the First Interface section)\nOne of the IP<>MAC pairs either\n-did not have a dividing space in the correct place, or\n-did not have a cr/lf in the correct place",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
          ch107CharLine[15] = 0;
          aARPstrc.FirstIFarpPairAr[eachRecToRead].IPaddr  = inet_addr(ch107CharLine);
          ch107CharLine[18] = 0;
          ch107CharLine[21] = 0;
          ch107CharLine[24] = 0;
          ch107CharLine[27] = 0;
          ch107CharLine[30] = 0;
          ch107CharLine[33] = 0;
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[0] = (UCHAR)strtol(&ch107CharLine[16],NULL,16);
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[1] = (UCHAR)strtol(&ch107CharLine[19],NULL,16);
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[2] = (UCHAR)strtol(&ch107CharLine[22],NULL,16);
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[3] = (UCHAR)strtol(&ch107CharLine[25],NULL,16);
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[4] = (UCHAR)strtol(&ch107CharLine[28],NULL,16);
          aARPstrc.FirstIFarpPairAr[eachRecToRead].MACaddr[5] = (UCHAR)strtol(&ch107CharLine[31],NULL,16);

        //In fact 255.255.255.255 is reserved, and is naturally returned as INADDR_NONE by inet_addr(). I'm allowing this address to be added, as Vista itself can add this to the arp table
          if ( (INADDR_NONE == aARPstrc.FirstIFarpPairAr[eachRecToRead].IPaddr) &&  (lstrcmp(ch107CharLine, "255.255.255.255")) ){
             MessageBox (hWnd, "Error: In the above-named file (in the First Interface section)\nOne of the IP addresses appeared to be illegal.\nFile format may be bad.",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
         }
       _lseek(open_file, 52, SEEK_CUR); //skip  [END OF FIRST INTERFACE] and [Second Interface Index] lines
      }
    if (((nCharsReadIn = _read(open_file,ch107CharLine, 10)) == 10 ) && (!memcmp(ch107CharLine, "0x", 2)) ){ //read "[Second Interface Index]\r\n"
       ch107CharLine[10] = 0;
       aARPstrc.dwSecondInterfaceIndex = strtol(ch107CharLine,NULL,16);
       _lseek(open_file, 109, SEEK_CUR);  //skip the cr/lf and the whole of the next line
       _read(open_file,ch107CharLine, 1);
       if((ch107CharLine[0] != 'Y') && (ch107CharLine[0] != 'N')){
          MessageBox (hWnd, "The field 'Should Holoprog Automatically Generate entries for the AVRs on the Second Interface' was neither Y or N in above-named file ",pszFile2Open,MB_OK);
          _close(open_file);
          return 1;
         }
       else
         aARPstrc.bSecondInterfaceHasAVRs = ('Y' == ch107CharLine[0]);
       _read(open_file,ch107CharLine, 27);
       if((ch107CharLine[0] != 13)||(ch107CharLine[19] != 'w')||(ch107CharLine[20] != ']')){
          MessageBox (hWnd, "Error: Couldn't read lines '[Records To Follow]' properly in the Second Interface section of above-named file",pszFile2Open,MB_OK);
          _close(open_file);
          return 1;
         }
       else{
          ch107CharLine[25] = 0;
          aARPstrc.nSecondInterfaceNoofRecs2Add = atoi(&ch107CharLine[23]);
          if( (aARPstrc.nSecondInterfaceNoofRecs2Add > 40) || (aARPstrc.nSecondInterfaceNoofRecs2Add < 0) ){
             MessageBox (hWnd, "Error: '[Records To Follow]' in the Second Interface section was > 40 or < 0 in the above-named file",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
         }
       int eachRecToRead = aARPstrc.nSecondInterfaceNoofRecs2Add;
       while(eachRecToRead--){
          _read(open_file,ch107CharLine, 35);
          if( (ch107CharLine[15] != ' ') || (ch107CharLine[33] != 13) ){ //wont bother checking the linefeed, just the carriage return
             MessageBox (hWnd, "Error: In the above-named file (in the Second Interface section)\nOne of the IP<>MAC pairs either\n-did not have a dividing space in the correct place, or\n-did not have a cr/lf in the correct place",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
          ch107CharLine[15] = 0;
          aARPstrc.SecondIFarpPairAr[eachRecToRead].IPaddr  = inet_addr(ch107CharLine);
          ch107CharLine[18] = 0;
          ch107CharLine[21] = 0;
          ch107CharLine[24] = 0;
          ch107CharLine[27] = 0;
          ch107CharLine[30] = 0;
          ch107CharLine[33] = 0;
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[0] = (UCHAR)strtol(&ch107CharLine[16],NULL,16);
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[1] = (UCHAR)strtol(&ch107CharLine[19],NULL,16);
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[2] = (UCHAR)strtol(&ch107CharLine[22],NULL,16);
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[3] = (UCHAR)strtol(&ch107CharLine[25],NULL,16);
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[4] = (UCHAR)strtol(&ch107CharLine[28],NULL,16);
          aARPstrc.SecondIFarpPairAr[eachRecToRead].MACaddr[5] = (UCHAR)strtol(&ch107CharLine[31],NULL,16);

        //In fact 255.255.255.255 is reserved, and is naturally returned as INADDR_NONE by inet_addr(). I'm allowing this address to be added, as Vista itself can add this to the arp table
          if ( (INADDR_NONE == aARPstrc.SecondIFarpPairAr[eachRecToRead].IPaddr) &&  (lstrcmp(ch107CharLine, "255.255.255.255")) ){
             MessageBox (hWnd, "Error: In the above-named file (in the Second Interface section)\nOne of the IP addresses appeared to be illegal.\nFile format may be bad.",pszFile2Open,MB_OK);
             _close(open_file);
             return 1;
            }
         }
      }
    else{
       MessageBox (hWnd, "Error: After skipping to the Second Interface Index \nUnable to read '0x' in the above-named file\nFile format may be bad.",pszFile2Open,MB_OK);
       _close(open_file);
       return 1;
      }
   } //end arp
 else if( !lstrcmp(fType,"cft") ){  //Read the "Calibration Free Text" file.
    returncode = 1;
   } //end cft
 else if( !lstrcmp(fType,"why") ){ //What Have You : other 'read file' code
    returncode = 2;
   } //end why
 else{
    returncode = 2;
   }
 if(returncode == 2)
   MessageBox(hWnd,"Error: myOpen_File() cannot read the above file-type",fType,MB_OK);
//fclose ( open_file );
 _close(open_file);
 return returncode;
}

//void MakeErrorSound(void){
// if(bMakeSounds)
//   PlaySound( "Error.wav", NULL, SND_FILENAME | SND_ASYNC);
//}
void MakeWarningSound(void){
 if(bMakeSounds)
 //PlaySound ("Warning.wav",NULL,SND_ASYNC);
   PlaySound ("IDR_WAVE2_WARN", hInst, SND_RESOURCE | SND_ASYNC);
}
void MakeAcknowledgeSound(void){
 if(bMakeSounds)
 //PlaySound ("Ack.wav",NULL,SND_ASYNC);
   PlaySound ("IDR_WAVE1_ACK", hInst, SND_RESOURCE | SND_ASYNC);
}
//void WarnIfSoundFileIsMissing(HWND hWnd){
// int     FileToTestFor;
// errno_t err;
// BOOL bOneMissing = 0;
// err = _sopen_s(&FileToTestFor, "Error.wav", O_BINARY, _SH_DENYNO, _S_IREAD);
// if( err )
//   bOneMissing = 1;
// else{
//    _close(FileToTestFor);
//    err = _sopen_s(&FileToTestFor, "Warning.wav", O_BINARY, _SH_DENYNO, _S_IREAD);
//    if( err )
//      bOneMissing = 1;
//    else{
//       _close(FileToTestFor);
//       err = _sopen_s(&FileToTestFor, "Ack.wav", O_BINARY, _SH_DENYNO, _S_IREAD);
//       if( err )
//         bOneMissing = 1;
//       else
//         _close(FileToTestFor);
//      }
//   }
// if(bOneMissing)
//   MessageBox(hWnd,"'Error.wav', 'Warning.wav', 'Ack.wav' should exist in the same\nfolder as HoloProg.exe\n\nYou can still use HoloProg, but you will not hear the proper sounds.",
//      "WARNING - At least one sound file is missing", MB_OK);
//}

void redraw(void){
 //GetClientRect(hWnd,r Client);
 /* //This was the update region for showing Energy Meter Readings:
 r Client->left   = 510;
 r Client->top    =  45;
 r Client->right  = 675;
 r Client->bottom = 100;
 */
 /*
 //This is the update region for showing the flashing polygons
     //{357,924,454,924,450,904,424,904};
 r Client->left   = 357;
 r Client->top    = 904;
 r Client->right  = 560;
 r Client->bottom = 924;
 */

//int recX, recY;               //This code, in WM_PAINT is useful
//recX = X_OFFSET + 465 + 107;  // for visualising the update region:
//recY = Y_OFFSET + 44 + 17;
//int wid = 75 + recX;
//int hit = 40 + recY;
//Rectangle(hdc,recX,recY,wid,hit);
//r Client->left   = 172;
//r Client->top    = 941;
//r Client->right  = 247;
//r Client->bottom = 981;
 InvalidateRect(hGLWnd,&rClient,TRUE);
 InvalidateRect(hGLWnd,&rNoofReadsToAverageRECT,TRUE);
 UpdateWindow(hGLWnd);
}

void redrawPIDrect(void){
 InvalidateRect(hGLWnd,&rPIDMsgsRect,TRUE);
 UpdateWindow(hGLWnd);
}

//BOOL DriveSpaceShort(long reqSpace, char Drive, double* avail){ //RETURNS TRUE IF SPACE IS SHORT OR IF COULDN'T CHECK.  IF COULDN'T CHECK, AVAIL IS SET TO MINUS ONE
BOOL DriveSpaceShort(long reqSpace, char Drive, PULARGE_INTEGER avail){ //RETURNS TRUE IF SPACE IS SHORT OR IF COULDN'T CHECK.  IF COULDN'T CHECK, AVAIL IS SET TO MINUS ONE
 avail->QuadPart = 0;
 char sDrv[4];
 sDrv[0]=Drive;
 sDrv[1]=':';
 sDrv[2]='\\';
 sDrv[3]=0;
 //DWORD SectorsPerCluster,BytesPerSector,NumberOfFreeClusters,TotalNumberOfClusters;
 //if (!GetDiskFreeSpace((LPCTSTR)sDrv,(LPDWORD)&SectorsPerCluster,(LPDWORD)&BytesPerSector, (LPDWORD)&NumberOfFreeClusters,(LPDWORD)&TotalNumberOfClusters ))
 ULARGE_INTEGER ULIFreeBytesAvailable;//, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
 if ( !GetDiskFreeSpaceEx((LPCTSTR)sDrv,&ULIFreeBytesAvailable,NULL,NULL))// &BytesPerSector, &lpTotalNumberOfFreeBytes))
    return TRUE;
 //*avail = (double)NumberOfFreeClusters * (double)BytesPerSector * (double)SectorsPerCluster;
 *avail = ULIFreeBytesAvailable;
 if (reqSpace > ULIFreeBytesAvailable.QuadPart )
   return TRUE;
 else
   return FALSE;
}

void StripTrailingSpaces(char *strplusspace){
 int strlength, nCnt;
 strlength = lstrlen(strplusspace)-1;
 for (nCnt=strlength;nCnt>-1;nCnt--)
   if(strplusspace[nCnt]==32)
     strplusspace[nCnt]=0;
   else
     nCnt= -1;
 return;
}

void myWaitForAVRdelay(int nInter_PacketDelayMilliSecs){
  Sleep(nInter_PacketDelayMilliSecs);
//for(int xx = 0; xx < noofLoops; xx++){//Delay sending the next packet, briefly to allow AVR time to receive last one
//   xx++;                       //Effectively I've moved this delay (search on n InterPacketDelayMilliSecs to see how calls 
//   xx--;                       // to S endMessagePackets() are spaced apart.
//  }
}

void DisplayFileInEditor(HWND hWnd, char* szFileToView){
 STARTUPINFO startupInfo;
 PROCESS_INFORMATION procInfo;
 BOOL bSuccess;
 char szFullPath[_MAX_DIR];
 int nLenFullPath;
 GetStartupInfo(&startupInfo);
// lstrcpy(szFullPath, "notepad.exe ");
// GetModuleFileName (hInst, szFullPath + 12, sizeof(szFullPath));

 //lstrcpy(szFullPath, "C:\\Program Files\\ULTRAEDT\\UEDIT32.EXE ");
 //StringCchCopy(szFullPath, _MAX_DIR, "C:\\Program Files\\ULTRAEDIT\\UEDIT32.EXE ");
 StringCchCopy(szFullPath,_MAX_DIR, stdstring(EditorForViewing).c_str() );
 StringCchCat (szFullPath,_MAX_DIR," \"");
 nLenFullPath = lstrlen(szFullPath);

 if((szFileToView[1] == ':')&&(szFileToView[2] == BACKSLASH)){//szFileToView is a fully qualified filename
  //lstrcat(szFullPath, szFileToView);
    StringCchCat(szFullPath,_MAX_DIR,szFileToView);
   }
 else{//szFileToView file in the same folder as a the executable (or a subfolder)
    GetModuleFileName (hInst, szFullPath + nLenFullPath, sizeof(szFullPath));
    char *ptrBslash;
    ptrBslash = strrchr(szFullPath + nLenFullPath, BACKSLASH);   //reverse search & remove the final backslash
    szFullPath[(int)(ptrBslash-szFullPath)]=0; //szFullPath now hold the directory the progam resides in
  //lstrcat(szFullPath, "\\");
  //lstrcat(szFullPath, szFileToView);
    StringCchCat(szFullPath,_MAX_DIR, "\\");
    StringCchCat(szFullPath,_MAX_DIR, szFileToView);
   }

 //MessageBox(hWnd,szFullPath,NULL,MB_OK);
 StringCchCat (szFullPath,_MAX_DIR,"\"");
 bSuccess = CreateProcess(NULL,
        szFullPath,
        NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &startupInfo,
        &procInfo);
 if (! bSuccess){
    //_itoa_s(GetLastError(),mes,SIZMES,10);
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,  NULL,  GetLastError(),  NULL,  mes,  SIZMES,  NULL);
    //MessageBox(hWnd, "Couldn't create process - i.e. start Notepad, UltraEdit or your chosen editor\n\nPlease check the registry key :\n\n     HKEY_LOCAL_MACHINE\\SOFTWARE\\OpTIC Technium\\HoloProg\\EditorForViewing\n\nMake sure it contains the full path and name of the editor you want to use.",mes, MB_ICONEXCLAMATION);
    MessageBox(hWnd, "Couldn't create process - i.e. start Notepad, UltraEdit or your chosen editor\n\nPlease check the registry key :\n\n     HKEY_CURRENT_USER\\SOFTWARE\\OpTIC Technium\\HoloProg\\EditorForViewing\n\nMake sure it contains the full path and name of the editor you want to use.",mes, MB_ICONEXCLAMATION);
   }
}
#include "StdAfx.h"
//extern char pM38PacketBuff[SIZ_8832000];
char *pCurrentProfileGenerationBuffer;     //replaces the above so we can write any buffer

extern int nLengthOfM39rowsIncLeadInOut_inPixels;

//                                         FUNTIONS.CPP
//This file contains:
//  m ySave_File()  //Opens and Writes various file types from a buffer

#define SIZ_CSV_SHORT_LINE       8 //includes the cr/lf pair - was 13 when 32 bit

//extern char nchCurrentMotorZB; replaced by an additional parameter: nch_CurrentMotorZB

extern char chCSVHeaderLine[SIZ_CSV_HEADERp4]; //when reading the binary version of the file, a 32-bit number follows the header (describing how long the rest of the file is)
extern HCFRECORD HCFrec;

extern PCFRECORD PCFrec; //This Was:
//pcf file consists of: ATMEL_IP_ADDRESStab AND chNoofUsteps4EachMotorTab AND nNoofUsteps4EachApertureTab AND nNoofUsteps4EachFocusTab AND nNoofUsteps4RefBeamTablePrintUnload
//extern char ATMEL_IP_ADDRESStab[NOOFMOTORS][16];
//1,2,4,8,16,32,64 are allowed in the next table. The values "32" are defaults... is is expected that, if the pcf file exists, they will be overwritten.
//extern char chNoofUsteps4EachMotorTab[NOOFMOTORS];// = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
//extern int nNoofUsteps4EachApertureTab[9];// = {3200,6400,76800,3200,6400,76800,3200,6400,76800}; //RedNo1,RedNo2,RedPin,GreNo1,GreNo2,GrePin,BluNo1,BluNo2,BluPin. This table stores the 
                                  // position the motor must move to, to select 1 of the three possible apertures (for each Laser)
//extern int nNoofUsteps4EachFocusTab[3];
//extern int nNoofUsteps4RefBeamTablePrintUnload[4];

extern BOOL bMoveNowRadioButPressed;
extern char mes[];

extern char *pMxPacketBuffAr[];
extern unsigned int nSeqNo;

extern int   nPixPositionsAr[];  //2200 (Just one Line)
extern int   nSizPixPositionsAr;

extern short sPixDiffsAr[];      //2200 * 2000
extern int   nPixFreqsAr[];
extern int   nSizPixDiffsAr;

extern ENERGYREC sArrEnergRecs[];
extern int nGlobalLastUSBrecNoRecvd;

extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern int globDumpBuff;
extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES];


//A pointer to the filename must now be passed.  The buffer storing the filename (allocated elsewhere) has to be
//  large enough to allow m ySave_File() to add a few characters to the end (e.g. the file suffix)
//Added nTypeOffType! The problem was, some file types (e.g. csv) are used for storing different data from different places e.g. Tables
// of ints from place A or Tables of shorts from place B.  In these cases, nTypeOffType controls what is written.
//Returns zero if file saved ok.
int mySave_File (HWND hWnd, long reqSpace, char *fType, int nTypeOffType, char *pszFile2Save, char nch_CurrentMotorZB) {//nch_CurrentMotorZB is used only if fType is csb or cse
 //double available;
 ULARGE_INTEGER available;
 FILE *save_file;
 errno_t err;
 int nFileNameLength = lstrlen(pszFile2Save);
 if(*(pszFile2Save+1) != ':'){
    myLoadString(STR_6);  //"mySave_File() failed!\n\nThe Filename that cannot be saved is listed above\n\nIt's second character is not a colon!"
    MessageBox (hWnd,mes,pszFile2Save,MB_OK);
    return 1;
   }
 if ( lstrcmp( pszFile2Save + nFileNameLength - 3, fType) ){    
    StringCchCat(pszFile2Save,_MAX_DIR,".");    //We will assume that if the filename the user chose does not have the expected suffix then
    StringCchCat(pszFile2Save,_MAX_DIR,fType);  // the suffix needs to be appended.
   }
 if ( DriveSpaceShort(reqSpace, *pszFile2Save, &available) && (available.QuadPart != -1) ){//DriveSpaceShort sets available to -1
    myLoadString(STR_3);                                                                // if it cannot check the space.
    MessageBox (hWnd,mes,NULL,MB_OK);//"File Not Saved.\nInsufficient Disk Space on the selected drive"
   }
 else{
    if (-1 == available.QuadPart){
       myLoadString(STR_4);  //"Unable to check DriveSpace... Aborting!\n\nThe Filename that cannot be saved is listed above"
       MessageBox (hWnd,mes,pszFile2Save,MB_OK); //This can be caused by there not being a drive letter present as first character.
       return 1;
      }
    err = fopen_s(&save_file, pszFile2Save, "wb"); // Open file; get descriptor
    if (err){
       char errCodeAsString[12];
       myLoadString(STR_5);  //"Unable to open and save file...\n\n""
       StringCchCat(mes,SIZMES,pszFile2Save);
       if(err != 2){ //2 is ENOENT
          StringCchCat(mes,SIZMES,"\n\nNote Error Code Above");//let's not be as annoying as those programs which tell you they cannot open
          _itoa_s(err, errCodeAsString, 12, 10);                 // a file but then dont tell you which file so you cant do anything about it!
          MessageBox (hWnd,mes,errCodeAsString,MB_OK);
         }
       else{
          StringCchCat(mes,SIZMES,"\n\nError code: No such file or directory");//let's not be as annoying as those programs which tell you they cannot open
          MessageBox (hWnd,mes,NULL,MB_OK);// a file but then dont tell you which file so you cant do anything about it!
         }
       return 1;
      }
    else{//write file opened
       err = 0;
       if( !lstrcmp(fType,"bin") ){
          if(nTypeOffType == 0)
            fwrite(sPixDiffsAr,nSizPixDiffsAr,sizeof(short),save_file);//nSizPixDiffsAr must be set, & will be <= 4,400,000
          else if (nTypeOffType == 1)
            fwrite((short*)sArrEnergRecs,nGlobalLastUSBrecNoRecvd+1,sizeof(ENERGYREC),save_file);//sArrEnergRecs could contain up to 4,320,000 rows of 18 bytes
          else
            MessageBox(hWnd,"mySave_File() called with invalid nTypeOffType!","Programming Error",MB_OK);
         }
       else if( !lstrcmp(fType,"csv") ){
          char szCrLF[2];
          szCrLF[0] = 13;
          szCrLF[1] = 10;
          if(nTypeOffType == 1)//Type 1 either writes a line, or a table, of shorts.  nSizPixDiffsAr must be set, & may be <= 2200 or may be <= 4,400,000
            for (int k = 0; k < nSizPixDiffsAr; k++){
               _itoa_s(sPixDiffsAr[k], mes, SIZMES, 10);
               fwrite(mes,lstrlen(mes),1,save_file);
               fwrite(szCrLF,2,1,save_file);
              }
          else if(nTypeOffType == 2)//Type 2 writes a line of ints.  nSizPixPositionsAr must be set, & will probably be <= 2200 
            for (int k = 0; k < nSizPixPositionsAr; k++){
               _itoa_s(nPixPositionsAr[k], mes, SIZMES, 10);
               fwrite(mes,lstrlen(mes),1,save_file);
               fwrite(szCrLF,2,1,save_file);
              }
          else if(nTypeOffType == 3){//Type 3 writes ENERGYRECs. Write each channel seperately, to make the file easier to read (for humans)
             int nFixedLast485RecNoRecvdP1 = nGlobalLastUSBrecNoRecvd+1; //prevent nGlobalLastUSBrecNoRecvd from incrementing in other thread
             StringCchCopy(mes, SIZMES, "*NoofRecs For Each Channel: ");
             _itoa_s(nGlobalLastUSBrecNoRecvd, mes+27, _MAX_DIR - 27,10);
             fwrite(mes,lstrlen(mes),1,save_file);
             fwrite(szCrLF,2,1,save_file);
             for (int Channel = 0; Channel < NOOFENERGYRECFIELDS; Channel++){
                if(Channel == 0)
                  fwrite("*Pixel Nos",9,1,save_file); //Before writing all the Readings for each channel, give indication of what's coming
                else if(Channel == 1)
                  fwrite("*Line Nos",8,1,save_file);  //Before writing all the Readings for each channel, give indication of what's coming
                else if(Channel < 5){
                   fwrite("*WavePlate Red",13,1,save_file); //Before writing all the Readings for each channel, give indication of what's coming
                   _itoa_s(Channel-1, mes, SIZMES, 10);
                   fwrite(mes,lstrlen(mes),1,save_file);
                  }
                else if(Channel < 8){
                   fwrite("*WavePlate Green",15,1,save_file); //Before writing all the Readings for each channel, give indication of what's coming
                   _itoa_s(Channel-4, mes, SIZMES, 10);
                   fwrite(mes,lstrlen(mes),1,save_file);
                  }
                else{
                   fwrite("*WavePlate Blue",14,1,save_file); //Before writing all the Readings for each channel, give indication of what's coming
                   _itoa_s(Channel-7, mes, SIZMES, 10);
                   fwrite(mes,lstrlen(mes),1,save_file);
                  }
                fwrite(szCrLF,2,1,save_file);
                short *pShort = (short*)sArrEnergRecs;
                pShort += Channel;
                for (int k = 0; k < nFixedLast485RecNoRecvdP1; k++){ //Note 'channels' 0 & 1 are actually Pixel Nos and Line Nos, 'channels' 2, 3 & 4 are
                   _itoa_s(*pShort, mes, SIZMES, 10);                // Red WP1, 2 & 3, (then Gre then Blu)
                   fwrite(mes,lstrlen(mes),1,save_file);
                   fwrite(szCrLF,2,1,save_file);
                   pShort += NOOFENERGYRECFIELDS;
                  }
               }
            }
          else if(nTypeOffType == 4){//if(nTypeOffType == 4)//Type 4 writes the Pixel frequency table. 
          /*for (int k = 0; k < 4; k++){
               _itoa_s(nPixFreqsAr[k], mes, SIZMES, 10);
               fwrite(mes,lstrlen(mes),1,save_file);
               fwrite(szCrLF,2,1,save_file);
              } The above was for producing 40 seperate files. From now on, ALL motor's freq data is in 1 file: */
             int nEachFreqInArray = 0;
             for (int k = 0; k < 40; k++){
                mes[0] = 'M';                         //<-- Write the
                _itoa_s(k+1, mes+1, SIZMES-1, 10);    //<-- Motor Number
                fwrite(mes,lstrlen(mes),1,save_file);
                fwrite(szCrLF,2,1,save_file);
                for (int kk = 0; kk < 4; kk++){
                   _itoa_s(nPixFreqsAr[nEachFreqInArray++], mes, SIZMES, 10); //<-- then Write the
                   fwrite(mes,lstrlen(mes),1,save_file);     //<--  four readings
                   fwrite(szCrLF,2,1,save_file);
                  }
                fwrite(szCrLF,2,1,save_file);
               }
            }
          else{//if(nTypeOffType == 5)//Type 5 writes the M39ProfileFile, M40ProfileFile, M11ProfileFile, etc.  //chCSVHeaderLine must be loaded with data before writing Type 5 csv files
             fwrite(chCSVHeaderLine,SIZ_CSV_HEADER,1,save_file);

             int rows;  //rows and cols can be retrieved from the header to see how much of the Table should be written:
             sscanf_s( chCSVHeaderLine + 91, "%d", &rows );
             int cols;
             sscanf_s( chCSVHeaderLine + 104, "%d", &cols );
             int LeadIn;
             sscanf_s( chCSVHeaderLine + 120, "%d", &LeadIn );
             //Write the remainder of the file body.  The format is to write the number of columns that are in the coming line, followed by all the position 
             //(relative to where ever)

             char szDum[SIZSZDUM12];
//UPDATE THE NEXT LINE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! (Now done)
             int realNoofCols_AfterSubtractingUnusedAccelPixels = cols  - 1  + 2 * LeadIn;
             _itoa_s(realNoofCols_AfterSubtractingUnusedAccelPixels, szDum, SIZSZDUM12, 10);
             int nLen_szDum = lstrlen(szDum);
             int nStartPoint = 6 - nLen_szDum;
             char RightJustfiedString[8] = "      ";
             char RightJustfiedString2[8];
             int nCopyChars = 0;
             while(nCopyChars < nLen_szDum)
               RightJustfiedString[nStartPoint++] = szDum[nCopyChars++]; //This Inserts and formats noof Lines
             RightJustfiedString[6] = 13;
             RightJustfiedString[7] = 10;
             int nTotalPixelNo;
             for(int eachline = 0; eachline < rows; eachline++){
                fwrite(RightJustfiedString,8,1,save_file);
                nTotalPixelNo = 2200 * eachline;
                for(int eachCol = 0; eachCol < nLengthOfM39rowsIncLeadInOut_inPixels; eachCol++){//nLengthOfM39rowsIncLeadInOut_inPixels is Global, from G enerateProfileForM39()
                   szDum[SIZSZDUM12];
                 //_itoa_s( *((short*)pM38PacketBuff + nTotalPixelNo++), szDum, SIZSZDUM12, 10);
                   _itoa_s( *((short*)pCurrentProfileGenerationBuffer + nTotalPixelNo++), szDum, SIZSZDUM12, 10);                   
                   nLen_szDum = lstrlen(szDum);
                   nStartPoint = 6 - nLen_szDum;
                   StringCchCopy(RightJustfiedString2, 8, "      ");
                   nCopyChars = 0;
                   while(nCopyChars < nLen_szDum)
                     RightJustfiedString2[nStartPoint++] = szDum[nCopyChars++]; //This Inserts formats noof Lines
                   RightJustfiedString2[6] = 13;
                   RightJustfiedString2[7] = 10;
                   fwrite(RightJustfiedString2,8,1,save_file);
                  }
//              fwrite(pMxPacketBuffAr[38],sizeof(short),1,save_file);
               }
           //fclose(save_file);
           //for (int jk = 0; jk < SIZ_8832000; jk++)
           //  *(pM38PacketBuff + jk) = 0;
           //myOpen_File(hWnd, "csv", pszFile2Save, 38);//The sole reason for calling this, here, is to reopen the csv so that it gets saved as a csb
           //return 0;
            }
         }
       else if (!lstrcmp(fType,"csb")){                    //chCSVHeaderLine must be loaded with data before writing csb files
          int nSizDataInBuffer = nSeqNo * PACKETLENGTH; // AND pMxPacketBuff must point to the buffer to be saved.
          fwrite(chCSVHeaderLine,SIZ_CSV_HEADER,1,save_file);
          fwrite(&nSizDataInBuffer,sizeof(int),1,save_file);
          fwrite(pMxPacketBuffAr[nch_CurrentMotorZB],nSizDataInBuffer,1,save_file);
         }
       else if (!lstrcmp(fType,"cse")){                 //chCSVHeaderLine must be loaded with data before writing cse files
          fwrite(chCSVHeaderLine, SIZ_CSV_HEADER - 8, 1, save_file);
 //"                        This Is THE Time||                       The Hologram Name||Lines: xxxx   Cols: yyyy               *||Don't Edit This file!!!!               *||");
          int rows;  //rows and cols can be retrieved from the header to see how much of the Table should be written:
          sscanf_s( chCSVHeaderLine + 91, "%d", &rows );
          int cols;
          sscanf_s( chCSVHeaderLine + 104, "%d", &cols );
        //fwrite(pMxPacketBuffAr[2],sizeof(double),cols*rows,save_file);
  //int THE_ABOVE_2_MUST_BE_CHANGED; //To the following line, probably:
          fwrite(pMxPacketBuffAr[nch_CurrentMotorZB],sizeof(double),cols*rows,save_file);
        //fwrite(pMxPacketBuffAr[nchCurrentMotorZB],nSizDataInBuffer,1,save_file);
         }
       else if (!lstrcmp(fType,"hcf")){
          fwrite(&HCFrec, sizeof(HCFrec),1,save_file);
         }
       else if (!lstrcmp(fType,"bdmp")){  //globDumpBuff Conveys to this routine which of the 40 buffers is to be dumped
          fwrite(&chReceiveBuffer[globDumpBuff - 1][0], SIZ_REC_BUFF,1,save_file);
         }
       else if (!lstrcmp(fType,"pcf")){
          fwrite(&PCFrec, sizeof(PCFrec),1,save_file);
        /*fwrite(ATMEL_IP_ADDRESStab, sizeof(ATMEL_IP_ADDRESStab),1,save_file);
          fwrite(chNoofUsteps4EachMotorTab, sizeof(chNoofUsteps4EachMotorTab),1,save_file);
          fwrite(nNoofUsteps4EachApertureTab, sizeof(nNoofUsteps4EachApertureTab),1,save_file);
          fwrite(nNoofUsteps4EachFocusTab, sizeof(nNoofUsteps4EachFocusTab),1,save_file);
          fwrite(nNoofUsteps4RefBeamTablePrintUnload, sizeof(nNoofUsteps4RefBeamTablePrintUnload),1,save_file); */
         }
       else if (!lstrcmp(fType,"spf"))
          fwrite(nLineXstartPositionsFEM, reqSpace,1,save_file);
       else if (!lstrcmp(fType,"arp")){
          fwrite("[Configuration file for Holoprog to create additional, static, arp-entries for Display PC, Switch, etc. ]\r\n", 107,1,save_file);
          fwrite("[**** DO NOT amend records with square brackets, but DO amend the records without square brackets. **** ]\r\n", 107,1,save_file);
          fwrite("[To find the INTERFACE INDEXes to use below (for each Network Card) type 'arp -a' at the command prompt.]\r\n", 107,1,save_file);
          fwrite("[(You may have to type 'arp -s' first, and add a dummy entry for each card, before 'arp -a' displays    ]\r\n", 107,1,save_file);
          fwrite("[ the interfaces indexes).  Example output from 'arp -a' is Interface: 192.168.0.200 --- 0x10003  etc.  ]\r\n", 107,1,save_file);
          fwrite("[The interface's index will look like 0x10003 but you must add leading zeros because 8 chars are needed-]\r\n", 107,1,save_file);
          fwrite("[after the '0x'   Note that you must specify both network card's interface indexes.                     ]\r\n", 107,1,save_file); 
          fwrite("[When you add or remove IPaddress<>MACaddress records, state how many records there are (in the         ]\r\n", 107,1,save_file);
          fwrite("[2-character, fixed-length, 'Records To Follow field'). This field must be in the range 00 to 40 because]\r\n", 107,1,save_file);
          fwrite("[Holoprog can add up to 40 static entries per network card (in addition to the 40 AVR addresses it will ]\r\n", 107,1,save_file);
          fwrite("[generate automatically.  The Control PC, Switch, Display PC, etc. should all have fixed IP addresses.  ]\r\n", 107,1,save_file);
          fwrite("[Finally, the default records below are examples to show how the records should look:-PLEASE AMEND THEM!]\r\n", 107,1,save_file);
          fwrite("[First Interface Index]\r\n0x0000000a\r\n", 37,1,save_file);
          fwrite("[Should Holoprog Automatically Generate entries for the AVRs on the First Interface (Y/N) for Yes / No  ]\r\n", 107,1,save_file);
          fwrite("N\r\n[Records To Follow]\r\n09\r\n", 28,1,save_file);
          fwrite("192.168.100.199 00-1d-7d-05-8c-f2\r\n", 35,1,save_file); //This is the Display PC
                                                                                                 //Maybe the switch should be added here

          fwrite("192.168.100.001 00-13-d4-02-ef-74\r\n", 35,1,save_file); //This 3 were         //Maybe these three
          fwrite("192.168.100.201 00-1d-7d-07-b5-f2\r\n", 35,1,save_file); // shown              // should not be here 
          fwrite("192.168.100.253 00-14-22-57-bb-b9\r\n", 35,1,save_file); // as Dynamic         //  unless they're fixed IP

          fwrite("192.168.100.255 ff-ff-ff-ff-ff-ff\r\n", 35,1,save_file); //These               //Maybe these
          fwrite("224.000.000.022 01-00-5e-00-00-16\r\n", 35,1,save_file); // five               // didn't need
          fwrite("224.000.000.252 01-00-5e-00-00-fc\r\n", 35,1,save_file); // were shown         // adding
          fwrite("239.255.255.250 01-00-5e-7f-ff-fa\r\n", 35,1,save_file); // as static          // because they
          fwrite("255.255.255.255 ff-ff-ff-ff-ff-ff\r\n", 35,1,save_file); // anyway!            // were static

          fwrite("[END OF FIRST INTERFACE]\r\n", 26,1,save_file);
          fwrite("[Second Interface Index]\r\n0x0000000b\r\n", 38,1,save_file);
          fwrite("[Should Holoprog Automatically Generate entries for the AVRs on the Second Interface (1:yes, 0:no)      ]\r\n", 107,1,save_file);
          fwrite("Y\r\n[Records To Follow]\r\n04\r\n", 28,1,save_file);

          fwrite("192.168.000.255 ff-ff-ff-ff-ff-ff\r\n", 35,1,save_file); //These four          //Maybe these
          fwrite("224.000.000.022 01-00-5e-00-00-16\r\n", 35,1,save_file); // were shown         // didn't need
          fwrite("224.000.000.252 01-00-5e-00-00-fc\r\n", 35,1,save_file); // as static          // adding because
          fwrite("239.255.255.250 01-00-5e-7f-ff-fa\r\n", 35,1,save_file); // anyway!            // they were static

          fwrite("[END OF SECOND INTERFACE]", 25,1,save_file);         }
       else{
          MessageBox (hWnd,"Error: Unsupported file type to save \n\nAn empty file may have been created!",pszFile2Save,MB_OK);
          err = 1;
         }
       fclose(save_file);
       if(err)
         return 1;
      }
   }//end DriveSpaceShort(else)
 return 0;
}
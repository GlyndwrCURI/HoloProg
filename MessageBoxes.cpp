#include "stdafx.h"
extern char mes[];
extern COMMS_ERROR_COUNT CEC; //Structure containing counts of the various comms errors that occur in different places

void DisplayErrorMessageBoxWithErrorAndMotorNo(HWND hDlg, char nErrorMotorNum, char *pszMess, char *pszTitle){
 int nLength_mes;
 StringCchCopy(mes,SIZMES,pszMess);
 nLength_mes = lstrlen(mes);
 _itoa_s(nErrorMotorNum + 1, mes + nLength_mes,  SIZMES - nLength_mes, 10);
 MessageBox (hDlg, mes, pszTitle,MB_OK);
}

void DisplayErrorMessageBoxWithErrorAndLineNo(HWND hWnd, bool bRangeError, int nErrorLineNum, const char *pszFile2Display, int nFileToClose){
 int nLength_mes;
 _close(nFileToClose);
 StringCchCopy(mes,SIZMES,"Open ");
 StringCchCat(mes,SIZMES,pszFile2Display);
 StringCchCat(mes,SIZMES,"\nCheck the header to see if it's a Desired Motor Positions file,\nthen look at line ");
 nLength_mes = lstrlen(mes);
 _itoa_s(nErrorLineNum, mes + nLength_mes,  SIZMES - nLength_mes, 10);
 if (bRangeError)
   MessageBox (hWnd, mes, "Cannot interpret file data, Overflow condition occurred!",MB_OK);
 else
   MessageBox (hWnd, mes, "Cannot interpret file data, atoi() conversion error occurred!",MB_OK);
}
void DisplayPRNErrorMessageBoxWithErrorAndLineNo(HWND hWnd, bool bRangeError, int nErrorLineNum, const char *pszFile2Display){
 int nLength_mes;
 StringCchCopy(mes,SIZMES,"Open ");
 StringCchCat(mes,SIZMES,pszFile2Display);
 StringCchCat(mes,SIZMES,"\nand look at line ");
 nLength_mes = lstrlen(mes);
 _itoa_s(nErrorLineNum, mes + nLength_mes,  SIZMES - nLength_mes, 10);
 if (bRangeError)
   MessageBox (hWnd, mes, "Cannot interpret file data, Overflow condition occurred!",MB_OK);
 else
   MessageBox (hWnd, mes, "Cannot interpret file data, atoi() conversion error occurred!\nExpected positive integer in range 1 to 32,767",MB_OK);
}

void DisplayCALErrorMessageBoxWithErrorAndLineNo(HWND hWnd, bool bRangeError, int nErrorLineNum, const char *pszFile2Display){
 int nLength_mes;
 StringCchCopy(mes,SIZMES,"Open ");
 StringCchCat(mes,SIZMES,pszFile2Display);
 StringCchCat(mes,SIZMES,"\nCheck the header to see if it's a WavPlate1PcentSplit calibration file,\nthen look at line ");
 nLength_mes = lstrlen(mes);
 _itoa_s(nErrorLineNum, mes + nLength_mes,  SIZMES - nLength_mes, 10);
 if (bRangeError)
   MessageBox (hWnd, mes, "Cannot interpret file data, Overflow condition occurred!",MB_OK);
 else
   MessageBox (hWnd, mes, "Cannot interpret file data, atoi() conversion error occurred!",MB_OK);
}

void MessageBoxToDisplaySeqNoError(HWND hDlg, char *pszMesTitle, unsigned char cMesSentSeqno, unsigned char chRec_Seqno, int nMotorNum){
#define SIZszMBxTitle 70
#define SIZszRecd 4
 char szMBxTitle[SIZszMBxTitle];
 char szRecd[SIZszRecd];
 CEC.RecPktErrTFnPSeqNo++;
 StringCchCopy(szMBxTitle,SIZszMBxTitle,pszMesTitle);
 StringCchCat(szMBxTitle,SIZszMBxTitle," Aborting Print! Messages out of synch for motor "); //e.g.  StringCchCopy(szMBxTitle,64,"At1: Aborting Print! Messages out of synch for motor ");
 _itoa_s(nMotorNum+1, szMBxTitle+55, SIZszMBxTitle - 55, 10);
 StringCchCopy(mes, SIZMES, "Message seqNo Sent: ");
 _itoa_s(cMesSentSeqno, mes+20,  _MAX_DIR - 20, 10);
 StringCchCat(mes, SIZMES, "  Message seqNo Received: ");
 _itoa_s(chRec_Seqno, szRecd, SIZszRecd, 10);
 StringCchCat(mes, SIZMES, szRecd);
 MessageBox(hDlg, mes, szMBxTitle, MB_OK);
}
void MessageBoxWarnNotConnected(HWND hDlg){
  MessageBox(hDlg,"The Printer Configuration does not show this motor as being connected.\n\nCheck this motor in the printer configuration screen, if you want to talk to it.","Warning",MB_OK);
}

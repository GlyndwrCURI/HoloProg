#include "stdafx.h"
extern HINSTANCE hInst;
unsigned char cMessageSeqNoSR[NOOFMOTORS];


#define NOOFMOTORS2DISPLAY 40

//NEVER set NOOFTIMESTHRU_B4OBJECTING >= 999,999,997 because that will break the hard-coded use of such numbers
extern int  NOOFTIMESTHRU_B4OBJECTING;

extern HANDLE ghMutex;
COMMS_ERROR_COUNT  CEC; //Structure containing counts of the various comms errors that occur in defferent placed

extern int bWaitingForReply1[];

//extern int nCurrentMotorNo;    //Not using yet
extern unsigned char chReceiveBuffer[NOOFMOTORS][SIZ_REC_BUFF];
extern int  nMotor2ExpectRepliesFrom[NOOFMOTORS];
extern int nArFastSpeeds4HomingDeptOn_uSteps[];
extern PCFRECORD PCFrec;
STATUSRECORD *pStatRec;
int    bSTATREC_BOXisBeingDisplayed;

//extern BYTE RevBits[256]; //= {0x00,0x80,0x40,0xC0,0x20 ... etc};
//The RevBits table is used as a fast way to reverse (i.e. mirror) the bits in a byte. The bits are in reverse order for the UC3's status record.
//0000 0000 -> 0000 0000
//0000 0001 -> 1000 0000
//0000 0010 -> 0100 0000
//0000 0011 -> 1100 0000
//We store the pattern windows expects in an array and use the value received to index into the array.
#define SIZ_SERIOUS_WARNS_TABLE 22
char SeriousWarningsStrings[SIZ_SERIOUS_WARNS_TABLE][46] = {//These are text descriptions (for the CPC to display) for status codes reported by the UC3 in it's status record
  "ok", //Error zero is No Error
  "E1: Limit switch 1 hit",
  "E2: Limit switch 2 hit",
  "E3: nTransitionNo >= MAXNOOFTRANSITIONALLOWED", //Error 3
  "E4: PixPrintSignal is Stopped",
  "E5: Unrecognised Sub-Type on Type 1 mess",
  "E6: BAD UDP CHECKSUM",             //Error 6
  "E7: 1514 byte Packet, unrecog Type",
  "E8: Critical Error!",
  "E9: Pulse Time Out of Bounds: ",   //Error 9
  "E10: Stored Error from previous Session:",  //Error was stored in flash ram - perhaps couldn't initialise PHY, or some other comms error (unimplemented)
  "E11: Unexpected Line No",
  //Don't change errors 12,13,14 without considering them being cleared in tc_irqWhenHoming()
  "E12: Slow Homing TC Not 60 or 120K",  //When homing out (slower) TC count must be no lower than 60000 (be 60K 120K)
  "E13: Error Homing-LONG Pulse fnd!",   //When homing in (faster) TC count must be no higher than 59999
  "E14: Homing-Run OUT, No Home Fund!",  //More Serious than not finding it on the way in (that could just be the stage being too far away)
  "E15: NoofUstepsPerStep : Out of Bounds",
  "E16: Error: U32Pulses2Go Set!",
  "E17: usLineNum2Convert not zero b4 conversion",
  "E18: 32 Mbyte SDRAM test Failed!",
  "E19: 4 Mbyte SDRAM test Failed!",
  "E20: Invalid Fill_raArray arg",
//Next One is Spare:
  "E21: ************************"
};

#define SIZ_WARNS2REPORT_TABLE 19
char Warnings2ReportStrings[SIZ_WARNS2REPORT_TABLE][40] = {//These are text descriptions (for the CPC to display) for status codes reported by the UC3 in it's status record
  "ok", //Warning zero is No Warning
  "W1: Invalid Message Param6",
  "W2: Invalid Message Param2",
  "W3: 64 byte Packet, unrecog Type", //Warning 3
  "W4: Invalid Message Param9",
  "W5: Dest Port Unreachable",
  "W6: Unimplemented ICMP packet",    //Warning 6
  "W7: MyIPfromDIPswitches > 35",
  "W8: Unexpected Seq No",
//Next One is Spare:
  "W9: ************************",     //Warning 9

  "W10: Homing-Run IN: Home not Found",      //Don't change W10 without considering it being cleared in tc_irqWhenHoming()
  "W11: Received RX SPECMM while running",  
  "W12: RX SingMM_OTF when running-IGNORED", //increase message length up from 40 if extending this or any other message
  "W13: RX MPos Data when running-IGNORED",
  "W14: RX Home when running-IGNORED",
  "W15: LS Hi when looking for it (Line1)",
  "W16: LS Hi when looking for it (Main)",
  "W17: Update_raArray Out of Bounds",
  "W18: Update_raArray PixSignal Stopped"
};

//void UpdateStatusDialogsControlsM(HWND, int MotorNo); //Function prototype - This function is essentially WM_INITDIALOG processing. Having this
                                                      // as a seperate function allows it to be called whenever a refresh is required.
void ClearMostFields(HWND hDlg, int nMotorNo);

INT_PTR CALLBACK DisplayStatRec_DialogM(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 int MotorNo;
 static HFONT newFont;
 static HWND hOKbutton;
 switch (message){
    case WM_INITDIALOG:{
       HDC hdc = GetDC(NULL);
       int lfHieght = -MulDiv(8,GetDeviceCaps(hdc,LOGPIXELSY),72);
       newFont = CreateFont(lfHieght,0,0,0,0,FALSE,0,0,0,0,0,0,0,"Helv");
       ReleaseDC(NULL, hdc);

       MoveWindow(hDlg, 144,0,1536,1017,FALSE);

int IDB_STATREC_POS2to40 = IDB_STATREC_POS2;
int IDB_STATREC_HOME_STATE2to40  = IDB_STATREC_HOME_STATE2;
int IDB_STATREC_HOME_SW2to40     = IDB_STATREC_HOME_SW2;
int IDB_STATREC_LIMIT_SWa2to40   = IDB_STATREC_LIMIT_SWa2;
int IDB_STATREC_LIMIT_SWb2to40   = IDB_STATREC_LIMIT_SWb2;
int IDB_STATREC_DRIVER_ENAB2to40 = IDB_STATREC_DRIVER_ENAB2;
int IDB_STATREC_RS485_ENAB2to40  = IDB_STATREC_RS485_ENAB2;
int IDB_STATREC_GENERALWARN2to40 = IDB_STATREC_GENERALWARN2;
int IDB_STATREC_SERIOUSWARN2to40 = IDB_STATREC_SERIOUSWARN2;
int IDB_STATREC_REDYTPAL2to40  = IDB_STATREC_REDYTPAL2;
int IDB_STATREC_IWANTLow2to40  = IDB_STATREC_IWANTLow2;
int IDB_STATREC_IT_IS_Low2to40 = IDB_STATREC_IT_IS_Low2;
int IDB_STATREC_VARa2to40 = IDB_STATREC_VARa2;
int IDB_STATREC_VARb2to40 = IDB_STATREC_VARb2;
int IDB_STATREC_VARc2to40 = IDB_STATREC_VARc2;
int IDB_STATREC_RESETm02to40 = IDB_STATREC_RESETm02;

#define LOWERY 55
#define REQROWS 39
int upper = LOWERY + 21 * REQROWS + 30; //30 allows for the spaces
       int Ypos;
       for(Ypos = LOWERY; Ypos < upper; Ypos+=21){
          if(Ypos == 286)  //add a space between rows after motor 12
            Ypos += 10;
          if(Ypos == 548)  //add a space between rows after motor 24
            Ypos += 10;
          if(Ypos == 810)  //add a space between rows after motor 36
            Ypos += 10;
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    122, Ypos, 116, 22,  hDlg, (HMENU)IDB_STATREC_POS2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_POS2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    263, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_HOME_STATE2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_HOME_STATE2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    313, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_HOME_SW2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_HOME_SW2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    344, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_LIMIT_SWa2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_LIMIT_SWa2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    377, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_LIMIT_SWb2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_LIMIT_SWb2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, //
                    428, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_DRIVER_ENAB2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_DRIVER_ENAB2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, //
                   460, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_RS485_ENAB2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_RS485_ENAB2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    512, Ypos, 271, 22,  hDlg, (HMENU)IDB_STATREC_GENERALWARN2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_GENERALWARN2to40++,WM_SETFONT,(WPARAM)newFont,0);


          CreateWindowEx(NULL,"BUTTON","R", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                   785, Ypos+1, 13, 20,  hDlg, (HMENU)IDB_STATREC_RESETm02to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_RESETm02to40++,WM_SETFONT,(WPARAM)newFont,0);


          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    800, Ypos, 271, 22,  hDlg, (HMENU)IDB_STATREC_SERIOUSWARN2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_SERIOUSWARN2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1096, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_IWANTLow2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_IWANTLow2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1142, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_IT_IS_Low2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_IT_IS_Low2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                    1214, Ypos, 25, 22,  hDlg, (HMENU)IDB_STATREC_REDYTPAL2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_REDYTPAL2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, //
                    1252, Ypos, 77, 22,  hDlg, (HMENU)IDB_STATREC_VARa2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_VARa2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, //
                   1342, Ypos, 77, 22,  hDlg, (HMENU)IDB_STATREC_VARb2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_VARb2to40++,WM_SETFONT,(WPARAM)newFont,0);
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, //
                   1430, Ypos, 77, 22,  hDlg, (HMENU)IDB_STATREC_VARc2to40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_VARc2to40++,WM_SETFONT,(WPARAM)newFont,0);	  
         }

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Enable All Drives", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                357, Ypos+22, 100, 20,  hDlg, (HMENU)IDB_STATREC_DRIVER_ENABall,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_DRIVER_ENABall,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Disable All Drives", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                357, Ypos+43, 100, 20,  hDlg, (HMENU)IDB_STATREC_DRIVER_DISABall,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_DRIVER_DISABall,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Enable All RS485", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                457, Ypos+22, 100, 20,  hDlg, (HMENU)IDB_STATREC_RS485_ENABall,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_RS485_ENABall,WM_SETFONT,(WPARAM)newFont,0);
       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Disable All RS485", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                457, Ypos+43, 100, 20,  hDlg, (HMENU)IDB_STATREC_RS485_DISABall,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_RS485_DISABall,WM_SETFONT,(WPARAM)newFont,0);



       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Help", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                357, Ypos+64, 200, 20,  hDlg, (HMENU)IDB_STATREC_HELP,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_HELP,WM_SETFONT,(WPARAM)newFont,0);


       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Home All", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                247, Ypos+1, 60, 20,  hDlg, (HMENU)IDB_STATREC_HOMEALL,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_HOMEALL,WM_SETFONT,(WPARAM)newFont,0);

       CreateWindowEx(WS_EX_CLIENTEDGE,"BUTTON","Reset All", WS_CHILD | WS_VISIBLE | WS_TABSTOP, //
                765, Ypos+1, 60, 20,  hDlg, (HMENU)IDB_STATREC_RESETmALL,hInst,NULL);
       SendDlgItemMessage(hDlg,IDB_STATREC_RESETmALL,WM_SETFONT,(WPARAM)newFont,0);

       hOKbutton = GetDlgItem(hDlg,IDCANCEL); //Cancel button is labelled OK for user's benefit... OK/Cancel : same sort of thing!

       bSTATREC_BOXisBeingDisplayed = TRUE;
       for(MotorNo = 0; MotorNo < NOOFMOTORS2DISPLAY; MotorNo++)
         UpdateStatusDialogsControlsM(hDlg, MotorNo);
       PostMessage(hDlg, WM_COMMAND, IDB_STATREC_AUTOREFRESH, 0);  //Comment out to stop auto-refresh starting automatically when this box is displayed.
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_STATREC_REFRESH:             //The 'Refresh Once' button has been pressed
           for(MotorNo = 0; MotorNo < NOOFMOTORS2DISPLAY; MotorNo++)
             UpdateStatusDialogsControlsM(hDlg, MotorNo);
           break;
         case IDB_STATREC_REFRESH_NOKILLTIMER: //Same as pressing 'Refresh Once', but sent repeatedly by the timer (So doesn't kill the timer)
           for(MotorNo = 0; MotorNo < NOOFMOTORS2DISPLAY; MotorNo++)
             UpdateStatusDialogsControlsM(hDlg, MotorNo);
           if(!SetTimer(hDlg, IDT_TIMER1, AUTOREFRESHRATE, (TIMERPROC) NULL))
             MessageBox(hDlg,"STATREC_AUTO_TIMER problem!",NULL,MB_OK);
           break;
         case IDB_STATREC_AUTOREFRESH:
           if(!SetTimer(hDlg, IDT_TIMER1, AUTOREFRESHRATE, (TIMERPROC) NULL))
             MessageBox(hDlg,"STATREC_AUTO_TIMER problem!",NULL,MB_OK);
           break;
         case IDB_STATREC_DRIVER_ENAB:
         case IDB_STATREC_DRIVER_ENAB2:
         case IDB_STATREC_DRIVER_ENAB3:
         case IDB_STATREC_DRIVER_ENAB4:
         case IDB_STATREC_DRIVER_ENAB5:
         case IDB_STATREC_DRIVER_ENAB6:
         case IDB_STATREC_DRIVER_ENAB7:
         case IDB_STATREC_DRIVER_ENAB8:
         case IDB_STATREC_DRIVER_ENAB9:
         case IDB_STATREC_DRIVER_ENAB10:
         case IDB_STATREC_DRIVER_ENAB11:
         case IDB_STATREC_DRIVER_ENAB12:
         case IDB_STATREC_DRIVER_ENAB13:
         case IDB_STATREC_DRIVER_ENAB14:
         case IDB_STATREC_DRIVER_ENAB15:
         case IDB_STATREC_DRIVER_ENAB16:
         case IDB_STATREC_DRIVER_ENAB17:
         case IDB_STATREC_DRIVER_ENAB18:
         case IDB_STATREC_DRIVER_ENAB19:
         case IDB_STATREC_DRIVER_ENAB20:
         case IDB_STATREC_DRIVER_ENAB21:
         case IDB_STATREC_DRIVER_ENAB22:
         case IDB_STATREC_DRIVER_ENAB23:
         case IDB_STATREC_DRIVER_ENAB24:
         case IDB_STATREC_DRIVER_ENAB25:
         case IDB_STATREC_DRIVER_ENAB26:
         case IDB_STATREC_DRIVER_ENAB27:
         case IDB_STATREC_DRIVER_ENAB28:
         case IDB_STATREC_DRIVER_ENAB29:
         case IDB_STATREC_DRIVER_ENAB30:
         case IDB_STATREC_DRIVER_ENAB31:
         case IDB_STATREC_DRIVER_ENAB32:
         case IDB_STATREC_DRIVER_ENAB33:
         case IDB_STATREC_DRIVER_ENAB34:
         case IDB_STATREC_DRIVER_ENAB35:
         case IDB_STATREC_DRIVER_ENAB36:
         case IDB_STATREC_DRIVER_ENAB37:
         case IDB_STATREC_DRIVER_ENAB38:
         case IDB_STATREC_DRIVER_ENAB39:
         case IDB_STATREC_DRIVER_ENAB40:{
            #define SIZITEMTEXT 4
            if(HIWORD(wParam) == EN_CHANGE){
               char szItemText[SIZITEMTEXT];
               GetDlgItemText(hDlg,LOWORD(wParam),szItemText,SIZITEMTEXT);
               if( (szItemText[0] == 'E') || (szItemText[0] == 'e') ){
                  SendMessagePackets(hDlg, LOWORD(wParam) - IDB_STATREC_DRIVER_ENAB, MsTyp6_ENAB_DISAB,  ENAB_POWR, 0);
                  SetFocus(hOKbutton);
                 }
               else if( (szItemText[0] == 'D') || (szItemText[0] == 'd') ){
                  SendMessagePackets(hDlg, LOWORD(wParam) - IDB_STATREC_DRIVER_ENAB, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0);
                  SetFocus(hOKbutton);
                 }
              }
           }
           break;
         case IDB_STATREC_DRIVER_ENABall:{
            int nEM_SR5;
            for(nEM_SR5 = 0; nEM_SR5 < NOOFMOTORS; nEM_SR5++){
               if( SendMessagePackets(hDlg, nEM_SR5, MsTyp6_ENAB_DISAB,  ENAB_POWR, 0) )
                 nEM_SR5 =  1000;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR5 > 999)
              MessageBox(hDlg,"As a message to ENABLE one of the motors failed,\nI didn't send the ENABLE message to other motors.\n\nThis may mean that some of (or all) the motors are not ENABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_DRIVER_DISABall:{
            int nEM_SR6;
            for(nEM_SR6 = 0; nEM_SR6 < NOOFMOTORS; nEM_SR6++){
               if( SendMessagePackets(hDlg, nEM_SR6, MsTyp6_ENAB_DISAB,  DISAB_POWR, 0) )
                 nEM_SR6 =  1000;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR6 > 999)
              MessageBox(hDlg,"As a message to DISABLE one of the motors failed,\nI didn't send the DISABLE message to other motors.\n\nThis may mean that some of (or all) the motors are not DISABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RS485_ENAB:
         case IDB_STATREC_RS485_ENAB2:
         case IDB_STATREC_RS485_ENAB3:
         case IDB_STATREC_RS485_ENAB4:
         case IDB_STATREC_RS485_ENAB5:
         case IDB_STATREC_RS485_ENAB6:
         case IDB_STATREC_RS485_ENAB7:
         case IDB_STATREC_RS485_ENAB8:
         case IDB_STATREC_RS485_ENAB9:
         case IDB_STATREC_RS485_ENAB10:
         case IDB_STATREC_RS485_ENAB11:
         case IDB_STATREC_RS485_ENAB12:
         case IDB_STATREC_RS485_ENAB13:
         case IDB_STATREC_RS485_ENAB14:
         case IDB_STATREC_RS485_ENAB15:
         case IDB_STATREC_RS485_ENAB16:
         case IDB_STATREC_RS485_ENAB17:
         case IDB_STATREC_RS485_ENAB18:
         case IDB_STATREC_RS485_ENAB19:
         case IDB_STATREC_RS485_ENAB20:
         case IDB_STATREC_RS485_ENAB21:
         case IDB_STATREC_RS485_ENAB22:
         case IDB_STATREC_RS485_ENAB23:
         case IDB_STATREC_RS485_ENAB24:
         case IDB_STATREC_RS485_ENAB25:
         case IDB_STATREC_RS485_ENAB26:
         case IDB_STATREC_RS485_ENAB27:
         case IDB_STATREC_RS485_ENAB28:
         case IDB_STATREC_RS485_ENAB29:
         case IDB_STATREC_RS485_ENAB30:
         case IDB_STATREC_RS485_ENAB31:
         case IDB_STATREC_RS485_ENAB32:
         case IDB_STATREC_RS485_ENAB33:
         case IDB_STATREC_RS485_ENAB34:
         case IDB_STATREC_RS485_ENAB35:
         case IDB_STATREC_RS485_ENAB36:
         case IDB_STATREC_RS485_ENAB37:
         case IDB_STATREC_RS485_ENAB38:
         case IDB_STATREC_RS485_ENAB39:
         case IDB_STATREC_RS485_ENAB40:{
            #define SIZITEMTEXT 4
            if(HIWORD(wParam) == EN_CHANGE){
               int nEM_SR1;
               int nSelctdMotor = LOWORD(wParam) - IDB_STATREC_RS485_ENAB;
               char szItemText[SIZITEMTEXT];
               GetDlgItemText(hDlg,LOWORD(wParam),szItemText,SIZITEMTEXT);
             //if(szItemText[0] == '4'){
               if( (szItemText[0] == 'E') || (szItemText[0] == 'e') ){
                  if(szItemText[0] == 'e'){ //For lower case enabling of RS485, we disable RS485 for all the other motor, so only 1 motor is RS485 ACTIVE
                     for(nEM_SR1 = 0; nEM_SR1 < NOOFMOTORS; nEM_SR1++){
                        if(nEM_SR1 != nSelctdMotor) //Don't try to disable the current motor... aftr all we are trying to enable it!
                          if( SendMessagePackets(hDlg, nEM_SR1, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
                            nEM_SR1 =  1000;
                        Sleep(5);
                       }
                     if(nEM_SR1 > 999)
                       MessageBox(hDlg,"Normally, before enabling RS485 for the selected motor, 'Disable RS485' messages\nare sent to all the other motors.\n\nOne of these 'Disable RS485' messages failed (on this occassion) so I gave up on\nsending this message to the other motors. (Suggest you view the Status Screen)\n\nThis may mean the is contention for the RS485 bus.\n\nWhen you press 'OK' I will still send the 'Enable RS485' message to the current motor","Warning - contention for the RS485 bus is a possibility",MB_OK);
                     SendMessagePackets(hDlg, nSelctdMotor, MsTyp6_ENAB_DISAB,   ENAB_485, 0); //Enable
                    }
                  else //For Upper case enabling of RS485, MANY MOTORS ARE ALLOWED TO BE RS485 ACTIVE (*** which is likely to cause bus contention ***)
                    SendMessagePackets(hDlg, LOWORD(wParam) - IDB_STATREC_RS485_ENAB, MsTyp6_ENAB_DISAB,   ENAB_485, 0);
                  SetFocus(hOKbutton);
                 }
             //else if(szItemText[0] == '5'){
               else if( (szItemText[0] == 'D') || (szItemText[0] == 'd') ){
                  SendMessagePackets(hDlg, LOWORD(wParam) - IDB_STATREC_RS485_ENAB, MsTyp6_ENAB_DISAB,  DISAB_485, 0);
                  SetFocus(hOKbutton);
                 }
              }
           }
           break;
         case IDB_STATREC_RS485_ENABall:{
            int nEM_SR2;
            for(nEM_SR2 = 0; nEM_SR2 < NOOFMOTORS; nEM_SR2++){
               if( SendMessagePackets(hDlg, nEM_SR2, MsTyp6_ENAB_DISAB,  ENAB_485, 0) )
                 nEM_SR2 =  1000;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR2 > 999)
              MessageBox(hDlg,"As a message to ENABLE RS485 on one of the motors failed,\nI didn't send the ENABLE RS485 message to other motors.\n\nThis may mean that some of (or all) the motors are not RS485 ENABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RS485_DISABall:{
            int nEM_SR3;
            for(nEM_SR3 = 0; nEM_SR3 < NOOFMOTORS; nEM_SR3++){
               if( SendMessagePackets(hDlg, nEM_SR3, MsTyp6_ENAB_DISAB,  DISAB_485, 0) )
                 nEM_SR3 =  1000;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR3 > 999)
              MessageBox(hDlg,"As a message to DISABLE RS485 on one of the motors failed,\nI didn't send the DISABLE RS485 message to other motors.\n\nThis may mean that some of (or all) the motors are not RS485 DISABLED.","Warning",MB_OK);
           }
           break;
         case IDB_STATREC_RESETm01:
         case IDB_STATREC_RESETm02:
         case IDB_STATREC_RESETm03:
         case IDB_STATREC_RESETm04:
         case IDB_STATREC_RESETm05:
         case IDB_STATREC_RESETm06:
         case IDB_STATREC_RESETm07:
         case IDB_STATREC_RESETm08:
         case IDB_STATREC_RESETm09:
         case IDB_STATREC_RESETm10:
         case IDB_STATREC_RESETm11:
         case IDB_STATREC_RESETm12:
         case IDB_STATREC_RESETm13:
         case IDB_STATREC_RESETm14:
         case IDB_STATREC_RESETm15:
         case IDB_STATREC_RESETm16:
         case IDB_STATREC_RESETm17:
         case IDB_STATREC_RESETm18:
         case IDB_STATREC_RESETm19:
         case IDB_STATREC_RESETm20:
         case IDB_STATREC_RESETm21:
         case IDB_STATREC_RESETm22:
         case IDB_STATREC_RESETm23:
         case IDB_STATREC_RESETm24:
         case IDB_STATREC_RESETm25:
         case IDB_STATREC_RESETm26:
         case IDB_STATREC_RESETm27:
         case IDB_STATREC_RESETm28:
         case IDB_STATREC_RESETm29:
         case IDB_STATREC_RESETm30:
         case IDB_STATREC_RESETm31:
         case IDB_STATREC_RESETm32:
         case IDB_STATREC_RESETm33:
         case IDB_STATREC_RESETm34:
         case IDB_STATREC_RESETm35:
         case IDB_STATREC_RESETm36:
         case IDB_STATREC_RESETm37:
         case IDB_STATREC_RESETm38:
         case IDB_STATREC_RESETm39:
         case IDB_STATREC_RESETm40:
           SendMessagePackets(hDlg, LOWORD(wParam) - IDB_STATREC_RESETm01, MsTyp4_RESET, 0, 0);   //0x04 (RESET Message)
           SetFocus(hOKbutton);
           break;
         case IDB_STATREC_HELP:
         //MessageBox(hDlg,"This button needs a more informative dialogbox!","Note",MB_OK);
           DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);    //Sends 0x07 MsTyp7_SING_MM_OTF, MsTyp8_SET_PULSE_LENGTH
           SetFocus(hOKbutton);
           break;
         case IDB_STATREC_HOMEALL:{
#if 0
            int nEM_SR7, nCurrent_uSteps;
            //for(nEM_SR7 = 0; nEM_SR7 < NOOFMOTORS; nEM_SR7++){
            for(nEM_SR7 = 0; nEM_SR7 <29; nEM_SR7++){
               nCurrent_uSteps = PCFrec.chNoofUsteps4EachMotorTab[nEM_SR7];//nCurrent_uSteps can be 1,2,4,8,16,32,or64
               if(SendMessagePackets(hDlg,nEM_SR7, MsTyp3_HOME,nCurrent_uSteps/*The constant Slow speed to move at, when homing out*/, nArFastSpeeds4HomingDeptOn_uSteps[nCurrent_uSteps] ) ) //The constant Fast speed to move at, when homing in
                 nEM_SR7 =  1000;
               Sleep(20); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR7 > 999)
              MessageBox(hDlg,"As a message to HOME one of the motors failed,\nI didn't send the HOME message to other motors.\n\nThis may mean that some of (or all) the motors are not HOMED.","Warning",MB_OK);
#endif
            MoveToStartPositions(hDlg, bHIDEMESSAGES, bSTOP_WHEN_HOMED);//really this is MoveToHOMPositions() because of last parameter
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
           }
           break;
         case IDB_STATREC_RESETmALL:{
            int nEM_SR4;
            for(nEM_SR4 = 0; nEM_SR4 < NOOFMOTORS; nEM_SR4++){
               if( SendMessagePackets(hDlg, nEM_SR4, MsTyp4_RESET,  0, 0) )
                 nEM_SR4 =  1000;
               Sleep(5); //Allows all Arp requests for all motors to be seen through RS485 when using an unintelligent switch
              }
            SetFocus(hOKbutton);//This button, being small, looks unsightly if focused
            if(nEM_SR4 > 999)
              MessageBox(hDlg,"As a message to RESET one of the motors failed,\nI didn't send the RESET message to other motors.\n\nThis may mean that some of (or all) the motors are not RESET.","Warning",MB_OK);
           }
           break;
         case IDCANCEL:
           KillTimer(hDlg, IDT_TIMER1);
           bSTATREC_BOXisBeingDisplayed = FALSE;

           DeleteObject(newFont);
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
//    case WM_SETFONT:
      //return (INT_PTR)TRUE;
//      return (INT_PTR)FALSE;	  
    case WM_TIMER:
      KillTimer(hDlg, IDT_TIMER1);
      SendMessage(hDlg, WM_COMMAND, IDB_STATREC_REFRESH_NOKILLTIMER, 0);
      return (INT_PTR)TRUE;
   }//switch (message){
 return (INT_PTR)FALSE;
}


#define SIZSTRING_SSM 80   //SSM==ShowStatusMultiple

void UpdateStatusDialogsControlsM(HWND hDlg, int MotorNo){
 char szStringSSM[SIZSTRING_SSM];
 //int ItemporarilyOverRodeMotorNo; //Because there is only One board to talk to at the moment
 //MotorNo = 18;
 int nReturnCodeFromReceivePackets;
 int nReturnCodeFromSendPackets;

 if(!nMotor2ExpectRepliesFrom[MotorNo]){
    SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "OK No Reply expected");
    SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, " ");
    ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
    return;
   }

 DWORD dwWaitResult = WaitForSingleObject( ghMutex, INFINITE);  // no time-out interval
 switch (dwWaitResult) {  // The thread got ownership of the mutex
    case WAIT_OBJECT_0:
      __try {
         //jj bWaitingForReply1[MotorNo] = 1;//Set before calling SendMessagePackets() whenever a reply is expected
         nReturnCodeFromSendPackets = SendMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, ++cMessageSeqNoSR[MotorNo], 0); //As the comms are repeatedly called by a timer it is 

         if(nReturnCodeFromSendPackets){
            KillTimer(hDlg, IDT_TIMER1);
            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
            if(nReturnCodeFromSendPackets == 5001){
               CEC.SendPktErrStatScr5001++;
               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "Send() function took > 0.2 seconds");
               SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, " ");
              }
            else{
               CEC.SendPktErrStatScrOther++;
               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "COMMS Send Problem");
               StringCchCopy(szStringSSM,SIZSTRING_SSM, "Error Code: ");
               _itoa_s(nReturnCodeFromSendPackets, szStringSSM + 12, SIZSTRING_SSM - 12, 10);
               SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, szStringSSM);
              }
            MessageBox(hDlg,"I stopped so you can see(1)!","nReturnCodeFromSendPackets Error",MB_OK);
            return;
           }

         bWaitingForReply1[MotorNo] = 1; //Set before calling R eceiveMessagePackets() as a reply is expected
         nReturnCodeFromReceivePackets = ReceiveMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, 0);      // best to avoid MessageBoxes (hence use of NULLs)

//This next if Statement is ALSO VERY doubtful...
//This next if Statement is VERY doubtful... but will help, during debugging, to show if we have not waited long enough for the reply.
         if(nReturnCodeFromReceivePackets == 6666){
            KillTimer(hDlg, IDT_TIMER1);
            int j;
            for(j = 0; j<100; j++){
               bWaitingForReply1[MotorNo] = 1;//set again here because the reply failed
               nReturnCodeFromReceivePackets = ReceiveMessagePackets(NULL, MotorNo, MsTyp2_ZENDstatREC, 0); //will have cleared this.
               if(!nReturnCodeFromReceivePackets){
                  MessageBox(hDlg,"Is now OK!\nWe are in UpdateStatusDialogsControlsM()","nReturnCodeFromSendPackets Error",MB_OK);
                  j = 1000;
                 }
               if(j > 200)
                 if(!SetTimer(hDlg, IDT_TIMER1, AUTOREFRESHRATE, (TIMERPROC) NULL))
                    MessageBox(hDlg,"STATREC_AUTO_TIMER reset problem!",NULL,MB_OK);
              }
           }


         if(nReturnCodeFromReceivePackets){ //i.e. != 0
            KillTimer(hDlg, IDT_TIMER1);
            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
            SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, "COMMS Problem");
            if(nReturnCodeFromReceivePackets == 6666){
               CEC.RecPktErrStatScr6666++;
               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "Error Code: 6666 - No Reply");
              }
            else{
               if(nReturnCodeFromReceivePackets == 123456){
                  CEC.RecPktErrStatScr123456++;
                  StringCchCopy(szStringSSM,SIZSTRING_SSM,"Didn't receive a StatusRec!"); //Received something, but not 0x77 message type
                 }
               else{
                  CEC.RecPktErrStatScrOther++;
                  StringCchCopy(szStringSSM,SIZSTRING_SSM,"Error Code: ");
                  _itoa_s(nReturnCodeFromReceivePackets, szStringSSM + 12, SIZSTRING_SSM - 12, 10);
                 }
               SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szStringSSM);
              }
            MessageBox(hDlg,"I stopped so you can see(2)!\n\nYou may need to wait a few seconds for\nsystem reponse to  be restored","nReturnCodeFromReceivePackets Error",MB_OK);
            return;
           }
         if(chReceiveBuffer[MotorNo][1] != cMessageSeqNoSR[MotorNo]){
            CEC.RecPktErrStatScrSeqNo++;
            KillTimer(hDlg, IDT_TIMER1);
            ClearMostFields(hDlg, MotorNo); //leaves IDB_STATREC_SERIOUSWARN & IDB_STATREC_GENERALWARN
            #define SIZszSeriousText 24
            #define SIZszWarnText    28
            char szSeriousText[SIZszSeriousText];
            char szWarnText[SIZszWarnText];
            StringCchCopy(szSeriousText, SIZszSeriousText, "Message seqNo Sent:    ");
            _itoa_s(cMessageSeqNoSR[MotorNo], szSeriousText+20, SIZszSeriousText - 20, 10);
            SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szSeriousText);
            StringCchCopy(szWarnText, SIZszWarnText, "Message seqNo Received:    ");
          //_itoa_s(chReceiveBuffer[nCurrentMotorNo][1], szWarnText+24, SIZszWarnText - 24, 10);
            _itoa_s(chReceiveBuffer[MotorNo][1], szWarnText+24, SIZszWarnText - 24, 10);
            SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, szWarnText);
            MessageBox(hDlg,"I stopped so you can see(3)!","Seqno Error",MB_OK);
            return;
           }
         pStatRec = (STATUSRECORD*)&chReceiveBuffer[MotorNo][2]; //+2 because the reply from AVR contains a message-type in the first 2 bytes


         UseRecvStatRecToUpdateTheFields(hDlg, MotorNo, pStatRec);

         //pStatRec->Length = 0x3f;
        }
      __finally {  // Release ownership of the mutex object
         if (! ReleaseMutex(ghMutex)) 
           MessageBox(hDlg,"ReleaseMutex(ghMutex) Failed 7!",NULL,MB_OK);
        }
      break;
    case WAIT_ABANDONED:  // The thread got ownership of an abandoned mutex
      MessageBox(hDlg,"Got ownership of an abandoned mutex@7!",NULL,MB_OK);
      break;
   }
}

void ClearMostFields(HWND hDlg, int MotorNo){
 SetDlgItemText(hDlg, IDB_STATREC_POS         + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_HOME_STATE  + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWa   + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWb   + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_HOME_SW     + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_DRIVER_ENAB + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_RS485_ENAB  + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_IWANTLow    + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_IT_IS_Low   + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_REDYTPAL    + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_VARa        + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_VARb        + MotorNo, "");
 SetDlgItemText(hDlg, IDB_STATREC_VARc        + MotorNo, "");
}

void UseRecvStatRecToUpdateTheFields(HWND hDlg, int MotorNo, STATUSRECORD *pStatRec){
 char szuStepsFromHome[15];
 char szVARa[15];
 char szVARb[15];
 char szVARc[15];
 char szStringSSM[SIZSTRING_SSM];

 endian_swap(*(unsigned int*)pStatRec); //This is for the uStepsFromHome integer only
 endian_swap(*(((unsigned int*)pStatRec)+1));
 endian_swap(*(((unsigned int*)pStatRec)+2));
 endian_swap(*(((unsigned int*)pStatRec)+3));

 _itoa_s(pStatRec->uStepsFromHome, szuStepsFromHome, 15, 10);
 if(pStatRec->usHomingState) //usHomingState should be zero, if homing has occurred (& so position is known)
   StringCchCat(szuStepsFromHome,15," UH");

 SetDlgItemText(hDlg, IDB_STATREC_POS + MotorNo, szuStepsFromHome);

 _itoa_s(pStatRec->uVARa, szVARa, 15, 10);
 SetDlgItemText(hDlg, IDB_STATREC_VARa + MotorNo, szVARa);
 _itoa_s(pStatRec->uVARb, szVARb, 15, 10);
 SetDlgItemText(hDlg, IDB_STATREC_VARb + MotorNo, szVARb);
 _itoa_s(pStatRec->uVARc, szVARc, 15, 10);
 SetDlgItemText(hDlg, IDB_STATREC_VARc + MotorNo, szVARc);

                                ///1st 4b
 //EVK sent     0000 1011
 //PC received 1101 0000, so the bits are completely reversed in order.
 //To check this applies also to the 2-bit usHomingState field,
 //EVK sent     0000 1001
 //PC received 1001 0000 (so it does)
 //The first byte sent is the first byte received, so to correct the Endianess (or bit ordering?), we need a function that processes bytes
 if(pStatRec->Limit1Switch)
   SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWa + MotorNo, "X");
 else
   SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWa + MotorNo, "");
 if(pStatRec->Limit2Switch)
   SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWb + MotorNo, "X");
 else
   SetDlgItemText(hDlg, IDB_STATREC_LIMIT_SWb + MotorNo, "");
 if(pStatRec->HomeSwitch)
   SetDlgItemText(hDlg, IDB_STATREC_HOME_SW + MotorNo, "X");
 else
   SetDlgItemText(hDlg, IDB_STATREC_HOME_SW + MotorNo, "");

 if(pStatRec->ImReadyTPAL)
   SetDlgItemText(hDlg, IDB_STATREC_REDYTPAL + MotorNo, "Y");
 else
   SetDlgItemText(hDlg, IDB_STATREC_REDYTPAL + MotorNo, "N");

                                ///2nd 4b
 if(pStatRec->bDriverEnabled)
   SetDlgItemText(hDlg, IDB_STATREC_DRIVER_ENAB + MotorNo, "X");
 else 
   SetDlgItemText(hDlg, IDB_STATREC_DRIVER_ENAB + MotorNo, "");
 if(pStatRec->bRS485Enabled)
   SetDlgItemText(hDlg, IDB_STATREC_RS485_ENAB + MotorNo, "X");
 else
   SetDlgItemText(hDlg, IDB_STATREC_RS485_ENAB + MotorNo, "");

 if(!pStatRec->usHomingState)     //I think I just have to accept the bit reversing means HomingState 1 is shown
   SetDlgItemText(hDlg, IDB_STATREC_HOME_STATE + MotorNo, "H");
 else if(pStatRec->usHomingState == 1)
   SetDlgItemText(hDlg, IDB_STATREC_HOME_STATE + MotorNo, "U");
 else if(pStatRec->usHomingState == 2)
   SetDlgItemText(hDlg, IDB_STATREC_HOME_STATE + MotorNo, "I");//Currently homing In  (towards Home / motor)
 else if(pStatRec->usHomingState == 3)
   SetDlgItemText(hDlg, IDB_STATREC_HOME_STATE + MotorNo, "O");//Currently homing Out (Away from Home / motor)
                                ///BYTE
 if(pStatRec->SeriousWarnings > SIZ_SERIOUS_WARNS_TABLE)
   StringCchCopy(szStringSSM, SIZSTRING_SSM, "Need2updateHoloProg Lookup SW" );
 else
   StringCchCopy(szStringSSM, SIZSTRING_SSM, *(SeriousWarningsStrings + pStatRec->SeriousWarnings) );
 SetDlgItemText(hDlg, IDB_STATREC_SERIOUSWARN + MotorNo, szStringSSM);

                                ///BYTE
 /*if(pStatRec->Warnings2Report){
  _itoa_s(pStatRec->Warnings2Report, szStringSSM, SIZSTRING_SSM, 10);
    SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szStringSSM);
   }
 else
   SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, "ok"); */
 if(pStatRec->Warnings2Report > SIZ_WARNS2REPORT_TABLE)
   StringCchCopy(szStringSSM, SIZSTRING_SSM, "Need2updateW2RLookup (HoloProg)" );
 else
   StringCchCopy(szStringSSM, SIZSTRING_SSM, *(Warnings2ReportStrings + pStatRec->Warnings2Report) );
 SetDlgItemText(hDlg, IDB_STATREC_GENERALWARN + MotorNo, szStringSSM);

                                ///4th BYTE
 if(pStatRec->IWantStatusOutLineLow)
   SetDlgItemText(hDlg, IDB_STATREC_IWANTLow + MotorNo, "L");
 else
   SetDlgItemText(hDlg, IDB_STATREC_IWANTLow + MotorNo, "H");
 if(pStatRec->StatusOutLineIsHi)
   SetDlgItemText(hDlg, IDB_STATREC_IT_IS_Low + MotorNo, "H");
 else
   SetDlgItemText(hDlg, IDB_STATREC_IT_IS_Low + MotorNo, "L");
}
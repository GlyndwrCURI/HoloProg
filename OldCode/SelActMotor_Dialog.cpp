#include "stdafx.h"
                                                 This whole dialogbox is no longer required!
//extern int  CurrentMotor_ZeroBased;
extern CRegStdWORD CurrentMotor_ZeroBased;
//extern int  nMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec
extern HWND hGLWnd;
extern PCFRECORD PCFrec;
//Note: the user will think of motors in the range 1 to 36 (now 40) but, apart from giving the user this impression, this program
// will consider the motors to be in the range 0 to 35 (now 39).  They will have IP addresses 192.168.0.201 to 192.168.0.240
INT_PTR CALLBACK SelActMotor_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 static int nOpeningCurrentMotorNo;
 RECT rClient;
 switch (message){
    case WM_INITDIALOG:{
       #define SIZszDumMotr 20
       char szDumMotr[SIZszDumMotr];
       nOpeningCurrentMotorNo = CurrentMotor_ZeroBased;
       StringCchCopy(szDumMotr, SIZszDumMotr, "Motor ");
       for(int j = 0; j<NOOFMOTORS; j++){
          _itoa_s(j+1, szDumMotr+6, SIZszDumMotr - 6, 10);
          SendDlgItemMessage(hDlg,IDB_ACTIVE_MOTOR_CHOSEN,CB_ADDSTRING,0,(LPARAM)szDumMotr);
         }
       SendDlgItemMessage (hDlg, IDB_ACTIVE_MOTOR_CHOSEN, CB_SETCURSEL, CurrentMotor_ZeroBased, 0L);
       //I want the drop down list to be dropped down, but Windows' timing issues prevent sending a CB_SHOWDROPDOWN message here (before 
       // the list is even displayed), hence the need to set a timer.  When the timer fires we can send the CB_SHOWDROPDOWN message.
       if(!SetTimer(hDlg, IDT_TIMER_SEL_ACT_Mot, 140, (TIMERPROC) NULL))
         MessageBox(hDlg,"IDT_TIMER_SEL_ACT_Mot problem",NULL,MB_OK);
      }
      return (INT_PTR)TRUE;
    case WM_TIMER:
      KillTimer(hDlg, IDT_TIMER_SEL_ACT_Mot);
      SendDlgItemMessage (hDlg, IDB_ACTIVE_MOTOR_CHOSEN, CB_SHOWDROPDOWN, 1, 0L);
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_ACTIVE_MOTOR_CHOSEN:
           if (HIWORD(wParam) == CBN_SELENDOK) {//user has selected something from the list
              if (CurrentMotor_ZeroBased != SendMessage(GetDlgItem(hDlg,IDB_ACTIVE_MOTOR_CHOSEN),CB_GETCURSEL,0,0L)){
                 PCFrec.nMotor2ExpectRepliesFrom[CurrentMotor_ZeroBased] = 0;
                 CurrentMotor_ZeroBased = (UINT)SendMessage(GetDlgItem(hDlg,IDB_ACTIVE_MOTOR_CHOSEN),CB_GETCURSEL,0,0L);
                 PCFrec.nMotor2ExpectRepliesFrom[CurrentMotor_ZeroBased] = 1;
               //config.SetScreenRes(CurrentMotor_ZeroBased);
                 redraw(hGLWnd, &rClient);
                }
             }
           break;
         case IDCANCEL:
           PCFrec.nMotor2ExpectRepliesFrom[CurrentMotor_ZeroBased] = 0;
           CurrentMotor_ZeroBased = nOpeningCurrentMotorNo;
           PCFrec.nMotor2ExpectRepliesFrom[CurrentMotor_ZeroBased] = 1;
         case IDOK: //Note there is no break in the preceding line
           EndDialog(hDlg, LOWORD(wParam));
           redraw(hGLWnd, &rClient);
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
   }//switch (message){
 return (INT_PTR)FALSE;
}
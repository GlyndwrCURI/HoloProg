#include "stdafx.h"
int nHelpMessageToLoad;

INT_PTR CALLBACK HelpDialog1_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 switch (message){
    case WM_INITDIALOG:{
       RECT ParentRc;
       SETTEXTEX stex;  //typedef struct _settextex { DWORD flags;  UINT codepage; } SETTEXTEX;
       stex.flags = ST_DEFAULT;
       stex.flags = ST_SELECTION;
       stex.codepage = CP_ACP;
       GetWindowRect(GetParent(hDlg), &ParentRc);
       if(nHelpMessageToLoad == 1)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"The two columns showing which motors have power enabled and which\nmotors have RS485 enabled, can also be used to switch these on or off\non a motor-by-motor basis.\n\nYou achieve this by clicking on the fields and typing 'E' to Enable them\nor 'D' to Disable them.  NOTE Auto-Refresh has to be running!\n\nFor the RS485 column, depending on if you type a lower case 'e' or an\nupper case 'E', RS485 will be enabled either for that one motor alone,\nor for that one motor in addition to any other motors that already have\nRS485 enabled.\n\nNote that it is generally NOT A GOOD IDEA to have RS485 enabled for\nmore than one motor at a time because two controllers sending messages\nat the same time will cause bus contention, which will scramble or destroy\nboth of their messages.");
       else if(nHelpMessageToLoad == 2)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Dip switches 1 to 6 set IP addresses (1 is LSB).\n\nIf all are up, IP address is 192.168.0.201,\n\nIf dip 1 is down IP address will be ...202  etc.\n\nDip 7 is checksum disable (when set).\n\nDip 8 is Enable RS485 (when set).");
       else if(nHelpMessageToLoad == 3){
          SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"\n\t   The most useful, single-key Hotkeys are:\n\n\t            'S'\t  Stop the PID Motors\n\n\t           'O'\t  Open a hologram config file\n\n\t           'Q'\t  Printer Qonfig file\n\n\t            'V'\t  View Hologram Info\n\n\t   'P' or 'N'\t  Power / No Power\n\n\t   '4' or '5'\t  RS485 Enable / Disable\n\n\t   'X' or 'C'\t  Exec Terminal / Clear Terminal");
          SetWindowPos(hDlg, NULL, 380, ParentRc.top +50, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
          break;
         }
       else if(nHelpMessageToLoad == 4)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Suggestions:\n\nSee how far outside the limits the motor needs to move.\n\nIf X axis Minimum Physical Limit exceeded, you could:\n\t-Increase XposOfOrigin for the hologram\n\t-Reduce number of pre Pixel zero Const speed Pixels\n\t-Use a faster acceleration profile\n\t-Alter the Printer Configuration Physical Limits Max and Min\n\t  settings (but only if they're wrong!)\n\t-Reduce the distance between pixel centers (slower speeds\n\t  require less acceleration)\n\nIf X axis Maximum Physical Limit exceeded, you could:\n\t-All the above are applicable, but reduce XposOfOrigin for the\t hologram rather than increase it.");
       else if(nHelpMessageToLoad == 5)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Suggestions:\n\nSee how far outside the limits the motor needs to move.\n\nIf Y axis Minimum Physical Limit exceeded, you could:\n\t-Increase YposOfOrigin for the hologram\n\t-Alter the Printer Configuration Physical Limits Max and Min\n\t  settings (but only if they're wrong!)\n\nIf Y axis Maximum Physical Limit exceeded, you could:\n\t-Reduce YposOfOrigin for the hologram\n\t-Reduce the distance between pixel centers");
       else if(nHelpMessageToLoad == 6)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Suggestions:\n\nSee how far outside the limits the motor needs to move.\n\nFor Horizontal goniometer you could try:\n\n\t-Alter the Printer Configuration Physical Limits Max and Min\n\t  settings (but only if they're wrong!)\n\n\t-Reduce this motor's prePIXzeroCONSTspeedPIXELS\n\n\t-Increase the Accel/Deleration profile (if motor is capable)\n\n\t-Alter the hologram geometry");
       else if(nHelpMessageToLoad == 7)
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Suggestions:\n\nSee how far outside the limits the motor needs to move.\n\nFor Vertical goniometer you could try:\n\n\t-Alter the Printer Configuration Physical Limits Max and Min\n\t  settings (but only if they're wrong!)\n\n\t-Reduce this motor's prePIXzeroCONSTspeedPIXELS\n\n\t-Increase the Accel/Deleration profile (if motor is capable)\n\n\t-Alter the hologram geometry");
       else
         SendDlgItemMessage(hDlg,IDC_RICHEDIT21,EM_SETTEXTEX,(WPARAM)&stex,(LPARAM)"Someone hasn't coded this nHelpMessageToLoad!");
       SetWindowPos(hDlg, NULL, 380, ParentRc.bottom -320, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}
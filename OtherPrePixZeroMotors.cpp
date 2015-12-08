#include "stdafx.h"

extern HCFRECORD HCFrec;
extern char mes[];

INT_PTR CALLBACK VHI_OtherPrePixZeroConsts_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 char szDum[SIZSZDUM12];
 static HCFRECORD VHICopyOfOrigHCFrec;
 static HWND hNOOFprePIXzeroCONSTspeedPIXELS[36];
 int j,k;
 switch (message){
    case WM_INITDIALOG:{
       VHICopyOfOrigHCFrec = HCFrec;
       for(k = 0; k < 100; k++){
          _itoa_s(k,szDum,SIZSZDUM12,10);
          for(j = 0; j < 6; j++){
             SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM07 + j, CB_ADDSTRING, 0,(LPARAM)szDum);
             SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM19 + j, CB_ADDSTRING, 0,(LPARAM)szDum);
             SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM31 + j, CB_ADDSTRING, 0,(LPARAM)szDum);
            }
         }
       for(j = 0; j < 25; j += 12){ //J is 0 12 & 24
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM07 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[6  + j], 0L);
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM08 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[7  + j], 0L);
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM09 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[8  + j], 0L);
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM10 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[9  + j], 0L);
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM11 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[10 + j], 0L);
          SendDlgItemMessage(hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM12 + j, CB_SETCURSEL, HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[11 + j], 0L);
          hNOOFprePIXzeroCONSTspeedPIXELS[6  + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM07 + j);
          hNOOFprePIXzeroCONSTspeedPIXELS[7  + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM08 + j);
          hNOOFprePIXzeroCONSTspeedPIXELS[8  + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM09 + j);
          hNOOFprePIXzeroCONSTspeedPIXELS[9  + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM10 + j);
          hNOOFprePIXzeroCONSTspeedPIXELS[10 + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM11 + j);
          hNOOFprePIXzeroCONSTspeedPIXELS[11 + j] =  GetDlgItem (hDlg, IDC_VHI_OTHER_prePIXzeroCONSTspeedPIXELSM12 + j);
         } 
       RECT ParentRc;           //Display dialog, usefully positioned 
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, ParentRc.left + 279, ParentRc.top + 10, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDOK:
           for(j = 0; j < 25; j += 12) //J is 0, 12, & 24
             for(k = 6; k < 12; k++){    //So we look at motors 7-12 19-24 and 31-36
                HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[j + k] = SendMessage(hNOOFprePIXzeroCONSTspeedPIXELS[j + k], CB_GETCURSEL,0,0);
                if( HCFrec.NOOFprePIXzeroCONSTspeedPIXELS[j + k] == CB_ERR){
                   sprintf_s(mes,SIZMES, "Error trying to read field: M%d\n\nPlease **select** values (Don't type them)", j+k+1 );
                   MessageBox(hDlg,mes,NULL,MB_OK);
                   j = 99; k = 13;
                  }
                }
           if(j < 98)
             EndDialog(hDlg, LOWORD(wParam));
           break;
         case IDCANCEL:
           HCFrec = VHICopyOfOrigHCFrec;
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}
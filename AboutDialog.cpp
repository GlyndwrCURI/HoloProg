#include "stdafx.h"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 UNREFERENCED_PARAMETER(lParam);
 switch (message){
    case WM_INITDIALOG:{
       RECT ParentRc;           //Display dialog positioned to cover the bitmap on the menu bar
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, ParentRc.right - 320, ParentRc.top + 32, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
         EndDialog(hDlg, LOWORD(wParam));
         return (INT_PTR)TRUE;
        }
      break;
   }
 return (INT_PTR)FALSE;
}
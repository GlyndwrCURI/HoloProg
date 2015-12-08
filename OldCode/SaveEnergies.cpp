#include "stdafx.h"

//Procedure for handling the "Save Hologram config file" DialogBox.  This collects a filename.
INT_PTR CALLBACK SaveEnergiesReadFile_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 //char szLocalSaveConfigFileName[_MAX_DIR];
 switch (message){
    case WM_INITDIALOG:
      //StringCchCopy( szGlobalFileName, _MAX_DIR, stdstring(LastHGramFileOpened).c_str() );
      //SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szGlobalFileName);

      MessageBox(hDlg, "If the current hologram is open for Editing, use the View Hologram Info screen\nand, from there, press either:\n   the 'Save Incomplete'\nor\n   the 'Generate, Complete and Save' Button\n\nHolgrams opened for printing cannot be saved [again] - they are already saved",
           "Message - access to 'Save Hologram Config file' is disabled...", MB_OK);

      SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, stdstring(LastHGramFileOpened).c_str() );
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_TRANSBLOCK_CHOOSEFILE:
           mGetFile(hDlg, IDS_FILTERSTR_HOLO_HCF_FILES, TRUE);
           SetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szGlobalFileName);
         //if(szGlobalFileName[0])
         //  LastHGramFileOpened = szGlobalFileName;
           break;
         case IDB_CONFIGFILE_SAVE:{
            //Read filename from IDC_SAVE_CONFIG_FILE_FILE_CHOSEN and save the hcf data to it
#if 1
            MessageBox (hDlg,"- Disabled!","Told you",MB_OK);
#else
            GetDlgItemText(hDlg, IDC_SAVE_CONFIG_FILE_FILE_CHOSEN, szLocalSaveConfigFileName,_MAX_PATH - 1);
            if( lstrlen(szLocalSaveConfigFileName) > 7 ){ //e.g. C:\a.hcf
               int ThisIsWeak; //Unless user does something really silly it works OK.  Chances are, this whole SaveConfigFile_Dialog() routine will be removed from final App.
               if( !mySave_File(hDlg, 1000,"hcf",0, szLocalSaveConfigFileName) ){
                  LastHGramFileOpened = szLocalSaveConfigFileName;
                  StringCchCopy(szGLWndTitle,11,"Holoprog: ");
                  StringCchCopy(szGLWndTitle + 10,_MAX_DIR,szLocalSaveConfigFileName);
                  SetWindowText(hGLWnd,szGLWndTitle);
                 }
               //else file didn't save, but a message will have been displayed to user
               EndDialog(hDlg, LOWORD(wParam));
              }
            else
              MessageBox (hDlg,"You need a valid filename\n(in an existing folder)",NULL,MB_OK);
#endif
           }
           break;
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
      return (INT_PTR)TRUE;//case WM_COMMAND:
   }//switch (message){
 return (INT_PTR)FALSE;
}
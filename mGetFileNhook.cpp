#include "StdAfx.h"
extern HINSTANCE hInst;      // current instance

char szGlobalFileName[_MAX_DIR];            //mGetFile() absolutely has to change szGlobalFileName...  that's how it works!
HCFRECORDsCURRENTfolders   HCFcfs;
char szHGRAMSDirName4Reading[_MAX_DIR];

//File Scope:
static BOOL bCancelPressed;

UINT CALLBACK GOFNHookFunction(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam); //FileScope prototypd

//mGetFile's Primary purpose is to get a filename when loading data.  Different
//filetypes can be catered for by using uiFilter to load a string from the string table.
//For an example see: IDS_FILTERSTR_MOTORPOS_FILES, "Motor Positions(*.CSV)|*.csv|" in the .rc file.
//If bSavingAFile is TRUE then the Window's standard GetSaveFileName() dialogbox is called.
//If bSavingAFile is FALSE then the Window's standard GetOpenFileName() dialogbox is called.
//The GOFNHookFunction() has, by and large been disabled.
//if mGetFile returns TRUE, szGlobalFileName contains a (not necessarily valid) filename
BOOL mGetFile(HWND hWnd, UINT uiFilter, BOOL bSavingAFile){
 typedef UINT (CALLBACK * LPFNHOOK) (HWND,UINT,WPARAM,LPARAM);
 static LPFNHOOK lpfnGFHook = NULL;
 OPENFILENAME ofn;
 char szFileTitle[256], szFilter[SIZFILTER];
 char szHGRAMSDirName4ReadingCpy[_MAX_DIR];
 UINT cbString;
 if ((cbString=LoadString(hInst,uiFilter,szFilter,SIZFILTER))==0){
    MessageBox (hWnd,"Error Loading Filter String",NULL,MB_OK);
    return FALSE;
   }
 for (UINT i = 0; szFilter[i] != '\0'; i++)
   if (szFilter[i] == szFilter[cbString - 1]) //last char is the string separator
     szFilter[i] = '\0';                      //ensures strings are in contiguous memory*/
 memset(&ofn, 0, sizeof(OPENFILENAME));   //Set all structure members to zero.
 szGlobalFileName[0] = '\0';              //Initialize the OPENFILENAME members.
 lpfnGFHook = (LPFNHOOK) MakeProcInstance((FARPROC)GOFNHookFunction, ghInstance);
 ofn.lStructSize = sizeof(OPENFILENAME);
 ofn.hwndOwner   = hWnd;
 ofn.hInstance   = hInst;
 ofn.lpstrFilter = szFilter;
 ofn.lpstrFile   = szGlobalFileName;
 ofn.nMaxFile    = sizeof(szGlobalFileName);
 ofn.lpstrFileTitle  = szFileTitle;
 ofn.nMaxFileTitle   = sizeof(szFileTitle);
 if(uiFilter == IDS_FILTERSTR_MOTORPOS_FILES)
   ofn.lpstrInitialDir = HCFcfs.szCSVDirName4Reading;
 else{
    StringCchCopy(szHGRAMSDirName4ReadingCpy,_MAX_DIR,szHGRAMSDirName4Reading);//This ensures szHGRAMSDirName4Reading is not changed
    ofn.lpstrInitialDir = szHGRAMSDirName4ReadingCpy;
   }
 bCancelPressed = FALSE;
 ofn.Flags      = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK | OFN_NODEREFERENCELINKS | OFN_NONETWORKBUTTON;
 ofn.lpfnHook   = lpfnGFHook;
 static char cTemplat[16];
// if (uiFilter == IDS_FILTERSTRNGTXT)//if we are saving as text we want to display the "CUSTOM_EXPORT"
//   lstrcpy(cTemplat,"CUSTOM_EXPORT");// version of the dialog box so we can check if a header is
// else                                // required (& also which view).
//   lstrcpy(cTemplat,"CUSTOM_OPENSAVE");//This version of the dialog box does not have "Read only" checkbox
 ofn.lpTemplateName = cTemplat;//(LPCSTR)CUSTOM_SAVE);
 if (bSavingAFile){
    ofn.lCustData = 0;
    if (GetSaveFileName(&ofn)) //displays the CUSTOM_SAVE or CUSTOM_EXPORT dialog box
      return TRUE;
    else{
       //DWORD errCode = CommDlgExtendedError();  //CDERR_FINDRESFAILURE
       if (!bCancelPressed)// DWORD erCode1 = CommDlgExtendedError();
         MessageBox (hWnd,"No File Name - Cannot Save",NULL,MB_OK);
       return FALSE;
      }
    return FALSE;
   }
 else{   //User Wants to LOAD a file
    ofn.lCustData = (WPARAM)uiFilter;//NOTE THAT lCustData ONLY HAS A VALUE WHEN WE ARE LOADING A FILE
    if (GetOpenFileName(&ofn)) //displays the Open dialog box
      return TRUE;
    else{
       if (!bCancelPressed)
         MessageBox (hWnd,"No File Name - You will not be able to open it!","Warning",MB_OK);
       return FALSE;
      }
   }
}

UINT CALLBACK GOFNHookFunction(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
 static HWND hFileList, hEditDirPath, hFileName;
 static OPENFILENAME *pOFN;
 static int nDialogBoxType;
 switch (msg){
    case WM_INITDIALOG:{
       hFileName    = GetDlgItem(hDlg,1152);
       hFileList    = GetDlgItem(hDlg,1120);
       hEditDirPath = GetDlgItem(hDlg,1088);
       pOFN = (OPENFILENAME*) lParam;
       if (!lstrcmp(pOFN->lpTemplateName,"CUSTOM_OPENSAVE")){
          if (pOFN->lCustData)
            if (lstrcmp(pOFN->lpstrFilter,"Setup Files(*.STP)")){
               SetWindowText(hDlg, "Open Files");
               nDialogBoxType = 1;
              }
            else{
               SetWindowText(hDlg, "Open A Setup File");
               nDialogBoxType = 2;
              }
          else{
             SetWindowText(hDlg, "Save");
             nDialogBoxType = 3;
            }
         }
       else
         nDialogBoxType = 4;
       if (nDialogBoxType == 1){
          RECT rcDeskT;
          GetWindowRect(GetParent(hDlg), &rcDeskT);
          SetWindowPos(hDlg, NULL,rcDeskT.left+10,rcDeskT.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);//display dialog in top left corner
         }
      }
      return FALSE;
    case WM_COMMAND:
      switch (wParam){
         case IDCANCEL:
           bCancelPressed = TRUE;
           break;
        }
      return FALSE;
   }
 return FALSE;
}
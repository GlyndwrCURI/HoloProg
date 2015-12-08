#include "stdafx.h"
//Contents:  Only myGetFolder() can be called globally

extern HINSTANCE hInst;


extern char szHGRAMSDirName4Reading[];
char   szGlobDefaultFolderShortName[60]; //eg just the "HG1" bit
char   szConcatinatedFolderName[_MAX_PATH];
char   szDirName[_MAX_PATH];
//BOOL   bCancelPressed;



//These function prototypes are declared locally to prevent external linkage
LRESULT CALLBACK GOFNHookFunction4myGetFolder(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD myGetLongPathName(char * p_strShortPath, char* strLongPath  );

//myGetFolder's Primary purpose is to get a folder name to copy the hologram into.
//Addition Browser Functionality is gained by the hook function GOFNHookFunction4myGetFolder().
BOOL myGetFolder(HWND hWnd, UINT uiFilter){//, BOOL bSavingAFile){
 typedef LRESULT (CALLBACK * LPFNHOOK) (HWND,UINT,WPARAM,LPARAM);
 static LPOFNHOOKPROC lpfnGFHook = NULL;
 OPENFILENAME ofn;
 char szFileTitle[256], szFilter[SIZFILTER];
 UINT cbString;
 char   szGlobalFolderName[_MAX_PATH]; //I moved this down from the top, where it had file-scope (it's not referenced elsewhere in the project)

 if ((cbString=LoadString(hInst,uiFilter,szFilter,SIZFILTER))==0){
    MessageBox (hWnd,"Error Loading Filter String",NULL,MB_OK);
    return FALSE;}
 for (UINT i = 0; szFilter[i] != '\0'; i++)
   if (szFilter[i] == szFilter[cbString - 1]) //last char is the string separator
     szFilter[i] = '\0';                      //ensures strings are in contiguous memory*/
 memset(&ofn, 0, sizeof(OPENFILENAME));       //Set all structure members to zero.

 szGlobalFolderName[0] = '\0';                //Initialize the OPENFILENAME members.

 lpfnGFHook = (LPOFNHOOKPROC) MakeProcInstance((FARPROC)GOFNHookFunction4myGetFolder, hInst);
 ofn.lStructSize = sizeof(OPENFILENAME);
 ofn.hwndOwner   = hWnd;
 ofn.hInstance   = hInst;
 ofn.lpstrFilter = szFilter;
 ofn.lpstrFile   = szGlobalFolderName;
 ofn.nMaxFile    = sizeof(szGlobalFolderName); //This returns the selected Folder concatinated with the Filename (which 
 ofn.lpstrFileTitle  = szFileTitle;            // will be the holgram Folder" (e.g. HG1)
 ofn.nMaxFileTitle   = sizeof(szFileTitle);

 ofn.lpstrInitialDir = szDirName;
 //bCancelPressed = FALSE;
 ofn.Flags      = OFN_SHOWHELP | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
 ofn.lpfnHook   = lpfnGFHook;
 static char cTemplat[16];

 StringCchCopy(cTemplat, sizeof(cTemplat), "CUSTOM_EXPORT" );

 ofn.lpTemplateName = cTemplat;//(LPCSTR)CUSTOM_SAVE);
/*if (bSavingAFile){
    ofn.lCustData = 0;
    if (GetSaveFileName(&ofn)) //displays the CUSTOM_SAVE or CUSTOM_EXPORT dialog box
      return TRUE;
    else{
       if (bCancelPressed)// DWORD erCode1 = CommDlgExtendedError();
         //MessageBox (hWnd,"No File Name - Cannot Save",NULL,MB_OK);
       return FALSE;
      }
   }
 else{  */
    ofn.lCustData = (WPARAM)uiFilter;//NOTE THAT lCustData ONLY HAS A VALUE WHEN WE ARE LOADING A FILE
    if (GetOpenFileName(&ofn)) //displays the Open dialog box
       return TRUE;
    else{
//     if (!bCancelPressed)
//       MessageBox (hWnd,"No File Name - Cannot Open",NULL,MB_OK);
       return FALSE;
      }
//   }
}

//The GOFNHookFunction4myGetFolder hooks into messages bound for the GetOpenFileName Dialog box (which is one of the system
// defined standard dialogboxes controlled by commdlg.dll).  The hook modifies the standard behaviour of the
// box by detecting when the highlighted file changes.
LRESULT CALLBACK GOFNHookFunction4myGetFolder(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
 static HWND hFileList, hFileName, hEditDirPath, hFolderName, hEditToCreateName;
 static OPENFILENAME *pOFN;
 char szTempLongPath[_MAX_PATH];
 switch (msg){
    case WM_INITDIALOG:{
       hFileName    = GetDlgItem(hDlg,1152); //1152 would be hFileName in a normal common control
       hFolderName  = GetDlgItem(hDlg,ID_SHORTFOLDERNAME);
       hFileList    = GetDlgItem(hDlg,1120);
       hEditDirPath = GetDlgItem(hDlg,1088);
       hEditToCreateName = GetDlgItem(hDlg,ID_TOCREATENAME);
       SendMessage(hFolderName, WM_SETTEXT, 0,(LPARAM)((LPSTR)szGlobDefaultFolderShortName));
       SendMessage(hFileName, WM_SETTEXT, 0,  (LPARAM)((LPSTR)szGlobDefaultFolderShortName));
       pOFN = (OPENFILENAME*) lParam;
       SetWindowPos(hDlg, NULL,30,119, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return FALSE;
    case WM_COMMAND:
      switch (  LOWORD(wParam) ){
         case ID_SHORTFOLDERNAME: //control normally contains "*.bmp" but we will display the holgram name in it instead
           switch ( HIWORD(wParam) ){
              case EN_UPDATE:
              case EN_CHANGE:
                SendMessage(hFolderName, WM_GETTEXT, 60,(LPARAM)((LPSTR)szGlobDefaultFolderShortName));
                SendMessage(hFileName, WM_SETTEXT, 0,(LPARAM)((LPSTR)szGlobDefaultFolderShortName));
                StringCchCopy(szConcatinatedFolderName, _MAX_PATH, szDirName);
                if(szConcatinatedFolderName[lstrlen(szConcatinatedFolderName)-1] != BACKSLASH)
                  StringCchCat(szConcatinatedFolderName,_MAX_PATH,"\\");
                StringCchCat(szConcatinatedFolderName,_MAX_PATH,szGlobDefaultFolderShortName);
                SendMessage(hEditToCreateName, WM_SETTEXT, 0,(LPARAM)((LPSTR)szConcatinatedFolderName));
                break;
             }
           break;
      /*case 1120: //the files list box  Now deleted  */
         case 1: //The OK button
           if(!lstrcmp(szGlobDefaultFolderShortName,""))
             MessageBox (NULL,"This is the folder that will be created\nto put the hologram in","\"Name of hologram's Folder\" is missing!",MB_OK);
           else{
    //          char szTestFieldContents[100];
//              SendMessage(hFileName, WM_GETTEXT, 60,(LPARAM)((LPSTR)szTestFieldContents));
  //            SendMessage(hFileName, WM_SETTEXT, 0,(LPARAM)((LPSTR)szGlobDefaultFolderShortName));
        //	  if (!lstrcmp(szTestFieldContents,szGlobDefaultFolderShortName))
//				 StringCchCat(szTestFieldContents,100," ");
//				 SendMessage(hFolderName, WM_SETTEXT, 0,(LPARAM)((LPSTR)""));
                 PostMessage(hDlg,WM_USER+7,0,0L);//Wait briefly before displaying message box
                //MessageBox (hDlg,"The Folder in the \"Hologram name\" field already exists in the chosen Parent Folder.\nThe Hologram can be copied only into a new folder that does not already exist.","Folder Already Exists",MB_OK);
             }
           break;
         case 1088://control contains the selected folder name (but puts it in Lower Case!!!)
           switch ( HIWORD(wParam) ){
              case EN_UPDATE:
              case EN_CHANGE:
                SendMessage(hEditDirPath, WM_GETTEXT, _MAX_PATH,(LPARAM)((LPSTR)szDirName));     
                if( strstr( szDirName, "..." ) == NULL ){        //This Check is almost redundent now that I've made the 1088 
                   myGetLongPathName(szDirName, szTempLongPath); // field very long in the template.  (The three dots will)
                   StringCchCopy(szDirName, _MAX_PATH, szTempLongPath);// be found only on VERY long paths, so myGetLongPathName() will nearly 
                  }// always be called.  (The check is necessary because myGetLongPathName cannot handle the 3 dots)
                StringCchCopy(szConcatinatedFolderName, _MAX_PATH, szDirName);
                if(szConcatinatedFolderName[lstrlen(szConcatinatedFolderName)-1] != BACKSLASH)
                  StringCchCat(szConcatinatedFolderName,_MAX_PATH, "\\");
                StringCchCat(szConcatinatedFolderName,_MAX_PATH, szGlobDefaultFolderShortName);
                SendMessage(hEditToCreateName, WM_SETTEXT, 0,(LPARAM)((LPSTR)szConcatinatedFolderName));
                break;
             }
           break;
         case IDCANCEL:
//         bCancelPressed = TRUE;
           break;
        }
      return FALSE;
    case WM_USER+7:
      MessageBox (hDlg,"The \"Hologram name\" entered already exists in the Parent Folder.\nHolograms can be created only in folders that do not already exist, so\nyou must choose a new name.\n\nAlso, REMEMBER to select the Parent Folder again - IT WAS JUST CHANGED!","Folder Already Exists",MB_OK);
      //SendMessage(hEditDirPath, WM_SETTEXT, 0,(LPARAM)((LPSTR)"C:\\MyProjects\\HoloProg\\Debug\\HGrams"));
      //SendMessage(hFolderName, WM_SETTEXT, 0,(LPARAM)((LPSTR)"C:\\MyProjects\\HoloProg\\Debug\\HGrams"));
    //SendMessage(hFileName, WM_SETTEXT, 0,  (LPARAM)((LPSTR)"C:\\MyProjects\\HoloProg\\Debug\\HGrams")); //This hard coded name causes problems if the application is moved
      SendMessage(hFileName, WM_SETTEXT, 0,  (LPARAM)((LPSTR)szHGRAMSDirName4Reading));
      break;
   }
 return FALSE;
}
//This Function doesn't change what is in the shortpath arguement, only what is at *strLongPath
//p_strShortPath is expected to point to a path containing at least <drive letter> + colon
//
//This function goes a long way to undoing a serious fault / limitation (or even bug!) in Microsofts' Standard 
// Common dialog box for Saving and getting filenames: Namely that when changing the highlighted folder, the
// proper capitalisation (or case) is not available to the dialog box's hook function (eg "c:\Program Files" is
// reported as "c:\program files".  What a load of *!@~.
//
//This function is a lovely example of recursion, if I say so myself! :-) mj.

DWORD myGetLongPathName(char * p_strShortPath, char* strLongPath  ){
  char strShortPath[_MAX_DIR];
  //static char Bslash3='\\';
  //lstrcpy(strShortPath, p_strShortPath);
  StringCchCopy(strShortPath, _MAX_DIR, p_strShortPath);
  char *ptrBslash;
  ptrBslash = strrchr(strShortPath, BACKSLASH);  //reverse search & remove the final backslash
  if ((ptrBslash != NULL)&&((ptrBslash-strShortPath)>1)){  //if (iFound > 1) // recurse to peel off components
     strShortPath[(int)(ptrBslash-strShortPath)]=0;
     int lenRetnd = myGetLongPathName(strShortPath, strLongPath); //  <<-- Recursion is here ************
     strShortPath[(int)(ptrBslash-strShortPath)]=BACKSLASH;   //Put final backslash back in again
     if (lenRetnd > 0){
      //lstrcat(strLongPath, "\\");
        StringCchCat(strLongPath,_MAX_PATH, "\\");
        if (strShortPath[lstrlen(strShortPath)-1] != BACKSLASH){
           WIN32_FIND_DATA findData;
           // append the long component name to the path
           HANDLE hFFF = FindFirstFile  (strShortPath, &findData);
           if (INVALID_HANDLE_VALUE != hFFF)
           //lstrcat(strLongPath, findData.cFileName);
             StringCchCat(strLongPath,_MAX_PATH, findData.cFileName);
           else { // if FindFirstFile fails, return the error code
              FindClose(hFFF); 
              strLongPath[0]=0;
              return 0;
             }
           FindClose(hFFF);
          }
       }
     else
       MessageBox(NULL,"myGetLongPathName returned zero!",NULL,MB_OK);
    }
  else
  //lstrcpy(strLongPath, strShortPath);
    StringCchCopy(strLongPath, _MAX_PATH, strShortPath);
  return lstrlen(strShortPath);
}
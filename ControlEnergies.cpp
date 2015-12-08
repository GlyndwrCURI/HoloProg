#include "stdafx.h"

//This function prototypes are local to prevent external linkage:
BOOL bCheckSingleDecimalFieldIsValid_WP3(HWND hDlg, int *nGood_rNumberTimes100, int nControlID);

//extern CRegStdString LastHGramFileOpened; //????????????????????????????

extern char mes[];
extern HCFRECORDsCURRENTfolders HCFcfs;
extern HCFRECORD HCFrec;

extern double dblArDesiredEnergyWP2r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP2b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3r[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3g[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];
extern double dblArDesiredEnergyWP3b[MAX_NOOFLINES][MAXNOOFPIXELS_IN_ROW];


INT_PTR CALLBACK Control_Energies_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 int ViewHologInfoWP2RObjEnergyRowChosen; //This is not really a row - it's an int representing the floating point typed in by the user (times 100, for WP2 and 1000 for WP3)
 int ViewHologInfoWP2GObjEnergyRowChosen; // (In the original code, the float was collected from a drop down list).
 int ViewHologInfoWP2BObjEnergyRowChosen;
 int ViewHologInfoWP3RObjEnergyRowChosen;
 int ViewHologInfoWP3GObjEnergyRowChosen;
 int ViewHologInfoWP3BObjEnergyRowChosen;
 switch (message){
    case WM_INITDIALOG:{
       char sz_dbMes[14];
       if(!lstrcmp(NO_HOLOGRAM_NAME,HCFrec.szHologramName)){
          MessageBox (hDlg,"No Hologram is Opened","Cannot Control Energies...",MB_OK);
          EndDialog(hDlg, LOWORD(wParam));
         }
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2rObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_R_OBJ_ENERGY, sz_dbMes);
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2gObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_G_OBJ_ENERGY, sz_dbMes);
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2bObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_B_OBJ_ENERGY, sz_dbMes);
       //
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2rObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_R_OBJ_ENERGY, sz_dbMes);
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2gObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_G_OBJ_ENERGY, sz_dbMes);
       //sprintf_s(sz_dbMes,13,"%9.2f",(double)HCFrec.WP2bObjEnergyTimes1000/1000);
       //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_B_OBJ_ENERGY, sz_dbMes);

       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP2r[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_R_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP2g[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_G_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP2b[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP2_B_OBJ_ENERGY, sz_dbMes);
       
       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP3r[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_R_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP3g[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_G_OBJ_ENERGY, sz_dbMes);
       sprintf_s(sz_dbMes,13,"%9.2f",dblArDesiredEnergyWP3b[0][0]);
       SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_WP3_B_OBJ_ENERGY, sz_dbMes);

       RECT ParentRc;           //Display dialog, usefully positioned 
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, ParentRc.left + 279, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_CONTROL_ENERGIES_EXIT_LEAVING_PIDS:
           EndDialog(hDlg, LOWORD(wParam));
           break;
         case IDB_CONTROL_ENERGIES_EXIT_RELOAD:{
          //MessageBox(hDlg, "Implement Reload","To Do", MB_OK);
            char szLocalM2or3ProfileFileName[_MAX_DIR];
            int readFromExcelM2or3Error = 0;
            StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
            StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3r.csv"
            if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                          bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 0, HCFrec.nNoofRowsBetweenBeams))
              readFromExcelM2or3Error = 3;
            else{
               StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
               StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3g.csv"
               if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                             bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 1, HCFrec.nNoofRowsBetweenBeams))
                 readFromExcelM2or3Error = 15;
               else{
                  StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                  StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM3b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                  if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 2, HCFrec.nNoofRowsBetweenBeams))
                    readFromExcelM2or3Error = 27;
                  else{
                     StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                     StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2r.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2r.csv"
                     if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2r.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols, 
                                                   bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 3, HCFrec.nNoofRowsBetweenBeams))
                       readFromExcelM2or3Error = 2;
                     else{
                        StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                        StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2g.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM2g.csv"
                        if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM2g.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                      bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 4, HCFrec.nNoofRowsBetweenBeams))
                          readFromExcelM2or3Error = 14;
                        else{
                           StringCchCopy(szLocalM2or3ProfileFileName,_MAX_DIR, HCFcfs.szCSVDirName4Reading);
                           StringCchCat(szLocalM2or3ProfileFileName,_MAX_DIR,"FromExcelM2b.csv");//e.g. "<HologramDIR>\\CSVs\\FromExcelM3b.csv"
                           if(myOpen_File_VariableAp_CSV(szLocalM2or3ProfileFileName/*"C:\\MyProjects\\HoloProg\\Debug\\HGrams\\4editing1n\\CSVs\\FromExcelM3b.csv"*/, HCFrec.uiNoofLines, HCFrec.uiNoofCols,
                                                         bCOPY2ENERGYARRAY, HCFrec.nLeadInForM39, 5, HCFrec.nNoofRowsBetweenBeams))
                             readFromExcelM2or3Error = 26;
                          }
                       }
                    }
                 }
              }
            if(readFromExcelM2or3Error){
               StringCchCopy(mes, SIZMES, "Motor: ");
               _itoa_s(readFromExcelM2or3Error, mes+7,_MAX_DIR - 7,10);
               MessageBox (hDlg,"There was a problem reading (at least) one Energy File.",mes,MB_OK);
              }
            else{ //T ransfer_ALL_FilesAndPerform succeeded() & b CheckAlltheChecksumsAreOK() did too, so Exit the dialog box.
               EndDialog(hDlg, LOWORD(wParam));
              }
           }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_ALL:{
            if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2RObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_R_OBJ_ENERGY) )
              ;
            else{
               //ViewHologInfoWP2GObjEnergyRowChosen = SendMessage(hWP2G_ObjEnergy, CB_GETCURSEL,0,0);
               //if(ViewHologInfoWP2GObjEnergyRowChosen == CB_ERR)
               //  MessageBox(hDlg,"Please select a value","Error reading WP2 GREEN Obj Energy",MB_OK);
               //else{
               if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2GObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_G_OBJ_ENERGY) )
                 ;
               else{
                  //ViewHologInfoWP2BObjEnergyRowChosen = SendMessage(hWP2B_ObjEnergy, CB_GETCURSEL,0,0);
                  //if(ViewHologInfoWP2BObjEnergyRowChosen == CB_ERR)
                  //  MessageBox(hDlg,"Please select a value","Error reading WP2 BLUE Obj Energy",MB_OK);
                  //else{
                  if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2BObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_B_OBJ_ENERGY) )
                    ;
                  else{
                     if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3RObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_R_OBJ_ENERGY) )
                       ;
                     else{
                        if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3GObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_G_OBJ_ENERGY) )
                          ;
                        else{
                           if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3BObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_B_OBJ_ENERGY) )
                             ;
                           else{
                              dblArDesiredEnergyWP2r[0][0] = (double)ViewHologInfoWP2RObjEnergyRowChosen / 100; //Note the WP3 version of bCheckSingleDecimalFieldIsValid() returns the exact number
                              dblArDesiredEnergyWP2g[0][0] = (double)ViewHologInfoWP2GObjEnergyRowChosen / 100; // but the [WP2] version multiplys the number by 100
                              dblArDesiredEnergyWP2b[0][0] = (double)ViewHologInfoWP2BObjEnergyRowChosen / 100;

                              dblArDesiredEnergyWP3r[0][0] = (double)ViewHologInfoWP3RObjEnergyRowChosen / 1000;
                              dblArDesiredEnergyWP3g[0][0] = (double)ViewHologInfoWP3GObjEnergyRowChosen / 1000;
                              dblArDesiredEnergyWP3b[0][0] = (double)ViewHologInfoWP3BObjEnergyRowChosen / 1000;
                             }
                          }
                       }
                    }
                 }
              }
           }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP2_R:
           if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2RObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_R_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP2r[0][0] = (double)ViewHologInfoWP2RObjEnergyRowChosen / 100; //Note the WP3 version of bCheckSingleDecimalFieldIsValid() returns the exact number
             }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP2_G:
           if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2GObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_G_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP2g[0][0] = (double)ViewHologInfoWP2GObjEnergyRowChosen / 100; // but the [WP2] version multiplys the number by 100
             }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP2_B:
           if( bCheckSingleDecimalFieldIsValid(hDlg, &ViewHologInfoWP2BObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP2_B_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP2b[0][0] = (double)ViewHologInfoWP2BObjEnergyRowChosen / 100;
             }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP3_R:
           if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3RObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_R_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP3r[0][0] = (double)ViewHologInfoWP3RObjEnergyRowChosen / 1000;
             }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP3_G:
           if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3GObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_G_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP3g[0][0] = (double)ViewHologInfoWP3GObjEnergyRowChosen / 1000;
             }
           break;
         case IDB_CONTROL_ENERGIES_APPLY_WP3_B:
           if( bCheckSingleDecimalFieldIsValid_WP3(hDlg, &ViewHologInfoWP3BObjEnergyRowChosen, IDC_CONTROL_ENERGIES_WP3_B_OBJ_ENERGY) )
             ;
           else{
              dblArDesiredEnergyWP3b[0][0] = (double)ViewHologInfoWP3BObjEnergyRowChosen / 1000;
             }
           break;
         case IDCANCEL:
         //MessageBox(hDlg, "Please use one of the main 'Exit' buttons to leave this Dialogbox","Cancel is disabled...", MB_OK);
         //SetDlgItemText(hDlg, IDC_CONTROL_ENERGIES_MESSAGE, "Please use one of the main 'Exit' buttons to close Dialogbox");
           //HCFrec = VHICopyOfOrigHCFrec;
           //SetPointWP2red = (double)HCFrec.WP2rObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           //SetPointWP2gre = (double)HCFrec.WP2gObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           //SetPointWP2blu = (double)HCFrec.WP2bObjEnergyTimes1000/1000; //SetPoints are no longer looked at
           //EndDialog(hDlg, LOWORD(wParam));
           MakeWarningSound();
           ShowWindow( GetDlgItem(hDlg,IDC_CONTROL_ENERGIES_MESSAGE),SW_HIDE);
           Sleep(200);
           ShowWindow( GetDlgItem(hDlg,IDC_CONTROL_ENERGIES_MESSAGE),SW_SHOW);
           break;
        }//end switch
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}

BOOL bCheckSingleDecimalFieldIsValid(HWND hDlg, int *nGood_rNumberTimes100, int nControlID){ //returns TRUE if there's a problem
 char szDecNoToRead[15];
 int len;
 double dblNum;
 GetDlgItemText(hDlg, nControlID, szDecNoToRead, sizeof(szDecNoToRead) - 1);
 len = lstrlen(szDecNoToRead);
 StringCchCopy(mes,SIZMES,"Values in the range 0.0 to 4095.0 are allowed");
 if(!len){
    MessageBox(hDlg, mes, "Unable to read a WP2 value", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
 dblNum = atof(szDecNoToRead) * 100;
 if(dblNum > 409500.01){
    MessageBox(hDlg, mes, "A WP2 (Obj Energy) value was too big", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
 if(dblNum < -0.00000001){
    MessageBox(hDlg, mes, "WP2 (Obj Energy) values must not be negative", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
*nGood_rNumberTimes100 = (int)(dblNum + 0.00000001);
 ShowWindow( GetDlgItem(hDlg,IDC_CONTROL_ENERGIES_MESSAGE),SW_HIDE);
 return 0;
}

BOOL bCheckSingleDecimalFieldIsValid_WP3(HWND hDlg, int *nGood_rNumberTimes1000, int nControlID){ //returns TRUE if there's a problem
 char szDecNoToRead[15];
 int len;
 double dblNum;
 GetDlgItemText(hDlg, nControlID, szDecNoToRead, sizeof(szDecNoToRead) - 1);
 len = lstrlen(szDecNoToRead);
 StringCchCopy(mes,SIZMES,"Values in the range 0.0 to 4095.0 are allowed");
 if(!len){
    MessageBox(hDlg, mes, "Unable to read a WP3 value", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
 dblNum = atof(szDecNoToRead) * 1000;
 if(dblNum > 4095000.01){
    MessageBox(hDlg, mes, "A WP3 (Ref Energy) value was too big", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
 if(dblNum < -0.00000001){
    MessageBox(hDlg, mes, "WP3 (Ref Energy) values must not be negative", MB_OK); //"Values in the range 0.0 to 4095.0 are allowed"
    return 1;
   }
*nGood_rNumberTimes1000 = (int)(dblNum + 0.00000001);
 return 0;
}
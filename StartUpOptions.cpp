#include "stdafx.h"

extern CRegStdWORD   bregMakeSounds;
extern BOOL bMakeSounds;
extern CRegStdWORD   nEnableControlBoxSW;
extern CRegStdWORD   bStartWithPIDsDisabled;
extern CRegStdWORD   bStartWithViewHologramInfoOpened;
extern CRegStdWORD   bDoProperCheckSumChecks_MposFiles;
extern CRegStdWORD   bDoProperCheckSumChecks_AccelProfs;
extern CRegStdWORD   bUseCSBs4AccProfs;

static BOOL bInitMakeSounds;
static BOOL bEnableControlBoxSW;
static BOOL bInitStartWithPIDsDisabled;
static BOOL bInitStartWithViewHologramInfoOpened;
static BOOL bInitDoProperCheckSumChecks_MposFiles;
static BOOL bInitDoProperCheckSumChecks_AccelProfs;
static BOOL bInitUseCSBs4AccProfs;

INT_PTR CALLBACK StartUpOptions_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 switch (message){
    case WM_INITDIALOG:{
       bInitMakeSounds = bMakeSounds;
       bEnableControlBoxSW = nEnableControlBoxSW;
       bInitStartWithPIDsDisabled = bStartWithPIDsDisabled;
       bInitStartWithViewHologramInfoOpened = bStartWithViewHologramInfoOpened;
       bInitDoProperCheckSumChecks_MposFiles = bDoProperCheckSumChecks_MposFiles;
       bInitDoProperCheckSumChecks_AccelProfs = bDoProperCheckSumChecks_AccelProfs;
       bInitUseCSBs4AccProfs = bUseCSBs4AccProfs;

       if( bStartWithPIDsDisabled )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PIDS_DISABLED, IDD_REGISTRYOPTIONS_PIDS_DISABLED, IDD_REGISTRYOPTIONS_PIDS_DISABLED);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PIDS_DISABLED,IDD_REGISTRYOPTIONS_PIDS_DISABLED,FALSE);

       if( bStartWithViewHologramInfoOpened )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_VHI_OPEN, IDD_REGISTRYOPTIONS_VHI_OPEN, IDD_REGISTRYOPTIONS_VHI_OPEN);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_VHI_OPEN,IDD_REGISTRYOPTIONS_VHI_OPEN,FALSE);

       if( bMakeSounds )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_MAKE_SOUNDS, IDD_REGISTRYOPTIONS_MAKE_SOUNDS, IDD_REGISTRYOPTIONS_MAKE_SOUNDS);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_MAKE_SOUNDS,IDD_REGISTRYOPTIONS_MAKE_SOUNDS,FALSE);

       if( nEnableControlBoxSW )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,FALSE);

       if( bDoProperCheckSumChecks_MposFiles )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,FALSE);

       if( bDoProperCheckSumChecks_AccelProfs )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,FALSE);

       if( bUseCSBs4AccProfs )
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS);
       else
         CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,FALSE);

       RECT ParentRc;           //Display dialog positioned to cover the bitmap on the menu bar
       GetWindowRect(GetParent(hDlg), &ParentRc);
       SetWindowPos(hDlg, NULL, 3, ParentRc.top + 80, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDD_REGISTRYOPTIONS_PIDS_DISABLED:
           if( bStartWithPIDsDisabled ){
              bStartWithPIDsDisabled = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PIDS_DISABLED,IDD_REGISTRYOPTIONS_PIDS_DISABLED,FALSE);
             }
           else{
              bStartWithPIDsDisabled = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PIDS_DISABLED,IDD_REGISTRYOPTIONS_PIDS_DISABLED,IDD_REGISTRYOPTIONS_PIDS_DISABLED);
             }
           break;
         case IDD_REGISTRYOPTIONS_VHI_OPEN:
           if( bStartWithViewHologramInfoOpened ){
              bStartWithViewHologramInfoOpened = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_VHI_OPEN,IDD_REGISTRYOPTIONS_VHI_OPEN,FALSE);
             }
           else{
              bStartWithViewHologramInfoOpened = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_VHI_OPEN,IDD_REGISTRYOPTIONS_VHI_OPEN,IDD_REGISTRYOPTIONS_VHI_OPEN);
             }
           break;
         case IDD_REGISTRYOPTIONS_MAKE_SOUNDS:
           if( bMakeSounds ){
              bMakeSounds = 0;
              bregMakeSounds = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_MAKE_SOUNDS,IDD_REGISTRYOPTIONS_MAKE_SOUNDS,FALSE);
             }
           else{
              bMakeSounds = 1;
              bregMakeSounds = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_MAKE_SOUNDS,IDD_REGISTRYOPTIONS_MAKE_SOUNDS,IDD_REGISTRYOPTIONS_MAKE_SOUNDS);
             }
           break;
         case IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX:
           if( nEnableControlBoxSW ){
              nEnableControlBoxSW = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,FALSE);
             }
           else{
              nEnableControlBoxSW = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX,IDD_REGISTRYOPTIONS_ENABLE_CONTROLBOX);
             }
           break;
         case IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES:
           if( bDoProperCheckSumChecks_MposFiles ){
              bDoProperCheckSumChecks_MposFiles = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,FALSE);
             }
           else{
              bDoProperCheckSumChecks_MposFiles = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES,IDD_REGISTRYOPTIONS_PROPERCHCKSUMS_MPOSFILES);
             }
           break;
         case IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES:
           if( bDoProperCheckSumChecks_AccelProfs ){
              bDoProperCheckSumChecks_AccelProfs = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,FALSE);
             }
           else{
              bDoProperCheckSumChecks_AccelProfs = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES,IDD_REGISTRYOPTIONS_PROPERCHECKSUMS_ACCPROFFILES);
             }
           break;
         case IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS:
           if( bUseCSBs4AccProfs ){
              bUseCSBs4AccProfs = 0;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,FALSE);
             }
           else{
              bUseCSBs4AccProfs = 1;
              CheckRadioButton(hDlg, IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS,IDD_REGISTRYOPTIONS_USE_CSBS4ACCPROFS);
             }
           break;
         case IDCANCEL:
           bMakeSounds = bInitMakeSounds;
           bregMakeSounds = bInitMakeSounds;
           nEnableControlBoxSW = bEnableControlBoxSW;
           bStartWithPIDsDisabled = bInitStartWithPIDsDisabled;
           bStartWithViewHologramInfoOpened = bInitStartWithViewHologramInfoOpened;
           bDoProperCheckSumChecks_MposFiles = bInitDoProperCheckSumChecks_MposFiles;
           bDoProperCheckSumChecks_AccelProfs = bInitDoProperCheckSumChecks_AccelProfs;
           bUseCSBs4AccProfs = bInitUseCSBs4AccProfs;
           EndDialog(hDlg, LOWORD(wParam));
           break;
         case IDOK:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }
    return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}
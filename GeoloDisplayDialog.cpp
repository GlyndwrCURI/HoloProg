#include "stdafx.h"

extern HWND hGLWnd;
extern char mes[];

CTestNetClientDlg dlg;

void UpdateTabbedListBox(HWND hDisplayPcLB){
 char szDum[SIZSZDUM12];
 SendMessage(hDisplayPcLB, LB_RESETCONTENT,0,(LPARAM)mes );
 for(int nNoofRow = 0; nNoofRow < dlg.m_iTotalJobs; nNoofRow++){
    StringCchCopy(mes,SIZMES,dlg.m_JobList[nNoofRow].sJobName );
    StringCchCat(mes,SIZMES,"\t");
    StringCchCat(mes,SIZMES,dlg.m_JobList[nNoofRow].sUIN );
    StringCchCat(mes,SIZMES,"\t");
    _itoa_s(dlg.m_JobList[nNoofRow].iHoloWidth,szDum,SIZSZDUM12,10);
    StringCchCat(mes,SIZMES,szDum);
    StringCchCat(mes,SIZMES,"\t");
    _itoa_s(dlg.m_JobList[nNoofRow].iHoloHeight,szDum,SIZSZDUM12,10);
    StringCchCat(mes,SIZMES,szDum);
    StringCchCat(mes,SIZMES,"\t");
    _itoa_s(dlg.m_JobList[nNoofRow].iPixelWidth,szDum,SIZSZDUM12,10);
    StringCchCat(mes,SIZMES,szDum);
    StringCchCat(mes,SIZMES,"\t");
    _itoa_s(dlg.m_JobList[nNoofRow].iPixelHeight,szDum,SIZSZDUM12,10);
    StringCchCat(mes,SIZMES,szDum);
    SendMessage(hDisplayPcLB, LB_ADDSTRING,0,(LPARAM)mes );
   }
}

INT_PTR CALLBACK JobsOnDisplayPC_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
#define SIZTAB_AR 5
 static int TabAr[SIZTAB_AR] = {300,525,595,665,715};
 static int nRowCount;
 static HWND hDisplayPcLB, hCout;
 switch (message){
    case WM_INITDIALOG:{
       nRowCount = 0;
       hDisplayPcLB = GetDlgItem (hDlg, IDC_GEOLALISTBOX);
       hCout = GetDlgItem (hDlg, IDC_GEOLA_LBMESSAGES);
       if(!SendMessage(hDisplayPcLB,LB_SETTABSTOPS,(WPARAM)SIZTAB_AR,(LPARAM)(LPINT)&TabAr) )
         MessageBox(hDlg,"Couldn't set TabStops",NULL, MB_OK|MB_ICONEXCLAMATION);
       UpdateTabbedListBox(hDisplayPcLB);
      }
      return (INT_PTR)TRUE;
//                                     Number of Packets returned by display PC
//CLIENT_JOBS_LIST_REQUEST             1 + 0 or more
//CLIENT_STATUS_REQUEST                1
//CLIENT_SET_REPETITION_RATE           1?
//CLIENT_GET_REPETITION_RATE           1
//CLIENT_START_PRINTING                ?
//CLIENT_STOP_PRINTING                 ?
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_GEOLASENDMESSAGEjobsListReq:
         case IDB_GEOLASENDMESSAGEstatReq:
         case IDB_GEOLASENDMESSAGEsetRepRate:
         case IDB_GEOLASENDMESSAGEgetRepRate:
         case IDB_GEOLASENDMESSAGEstartPrinting:
         case IDB_GEOLASENDMESSAGEstopPrinting:{
             int nButtonPressedZB = LOWORD(wParam) - IDB_GEOLASENDMESSAGEjobsListReq;
             int nRetCod;
             char szDum[SIZSZDUM12];
             SetDlgItemText(hDlg, IDC_GEOLASTATUS, "blank" );
             nRetCod = SendMessToDisplayPC(hDlg,hCout,&nRowCount, nButtonPressedZB);
             if (nRetCod){
                winLog.flush();
                _itoa_s(nRetCod,szDum,SIZSZDUM12,10);
                if( IDYES == MessageBox(hDlg,"SendMessToDisplayPC() returned the above error code!\nDo you want to view syslog.txt now to see if there is more info?\n\n(You may need to Scroll to bottom to see the error)"
                                            ,szDum, MB_YESNO) )
                  DisplayFileInEditor(hDlg, "syslog.txt");
               }
             else{
                SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"SendMessToDisplayPC() returned Success!");
                nRowCount++;
               }
             SendMessage( hCout, LB_SETTOPINDEX, nRowCount - 5,0);
             UpdateTabbedListBox(hDisplayPcLB);
             SetDlgItemText(hDlg, IDC_GEOLASTATUS, dlg.sReturnStatus() );
            }
           break;
         case IDOK:
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
      return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}

void CTestNetClientDlg::MessageCallbackNotify(string sMessage, int iMsgSize) {
 CString sTemp;
 char chTemp = ' ';
 int iCnt = 0;
 // Parsing COMMAND
 while((chTemp = sMessage[iCnt]) != COMMANDS_DELIMITER){
    sTemp += sMessage[iCnt];
    iCnt++;
    if(iCnt >= iMsgSize)
      break;
   }
 if(sTemp == SERVER_REPETITION_RATE_REPLY){
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    while((chTemp = sMessage[iCnt]) != PACKET_END_CHARACTER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         break;
      }
    int iRepRate = atoi(sTemp);
    CString sStatus;
    sStatus.Format("Repetition rate is set to: %d", iRepRate);
    return;
   }
 if(sTemp == SERVER_STATUS_REPLY){ 
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    while((chTemp = sMessage[iCnt]) != PACKET_END_CHARACTER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         break;
      }
    CTime time = CTime::GetCurrentTime();
    //m_sStatus.Format("%s Status: %s", time.Format("[%H:%M:%S]"), sTemp);
    CString sStatus;
    sStatus.Format("%s Status: %s", time.Format("[%H:%M:%S]"), sTemp);
    dlg.m_sStatus = sStatus;
    //pThis->m_Status.SetWindowText(sStatus);
    return;
   }
 if(sTemp == SERVER_JOBS_LIST_REPLY){
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;     
    while((chTemp = sMessage[iCnt]) != PACKET_END_CHARACTER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         break;
      }
    dlg.m_iTotalJobs = atoi(sTemp);
    //pThis->m_List.ResetContent();
    //pThis->m_DataField.SetWindowText("");
    dlg.m_iJobsCounter = 0;
    for(int i=0; i<100; i++){ 
       dlg.m_JobList[i].iHoloHeight = 0;
       dlg.m_JobList[i].iHoloWidth = 0;
       dlg.m_JobList[i].iPixelHeight = 0;
       dlg.m_JobList[i].iPixelWidth = 0;
       dlg.m_JobList[i].sJobName = "";
       dlg.m_JobList[i].sUIN = "";
      }
    return;
   }
 if(sTemp == SERVER_JOB_DATA_COMMAND){
    CString sJobName;
    CString sUIN;
    int iHoloWidth = 0;
    int iHoloHeight = 0;
    int iPixelWidth = 0;
    int iPixelHeight = 0;

    sJobName.Empty();
    sUIN.Empty();
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    // Parsing Job Name
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sJobName += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         return;
       if(chTemp == PACKET_END_CHARACTER)
         return;
      }
    chTemp = ' ';
    iCnt++;
    // Parsing Job UIN
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sUIN += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         return;
       if(chTemp == PACKET_END_CHARACTER)
         return;
      }
    chTemp = ' ';
    iCnt++;
    // Parsing Hologram Width
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         return;
       if(chTemp == PACKET_END_CHARACTER)
         return;
      }
    iHoloWidth = atoi(sTemp);
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    // Parsing Hologram Height
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         return;
       if(chTemp == PACKET_END_CHARACTER)
         return;
      }
    iHoloHeight = atoi(sTemp);
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    // Parsing Holopixel Width
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         return;
       if(chTemp == PACKET_END_CHARACTER)
         return;
      }
    iPixelWidth = atoi(sTemp);
    sTemp.Empty();
    chTemp = ' ';
    iCnt++;
    // Parsing Holopixel Height
    while((chTemp = sMessage[iCnt]) != PARAMETERS_DELIMITER){
       sTemp += sMessage[iCnt];
       iCnt++;
       if(iCnt >= iMsgSize)
         break;
       if(chTemp == PACKET_END_CHARACTER)
         break;
      }
    iPixelHeight = atoi(sTemp);
    //CString s;
    //s.Format("%d. %s", dlg.m_iJobsCounter + 1, sJobName);
    //pThis->m_List.AddString(s);
    dlg.m_JobList[dlg.m_iJobsCounter].iHoloHeight = iHoloHeight;
    dlg.m_JobList[dlg.m_iJobsCounter].iHoloWidth = iHoloWidth;
    dlg.m_JobList[dlg.m_iJobsCounter].iPixelHeight = iPixelHeight;
    dlg.m_JobList[dlg.m_iJobsCounter].iPixelWidth = iPixelWidth;
    dlg.m_JobList[dlg.m_iJobsCounter].sJobName = sJobName;
    dlg.m_JobList[dlg.m_iJobsCounter].sUIN = sUIN;
    dlg.m_iJobsCounter++;
    return;
   }
}

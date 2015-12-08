#include "stdafx.h"

extern HCFRECORD HCFrec;

//extern int nNoofUsteps4EachApertureTab[9];
//extern int nNoofUsteps4EachFocusTab[3];
//extern int nNoofUsteps4RefBeamTablePrintUnload[4];
extern PCFRECORD PCFrec;

extern int nLineXstartPositionsFEM[NOOFMOTORS][MAX_NOOFLINES]; //Use a pre-generated file to store all the file positions (could be calculated dynamically)

//File scope prototype
void UpdateAllFields(HWND hDlg, WPARAM wParam, unsigned int uiLine2ShowStartPositionsForZB);
//File scope variables shared with UpdateAllFields
static int interpolatedWP1rRatioUsteps;
static int interpolatedWP1gRatioUsteps;
static int interpolatedWP1bRatioUsteps;

INT_PTR CALLBACK ViewMotorStartPointsInHCF_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 static unsigned int uiLine2ShowStartPositionsForZB; //Zero Based
 switch (message){
    case WM_INITDIALOG:{
       //Load the Line Number Drop Down Combo Box (This must be done only once!)
       #define    SIZSZDUM12vmsp  12
       char szDumVMSP[SIZSZDUM12vmsp];
       unsigned int nTrueNoofRowsIncBetweenTheBeams = (unsigned int)(HCFrec.uiNoofLines + 2 * HCFrec.nNoofRowsBetweenBeams);
       for(uiLine2ShowStartPositionsForZB = 1; uiLine2ShowStartPositionsForZB <= nTrueNoofRowsIncBetweenTheBeams; uiLine2ShowStartPositionsForZB++){//Non Zero - based reuse of uiLine2ShowStartPositionsForZB!
          _itoa_s(uiLine2ShowStartPositionsForZB,szDumVMSP,SIZSZDUM12vmsp,10);
          SendDlgItemMessage(hDlg,IDC_MOTORSTARTPOS_LINE_NO,CB_ADDSTRING,0,(LPARAM)szDumVMSP);
         }
       uiLine2ShowStartPositionsForZB = HCFrec.nNextLineToPrint;
       SendDlgItemMessage (hDlg, IDC_MOTORSTARTPOS_LINE_NO, CB_SETCURSEL, uiLine2ShowStartPositionsForZB, 0L);

       interpolatedWP1rRatioUsteps = GetUstepsForThisRatio(HCFrec.WP1rRatioTimes1000, 1);
       interpolatedWP1gRatioUsteps = GetUstepsForThisRatio(HCFrec.WP1gRatioTimes1000, 2);
       interpolatedWP1bRatioUsteps = GetUstepsForThisRatio(HCFrec.WP1bRatioTimes1000, 3);

       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 1, "n/a");
       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 2, "n/a");
       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 13, "n/a");
       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 14, "n/a");
       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 25, "n/a");
       SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + 26, "n/a");
       //Display the Constant Energy for the 3 WP2 Pids
       _gcvt_s(szDumVMSP, SIZSZDUM12vmsp, (double)HCFrec.WP2rObjEnergyTimes1000/1000,3);
       if( szDumVMSP[lstrlen(szDumVMSP) - 1] == '.' )
         StringCchCat(szDumVMSP,12 , "0 u J");
       else
         StringCchCat(szDumVMSP,12 , " u J");
       SetDlgItemText(hDlg, HCF_REC_MOTOR02_SPOS_uSTEPSExtra, szDumVMSP );
       _gcvt_s(szDumVMSP, SIZSZDUM12vmsp, (double)HCFrec.WP2gObjEnergyTimes1000/1000,3);
       if( szDumVMSP[lstrlen(szDumVMSP) - 1] == '.' )
         StringCchCat(szDumVMSP,12 , "0 u J");
       else
         StringCchCat(szDumVMSP,12 , " u J");
       SetDlgItemText(hDlg, HCF_REC_MOTOR14_SPOS_uSTEPSExtra, szDumVMSP );
       _gcvt_s(szDumVMSP, SIZSZDUM12vmsp, (double)HCFrec.WP2bObjEnergyTimes1000/1000,3);
       if( szDumVMSP[lstrlen(szDumVMSP) - 1] == '.' )
         StringCchCat(szDumVMSP,12 , "0 u J");
       else
         StringCchCat(szDumVMSP,12 , " u J");
       SetDlgItemText(hDlg, HCF_REC_MOTOR26_SPOS_uSTEPSExtra, szDumVMSP );

       UpdateAllFields(hDlg, wParam, uiLine2ShowStartPositionsForZB);

       MoveWindow(hDlg, 450,7,566,1029,FALSE);
       if (GetDlgCtrlID((HWND) wParam) != IDC_MOTORSTARTPOS_LINE_NO){
          SetFocus(GetDlgItem(hDlg, IDC_MOTORSTARTPOS_LINE_NO));
          return FALSE;
         }
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDC_MOTORSTARTPOS_LINE_NO:{
            if (HIWORD(wParam) == CBN_SELENDOK) {//user has selected something from the list
               if (uiLine2ShowStartPositionsForZB != (UINT)SendMessage(GetDlgItem(hDlg,IDC_MOTORSTARTPOS_LINE_NO),CB_GETCURSEL,0,0L)){
                  uiLine2ShowStartPositionsForZB = (UINT)SendMessage(GetDlgItem(hDlg,IDC_MOTORSTARTPOS_LINE_NO),CB_GETCURSEL,0,0L);
                  UpdateAllFields(hDlg, wParam, uiLine2ShowStartPositionsForZB);
                 }
              }
           }
           break;
         case IDCANCEL:                            //End Dialog, without changing anything
           EndDialog(hDlg, LOWORD(wParam));
           break;
         case IDB_MOTORSTARTPOS_LINE_NO2PRINTNEXT: //End Dialog, having changed the Line to print next
           HCFrec.nNextLineToPrint = uiLine2ShowStartPositionsForZB;
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
      return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}

void UpdateAllFields(HWND hDlg, WPARAM wParam, unsigned int uiLineToShowStartPositionsForZB){//uiLine2ShowStartPositionsForZB
 #define    SIZSZDUM50uaf  50
 char szDumUAF[SIZSZDUM50uaf];
 /*
 int  nMotor;
 for(nMotor = 6; nMotor < NOOFMOTORS; nMotor++)              //Fill in the 'uSteps from Home' fields
   if((nMotor != 12)&&(nMotor != 13)&&(nMotor != 14)&&(nMotor != 15)&&(nMotor != 16)&&(nMotor != 17)
    &&(nMotor != 24)&&(nMotor != 25)&&(nMotor != 26)&&(nMotor != 27)&&(nMotor != 28)&&(nMotor != 29)
    &&(nMotor != 38)&&(nMotor != 39)){
      _itoa_s(nLineXstartPositionsFEM[nMotor][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
      SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS + nMotor, szDumUAF);
     } */
 //Fill in the Start Positions for the WP1ratio (Motors 1, 13 & 25).    These are the Start positions from the hcf file (as a ratio), but the start
 _gcvt_s(szDumUAF, 12, (double)HCFrec.WP1rRatioTimes1000/1000,3);//M1   positions in uSteps is interpolated, by looking at the CAL file
 if( szDumUAF[lstrlen(szDumUAF) - 1] == '.' )
   StringCchCat(szDumUAF,12 , "0 %");
 else
   StringCchCat(szDumUAF,12 , " %");
 SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPSExtra, szDumUAF ); 
 _itoa_s(interpolatedWP1rRatioUsteps,szDumUAF,SIZSZDUM50uaf,10);   //The function call to GetUstepsForThisRatio() above incorporates
 SetDlgItemText(hDlg, HCF_REC_MOTOR01_SPOS_uSTEPS, szDumUAF);      // the PCF offset (PCFrec.nNoofUsteps_Hm2WP1MinEnergy[Beam - 1])
 _gcvt_s(szDumUAF, 12, (double)HCFrec.WP1gRatioTimes1000/1000,3);//M13
 if( szDumUAF[lstrlen(szDumUAF) - 1] == '.' )
   StringCchCat(szDumUAF,12 , "0 %");
 else
   StringCchCat(szDumUAF,12 , " %");
 SetDlgItemText(hDlg, HCF_REC_MOTOR13_SPOS_uSTEPSExtra, szDumUAF );
 _itoa_s(interpolatedWP1gRatioUsteps,szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR13_SPOS_uSTEPS, szDumUAF);
 _gcvt_s(szDumUAF, 12, (double)HCFrec.WP1bRatioTimes1000/1000,3);//M25
 if( szDumUAF[lstrlen(szDumUAF) - 1] == '.' )
   StringCchCat(szDumUAF,12 , "0 %");
 else
   StringCchCat(szDumUAF,12 , " %");
 SetDlgItemText(hDlg, HCF_REC_MOTOR25_SPOS_uSTEPSExtra, szDumUAF );
 _itoa_s(interpolatedWP1bRatioUsteps,szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR25_SPOS_uSTEPS, szDumUAF);

//*1 done above, 2 & 3 are handled in WM_INITDIALOG

 //Fill in the Start Positions for Object Beam Length (Motors 4, 16 & 28). The Primary field is from the pcf file + start positions file.
 double dblObjBeamLength = (double)HCFrec.ObjBeamLengthR / 1000;
 sprintf_s(szDumUAF,SIZSZDUM50uaf, "%3.3f mm", dblObjBeamLength );
 SetDlgItemText(hDlg, HCF_REC_MOTOR04_SPOS_uSTEPSExtra, szDumUAF );//M4              ?
 dblObjBeamLength = (double)HCFrec.ObjBeamLengthG / 1000;
 sprintf_s(szDumUAF,SIZSZDUM50uaf, "%3.3f mm", dblObjBeamLength );
 SetDlgItemText(hDlg, HCF_REC_MOTOR16_SPOS_uSTEPSExtra, szDumUAF );//M16
 dblObjBeamLength = (double)HCFrec.ObjBeamLengthB / 1000;
 sprintf_s(szDumUAF,SIZSZDUM50uaf, "%3.3f mm", dblObjBeamLength );
 SetDlgItemText(hDlg, HCF_REC_MOTOR28_SPOS_uSTEPSExtra, szDumUAF );//M28

 _itoa_s(PCFrec.nNoofUsteps_Hm2LOobjXXXmm[0] + nLineXstartPositionsFEM[3][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR04_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps_Hm2LOobjXXXmm[1] + nLineXstartPositionsFEM[15][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR16_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps_Hm2LOobjXXXmm[2] + nLineXstartPositionsFEM[27][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR28_SPOS_uSTEPS, szDumUAF );

//*4 done above

 //Fill in the Start Positions for the Apertures (Motors 5, 17 & 29). The Start positions are actually from the pcf file.
 if(HCFrec.nApertureSize == 0)
   StringCchCopy(szDumUAF,SIZSZDUM50uaf, "0.8mm" );
 else if(HCFrec.nApertureSize == 1)
   StringCchCopy(szDumUAF,SIZSZDUM50uaf, "1.6mm" );
 else{
    MessageBox(hDlg,"HCFrec.nApertureSize was invalid!",NULL,MB_OK);
    EndDialog(hDlg, LOWORD(wParam));
   }
 SetDlgItemText(hDlg, HCF_REC_MOTOR05_SPOS_uSTEPSExtra, szDumUAF ); //Apertures are
 SetDlgItemText(hDlg, HCF_REC_MOTOR17_SPOS_uSTEPSExtra, szDumUAF ); // the same
 SetDlgItemText(hDlg, HCF_REC_MOTOR29_SPOS_uSTEPSExtra, szDumUAF ); //  for each beam

 int RED0p8or1p6NotPin = HCFrec.nApertureSize;
 _itoa_s(PCFrec.nNoofUsteps4EachApertureTab[RED0p8or1p6NotPin],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR05_SPOS_uSTEPS, szDumUAF );//M5
 int GRE0p8or1p6NotPin = RED0p8or1p6NotPin + 3;
 _itoa_s(PCFrec.nNoofUsteps4EachApertureTab[GRE0p8or1p6NotPin],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR17_SPOS_uSTEPS, szDumUAF );//M17
 int BLU0p8or1p6NotPin = RED0p8or1p6NotPin + 6;
 _itoa_s(PCFrec.nNoofUsteps4EachApertureTab[BLU0p8or1p6NotPin],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR29_SPOS_uSTEPS, szDumUAF );//M29

//*5 done above

 //M6  M18  M30  LC focus    Just the PCF file
 _itoa_s(PCFrec.nNoofUsteps4EachFocusTab[0],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR06_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps4EachFocusTab[1],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR18_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps4EachFocusTab[2],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR30_SPOS_uSTEPS, szDumUAF );

//*6 done above

 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointT[0] + nLineXstartPositionsFEM[ 6][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR07_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointB[0] + nLineXstartPositionsFEM[ 7][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR08_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointL[0] + nLineXstartPositionsFEM[ 8][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR09_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointR[0] + nLineXstartPositionsFEM[ 9][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR10_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[0] + nLineXstartPositionsFEM[10][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR11_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[0] + nLineXstartPositionsFEM[11][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR12_SPOS_uSTEPS, szDumUAF);
//*7-12 done above
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointT[1] + nLineXstartPositionsFEM[18][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR19_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointB[1] + nLineXstartPositionsFEM[19][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR20_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointL[1] + nLineXstartPositionsFEM[20][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR21_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointR[1] + nLineXstartPositionsFEM[21][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR22_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[1] + nLineXstartPositionsFEM[22][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR23_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[1] + nLineXstartPositionsFEM[23][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR24_SPOS_uSTEPS, szDumUAF);
//*19-24 done above
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointT[2] + nLineXstartPositionsFEM[30][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR31_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointB[2] + nLineXstartPositionsFEM[31][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR32_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointL[2] + nLineXstartPositionsFEM[32][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR33_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointR[2] + nLineXstartPositionsFEM[33][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR34_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[2] + nLineXstartPositionsFEM[34][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR35_SPOS_uSTEPS, szDumUAF);
 _itoa_s(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[2] + nLineXstartPositionsFEM[35][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR36_SPOS_uSTEPS, szDumUAF);
//*31-36 done above

 //Ref Beam Table            Just the PCF file
 _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR37_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR38_SPOS_uSTEPS, szDumUAF );

 //XY Stage
  _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[0] + nLineXstartPositionsFEM[38][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR39_SPOS_uSTEPS, szDumUAF );
 _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[1] + nLineXstartPositionsFEM[39][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR40_SPOS_uSTEPS, szDumUAF ); 
//Put Extra field on M39 / M40
 _itoa_s(nLineXstartPositionsFEM[38][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR39_SPOS_uSTEPSExtra, szDumUAF );
 _itoa_s(nLineXstartPositionsFEM[39][uiLineToShowStartPositionsForZB],szDumUAF,SIZSZDUM50uaf,10);
 SetDlgItemText(hDlg, HCF_REC_MOTOR40_SPOS_uSTEPSExtra, szDumUAF );





}
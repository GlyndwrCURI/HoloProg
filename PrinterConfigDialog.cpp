#include "stdafx.h"
extern HFONT newFont;
BOOL bUserDecidedNotToPrintOrMove;
//                                         PrinterConfigDialog.cpp contains:
//CALLBACK ViewPrinterConfig_Dialog()

#define DONT_REPORT_ERRORS FALSE
#define DO_REPORT_ERRORS   TRUE

char szWP1CalFilename[_MAX_PATH]; //This file also contains the data for WP13 and WP25
char szWP2CalFilename[_MAX_PATH];
char szWP3CalFilename[_MAX_PATH];
char szWP14CalFilename[_MAX_PATH];
char szWP15CalFilename[_MAX_PATH];
char szWP26CalFilename[_MAX_PATH];
char szWP27CalFilename[_MAX_PATH];
char szPingMall_bat_FullFileName[_MAX_PATH];

//extern int  bMotor2ExpectRepliesFrom[NOOFMOTORS]; //now in PCFrec

BOOL bErrorCheckAllPrinterConfigFields(HWND hDlg, BOOL bReportErrors);     //This function prototype is local to prevent external linkage.
//Ball park, probably achievable, constant speeds starting from stationary for different uStep settings are:
//1    25600 Actually
//2    12800  these
//4     6400   are
//8     3200    not
//16    1600     very
//32     800      good!
//64     400       See Below:
/* As I've stopped using nArFastSpeeds4HomingDeptOn_uSteps (now homing uses profiles) I'll send a nArSlowSpeeds4HomingDeptOn_uSteps[] value with MsTyp3_HOME instead
int  nArFastSpeeds4HomingDeptOn_uSteps[65] = { //This is not allowed to have TC values > 59999... This forces a minimum Homing-in speed.  Used in (& only in) MsTyp3_HOME messages
29997,  //set to 29997 to show these aren't used anymore
29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,
29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997,29997
   //0,
   //40000,  //The motor can stall sometimes if, on full steps, this is set to 25600
   //20000,  //The motor has little starting torque if this is set to 12800 (and you twist against it with your hand)
   //0,
   //10000,   //4 uSteps
   //0,0,0,
   //5000,    //8 uSteps
   //0,0,0,0,0,0,0,
   //2500,    //16 uSteps
   //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   //1250,     //32 uSteps
   ////800,     //32 uSteps test
   //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   //626       //64 uSteps                        I've decided all numbers must be even. Then I can added 1 to tell UC3 to use fast acceleration homing
  }; */

/*
int nArSlowSpeeds4HomingDeptOn_uSteps[65] = {//MAYBE this is not allowed to have TC values > 59999... This forces a minimum Homing-in speed.  Used in (& only in) MsTyp3_HOME messages
   0,
   3840000,   //The motor can stall sometimes if, on full steps, this is set to 25600
   1920000,    //The motor has little starting torque if this is set to 12800 (and you twist against it with your hand)
   0,
   960000,    //4 uSteps
   0,0,0,
   480000,    //8 uSteps
   0,0,0,0,0,0,0,
   240000,    //16 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   120000,     //32 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   60000      //64 uSteps : this will be twice the speed of all the above, but still not too fast for homeing
  }; */
int ChangeValuesToAboveWhenWorkingOK;
int nArSlowSpeeds4HomingDeptOn_uSteps[65] = {//MAYBE this is not allowed to have TC values > 59999... This forces a minimum Homing-in speed.  Used in (& only in) MsTyp3_HOME messages
   0,
   20000,   //The motor can stall sometimes if, on full steps, this is set to 25600
   20000,    //The motor has little starting torque if this is set to 12800 (and you twist against it with your hand)
   0,
   20000,    //4 uSteps
   0,0,0,
   20000,    //8 uSteps
   0,0,0,0,0,0,0,
   20000,    //16 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   20000,     //32 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   20000      //64 uSteps : this will be twice the speed of all the above, but still not too fast for homeing
  };
int nArMaxSpeedsDependentOn_uSteps[65] = {  //   Used in (& only in) M sTyp5_SING_MM messages
   0,
   40000,  //The motor can stall sometimes if, on full steps, this is set to 25600
   20000,  //The motor has little starting torque if this is set to 12800 (and you twist against it with your hand)
   0,
   10000,   //4 uSteps
   0,0,0,
   5000,    //8 uSteps
   0,0,0,0,0,0,0,
   2500,    //16 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 //12000000, //32 uSteps
   1250,     //32 uSteps
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   625       //64 uSteps
  };

//pcf file consists of: ATMEL_IP_ADDRESStab AND chNoofUsteps4EachMotorTab AND nNoofUsteps4EachApertureTab AND nNoofUsteps4EachFocusTab AND nNoofUsteps4RefBeamTablePrintUnload
//char ATMEL_IP_ADDRESStab[NOOFMOTORS][16] = {"192.168.000.201","192.168.000.202","192.168.000.203","192.168.000.204","192.168.000.205","192.168.000.206","192.168.000.207","192.168.000.208","192.168.000.209","192.168.000.210",
//"192.168.000.211","192.168.000.212","192.168.000.213","192.168.000.214","192.168.000.215","192.168.000.216","192.168.000.217","192.168.000.218","192.168.000.219","192.168.000.220","192.168.000.221","192.168.000.222","192.168.000.223",
//"192.168.000.224","192.168.000.225","192.168.000.226","192.168.000.227","192.168.000.228","192.168.000.229","192.168.000.230","192.168.000.231","192.168.000.232","192.168.000.233","192.168.000.234","192.168.000.235","192.168.000.236",
//"192.168.000.237","192.168.000.238","192.168.000.239","192.168.000.240"
    /* Motors 37, 38 are to move the beam reference table out of the way. */
//};
//1,2,4,8,16,32,64 are allowed in the next table. The values "32" are defaults... is is expected that, if the pcf file exists, they will be overwritten.
//char chNoofUsteps4EachMotorTab[NOOFMOTORS] = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
//int nNoofUsteps4EachApertureTab[9] = {3200,6400,76800,3200,6400,76800,3200,6400,76800}; //RedNo1,RedNo2,RedPin,GreNo1,GreNo2,GrePin,BluNo1,BluNo2,BluPin. This table stores the 
//                                  // position the motor must move to, to select 1 of the three possible apertures (for each Laser)
//int nNoofUsteps4EachFocusTab[3] = {3200,3300,3400};//Similar to above table. These are just default (nonsense values) for Red Green and Blue
//int nNoofUsteps4RefBeamTablePrintUnload[4] = {3700,3800,3373,3838};//M37Print, M38Print, M37Unload, M38Unload

PCFRECORD DefaultPCFrec = {
#include "DefaultPCFrec.h" //Note This is Not a standard Header file, it is an included file to ensure the default HCFrecord
};

PCFRECORD PCFrec;
PCFRECORD CopyOfPCFrec;
extern HCFRECORD HCFrec;
extern HINSTANCE hInst;
extern int nHelpMessageToLoad;

char szPrinterPCFfilename[_MAX_DIR];

#define LENszAps 10
static char szAps[9][LENszAps]={"Red No1","Red No2","Red pin","Green No1","Green No2","Green pin","Blue No1","Blue No2","Blue pin"};
#define LENszFocus 6
static char szFocus[3][LENszFocus]={"Red","Green","Blue"};
//"Object Beam Aperture "
INT_PTR CALLBACK ViewPrinterConfig_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 switch (message){
    case WM_INITDIALOG:{
      #define       SIZSZDUMAD      16
       char szDumAd[SIZSZDUMAD];
       int  nMotor;
       //memcpy(CopyOfNoofUsteps4EachMotorTab, chNoofUsteps4EachMotorTab, NOOFMOTORS);
       //memcpy(CopyOfNoofUsteps4EachApertureTab, nNoofUsteps4EachApertureTab, 9*sizeof(int));
       //memcpy(CopyOfNoofUsteps4EachFocusTab, nNoofUsteps4EachFocusTab, 3*sizeof(int));
       //memcpy(CopyOfNoofUsteps4RefBeamTablePrintUnload, nNoofUsteps4RefBeamTablePrintUnload, 4*sizeof(int));
       memcpy(&CopyOfPCFrec, &PCFrec, sizeof(PCFRECORD));


#define LOWERY 18
#define REQROWS 40
int upper = LOWERY + 21 * REQROWS + 36; //36 allows for the spaces
int IDB_STATREC_MOTOR_IP2a1_40 = IDB_STATREC_MOTOR_IPa;
int IDB_STATREC_MOTOR_IP2b1_40 = IDB_STATREC_MOTOR_IPb;
int IDB_STATREC_MOTOR_IP2c1_40 = IDB_STATREC_MOTOR_IPc;
int IDB_STATREC_MOTOR_IP2d1_40 = IDB_STATREC_MOTOR_IPd;
       int Ypos, YposP1;
       for(Ypos = LOWERY; Ypos < upper; Ypos+=21){
          if(Ypos == 270)  //add a space between rows after motor 12
            Ypos += 18;
          if(Ypos == 540)  //add a space between rows after motor 24
            Ypos += 18;
          if(Ypos == 810)  //add a space between rows after motor 36
            Ypos += 18;
          YposP1 = Ypos + 1;
          //EDITTEXT        IDB_STATREC_MOTOR_IP2a,99,24,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP3a,99,37,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP4a,99,50,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP5a,99,63,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP6a,99,76,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP7a,99,89,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP8a,99,102,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP9a,99,115,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP10a,99,128,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP11a,99,141,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          //EDITTEXT        IDB_STATREC_MOTOR_IP12a,99,154,17,12,ES_AUTOHSCROLL | ES_READONLY | NOT WS_TABSTOP
          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Position
                    149, Ypos, 27, 22,  hDlg, (HMENU)IDB_STATREC_MOTOR_IP2a1_40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_MOTOR_IP2a1_40++,WM_SETFONT,(WPARAM)newFont,0);

          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Position
                    179, Ypos, 27, 22,  hDlg, (HMENU)IDB_STATREC_MOTOR_IP2b1_40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_MOTOR_IP2b1_40++,WM_SETFONT,(WPARAM)newFont,0);

          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Position
                    209, Ypos, 27, 22,  hDlg, (HMENU)IDB_STATREC_MOTOR_IP2c1_40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_MOTOR_IP2c1_40++,WM_SETFONT,(WPARAM)newFont,0);

          CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT","", ES_AUTOHSCROLL | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,   //Position
                    239, Ypos, 27, 22,  hDlg, (HMENU)IDB_STATREC_MOTOR_IP2d1_40,hInst,NULL);
          SendDlgItemMessage(hDlg,IDB_STATREC_MOTOR_IP2d1_40++,WM_SETFONT,(WPARAM)newFont,0);

         }

       for(nMotor = 0; nMotor < NOOFMOTORS; nMotor++){                                      //Fill in the IP Addresses
          StringCchCopy(szDumAd,SIZSZDUMAD,PCFrec.ATMEL_IP_ADDRESStab[nMotor]);
          szDumAd[3] = szDumAd[7] = szDumAd[11]  = szDumAd[15] = 0;
          SetDlgItemText(hDlg, IDB_STATREC_MOTOR_IPa + nMotor, szDumAd     );
          SetDlgItemText(hDlg, IDB_STATREC_MOTOR_IPb + nMotor, szDumAd +  4);
          SetDlgItemText(hDlg, IDB_STATREC_MOTOR_IPc + nMotor, szDumAd +  8);
          SetDlgItemText(hDlg, IDB_STATREC_MOTOR_IPd + nMotor, szDumAd + 12);
         }
       for(nMotor = 0; nMotor < NOOFMOTORS; nMotor++){                                      //Fill in the uSteps and Physical Limits (Max & Min)
          _itoa_s(PCFrec.chNoofUsteps4EachMotorTab[nMotor],szDumAd,SIZSZDUMAD,10);
          SetDlgItemText(hDlg, PCF_REC_MOTOR01_uSTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nPhysLimMaxUsteps[nMotor],szDumAd,SIZSZDUMAD,10);
          SetDlgItemText(hDlg, PCF_RECm01_PHYSLimMAX_USTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nPhysLimMinUsteps[nMotor],szDumAd,SIZSZDUMAD,10);
          SetDlgItemText(hDlg, PCF_RECm01_PHYSLimMIN_USTEPS + nMotor, szDumAd     );
         }
       for(int EMotr = 0; EMotr < NOOFMOTORS; EMotr++)                                      //Set up the Radio Buttons
         if(PCFrec.bMotor2ExpectRepliesFrom[EMotr])
           CheckRadioButton(hDlg, IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr);
         else
           CheckRadioButton(hDlg, IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr, FALSE);

       for(nMotor = 0; nMotor < 3; nMotor++){
          _itoa_s(PCFrec.nNoofUsteps_Hm2WP1MinEnergy[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 WP1MinEnergy positions
          SetDlgItemText(hDlg, PCF_RECm1_WP1RedMinEngyUSTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2WP2MinEnergy[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 WP2MinEnergy positions
          SetDlgItemText(hDlg, PCF_RECm2_WP2RedMinEngyUSTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2WP3MinEnergy[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 WP3MinEnergy positions
          SetDlgItemText(hDlg, PCF_RECm3_WP3RedMinEngyUSTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2LOobjXXXmm[nMotor],szDumAd,SIZSZDUMAD,10);          //Fill in the 3 LOobjXXXmm positions
          SetDlgItemText(hDlg, PCF_RECm4_LOobjRedH2xxxMM + nMotor, szDumAd     );

          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointT[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointT positions
          SetDlgItemText(hDlg, PCF_RECm7_RBapTRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointB[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointB positions
          SetDlgItemText(hDlg, PCF_RECm8_RBapBRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointL[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointL positions
          SetDlgItemText(hDlg, PCF_RECm9_RBapLRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointR[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointR positions
          SetDlgItemText(hDlg, PCF_RECm10_RBapRRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonHzeroDegrees positions
          SetDlgItemText(hDlg, PCF_REC_GONhRed_uSH2zeroDEG + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonVzeroDegrees positions
          SetDlgItemText(hDlg, PCF_REC_GONvRed_uSH2zeroDEG + nMotor, szDumAd     );
         }
       for(nMotor = 0; nMotor < 2; nMotor++){
          _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[nMotor],szDumAd,SIZSZDUMAD,10);       //Fill in the 2 Hm2 point (0,0) XY Stage positions
          SetDlgItemText(hDlg, PCF_RECm39_XYxMMH2Origin + nMotor, szDumAd     );
         }

       for(nMotor = 0; nMotor < 9; nMotor++){                                               //Fill in the 9 object beam aperture positions
          _itoa_s(PCFrec.nNoofUsteps4EachApertureTab[nMotor],szDumAd,SIZSZDUMAD,10);
          SetDlgItemText(hDlg, PCF_REC_OBJBEAMAPNo1R + nMotor, szDumAd     );
         }
       for(nMotor = 0; nMotor < 3; nMotor++){                                               //Fill in the 3 LCOS Focus positions
          _itoa_s(PCFrec.nNoofUsteps4EachFocusTab[nMotor],szDumAd,SIZSZDUMAD,10);
          SetDlgItemText(hDlg, PCF_REC_LCOS_FOCUS_R + nMotor, szDumAd     );
         }
//Fill in the 2 RefBeamTable Print positions
       _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[0],szDumAd,SIZSZDUMAD,10);
       SetDlgItemText(hDlg, PCF_REC_REF_BEAM_M1PPOS, szDumAd);
       _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[1],szDumAd,SIZSZDUMAD,10);
       SetDlgItemText(hDlg, PCF_REC_REF_BEAM_M1LPOS, szDumAd);
//Fill in the 2 RefBeamTable Load positions
       _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[2],szDumAd,SIZSZDUMAD,10);
       SetDlgItemText(hDlg, PCF_REC_REF_BEAM_M2PPOS, szDumAd);
       _itoa_s(PCFrec.nNoofUsteps4RefBeamTablePrintUnload[3],szDumAd,SIZSZDUMAD,10);
       SetDlgItemText(hDlg, PCF_REC_REF_BEAM_M2LPOS, szDumAd);

       _itoa_s(PCFrec.nM39_XAXISBACKLASH,szDumAd,SIZSZDUMAD,10);
       SetDlgItemText(hDlg, PCF_RECm39_XAXISBACKLASH, szDumAd);

       MoveWindow(hDlg, 170,7,1510,1029,FALSE);
      }
      return (INT_PTR)TRUE;
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDB_PRINTCONFIG_VIEW1:
         case IDB_PRINTCONFIG_VIEW13:
         case IDB_PRINTCONFIG_VIEW25:
           DisplayFileInEditor(hDlg, szWP1CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW2:
           DisplayFileInEditor(hDlg, szWP2CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW3:
           DisplayFileInEditor(hDlg, szWP3CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW14:
           DisplayFileInEditor(hDlg, szWP14CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW15:
           DisplayFileInEditor(hDlg, szWP15CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW26:
           DisplayFileInEditor(hDlg, szWP26CalFilename);
           break;
         case IDB_PRINTCONFIG_VIEW27:
           DisplayFileInEditor(hDlg, szWP27CalFilename);
           break;
         case PCF_REC_EDIT_IPs:
           if( IDYES == MessageBox(hDlg,"-Backup First!\n-Last Part of each IP must be >= 100\n-Will need to recompile & reprogram all UC3 chips\n   (if using a different range of addresses)\n\nStill Continue?","Don't edit this file unless you know what you're doing!",MB_YESNO|MB_DEFBUTTON2) )
             DisplayFileInEditor(hDlg, szPrinterPCFfilename);
           break;
         case PCF_REC_PING_M_ALL:{
            char szPingMallResultsFile[_MAX_PATH]; //PingMall.bat writes its output to pingMallResults.txt
            if( _access(szPingMall_bat_FullFileName, 0) == 0){ //Make certain the batch file exists
               struct _stat64i32 status;         //Path exists, but is it a file?
               _stat(szPingMall_bat_FullFileName, &status);
               if((status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
                  MessageBox(hDlg,"PingMall.bat is a Folder!","An Essential file was missing.. please rectify this",MB_OK);
                  break;
                 }
              }
            else{//PingMall.bat doesn't exist
               MessageBox(hDlg,"It should exist in the same folder as HoloProg.exe","The program file 'PingMall.bat' was missing!",MB_OK);
               break;
              }
            _getcwd(szPingMallResultsFile,_MAX_PATH);
            system(szPingMall_bat_FullFileName);
//            StringCchCopy(szPingMallResultsFile,_MAX_PATH,szPingMall_bat_FullFileName);//The results file will be in the same folder as the batch file (hopefully!)
//            char *ptrBslash;
//            ptrBslash = strrchr(szPingMallResultsFile,BACKSLASH);             //reverse search for the final backslash
//            szPingMallResultsFile[(int)(ptrBslash-szPingMallResultsFile + 1)]=0;   //szGlobalFileName now TEMPORARILY holds the directory the progam resides in
            StringCchCat(szPingMallResultsFile,_MAX_PATH,"\\pingMallResults.txt");
            DisplayFileInEditor(hDlg, szPingMallResultsFile);
           // _unlink(szPingMallResultsFile);
           }
           break;
         case PCF_REC_EDIT_IPsHELP:
           nHelpMessageToLoad = 2;
           DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPDIALOG1), hDlg, HelpDialog1_Dialog);
           break;
         case IDPRINTERCONFIG_UPDATEnE:
           if( bErrorCheckAllPrinterConfigFields(hDlg, DO_REPORT_ERRORS) ) //returns true if there's an error - having displayed an approprate MessageBox.
             break;
           if( !mySave_File(hDlg, 1000,"pcf",0, szPrinterPCFfilename, MOTORNO_NOT_USED) )
             EndDialog(hDlg, LOWORD(wParam));
            //else file didn't save, but a message will have been displayed to user
           break;
         case IDC_PRINTCONFIG_RADIOexpRep_M01: case IDC_PRINTCONFIG_RADIOexpRep_M11: case IDC_PRINTCONFIG_RADIOexpRep_M21: case IDC_PRINTCONFIG_RADIOexpRep_M31:
         case IDC_PRINTCONFIG_RADIOexpRep_M02: case IDC_PRINTCONFIG_RADIOexpRep_M12: case IDC_PRINTCONFIG_RADIOexpRep_M22: case IDC_PRINTCONFIG_RADIOexpRep_M32:
         case IDC_PRINTCONFIG_RADIOexpRep_M03: case IDC_PRINTCONFIG_RADIOexpRep_M13: case IDC_PRINTCONFIG_RADIOexpRep_M23: case IDC_PRINTCONFIG_RADIOexpRep_M33:
         case IDC_PRINTCONFIG_RADIOexpRep_M04: case IDC_PRINTCONFIG_RADIOexpRep_M14: case IDC_PRINTCONFIG_RADIOexpRep_M24: case IDC_PRINTCONFIG_RADIOexpRep_M34:
         case IDC_PRINTCONFIG_RADIOexpRep_M05: case IDC_PRINTCONFIG_RADIOexpRep_M15: case IDC_PRINTCONFIG_RADIOexpRep_M25: case IDC_PRINTCONFIG_RADIOexpRep_M35:
         case IDC_PRINTCONFIG_RADIOexpRep_M06: case IDC_PRINTCONFIG_RADIOexpRep_M16: case IDC_PRINTCONFIG_RADIOexpRep_M26: case IDC_PRINTCONFIG_RADIOexpRep_M36:
         case IDC_PRINTCONFIG_RADIOexpRep_M07: case IDC_PRINTCONFIG_RADIOexpRep_M17: case IDC_PRINTCONFIG_RADIOexpRep_M27: case IDC_PRINTCONFIG_RADIOexpRep_M37:
         case IDC_PRINTCONFIG_RADIOexpRep_M08: case IDC_PRINTCONFIG_RADIOexpRep_M18: case IDC_PRINTCONFIG_RADIOexpRep_M28: case IDC_PRINTCONFIG_RADIOexpRep_M38:
         case IDC_PRINTCONFIG_RADIOexpRep_M09: case IDC_PRINTCONFIG_RADIOexpRep_M19: case IDC_PRINTCONFIG_RADIOexpRep_M29: case IDC_PRINTCONFIG_RADIOexpRep_M39:
         case IDC_PRINTCONFIG_RADIOexpRep_M10: case IDC_PRINTCONFIG_RADIOexpRep_M20: case IDC_PRINTCONFIG_RADIOexpRep_M30: case IDC_PRINTCONFIG_RADIOexpRep_M40:{
            int nButtonPressed = LOWORD(wParam) - IDC_PRINTCONFIG_RADIOexpRep_M01;
            if( (nButtonPressed < 0) || (nButtonPressed > 39) ){
               MessageBox(hDlg,"nButtonPressed out of bounds - Programming error4!",NULL,MB_OK);
               break;
              }
            if( PCFrec.bMotor2ExpectRepliesFrom[nButtonPressed] )
              PCFrec.bMotor2ExpectRepliesFrom[nButtonPressed]  = FALSE;
            else
              PCFrec.bMotor2ExpectRepliesFrom[nButtonPressed]  = TRUE;
            for(int EMotr = 0; EMotr < NOOFMOTORS; EMotr++)
              if(PCFrec.bMotor2ExpectRepliesFrom[EMotr])
                CheckRadioButton(hDlg, IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr);
              else
                CheckRadioButton(hDlg, IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr,IDC_PRINTCONFIG_RADIOexpRep_M01+EMotr, FALSE);
           }
           break;
//The following buttons are for moving the motors on the Printer Configuration Screen... They move the motors to absolute positions
         case IDB_PRINTCONFIG_OBJBEAM_APNO1R: //M5
         case IDB_PRINTCONFIG_OBJBEAM_APNO2R:
         case IDB_PRINTCONFIG_OBJBEAM_APpinR:
         case IDB_PRINTCONFIG_OBJBEAM_APNO1G: //M17
         case IDB_PRINTCONFIG_OBJBEAM_APNO2G:
         case IDB_PRINTCONFIG_OBJBEAM_APpinG:
         case IDB_PRINTCONFIG_OBJBEAM_APNO1B: //M29
         case IDB_PRINTCONFIG_OBJBEAM_APNO2B:
         case IDB_PRINTCONFIG_OBJBEAM_APpinB:
         case IDB_PRINTCONFIG_LCOS_FOCUS_R:   //M6
         case IDB_PRINTCONFIG_LCOS_FOCUS_G:   //M18
         case IDB_PRINTCONFIG_LCOS_FOCUS_B:   //M30
         case IDB_PRINTCONFIG_REF_BEAM_M1PPOS://M37
         case IDB_PRINTCONFIG_REF_BEAM_M1LPOS:
         case IDB_PRINTCONFIG_REF_BEAM_M2PPOS://M38
         case IDB_PRINTCONFIG_REF_BEAM_M2LPOS:
         case IDB_PRINTCONFIG_M1:
         case IDB_PRINTCONFIG_M2:
         case IDB_PRINTCONFIG_M3:
         case IDB_PRINTCONFIG_M4:
         case IDB_PRINTCONFIG_M7:
         case IDB_PRINTCONFIG_M8:
         case IDB_PRINTCONFIG_M9:
         case IDB_PRINTCONFIG_M10:
         case IDB_PRINTCONFIG_M11:
         case IDB_PRINTCONFIG_M12:
         case IDB_PRINTCONFIG_M13:
         case IDB_PRINTCONFIG_M14:
         case IDB_PRINTCONFIG_M15:
         case IDB_PRINTCONFIG_M16:
         case IDB_PRINTCONFIG_M19:
         case IDB_PRINTCONFIG_M20:
         case IDB_PRINTCONFIG_M21:
         case IDB_PRINTCONFIG_M22:
         case IDB_PRINTCONFIG_M23:
         case IDB_PRINTCONFIG_M24:
         case IDB_PRINTCONFIG_M25:
         case IDB_PRINTCONFIG_M26:
         case IDB_PRINTCONFIG_M27:
         case IDB_PRINTCONFIG_M28:
         case IDB_PRINTCONFIG_M31:
         case IDB_PRINTCONFIG_M32:
         case IDB_PRINTCONFIG_M33:
         case IDB_PRINTCONFIG_M34:
         case IDB_PRINTCONFIG_M35:
         case IDB_PRINTCONFIG_M36:
         case IDB_PRINTCONFIG_M39:
         case IDB_PRINTCONFIG_M40: {
            //800 is the speed (inversed, ie timer count)... It should be dependent on uSteps for this motor:
//Ball park, probably achievable, constant speeds starting from stationary for different uStep settings are:
//1    25600
//2    12800
//4     6400
//8     3200
//16    1600
//32     800
//64     400
            const BOOL bAbsPositioningMoveMotorFast = 1;//This could be declared in the PCF file, with a  checkbox to control whether or not to use acceleration profiles when pressing the Move Buttons on the Configuration screen.
            if( bErrorCheckAllPrinterConfigFields(hDlg, DO_REPORT_ERRORS) ) //Returns TRUE if there's an error - having displayed an approprate MessageBox.
              break;                                                        // I call it to read the fields.
            if( mySave_File(hDlg, 1000,"pcf",0, szPrinterPCFfilename, MOTORNO_NOT_USED) )     //Having read the fields OK, it's ok to save the file
              break;
            int MotorFromControlID[48] = {4,4,4,16,16,16,28,28,28,5,17,29,36,36,37,37,                                      //this array gets the correct motor
                                        //1,2,3,6,7,8,9,10,11,13,14,15,18,19,20,21,22,23,25,26,27,30,31,32,33,34,35,38,39}; // for the pressed button 
                                          0,12,24,1,13,25,2,14,26,3,15,27,6,18,30,7,19,31,8,20,32,9,21,33,10,22,34,11,23,35,38,39}; // for the pressed button 
          //int BUT_FOR_TEST; //Once tested, uncomment the above and comment this:
          //int MotorFromControlID[16] = {0,0,0,36,36,36,37,37,37,0,36,37,0,0,37,37};
            int nnMotorClickedOn = MotorFromControlID[wParam - IDB_PRINTCONFIG_OBJBEAM_APNO1R];
            char nchMotorClickedOn;
            if((nnMotorClickedOn < 0) ||(nnMotorClickedOn > 39)){
               MessageBox(hDlg,"nnMotorClickedOn out of bounds - Programming error3!",NULL,MB_OK);
               break;
              }
            else
              nchMotorClickedOn = (char)nnMotorClickedOn;
            if(     (nchMotorClickedOn ==  4) || (nchMotorClickedOn == 16) || (nchMotorClickedOn == 28))       //Object Beam Aperture Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps4EachApertureTab[wParam - IDB_PRINTCONFIG_OBJBEAM_APNO1R], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  5) || (nchMotorClickedOn == 17) || (nchMotorClickedOn == 29))  //Focus Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps4EachFocusTab[wParam - IDB_PRINTCONFIG_LCOS_FOCUS_R], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn == 36) || (nchMotorClickedOn == 37))  //RefBeamTable Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps4RefBeamTablePrintUnload[wParam - IDB_PRINTCONFIG_REF_BEAM_M1PPOS], bAbsPositioningMoveMotorFast);

            else if((nchMotorClickedOn ==  0) || (nchMotorClickedOn == 12) || (nchMotorClickedOn == 24))  //WavePlate1 Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2WP1MinEnergy[wParam - IDB_PRINTCONFIG_M1], bAbsPositioningMoveMotorFast);

            else if((nchMotorClickedOn ==  1) || (nchMotorClickedOn == 13) || (nchMotorClickedOn == 25))  //WavePlate2 Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2WP2MinEnergy[wParam - IDB_PRINTCONFIG_M2], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  2) || (nchMotorClickedOn == 14) || (nchMotorClickedOn == 26))  //WavePlate3 Motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2WP3MinEnergy[wParam - IDB_PRINTCONFIG_M3], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  3) || (nchMotorClickedOn == 15) || (nchMotorClickedOn == 27))  //LOobj uSteps to xxx MM
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2LOobjXXXmm[wParam - IDB_PRINTCONFIG_M4], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  6) || (nchMotorClickedOn == 18) || (nchMotorClickedOn == 30))  //RefBeam Apt Top motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2CenterPointT[wParam - IDB_PRINTCONFIG_M7], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  7) || (nchMotorClickedOn == 19) || (nchMotorClickedOn == 31))  //RefBeam Apt Bottom motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2CenterPointB[wParam - IDB_PRINTCONFIG_M8], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  8) || (nchMotorClickedOn == 20) || (nchMotorClickedOn == 32))  //RefBeam Apt Left motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2CenterPointL[wParam - IDB_PRINTCONFIG_M9], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn ==  9) || (nchMotorClickedOn == 21) || (nchMotorClickedOn == 33))  //RefBeam Apt Right motors
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2CenterPointR[wParam - IDB_PRINTCONFIG_M10], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn == 10) || (nchMotorClickedOn == 22) || (nchMotorClickedOn == 34))  //GonH motors (uSteps to 0 degrees)
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[wParam - IDB_PRINTCONFIG_M11], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn == 11) || (nchMotorClickedOn == 23) || (nchMotorClickedOn == 35))  //GonV motors (uSteps to 0 degrees)
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[wParam - IDB_PRINTCONFIG_M12], bAbsPositioningMoveMotorFast);
            else if((nchMotorClickedOn == 38) || (nchMotorClickedOn == 39))  //XY Stage (uSteps to 0,0)
              MoveMotor2AbsolutePosition(hDlg,  nchMotorClickedOn, PCFrec.nNoofUsteps_XYstageHm2Origin[wParam - IDB_PRINTCONFIG_M39], bAbsPositioningMoveMotorFast);
           }
           break;
         case IDCANCEL:
           if( bErrorCheckAllPrinterConfigFields(hDlg, DONT_REPORT_ERRORS) //NULL == Don't report errors, just try to update the fields to see if any have chanegd
            //|| memcmp(chNoofUsteps4EachMotorTab,   &CopyOfNoofUsteps4EachMotorTab, NOOFMOTORS)// ... then we can warn that updates haven't been saved
            //|| memcmp(nNoofUsteps4EachApertureTab, &CopyOfNoofUsteps4EachApertureTab,  9 * sizeof(int) )
            //|| memcmp(nNoofUsteps4EachFocusTab, &CopyOfNoofUsteps4EachFocusTab,  3 * sizeof(int) )
            || memcmp(&PCFrec, &CopyOfPCFrec,  sizeof(PCFRECORD) )  ){
              if(IDYES == MessageBox(hDlg,"Are you sure you want to exit and LOSE these changes?","You have made changes which have not been saved",MB_YESNO)){
                 //memcpy(chNoofUsteps4EachMotorTab,   CopyOfNoofUsteps4EachMotorTab, NOOFMOTORS);
                 //memcpy(nNoofUsteps4EachApertureTab, CopyOfNoofUsteps4EachApertureTab, 9 * sizeof(int) );
                 //memcpy(nNoofUsteps4EachFocusTab,    CopyOfNoofUsteps4EachFocusTab, 3 * sizeof(int) );
                 //memcpy(nNoofUsteps4RefBeamTablePrintUnload, CopyOfNoofUsteps4RefBeamTablePrintUnload, 3 * sizeof(int) );
                 memcpy(&PCFrec, &CopyOfPCFrec, sizeof(PCFRECORD));
//int I_SHOULD_REWRITE_THE_FILE_HERE; // I think, especially as mySave_File() may have been called above?
                 mySave_File(hDlg, 1000,"pcf",0, szPrinterPCFfilename, MOTORNO_NOT_USED);
                 EndDialog(hDlg, LOWORD(wParam));
                }
              else
                break;
             }
           else
             EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
      return (INT_PTR)TRUE;
   }
 return (INT_PTR)FALSE;
}
//Should set bUserDecidedNotToPrintOrMove FALSE before returning, if the user decides they still want to print
INT_PTR CALLBACK ViewPCFversusHCF_Dialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
 lParam; //stop compiler warning that parameter is not used
 HWND hDlgItem;
 static HBRUSH hBrushRed = NULL;
 static HBRUSH hBrushYellow = NULL;
 static HBRUSH hBrushAmber = NULL;
 static BOOL bUserCannotDecidedToPrintOrMove;
 static BOOL g_fShown;
 switch (message){
    case WM_INITDIALOG:{
      #define       SIZSZDUMAD1      16
       char szDumAd[SIZSZDUMAD1];
       int  nMotor;
       hBrushRed    = CreateSolidBrush(RGB(255,0,0));
       //hBrushYellow = CreateSolidBrush(RGB(223,255,5));
       hBrushYellow = CreateSolidBrush(RGB(255,255,0));
       hBrushAmber  = CreateSolidBrush(RGB(255,192,0));
       bUserCannotDecidedToPrintOrMove = FALSE;
       g_fShown = FALSE;
       for(nMotor = 0; nMotor < NOOFMOTORS; nMotor++){
          _itoa_s(PCFrec.chNoofUsteps4EachMotorTab[nMotor],szDumAd,SIZSZDUMAD1,10);      //Fill in Current uSteps (from PCFrec)
          SetDlgItemText(hDlg, PCF_REC_MOTOR01_uSTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nPhysLimMaxUsteps[nMotor],szDumAd,SIZSZDUMAD,10);               //Fill in Current PhysLimMax
          SetDlgItemText(hDlg, PCF_RECm01_PHYSLimMAX_USTEPS + nMotor, szDumAd     );
          _itoa_s(PCFrec.nPhysLimMinUsteps[nMotor],szDumAd,SIZSZDUMAD,10);               //Fill in Current PhysLimMin
          SetDlgItemText(hDlg, PCF_RECm01_PHYSLimMIN_USTEPS + nMotor, szDumAd     );

          _itoa_s(HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nMotor],szDumAd,SIZSZDUMAD1,10); //Fill in Original uSteps     (from HCFrec)
          SetDlgItemText(hDlg, HCF_REC_MOTOR01_uSTEPS + nMotor, szDumAd     );                     
          _itoa_s(HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[nMotor],szDumAd,SIZSZDUMAD1,10);         //Fill in Original PhysLimMax
          SetDlgItemText(hDlg, HCF_RECm01_PHYSLimMAX_USTEPS + nMotor, szDumAd     );               
          _itoa_s(HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[nMotor],szDumAd,SIZSZDUMAD1,10);         //Fill in Original PhysLimMin
          SetDlgItemText(hDlg, HCF_RECm01_PHYSLimMIN_USTEPS + nMotor, szDumAd     );

          if(PCFrec.chNoofUsteps4EachMotorTab[nMotor] == HCFrec.HCF_PCFrecCopy.chNoofUsteps4EachMotorTab[nMotor]){ //Disable uStep Controls whose HCF/PCF data matches
             hDlgItem = GetDlgItem(hDlg,PCF_REC_MOTOR01_uSTEPS + nMotor);                                          //These will display greyed
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_REC_MOTOR01_uSTEPS + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          else{ //uSteps/ Step are different. If this is for a dynamic motor, the user must be prevented from printing
             if(!(((nMotor > -1)&&(nMotor <  6))  //motors 1 - 6, 12- 18, & 24 - 30 are exempt
                ||((nMotor > 11)&&(nMotor < 18))
                ||((nMotor > 23)&&(nMotor < 30))))
             bUserCannotDecidedToPrintOrMove = TRUE;
            }
          if(PCFrec.nPhysLimMaxUsteps[nMotor] == HCFrec.HCF_PCFrecCopy.nPhysLimMaxUsteps[nMotor]){                 //Disable Physical Limits Controls whose HCF/PCF data matches
             hDlgItem = GetDlgItem(hDlg,PCF_RECm01_PHYSLimMAX_USTEPS + nMotor);                                    //These will display greyed
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm01_PHYSLimMAX_USTEPS + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nPhysLimMinUsteps[nMotor] == HCFrec.HCF_PCFrecCopy.nPhysLimMinUsteps[nMotor]){                 //Disable Physical Limits Controls whose HCF/PCF data matches
             hDlgItem = GetDlgItem(hDlg,PCF_RECm01_PHYSLimMIN_USTEPS + nMotor);                                    //These will display greyed
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm01_PHYSLimMIN_USTEPS + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
         }
       for(nMotor = 0; nMotor < 3; nMotor++){
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointT[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointT positions (from PCF)
          SetDlgItemText(hDlg, PCF_RECm7_RBapTRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointB[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointB positions
          SetDlgItemText(hDlg, PCF_RECm8_RBapBRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointL[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointL positions
          SetDlgItemText(hDlg, PCF_RECm9_RBapLRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2CenterPointR[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointR positions
          SetDlgItemText(hDlg, PCF_RECm10_RBapRRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonHzeroDegrees positions
          SetDlgItemText(hDlg, PCF_REC_GONhRed_uSH2zeroDEG + nMotor, szDumAd     );
          _itoa_s(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonVzeroDegrees positions
          SetDlgItemText(hDlg, PCF_REC_GONvRed_uSH2zeroDEG + nMotor, szDumAd     );

          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointT[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointT positions (from HCF)
          SetDlgItemText(hDlg, HCF_RECm7_RBapTRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointB[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointB positions
          SetDlgItemText(hDlg, HCF_RECm8_RBapBRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointL[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointL positions
          SetDlgItemText(hDlg, HCF_RECm9_RBapLRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointR[nMotor],szDumAd,SIZSZDUMAD,10);        //Fill in the 3 Hm2CenterPointR positions
          SetDlgItemText(hDlg, HCF_RECm10_RBapRRed_uSH2CP + nMotor, szDumAd     );
          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonHzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonHzeroDegrees positions
          SetDlgItemText(hDlg, HCF_REC_GONhRed_uSH2zeroDEG + nMotor, szDumAd     );
          _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonVzeroDegrees[nMotor],szDumAd,SIZSZDUMAD,10);     //Fill in the 3 Hm2GonVzeroDegrees positions
          SetDlgItemText(hDlg, HCF_REC_GONvRed_uSH2zeroDEG + nMotor, szDumAd     );
          if(PCFrec.nNoofUsteps_Hm2CenterPointT[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointT[nMotor]){    //Disable Physical Limits Controls whose HCF/PCF data matches
             hDlgItem = GetDlgItem(hDlg,PCF_RECm7_RBapTRed_uSH2CP + nMotor);                                              //These will display greyed
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm7_RBapTRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nNoofUsteps_Hm2CenterPointB[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointB[nMotor]){
             hDlgItem = GetDlgItem(hDlg,PCF_RECm8_RBapBRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm8_RBapBRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nNoofUsteps_Hm2CenterPointL[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointL[nMotor]){
             hDlgItem = GetDlgItem(hDlg,PCF_RECm9_RBapLRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm9_RBapLRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nNoofUsteps_Hm2CenterPointR[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2CenterPointR[nMotor]){
             hDlgItem = GetDlgItem(hDlg,PCF_RECm10_RBapRRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_RECm10_RBapRRed_uSH2CP + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonHzeroDegrees[nMotor]){
             hDlgItem = GetDlgItem(hDlg,PCF_REC_GONhRed_uSH2zeroDEG + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_REC_GONhRed_uSH2zeroDEG + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
          if(PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[nMotor] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_Hm2GonVzeroDegrees[nMotor]){
             hDlgItem = GetDlgItem(hDlg,PCF_REC_GONvRed_uSH2zeroDEG + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
             hDlgItem = GetDlgItem(hDlg,HCF_REC_GONvRed_uSH2zeroDEG + nMotor);
             SendMessage(hDlgItem, WM_ENABLE, 0,0);
            }
         }

       _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[0],szDumAd,SIZSZDUMAD,10);   //Fill in the XYstage - X position (from PCF)
       SetDlgItemText(hDlg, PCF_RECm39_XYxMMH2Origin, szDumAd     );
       _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[1],szDumAd,SIZSZDUMAD,10);   //Fill in the XYstage - Y position
       SetDlgItemText(hDlg, PCF_RECm40_XYyMMH2Origin, szDumAd     );

       _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[0],szDumAd,SIZSZDUMAD,10);   //Fill in the XYstage - X position (from HCF)
       SetDlgItemText(hDlg, HCF_RECm39_XYxMMH2Origin, szDumAd     );
       _itoa_s(HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[1],szDumAd,SIZSZDUMAD,10);   //Fill in the XYstage - Y position
       SetDlgItemText(hDlg, HCF_RECm40_XYyMMH2Origin, szDumAd     );

       _itoa_s(PCFrec.nM39_XAXISBACKLASH,szDumAd,SIZSZDUMAD,10);                 //Fill in the XYstage - XAXISBACKLASH (from PCF)
       SetDlgItemText(hDlg, PCF_RECm39_XAXISBACKLASH, szDumAd     );
       _itoa_s(HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH,szDumAd,SIZSZDUMAD,10);  //Fill in the XYstage - XAXISBACKLASH (from HCF)
       SetDlgItemText(hDlg, HCF_PCF_RECm39_XAXISBACKLASH, szDumAd     );

       if(PCFrec.nNoofUsteps_XYstageHm2Origin[0] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[0]){ //Disable XYstage - X Controls whose HCF/PCF data matches
          hDlgItem = GetDlgItem(hDlg,PCF_RECm39_XYxMMH2Origin);                                             //These will display greyed
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          hDlgItem = GetDlgItem(hDlg,HCF_RECm39_XYxMMH2Origin);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
         }
       if(PCFrec.nNoofUsteps_XYstageHm2Origin[1] == HCFrec.HCF_PCFrecCopy.nNoofUsteps_XYstageHm2Origin[1]){
          hDlgItem = GetDlgItem(hDlg,PCF_RECm40_XYyMMH2Origin);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          hDlgItem = GetDlgItem(hDlg,HCF_RECm40_XYyMMH2Origin);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
         }

       if(PCFrec.nM39_XAXISBACKLASH == HCFrec.HCF_PCFrecCopy.nM39_XAXISBACKLASH){
          hDlgItem = GetDlgItem(hDlg,PCF_RECm39_XAXISBACKLASH);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          hDlgItem = GetDlgItem(hDlg,HCF_PCF_RECm39_XAXISBACKLASH);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
         }

       if(bUserCannotDecidedToPrintOrMove){ //Show them only the OK button
          hDlgItem = GetDlgItem(hDlg,IDB_I_ACCEPT_THE_RISK);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          ShowWindow(hDlgItem,SW_HIDE);
          hDlgItem = GetDlgItem(hDlg,IDCANCEL);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          ShowWindow(hDlgItem,SW_HIDE);
         }
       else{                                //Show them only the 'Accept Risk' and 'Cancel' buttons
          hDlgItem = GetDlgItem(hDlg,IDOK);
          SendMessage(hDlgItem, WM_ENABLE, 0,0);
          ShowWindow(hDlgItem,SW_HIDE);
         }
       MoveWindow(hDlg, 144,7,1586,1029,FALSE);
      }
      return (INT_PTR)TRUE;
    case WM_CTLCOLOREDIT:{
    //We're going to get one of these messages for every (enabled) text control on the screen
       SetBkMode((HDC)wParam, TRANSPARENT);
       int nCtrlID = GetDlgCtrlID((HWND)lParam);
       if( ((nCtrlID >= HCF_REC_MOTOR07_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR12_uSTEPS))
             || ((nCtrlID >= HCF_REC_MOTOR19_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR24_uSTEPS))
             || ((nCtrlID >= HCF_REC_MOTOR31_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR36_uSTEPS))
             ||  (nCtrlID == HCF_REC_MOTOR39_uSTEPS)
             ||  (nCtrlID == HCF_REC_MOTOR40_uSTEPS)
             || ((nCtrlID >= PCF_REC_MOTOR07_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR12_uSTEPS))
             || ((nCtrlID >= PCF_REC_MOTOR19_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR24_uSTEPS))
             || ((nCtrlID >= PCF_REC_MOTOR31_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR36_uSTEPS))
             ||  (nCtrlID == PCF_REC_MOTOR39_uSTEPS)
             ||  (nCtrlID == PCF_REC_MOTOR40_uSTEPS)    )           
         return (LPARAM)hBrushRed;
       else if( ((nCtrlID >= HCF_REC_MOTOR01_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR06_uSTEPS))
        || ((nCtrlID >= HCF_REC_MOTOR13_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR18_uSTEPS))
        || ((nCtrlID >= HCF_REC_MOTOR25_uSTEPS) && (nCtrlID <= HCF_REC_MOTOR30_uSTEPS))         
        || ((nCtrlID >= HCF_RECm01_PHYSLimMAX_USTEPS) && (nCtrlID <= HCF_RECm06_PHYSLimMAX_USTEPS))
        || ((nCtrlID >= HCF_RECm13_PHYSLimMAX_USTEPS) && (nCtrlID <= HCF_RECm18_PHYSLimMAX_USTEPS))
        || ((nCtrlID >= HCF_RECm25_PHYSLimMAX_USTEPS) && (nCtrlID <= HCF_RECm30_PHYSLimMAX_USTEPS))         
        || ((nCtrlID >= HCF_RECm01_PHYSLimMIN_USTEPS) && (nCtrlID <= HCF_RECm06_PHYSLimMIN_USTEPS))
        || ((nCtrlID >= HCF_RECm13_PHYSLimMIN_USTEPS) && (nCtrlID <= HCF_RECm18_PHYSLimMIN_USTEPS))
        || ((nCtrlID >= HCF_RECm25_PHYSLimMIN_USTEPS) && (nCtrlID <= HCF_RECm30_PHYSLimMIN_USTEPS))
        || ((nCtrlID >= PCF_REC_MOTOR01_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR06_uSTEPS))
        || ((nCtrlID >= PCF_REC_MOTOR13_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR18_uSTEPS))
        || ((nCtrlID >= PCF_REC_MOTOR25_uSTEPS) && (nCtrlID <= PCF_REC_MOTOR30_uSTEPS))         
        || ((nCtrlID >= PCF_RECm01_PHYSLimMAX_USTEPS) && (nCtrlID <= PCF_RECm06_PHYSLimMAX_USTEPS))
        || ((nCtrlID >= PCF_RECm13_PHYSLimMAX_USTEPS) && (nCtrlID <= PCF_RECm18_PHYSLimMAX_USTEPS))
        || ((nCtrlID >= PCF_RECm25_PHYSLimMAX_USTEPS) && (nCtrlID <= PCF_RECm30_PHYSLimMAX_USTEPS))         
        || ((nCtrlID >= PCF_RECm01_PHYSLimMIN_USTEPS) && (nCtrlID <= PCF_RECm06_PHYSLimMIN_USTEPS))
        || ((nCtrlID >= PCF_RECm13_PHYSLimMIN_USTEPS) && (nCtrlID <= PCF_RECm18_PHYSLimMIN_USTEPS))
        || ((nCtrlID >= PCF_RECm25_PHYSLimMIN_USTEPS) && (nCtrlID <= PCF_RECm30_PHYSLimMIN_USTEPS))    )
         return (LPARAM)hBrushYellow;
       else
         return (LPARAM)hBrushAmber;
      }
    case WM_WINDOWPOSCHANGED:
      if((((WINDOWPOS*)lParam)->flags & SWP_SHOWWINDOW) && !g_fShown){  //This indicates that all the initial dialog box processing is finished, & the window has now displayed.  Now I can
         g_fShown = TRUE;                                               // display the Messages in PrinterConfigMatchesHCFconfig(hDlg). I've delayed until now, as it is desirable for the user 
         PostMessage(hDlg,WM_APP,0,0);                                  // to see those MessageBoxes with the ViewPCFversusHCF_Dialog in the background (showing them the details of what's wrong)
        }
      break;
    case WM_APP:
      PrinterConfigMatchesHCFconfig(hDlg);  //We already know this is going to return > 0 and display an error message or warning - We are in ViewPCFversusHCF_Dialog() only because
      break;                                // this has already happened (but had a NULL parameter to prevent messages the first time)
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
         case IDOK:
         case IDCANCEL:
           EndDialog(hDlg, LOWORD(wParam));
           break;
         case IDB_I_ACCEPT_THE_RISK:
           bUserDecidedNotToPrintOrMove = FALSE;
           EndDialog(hDlg, LOWORD(wParam));
           break;
        }//end switch
      return (INT_PTR)TRUE;
    case WM_DESTROY:
      if( hBrushRed != NULL)
        DeleteObject(hBrushRed);
      if( hBrushYellow != NULL)
        DeleteObject(hBrushYellow);
      if( hBrushAmber != NULL)
        DeleteObject(hBrushAmber);      
      break;
   }
 return (INT_PTR)FALSE;
}

BOOL bErrorCheckAllPrinterConfigFields(HWND hDlg, BOOL bReportErrors){ //returns TRUE if there's a problem
 int  nNumberInField;
 BOOL bWasTranslated;
 //Timestamp will need to be generated (probably as part of hologram mpos data calculation), see  //time( &uTimeStamp );
 //GetDlgItemText(hDlg, IDC_VIEWHOLOGINFO_HOLOGNAME, HCFrec.szHologramName, sizeof(HCFrec.szHologramName) - 1);
 for(int nEachMotor = 0; nEachMotor < NOOFMOTORS; nEachMotor++){
    nNumberInField = GetDlgItemInt(hDlg, PCF_REC_MOTOR01_uSTEPS + nEachMotor, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
       if( (nNumberInField != 1)  && (nNumberInField != 2)  && (nNumberInField != 4) && (nNumberInField != 8) &&
           (nNumberInField != 16) && (nNumberInField != 32) && (nNumberInField != 64) ){
          char szDumMot[13];
          szDumMot[0] = 'M';
          _itoa_s(nEachMotor+1,szDumMot+1,12,10);
          if(bReportErrors)
            MessageBox(hDlg,"uSTEPS per Step must be between either 1, 2, 4, 8, 16, 32 or 64!\n\n(The offending Motor's number is shown in the title of this message box)",szDumMot,MB_OK);
          return 1;
         }
       else
         PCFrec.chNoofUsteps4EachMotorTab[nEachMotor] = (char)nNumberInField;
      }
    else{
       char szDumMott[13];
       szDumMott[0] = 'M';
       _itoa_s(nEachMotor+1,szDumMott+1,12,10);
       if(bReportErrors)
         MessageBox(hDlg,"Couldn't Read uSTEPS per Step (Numeric field)\n\n(The offending Motor's number is shown in the title of this message box)",szDumMott,MB_OK);
       return 1;
      }

    nNumberInField = GetDlgItemInt(hDlg, PCF_RECm01_PHYSLimMIN_USTEPS + nEachMotor, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated) //The number in the field read OK.
      PCFrec.nPhysLimMinUsteps[nEachMotor] = nNumberInField;
    else{
       char szDumMott[13];
       szDumMott[0] = 'M';
       _itoa_s(nEachMotor+1,szDumMott+1,12,10);
       if(bReportErrors)
         MessageBox(hDlg,"Couldn't Read Physical Limits Min (Numeric field)\n\n(The offending Motor's number is shown in the title of this message box)",szDumMott,MB_OK);
       return 1;
      }

    nNumberInField = GetDlgItemInt(hDlg, PCF_RECm01_PHYSLimMAX_USTEPS + nEachMotor, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated) //The number in the field read OK.
      PCFrec.nPhysLimMaxUsteps[nEachMotor] = nNumberInField;
    else{
       char szDumMott[13];
       szDumMott[0] = 'M';
       _itoa_s(nEachMotor+1,szDumMott+1,12,10);
       if(bReportErrors)
         MessageBox(hDlg,"Couldn't Read Physical Limits Max (Numeric field)\n\n(The offending Motor's number is shown in the title of this message box)",szDumMott,MB_OK);
       return 1;
      }
   }

//#define PCF_REC_OBJBEAMAPNo1R           3301
//#define PCF_REC_OBJBEAMAPNo1G           3302
//#define PCF_REC_OBJBEAMAPNo1B           3303
//#define PCF_REC_OBJBEAMAPNo2R           3304
//#define PCF_REC_OBJBEAMAPNo2G           3305
//#define PCF_REC_OBJBEAMAPNo2B           3306
//#define PCF_REC_OBJBEAMAPpinR           3307
//#define PCF_REC_OBJBEAMAPpinG           3308
//#define PCF_REC_OBJBEAMAPpinB           3309
 for(int nEachApertureField = 0; nEachApertureField < 9; nEachApertureField++){ //There are 9 apertures (3 for each laser colour)
    nNumberInField = GetDlgItemInt(hDlg, PCF_REC_OBJBEAMAPNo1R + nEachApertureField, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
       if( (nNumberInField < -1300000)  || (nNumberInField > 1300000) ){
          char szDumOBA[22 + LENszAps] = "Object Beam Aperture ";
          StringCchCat(szDumOBA, 22 + LENszAps, szAps[nEachApertureField]);
          if(bReportErrors)
            MessageBox(hDlg,szDumOBA,"Field must be between 1 and 1,300,000. See :",MB_OK);
          return 1;
         }
       else
         PCFrec.nNoofUsteps4EachApertureTab[nEachApertureField] = nNumberInField;
      }
    else{
       char szDumOBA[22 + LENszAps] = "Object Beam Aperture ";
       StringCchCat(szDumOBA, 22 + LENszAps, szAps[nEachApertureField]);
     //MessageBox(hDlg,"Couldn't read an Object Beam Aperture field (Numeric field)\n\n(The offending Aperture is shown in the title of this message box)",szAps[nEachApertureField],MB_OK);
       if(bReportErrors)
         MessageBox(hDlg,szDumOBA,"Couldn't read this field (integer required)",MB_OK);
       return 1;
      }
   }
//#define PCF_REC_LCOS_FOCUS_R            3310
//#define PCF_REC_LCOS_FOCUS_G            3311
//#define PCF_REC_LCOS_FOCUS_B            3312
 for(int nEachFocusField = 0; nEachFocusField < 3; nEachFocusField++){ //There are 3 LCOS Focus' (1 for each laser colour)
    nNumberInField = GetDlgItemInt(hDlg, PCF_REC_LCOS_FOCUS_R + nEachFocusField, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
       if( (nNumberInField < -1300000)  || (nNumberInField > 1300000) ){
          char szDumLCOSfoc[12 + LENszFocus] = "LCOS Focus ";
          StringCchCat(szDumLCOSfoc, 12 + LENszFocus, szFocus[nEachFocusField]);
          if(bReportErrors)
            MessageBox(hDlg,szDumLCOSfoc,"Field must be between  -1,300,000  and 1,300,000. See :",MB_OK);
          return 1;
         }
       else
         PCFrec.nNoofUsteps4EachFocusTab[nEachFocusField] = nNumberInField;
      }
    else{
       char szDumLCOSfoc[12 + LENszFocus] = "LCOS Focus ";
       StringCchCat(szDumLCOSfoc, 12 + LENszFocus, szFocus[nEachFocusField]);
     //MessageBox(hDlg,"Couldn't read an Object Beam Aperture field (Numeric field)\n\n(The offending Aperture is shown in the title of this message box)",szFocus[nEachFocusField],MB_OK);
       if(bReportErrors)
         MessageBox(hDlg,szDumLCOSfoc,"Couldn't read this field (integer required)",MB_OK);
       return 1;
      }
   }
#define LENszField      4
 char szFieldNames[30][LENszField] = {
//   "WP2MinEnergyRed",   "WP2MinEnergyGre",   "WP2MinEnergyBlu",   "WP3MinEnergyRed",      "WP3MinEnergyGre",      "WP3MinEnergyBlu",      "LOobjXXXmmRed",        "LOobjXXXmmGre",        "LOobjXXXmmBlu",
//   "Hm2CenterPointTRed","Hm2CenterPointTGre","Hm2CenterPointTBlu","Hm2CenterPointBRed",   "Hm2CenterPointBGre",   "Hm2CenterPointBBlu",   "Hm2CenterPointLRed",   "Hm2CenterPointLGre",   "Hm2CenterPointLBlu",
//   "Hm2CenterPointRRed","Hm2CenterPointRGre","Hm2CenterPointRBlu","Hm2GonHzeroDegreesRed","Hm2GonHzeroDegreesGre","Hm2GonHzeroDegreesBlu","Hm2GonVzeroDegreesRed","Hm2GonVzeroDegreesGre","Hm2GonVzeroDegreesBlu" };
   "M1", "M13", "M25", "M2", "M14", "M26", "M3", "M15", "M27", "M4", "M16", "M28",
   "M7", "M19", "M31", "M8", "M20", "M32", "M9", "M21", "M33",
   "M10","M22", "M34", "M11","M23", "M35","M12", "M24", "M36" };
// "WP1MinEnergyRed","WP2MinEnergyRed","WP3MinEnergyRed","LOobjXXXmmRed","Hm2CenterPointTRed","Hm2CenterPointBRed","Hm2CenterPointLRed", "Hm2CenterPointRRed","Hm2GonHzeroDegreesRed","Hm2GonVzeroDegreesRed",
// "WP1MinEnergyGre","WP2MinEnergyGre","WP3MinEnergyGre","LOobjXXXmmGre","Hm2CenterPointTGre","Hm2CenterPointBGre","Hm2CenterPointLGre", "Hm2CenterPointRGre","Hm2GonHzeroDegreesGre","Hm2GonVzeroDegreesGre",
// "WP1MinEnergyBlu","WP2MinEnergyBlu","WP3MinEnergyBlu","LOobjXXXmmBlu","Hm2CenterPointTBlu","Hm2CenterPointBBlu","Hm2CenterPointLBlu", "Hm2CenterPointRBlu","Hm2GonHzeroDegreesBlu","Hm2GonVzeroDegreesBlu" };
 
 for(int nEachField = 0; nEachField < 30; nEachField++){ //There are 3 WP1MinEnergy, WP2MinEnergy, WP3MinEnergy, LOobjXXXmm, Hm2CenterPointT,
                                                         // Hm2CenterPointB,Hm2CenterPointL,Hm2CenterPointR,Hm2GonHzeroDegrees,Hm2GonVzeroDegrees (was + 2 XYStage numbers) to collect
    nNumberInField = GetDlgItemInt(hDlg, PCF_RECm1_WP1RedMinEngyUSTEPS + nEachField, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
       if( (nNumberInField < -1300000)  || (nNumberInField > 1300000) ){
          if(bReportErrors)
            MessageBox(hDlg,szFieldNames[nEachField],"Field must be between  -1,300,000  and 1,300,000. See :",MB_OK);
          return 1;
         }
       else{
          if(nEachField < 3)
            PCFrec.nNoofUsteps_Hm2WP1MinEnergy[nEachField] = nNumberInField;
          if(nEachField < 6)
            PCFrec.nNoofUsteps_Hm2WP2MinEnergy[nEachField - 3] = nNumberInField;
          else if(nEachField < 9)
            PCFrec.nNoofUsteps_Hm2WP3MinEnergy[nEachField - 6] = nNumberInField;
          else if(nEachField < 12)
            PCFrec.nNoofUsteps_Hm2LOobjXXXmm[nEachField - 9] = nNumberInField;
          else if(nEachField < 15)
            PCFrec.nNoofUsteps_Hm2CenterPointT[nEachField - 12] = nNumberInField;
          else if(nEachField < 18)
            PCFrec.nNoofUsteps_Hm2CenterPointB[nEachField - 15] = nNumberInField;
          else if(nEachField < 21)
            PCFrec.nNoofUsteps_Hm2CenterPointL[nEachField - 18] = nNumberInField;
          else if(nEachField < 24)
            PCFrec.nNoofUsteps_Hm2CenterPointR[nEachField - 21] = nNumberInField;
          else if(nEachField < 27)
            PCFrec.nNoofUsteps_Hm2GonHzeroDegrees[nEachField - 24] = nNumberInField;
          else if(nEachField < 30)
            PCFrec.nNoofUsteps_Hm2GonVzeroDegrees[nEachField - 27] = nNumberInField;
          else
            PCFrec.nNoofUsteps_XYstageHm2Origin[nEachField - 30] = nNumberInField;
         }
      }
    else{
     //MessageBox(hDlg,"Couldn't read an Object Beam Aperture field (Numeric field)\n\n(The offending Aperture is shown in the title of this message box)",szFocus[nEachField],MB_OK);
       if(bReportErrors)
         MessageBox(hDlg,szFieldNames[nEachField],"Couldn't read this field (integer required)",MB_OK);
       return 1;
      }
   }

 nNumberInField = GetDlgItemInt(hDlg, PCF_RECm39_XYxMMH2Origin, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -2000000)  || (nNumberInField > 2000000) ){
       if(bReportErrors)
         MessageBox(hDlg,"M39","Field must be between  -2,000,000  and 2,000,000. See :",MB_OK);
       return 1;
      }
    else{
       PCFrec.nNoofUsteps_XYstageHm2Origin[0] = nNumberInField;
      }
   }
 else{
    if(bReportErrors)
      MessageBox(hDlg,"M39","Couldn't read this field (integer required)",MB_OK);
    return 1;
   }

 nNumberInField = GetDlgItemInt(hDlg, PCF_RECm40_XYyMMH2Origin, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < -14800000)  || (nNumberInField > 14800000) ){ //Gearbox is 7.4:1
       if(bReportErrors)
         MessageBox(hDlg,"M40","Field must be between  -14,800,000  and 14,800,000. See :",MB_OK);
       return 1;
      }
    else{
       PCFrec.nNoofUsteps_XYstageHm2Origin[1] = nNumberInField;
      }
   }
 else{
    if(bReportErrors)
      MessageBox(hDlg,"M40","Couldn't read this field (integer required)",MB_OK);
    return 1;
   }

/*       DONT FORGET THIS
 for(nMotor = 0; nMotor < 2; nMotor++){
    _itoa_s(PCFrec.nNoofUsteps_XYstageHm2Origin[nMotor],szDumAd,SIZSZDUMAD,10);       //Fill in the 2 Hm2 point (0,0) XY Stage positions
    SetDlgItemText(hDlg, PCF_RECm39_XYxMMH2Origin + nMotor, szDumAd     );
   }
*/


//#define PCF_REC_REF_BEAM_M1PPOS             3313
//#define PCF_REC_REF_BEAM_M2PPOS             3314
//#define PCF_REC_REF_BEAM_M1PPOS             3315
//#define PCF_REC_REF_BEAM_M2PPOS             3316
 for(int nEachRefBeamTableMotor = 0; nEachRefBeamTableMotor < 4; nEachRefBeamTableMotor++){ //There are 2 RefBeamTable motors (each having 2 positions)
    nNumberInField = GetDlgItemInt(hDlg, PCF_REC_REF_BEAM_M1PPOS + nEachRefBeamTableMotor, &bWasTranslated, TRUE);      //True means return a signed int
    if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
       if( (nNumberInField < -1300000)  || (nNumberInField > 1300000) ){
          char szDumRBT[23] = "Ref Beam Table Motor ";
          if((nEachRefBeamTableMotor == 1)||(nEachRefBeamTableMotor == 3)){
             szDumRBT[21] = '2';
             szDumRBT[22] = 0;
            }
          else{
             szDumRBT[21] = '1';
             szDumRBT[22] = 0;
            }
          if(bReportErrors){
             if((nEachRefBeamTableMotor == 0)||(nEachRefBeamTableMotor == 1))
               MessageBox(hDlg,szDumRBT,"Print Position must be between -1,300,000 and 1,300,000. See :",MB_OK);
             else
               MessageBox(hDlg,szDumRBT,"Load Position  must be between -1,300,000 and 1,300,000. See :",MB_OK);
            }
          return 1;
         }
       else
         PCFrec.nNoofUsteps4RefBeamTablePrintUnload[nEachRefBeamTableMotor] = nNumberInField;
      }
    else{
       char szDumRBT[23] = "Ref Beam Table Motor ";
       if((nEachRefBeamTableMotor == 1)||(nEachRefBeamTableMotor == 3)){
          szDumRBT[21] = '2';
          szDumRBT[22] = 0;
         }
       else{
          szDumRBT[21] = '1';
          szDumRBT[22] = 0;
         }
       if(bReportErrors){
         if((nEachRefBeamTableMotor == 0)||(nEachRefBeamTableMotor == 1))
            MessageBox(hDlg,szDumRBT,"Couldn't read Print Position field (integer required)",MB_OK);
          else
            MessageBox(hDlg,szDumRBT,"Couldn't read Load Position field (integer required)",MB_OK);
         }
       return 1;
      }
   }
 int nAboluteLimitOnM39BacklashUsteps = 500 * PCFrec.chNoofUsteps4EachMotorTab[38];
 nNumberInField = GetDlgItemInt(hDlg, PCF_RECm39_XAXISBACKLASH, &bWasTranslated, TRUE);      //True means return a signed int
 if(bWasTranslated){ //The number in the field read OK.  Now do other checking on it:
    if( (nNumberInField < 0)  || (nNumberInField > nAboluteLimitOnM39BacklashUsteps) ){ //Gearbox is 7.4:1
       if(bReportErrors)
         MessageBox(hDlg,"Field must be between  0 and [the Equivalent of] 500 Full steps.","M39 Backlash",MB_OK);
       return 1;
      }
    else{
       PCFrec.nM39_XAXISBACKLASH = nNumberInField;
      }
   }
 else{
    if(bReportErrors)
      MessageBox(hDlg,"M39 Backlash","Couldn't read this field (integer required)",MB_OK);
    return 1;
   }
 return 0;
}

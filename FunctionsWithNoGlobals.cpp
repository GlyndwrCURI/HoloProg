#include "StdAfx.h"

extern char mes[]; //This global is an exception! It is just working storage... Never used to pass data between functions

//char szAccelFileNamesDir[_MAX_DIR];        //Acceleration Profiles are all loaded by using a Filename held in this buffer. The buffer is loaded at startup with the folder name in which holoprog is located... this
//extern int  nLenszAccelFileNamesDir;       // part of the buffer never changes, but different filenames are appended. It could be viewed simply as the location of holoprog.exe => accel files must be stored here.

//Non-Global Data (Local to this file):
static int nLenszAccelFileNamesDir;  
static const char szTabProfileFileNameUstepEndingsPRN[7][8] ={"-1.prn","-2.prn","-4.prn","-8.prn","-16.prn","-32.prn","-64.prn"}; //e.g. "AccProfM39-30Hz-64.prn"
static const char szTabProfileFileNameUstepEndingsCSV[7][8] ={"-1.csv","-2.csv","-4.csv","-8.csv","-16.csv","-32.csv","-64.csv"}; //e.g. "AccProfM39-30Hz-64.csv"
static const char szTabProfileFileNameUstepEndingsCSB[7][8] ={"-1.csb","-2.csb","-4.csb","-8.csb","-16.csb","-32.csb","-64.csb"}; //e.g. "AccProfM39-30Hz-64.csv"
static const char szTabProfileFileNameFreqEndings[4][5] ={"10Hz","15Hz","30Hz","60Hz"}; //e.g. "AccProfM39-30Hz"

//Functions Local to this file:
int DecodeNoofUstepsToAnIndex(int chHCFrecNoofUsteps4EachMotorFromTab){
 switch(chHCFrecNoofUsteps4EachMotorFromTab){
    case 1:  return 0;
    case 2:  return 1;
    case 4:  return 2;
    case 8:  return 3;
    case 16: return 4;
    case 32: return 5;
    case 64: return 6;
    default: return 1000001;
   }
}

int DecodeFreqToAnIndex(int nFrequencyOfPixelPrintsInHz){
 switch(nFrequencyOfPixelPrintsInHz){
    case 10:  return 0;
    case 15:  return 1;
    case 30:  return 2;
    case 60:  return 3;
    default: return 1000001;
   }
}

void SetLenszAccelFileNamesDir(int nUnchangingLength){
 nLenszAccelFileNamesDir = nUnchangingLength;
}

int ConstructedFileName(HWND hDlg, char *szAccelFileNamesDir, int idxMnoP1, char chHCFrecNoofUsteps4EachMotorFromTab, int nHCFrecFrequencyOfPixelPrintsInHz, int nEndingWanted){
 char szConstructedFileName[12] = "AccProfM39-";
 szAccelFileNamesDir[nLenszAccelFileNamesDir] = 0;
 switch(idxMnoP1){
    case 40:{szConstructedFileName[8] = '4'; szConstructedFileName[9] = '0'; break;} //change szConstructedFileName to "AccProfM40-"

    case  1:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '1'; break;} //change szConstructedFileName to "AccProfM01-"
    case 13:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '3'; break;} //change szConstructedFileName to "AccProfM13-"
    case 25:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '5'; break;} //change szConstructedFileName to "AccProfM25-"

    case  2:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '2'; break;}
    case 14:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '4'; break;}
    case 26:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '6'; break;}

    case  3:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '3'; break;}
    case 15:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '5'; break;}
    case 27:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '7'; break;}
            
    case  4:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '4'; break;}
    case 16:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '6'; break;}
    case 28:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '8'; break;}

    case  5:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '5'; break;}
    case 17:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '7'; break;}
    case 29:{szConstructedFileName[8] = '2';                                 break;}

    case  6:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '6'; break;}
    case 18:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '8'; break;}
    case 30:{                                szConstructedFileName[9] = '0'; break;}

    case  7:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '7'; break;}
    case 19:{szConstructedFileName[8] = '1';                                 break;}
    case 31:{                                szConstructedFileName[9] = '1'; break;}

    case  8:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '8'; break;}
    case 20:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '0'; break;}
    case 32:{                                szConstructedFileName[9] = '2'; break;}

    case  9:{szConstructedFileName[8] = '0'; szConstructedFileName[9] = '9'; break;}
    case 21:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '1'; break;}
    case 33:{                                szConstructedFileName[9] = '3'; break;}

    case 10:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '0'; break;}
    case 22:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '2'; break;}
    case 34:{                                szConstructedFileName[9] = '4'; break;}

    case 11:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '1'; break;}
    case 23:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '3'; break;}
    case 35:{                                szConstructedFileName[9] = '5'; break;}

    case 12:{szConstructedFileName[8] = '1'; szConstructedFileName[9] = '2'; break;}
    case 24:{szConstructedFileName[8] = '2'; szConstructedFileName[9] = '4'; break;}
    case 36:{                                szConstructedFileName[9] = '6'; break;}
    case 37:{                                szConstructedFileName[9] = '7'; break;}
    case 38:{                                szConstructedFileName[9] = '8'; break;}
    case 39: break;
    default: MessageBox(hDlg,"idxMnoP1 invalid in ConstructedFileName()",NULL,MB_OK);
   }
 StringCchCat(szAccelFileNamesDir,_MAX_DIR,szConstructedFileName);//originally ,"AccProfM39-"

 int nIdxToFileNameUstepEndings = DecodeNoofUstepsToAnIndex(chHCFrecNoofUsteps4EachMotorFromTab);  //turns 1,2,4,8 etc into 0,1,2,3 etc   (returns 1000001 on error)
 int nIdxToFileNameFreqEndings  = DecodeFreqToAnIndex(nHCFrecFrequencyOfPixelPrintsInHz);          //turns 10,15,30,60 into 0,1,2,3.      (returns 1000001 on error)
 if((nIdxToFileNameUstepEndings == 1000001)||(nIdxToFileNameFreqEndings == 1000001)){
    sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
    MessageBox(hDlg,"Invalid chHCFrecNoofUsteps4EachMotorFromTab or nHCFrecFrequencyOfPixelPrintsInHz parameter",mes,MB_OK);//"Generate Profile For M%d failed"
    return 1000001;
   }
 StringCchCat(szAccelFileNamesDir, _MAX_DIR, szTabProfileFileNameFreqEndings[nIdxToFileNameFreqEndings]);
 if(nEndingWanted == 2)
   StringCchCat(szAccelFileNamesDir,_MAX_DIR,szTabProfileFileNameUstepEndingsPRN[nIdxToFileNameUstepEndings]);
 else if(nEndingWanted == 1)
   StringCchCat(szAccelFileNamesDir,_MAX_DIR,szTabProfileFileNameUstepEndingsCSB[nIdxToFileNameUstepEndings]);
 else
   StringCchCat(szAccelFileNamesDir,_MAX_DIR,szTabProfileFileNameUstepEndingsCSV[nIdxToFileNameUstepEndings]);
 return 0;
}


//This function is called when generating hologram MPOS data. It loads PRN files.
int LoadAndCheckAccelProfilePRNfile(HWND hDlg, char *szAccelFileNamesDir, int idxMnoP1, int *nAccProfile, char chHCFrecNoofUsteps4EachMotorFromTab, int nHCFrecFrequencyOfPixelPrintsInHz){

if(ConstructedFileName(hDlg, szAccelFileNamesDir, idxMnoP1, chHCFrecNoofUsteps4EachMotorFromTab, nHCFrecFrequencyOfPixelPrintsInHz, WANT_PRN_ENDING))
  return 1000001;

 if(myOpen_File(hDlg, "prn", szAccelFileNamesDir, UNUSED_MOTORNO))
   return 1000002;// <-- The function failed
 //Do Checks to help ensure Acceleration Profile loaded from file is OK
 if((nAccProfile[0] < 1)||(nAccProfile[0] > 32767)){
    sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
    MessageBox(hDlg,"@1 The Acceleration Profile must not start with < 1 uStep\nor > 32,767 uSteps",mes,MB_OK);//"G enerateProfileForM11 failed"
    return 1000003; //G enerateProfileForMxx() failed!
   }
 int j;
 for(j = 1; j < NOOFaccelPROFILES; j++)
   if( (nAccProfile[j] < nAccProfile[j-1]) || (nAccProfile[j] > 32767) ){
      sprintf_s(mes,SIZMES,"Generate Profile For M%d failed",idxMnoP1);
      MessageBox(hDlg,"@2 The Acceleration Profile must not contain decreasing speeds\nor numbers > 32,767",mes,MB_OK);//"G enerateProfileForM11 failed"
      sprintf_s(mes,SIZMES,"File\n%s\n\nApprox Line Number %d", szAccelFileNamesDir, j);
      MessageBox(hDlg,mes, "Further Info: for GenerateProfileForMotor failure", MB_OK);
      return 1000004; //G enerateProfileForMxx() failed!
     }
 if(idxMnoP1 == 39) //Do an additional check to ensure only even speeds (2,4,6 etc) are specified in the acceleration profile. By keeping speeds even, the number of steps to move in a pixel is always a whole number
   for(j = 0; j < NOOFaccelPROFILES; j++)
     if( nAccProfile[j] & 0x1 ){
        sprintf_s(mes,SIZMES,"Speeds in the Acceleration Profile for M39 must be even (2,4,6 etc)\n\nFile\n%s\n\nLine Number %d", szAccelFileNamesDir, j+1);
        MessageBox(hDlg,mes, "GenerateProfile For Motor 39 failed", MB_OK);
        return 1000005; //G enerateProfileForM39() failed!
       }

 //I've Doubled the size of the AccProfile table.  The First half contains, for example, 100, 200, 300 (loaded from file).  The second half is loaded here with -100, -200, -300 etc.
 // This avoids multiplying each element (by minus 1) when creating motor acceleration positions for lines where the motor is going backwards. (these lines are given a pointer to the
 // second half of the table instead of the first half)
 int k = 0;
 for(j = NOOFaccelPROFILES; j < SIZaccelPROFILES; j++)
   nAccProfile[j] = nAccProfile[k++] * -1;

 return 0; //<--success
}
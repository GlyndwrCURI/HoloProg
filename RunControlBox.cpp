#include "StdAfx.h"
extern CRegStdWORD   nEnableControlBoxSW;
//nTypeOfRun is either: 1 (PixTrigger Without Parameters... still send Pixel Trigger)  
//                        "ControlBoxWinSoft.exe PixTrig"
//                    : 2 (Parse)
//                        "ControlBoxWinSoft.exe Parse LineLength=50 TotalLines=50 ShutterStartPixel=10 ShutterStopPixel=40 ShutterRedStartLine=2 ShutterRedStopLine=48 ColorOffset=1 LineDelay=500"
//                    : 3 (StartPrint)                                
//                        "ControlBoxWinSoft.exe StartPrint"

//LineLength is LeadIn+LeadOut+NumberOfColumns
//TotalLines is The number of printed lines in the hologram plus 2 times the distance between the beams. (Rem Distance between Red/Gre and Gre/Blu will be the same)
//ShutterStartPixel (The LeadIn minus One)
//ShutterStopPixel (The LeadIn minus One + Number Of Columns)
//ShutterRedStartLine (Should be Zero [perhaps it will be something else when resuming prints?])
//ShutterRedStopLine Should be Number of Rows (even if we are resuming a print)
//ColorOffset Distance between colors in Pixels
//LineDelay is the fixed time added in mSecs after all the pixels have been printed(including the LeadIn and LeadOut) before the Next LineStart is Signalled

void RunControlBoxProgram(HWND hWnd, int nTypeOfRun, int nLineLength, int nTotalLines, int nShutterStartPixel, int nShutterStopPixel, int nShutterRedStartLine, int nShutterRedStopLine, int nColorOffset, int nLineDelay, int bAutoClose){
 char szContBoxEXEfname[256];
 if(!nEnableControlBoxSW)
   return;
 StringCchCopy(szContBoxEXEfname, 256, "ControlBoxWinSoft.exe");
 if( _access(szContBoxEXEfname, 0) == 0){ //Make certain ControlBoxWinSoft.exe exists
    struct _stat64i32 status;         //Path exists, but is it a file?
    _stat(szContBoxEXEfname, &status);
    if((status.st_mode & S_IFDIR)){ //Yes its a file, not a  folder!
       MessageBox(hWnd,"ControlBoxWinSoft.exe is a Folder!","An Essential file was missing.. please rectify this",MB_OK);
       //break;
      }
   }
 else{//The path doesn't exist, so create it.
    MessageBox(hWnd,"Essential file 'ControlBoxWinSoft.exe' was missing!",NULL,MB_OK);
    //break;
   }

 if(nTypeOfRun == PIX_TRIGGER_ONLY)
   StringCchCopy(szContBoxEXEfname, 256, "ControlBoxWinSoft.exe PixTrig");
 else if(nTypeOfRun == PARSE)
   sprintf_s(szContBoxEXEfname,256, "ControlBoxWinSoft.exe Parse LineLength=%d TotalLines=%d ShutterStartPixel=%d ShutterStopPixel=%d ShutterRedStartLine=%d ShutterRedStopLine=%d ColorOffset=%d LineDelay=%d  AutoClose=%d",
    nLineLength, nTotalLines, nShutterStartPixel, nShutterStopPixel, nShutterRedStartLine, nShutterRedStopLine, nColorOffset, nLineDelay, bAutoClose);
 else if(nTypeOfRun == START_PRINT)
   StringCchCopy(szContBoxEXEfname, 256, "ControlBoxWinSoft.exe StartPrint");

 system(szContBoxEXEfname);
}
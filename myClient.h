#ifndef MYCLIENTHDR
#define MYCLIENTHDR

#define HEADERLENGTH 60 //must keep synch'd with PTDOSP.C & CTPROC.C

typedef struct{
 int nTotalFileLenExcludingHdr;  //rem change VERSION below
//This structure contains all of the Small Control Variables that are sent to the SBC.
//Fields above this comment don't need to be stored in persistent storage as the
//program calculates these before sending.

//the following have been defined but are currently hard coded and/or transient - will probably change.
 int nWholeTestMode;    //What do I mean by this? For time being I'll always set TRUE 
//The following are stored in persistent storage (& the ini file).
 int nNoofWavePoints;
 int nFeedbackMode;     //0=No Feedback 1=Forward power only 2=Reflected power only 3=Full Feedback
 int nDelay1;
 int nDelay2;
 int nDelay3;
 int nDelay4;
 int nMinLogs;
 int nMaxLogs;
 int n26Fields[26];
//int nAnotherField; could go here

} SCVsREC; //Small Control Variables I.e. Section One of the WAVEetc.ECF
//DO NOT CHANGE THE ABOVE STRUCTURE WITHOUT AMENDING CTPROC.H
#define  VERSION  9127

//Prototypes:
void crcgen(void);
unsigned int CalcCRC4Table(int nNoofTableChars, void *bufferpointer);
int          CRCcheckFails(int nNoofTableChars, void *bufferpointer);
void readServerConfig(string&);
void checkFileExistence(const string&);
BOOL brecBytesNotaMultipleOf60(int num2Test);

#endif
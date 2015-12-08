#include "stdafx.h"

PIDdebugStruct dbPID[6][6000];//big enough to store 200 seconds
extern int nGlobalLastUSBrecNoRecvd;
//static int nGlobalLastUSBrecNoRecvds = 0;
#if 1
double UpdatePID(SPid * pid, double error, double position) {
 double pTerm;
 double dTerm;
 double iTerm;
// calculate the proportional term
 pTerm = pid->pGain * error;
// calculate the integral state with appropriate limiting
 ///pid->iState += error;

 pid->iState -= pid->Tab[pid->ptr];
 pid->Tab[pid->ptr] = error;
 //pid->iState +=  pid->Tab[pid->ptr];
 pid->iState +=  error;
 if (++pid->ptr > 15)
   pid->ptr = 0;
/*if (pid->iState > pid->iMax)
    pid->iState = pid->iMax;
  else if (pid->iState < pid->iMin)
    pid->iState = pid->iMin;           */
                           // calculate the integral term
 iTerm = pid->iGain * pid->iState;
 dTerm = pid->dGain * (position - pid->dState);
 pid->dState = position;
 if(nGlobalLastUSBrecNoRecvd < 6000){
    dbPID[pid->pidID][nGlobalLastUSBrecNoRecvd].errorRead = error;
    dbPID[pid->pidID][nGlobalLastUSBrecNoRecvd].pTerm = pTerm;
    dbPID[pid->pidID][nGlobalLastUSBrecNoRecvd].iTerm = iTerm;
    dbPID[pid->pidID][nGlobalLastUSBrecNoRecvd].dTerm = dTerm;
    dbPID[pid->pidID][nGlobalLastUSBrecNoRecvd].PIDoutput = pTerm + iTerm - dTerm;
   }
 return pTerm + iTerm - dTerm;
}
#else
double UpdatePID(SPid * pid, double error, double position) {
 double pTerm;
 double dTerm;
 double iTerm;
// calculate the proportional term
 pTerm = pid->pGain * error;
// calculate the integral state with appropriate limiting
 pid->iState += error;
 if (pid->iState > pid->iMax)
   pid->iState = pid->iMax;
 else if (pid->iState < pid->iMin)
   pid->iState = pid->iMin;
// calculate the integral term
 iTerm = pid->iGain * pid->iState;
 dTerm = pid->dGain * (position - pid->dState);
 pid->dState = position;
 if(nGlobalLastUSBrecNoRecvd < 6000){
    dbPID[nGlobalLastUSBrecNoRecvd].errorRead = error;
    dbPID[nGlobalLastUSBrecNoRecvd].pTerm = pTerm;
    dbPID[nGlobalLastUSBrecNoRecvd].iTerm = iTerm;
    dbPID[nGlobalLastUSBrecNoRecvd].dTerm = dTerm;
    dbPID[nGlobalLastUSBrecNoRecvd].PIDoutput = pTerm + iTerm - dTerm;
   }
 return pTerm + iTerm - dTerm;
}
#endif

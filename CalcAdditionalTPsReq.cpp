#include "stdafx.h"
/*
ADDITIONAL_TPinfo_STRUCT RequiredAdditionalTPinfo; //If the motor has to move further than necessary (because an adjacent line needs to start in a different position), this structure returns information about
// how many additional TimePeriods are required, if this is an odd or even number of periods, and how much of the maximum period's time slot is used up. For example, if nnFoundMinNoofElements is 5 AND AccProfile is
// 100, 200, 300, 400, 500 etc AND ConstSpeed Pixels is 588 AND the adjacent line requires an additional 3 steps of the motor movement (or indeed any thing up to 600 steps) an additional TimePeriod will be allocated.
//In the case of a 3 steps, nNoofAdditionalTPsRequired will be 1; bIsAnOddNumber will be 1; nAmountOfMaxBarRemaining will be 3 (steps) 
//In the case of a 609 steps, nNoofAdditionalTPsRequired will be 2; bIsAnOddNumber will be 0; nAmountOfMaxBarRemaining will be 9 (steps) 
*/

//Note I have now embedded the ADDITIONAL_TPinfo_STRUCT fields into RECORD_AccelSFS.
// C alcAddnalTPsReq(ADDITIONAL_TPinfo_STRUCT *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements){
int CalcAddnalTPsReq(RECORD_AccelSFS *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements){
 int nDistanceGained = 0;
 int nAddTP = 0;
 int nRemainingElementsInAccProfile = NOOFaccelPROFILES - nnFoundMinNoofElements ; //nnFoundMinNoofElements could be zero. The first additional TP may produce a movement that is faster than the Constant Speed
 for(int nRE = 0; nRE < nRemainingElementsInAccProfile; nRE++){
    nDistanceGained += nAccProfile[nRE + nnFoundMinNoofElements];
    nAddTP++; //Note, if this function succeeds, nNoofAdditionalTPsRequired will always be >= 1. We can therefore test this to decide if an extended acceleration is required.
    if(nDistanceGained >= nAdditionalDistanceArg){
       ReturnedTPinfo->nNoofAdditionalTPsRequired = nAddTP;
       ReturnedTPinfo->bIsAnOddNumber = 1;                //nAddTP is Odd
       //How much of the Maximum bar is used? It is the total distance we require (nAdditionalDistanceArg) less the previous sum of the distances:
       ReturnedTPinfo->nAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
       return 0;
      }
    nDistanceGained += nAccProfile[nRE + nnFoundMinNoofElements];
    nAddTP++;
    if(nDistanceGained >= nAdditionalDistanceArg){
       ReturnedTPinfo->nNoofAdditionalTPsRequired = nAddTP;
       ReturnedTPinfo->bIsAnOddNumber = 0;                //nAddTP is Even
       ReturnedTPinfo->nAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
       return 0;
      }
   }
 return 1; //error
}
//This is basically a copy of the above function, but it sets variables relating to the end of the line, to help calculate Extended Deceleration profiles
int CalcAddnalEndTPsReq(RECORD_AccelSFS *ReturnedTPinfo, int *nAccProfile, int nAdditionalDistanceArg, int nnFoundMinNoofElements){
 int nDistanceGained = 0;
 int nAddTP = 0;
 int nRemainingElementsInAccProfile = NOOFaccelPROFILES - nnFoundMinNoofElements ; //nnFoundMinNoofElements could be zero. The first additional TP may produce a movement that is faster than the Constant Speed
 for(int nRE = 0; nRE < nRemainingElementsInAccProfile; nRE++){
    nDistanceGained += nAccProfile[nRE + nnFoundMinNoofElements];
    nAddTP++; //Note, if this function succeeds, nNoofAdditionalTPsRequired will always be >= 1. We can therefore test this to decide if an extended acceleration is required.
    if(nDistanceGained >= nAdditionalDistanceArg){
#if 0
       if(nDistanceGained > nAdditionalDistanceArg){
          ReturnedTPinfo->nEndNoofAdditionalTPsRequired = nAddTP;
          ReturnedTPinfo->bEndIsAnOddNumber = 1;             //nAddTP is Odd
          //How much of the Maximum bar is used? It is the total distance we require (nAdditionalDistanceArg) less the previous sum of the distances:
          ReturnedTPinfo->nEndAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
         }
       else if(nRE){ //it's equal 
          ReturnedTPinfo->nEndNoofAdditionalTPsRequired = nAddTP - 1;
          ReturnedTPinfo->bEndIsAnOddNumber = 0;             //nAddTP is Even!!
          //How much of the Maximum bar is used? It is the total distance we require (nAdditionalDistanceArg) less the previous sum of the distances:
          ReturnedTPinfo->nEndAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements] - nAccProfile[nRE + nnFoundMinNoofElements - 1]);
         }
       else{
          ReturnedTPinfo->nEndNoofAdditionalTPsRequired = nAddTP;
          ReturnedTPinfo->bEndIsAnOddNumber = 1;             //nAddTP is Odd
          //How much of the Maximum bar is used? It is the total distance we require (nAdditionalDistanceArg) less the previous sum of the distances:
          ReturnedTPinfo->nEndAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
         }
       return 0;
#else
       ReturnedTPinfo->nEndNoofAdditionalTPsRequired = nAddTP;
       ReturnedTPinfo->bEndIsAnOddNumber = 1;             //nAddTP is Odd
       //How much of the Maximum bar is used? It is the total distance we require (nAdditionalDistanceArg) less the previous sum of the distances:
       ReturnedTPinfo->nEndAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
       return 0;
#endif
      }
    nDistanceGained += nAccProfile[nRE + nnFoundMinNoofElements];
    nAddTP++;
    if(nDistanceGained >= nAdditionalDistanceArg){
       ReturnedTPinfo->nEndNoofAdditionalTPsRequired = nAddTP;
       ReturnedTPinfo->bEndIsAnOddNumber = 0;             //nAddTP is Even
       ReturnedTPinfo->nEndAmountOfMaxBarRemaining = nAdditionalDistanceArg - (nDistanceGained - nAccProfile[nRE + nnFoundMinNoofElements]);
       return 0;
      }
   }
 return 1; //error
}
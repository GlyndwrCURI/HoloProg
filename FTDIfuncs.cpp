#include "stdafx.h"

extern HMODULE m_hmodule;
extern DLLFuncs     DLLf;

//****************************************************************************************
int LoadDLL(HWND hWnd){
 m_hmodule = LoadLibrary("Ftd2xx.dll");  
 if(m_hmodule == NULL)
#ifdef B_ALLOW_USBCOMMS_ERRORS
//   return 0;
     return MessageBox(hWnd,"You will be unable to take readings from the USB hardware.\n\nThis version of the program allows you to continue despite this.","Comms error... library (Ftd2xx.dll) not found", MB_OK);
#else
     return MessageBox(hWnd,"Error: Can't Load Ftd2xx.dll",NULL, MB_OK);
#endif
 DLLf.m_pSetBaudRate = (PtrToSetBaudRate)GetProcAddress(m_hmodule, "FT_SetBaudRate");
 if (DLLf.m_pSetBaudRate == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_SetBaudRate",NULL, MB_OK);
 DLLf.m_pSetFlowControl = (PtrToSetFlowControl)GetProcAddress(m_hmodule, "FT_SetFlowControl");
 if (DLLf.m_pSetFlowControl == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_SetFlowControl",NULL, MB_OK);
 DLLf.m_pSetDataCharacteristics = (PtrToSetDataCharacteristics)GetProcAddress(m_hmodule, "FT_SetDataCharacteristics");
 if (DLLf.m_pSetDataCharacteristics == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_SetDataCharacteristics",NULL, MB_OK);
 DLLf.m_pWrite = (PtrToWrite)GetProcAddress(m_hmodule, "FT_Write");
 if (DLLf.m_pWrite == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_Write",NULL, MB_OK);
 DLLf.m_pRead = (PtrToRead)GetProcAddress(m_hmodule, "FT_Read");
 if (DLLf.m_pRead == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_Read",NULL, MB_OK);
 DLLf.m_pOpen = (PtrToOpen)GetProcAddress(m_hmodule, "FT_Open");
 if (DLLf.m_pOpen == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_Open",NULL, MB_OK);
 DLLf.m_pOpenEx = (PtrToOpenEx)GetProcAddress(m_hmodule, "FT_OpenEx");
 if (DLLf.m_pOpenEx == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_OpenEx",NULL, MB_OK);
 DLLf.m_pListDevices = (PtrToListDevices)GetProcAddress(m_hmodule, "FT_ListDevices");
 if(DLLf.m_pListDevices == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_ListDevices",NULL, MB_OK);
 DLLf.m_pClose = (PtrToClose)GetProcAddress(m_hmodule, "FT_Close");
 if (DLLf.m_pClose == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_Close",NULL, MB_OK);
 DLLf.m_pResetDevice = (PtrToResetDevice)GetProcAddress(m_hmodule, "FT_ResetDevice");
 if (DLLf.m_pResetDevice == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_ResetDevice",NULL, MB_OK);
 DLLf.m_pPurge = (PtrToPurge)GetProcAddress(m_hmodule, "FT_Purge");
 if (DLLf.m_pPurge == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_Purge",NULL, MB_OK);
 DLLf.m_pSetTimeouts = (PtrToSetTimeouts)GetProcAddress(m_hmodule, "FT_SetTimeouts");
 if (DLLf.m_pSetTimeouts == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_SetTimeouts",NULL, MB_OK);
 DLLf.m_pGetQueueStatus = (PtrToGetQueueStatus)GetProcAddress(m_hmodule, "FT_GetQueueStatus");
 if (DLLf.m_pGetQueueStatus == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_GetQueueStatus",NULL, MB_OK);
 DLLf.m_pStopInTask  = (PtrToStopInTask)GetProcAddress(m_hmodule, "FT_StopInTask");
 if (DLLf.m_pStopInTask == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_StopInTask",NULL, MB_OK);
 DLLf.m_pRestartInTask = (PtrToRestartInTask)GetProcAddress(m_hmodule, "FT_RestartInTask");
 if (DLLf.m_pRestartInTask == NULL)
     return MessageBox(hWnd,"Error: Can't Find FT_RestartInTask",NULL, MB_OK);
 return 0;
}   
//****************************************************************************************

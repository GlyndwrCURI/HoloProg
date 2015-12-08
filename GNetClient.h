#pragma once

#define GNETCLIENT_API __declspec(dllexport)

GNETCLIENT_API BOOL GNETCLIENT_Connect(CString sAddress, int iPort, int iAddressFamily = 2);
GNETCLIENT_API void GNETCLIENT_Disconnect();
GNETCLIENT_API void GNETCLIENT_SendMessage(CString sMessage);
GNETCLIENT_API void GNETCLIENT_SetMessageCallback(LPVOID pNotifyProc, LPVOID  pNotifyProcClass);

//mj added:
typedef GNETCLIENT_API BOOL (*GNETCLIENT_ConnectPROC)(CString, int, int);
typedef GNETCLIENT_API void (*GNETCLIENT_DisconnectPROC)(void);
typedef GNETCLIENT_API void (*GNETCLIENT_SendMessagePROC)(CString);
typedef GNETCLIENT_API void (*GNETCLIENT_SetMessageCallbackPROC)(LPVOID, LPVOID);


//#include "stdafx.h"
//#define _WIN32_DCOM
////#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscorlib.tlb" raw_interfaces_only auto_rename
//#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscorlib.tlb" raw_interfaces_only rename("value","nnnvalue")
//#import "C:\MyProjects\HoloProg\ControlBoxWinSoft\ControlBoxWinSoft\ControlBoxWinSoft.tlb" raw_interfaces_only
//using namespace ControlBoxWinSoft;
//void CallUnManagedModule(void){
// long lResult = 6;
// ImyDotNetInterfacePtr ipProg(__uuidof(Program));
// ipProg->myInitialize(lResult,&lResult); //This Unmanaged call does return a changed lResult. If lResult was 5 it's changed to 8, if lResult was 6 it's changed to 4.
//}                                        //This shows the argument is being read by the C# program, and that C# can return a result.



#if 0
//#include "stdafx.h"
//#include <windows.h>
//#include <atlbase.h>
//#include <atlcom.h>
#if 1
//#define _WIN32_DCOM

//using namespace ControlBoxWinSoft;

//See www.csharphelp.com/archives/archive190.html  for instuctions on using TLBEXP to create a type library from an assembly (&, more importantly, why)
//i.e. TLBEXP ControlBoxWinSoft.exe out/:ComServer.lib
//or cd C:\MyProjects\WindowsFormsApplication1\WindowsFormsApplication1\bin\Debug        followed by:
//   TLBEXP ControlBoxWinSoft.dll out/:ComServer.lib

//or cd C:\MyProjects\WindowsFormsApplication1\WindowsFormsApplication1      followed by:  (now in c.bat)
//   TLBEXP ControlBoxWinSoft.dll out/:ControlBoxWinSoft.lib   followed by:
//   gacutil.exe /i ControlBoxWinSoft.dll           to install a strongly named assembly into the Global Assembly Cache
//   regsvr32  ControlBoxWinSoft.dll ???

//But Where's regasm.exe in the above?
//   regasm.exe ControlBoxWinSoft.dll /tlb:ControlBoxWinSoft.tlb             (now in d.bat)  or maybe:
//   regasm.exe ControlBoxWinSoft.dll /tlb:ControlBoxWinSoft.tlb /codebase

//   sn -k StrongNameFile.snk    ... I did something to sign the file, not quite sure what, but came across this later.
//     Now I see... the above line is deprecated

//extern Programm jzy;

//#import "C:\MyProjects\ControlBoxWinSoft\ControlBoxWinSoft\bin\Debug\ControlBoxWinSoft.tlb" raw_interfaces_only
#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscorlib.tlb" raw_interfaces_only auto_rename
#import "C:\MyProjects\HoloProg\ControlBoxWinSoft\ControlBoxWinSoft\ControlBoxWinSoft.tlb" raw_interfaces_only
                  //rename("value","nnvalue")
//#import "C:\MyProjects\ControlBoxWinSoft\ControlBoxWinSoft\ControlBoxWinSoft.tlb" raw_interfaces_only auto_rename
//#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscoree.tlb" raw_interfaces_only

//#import <ControlBoxWinSoft.tlb> raw_interfaces_only
//#import "ControlBoxWinSoft.tlb" raw_interfaces_only

//using namespace ControlBoxWinSoft;
using namespace ControlBoxWinSoft;
//using namespace Form1;
//#using "ControlBoxWinSoft.dll"
//using namespace Interop;
//using namespace ControlBoxWinSoft;
//using "ControlBoxWinSoft.dll"
//using ControlBoxWinSoft;
//using namespace MyLib;

//void CallManagedModule(HWND hWnd){
void CallUnManagedModule(void){
  char szHres[46];
  HRESULT hr;

  //LPCOLESTR szProgID;
  //REFCLSID  rclsid = {};
  //LPUNKNOWN pUnkOuter = NULL;
  //REFIID    riid;//"MyProg";
  
  //DWORD     dwClsContext;
  //LPVOID    pv;
  //REFIID IID_IDotNetInterface = uuid("9a470259-f4bd-3a5b-a676-909da06d06f5");//&IDotNetInterface;// = {3e0913b6-3733-44df-a92b-6b9be9876177};
  //IDotNetInterface *pIDotNet; //3e0913b6-3733-44df-a92b-6b9be9876177

//DialogBox(hInst, MAKEINTRESOURCE(IDD_GEOLASHOWDIALOGBOX), hWnd, JobsOnDisplayPC_Dialog);
  //MessageBox (NULL,"CallManagedModule","CallManagedModule",MB_OK);
//CComPtr srpDotNet;
//CComPtr<DotNetClass> srpDotNet;
//CComPtr<ControlBoxWinSoft> srpDotNet;

  //char szHres[46];
    /*
  HRESULT hrCoI = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); //Now CLRTHREADATTIBUTE is set in Linker options / Advanced CoInitializeEx is done before program starts. Maybe change back is COM stuff is removed.
                                                                  //Does this Single Threaded model apply only to the Managed CLR code?  Is the main multithreading unaffected by this.
  if(hrCoI == S_OK)
    // sprintf_s(szHres,46,"Hresult 0x%x",hrCoI);
    // MessageBox (NULL,szHres,"CoInitialize S_OK",MB_OK);
    MessageBox (NULL,"CoInitialize S_OK","Hi",MB_OK);
  else if(hrCoI == S_FALSE){                                  
     MessageBox (NULL,"CoInitialize S_FALSE  ;-(","Hi",MB_OK);    //This now always happens... The COM library is already initialized on this thread.
     CoUninitialize();
     return;
    }
  else if(hrCoI == RPC_E_CHANGED_MODE){
     MessageBox (NULL,"CoInitialize RPC_E_CHANGED_MODE  ;-(","Hi",MB_OK);
     CoUninitialize();
     return;
    }
  else {
     MessageBox (NULL,"CoInitialize unexpected value  ;-(","Hi",MB_OK);
     CoUninitialize();
     return;
    } //*/
//CComPtr<IDotNetInterface> srpDotNet;

//srpDotNet.CoCreateInstance(CLSID_DotNetClass, CLSCTX_ALL);
//srpDotNet.CoCreateInstance(3e0913b6-3733-44df-a92b-6b9be9876177, CLSCTX_ALL);
//srpDotNet.CoCreateInstance(CLSID_DotNetClass, CLSCTX_ALL);
//HRESULT hr = srpDotNet.CoCreateInstance(ControlBoxWinSoft, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IClassFactory, (void**)&pIDotNet);
//HRESULT hr = srpDotNet.CoCreateInstance(CLSID_DotNetClass, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IDotNetInterface, (void**)&pIDotNet);
//HRESULT hr = srpDotNet.CoCreateInstance(szProgID, pUnkOuter, CLSCTX_INPROC_SERVER, IID_IClassFactory, (void**)&pIDotNet);
//HRESULT hr = srpDotNet.CoCreateInstance(CLSID_DotNetClass, pUnkOuter, CLSCTX_INPROC_SERVER, IDotNetInterface, (void**)&pIDotNet);
  
//srpDotNet.CreateInstance(CLSID_DotNetClass, CLSCTX_ALL);
//srpDotNet.CoCreateInstance(CLSID_DotNetClass, CLSCTX_ALL);
//srpDotNet.CoCreateInstance(szProgID, CLSCTX_ALL);
//HRESULT hr = srpDotNet.CoCreateInstance(riid, pUnkOuter, CLSCTX_ALL);


//srpDotNet.Initialize();
  //if(SUCCEEDED(hr)){
    //    MessageBox (NULL,"CallManagedModule","SUCCEEDED",MB_OK);

  //DotNetClass dnc;
   //_DotNetClass ptrrr;
   //_Form1 ptrrr;
   //ptrrr->Initialize();
/*   _COM_SMARTPTR_TYPEDEF(IDotNetInterface, __uuidof(IDotNetInterface));
IDotNetInterface pp;
_COM_SMARTPTR_TYPEDEF(IReadOnlyProfile, __uuidof(IReadOnlyProfile));
IReadOnlyProfile rg;
_COM_SMARTPTR_TYPEDEF(_SerialRecordEventArgs, __uuidof(_SerialRecordEventArgs));
_SerialRecordEventArgs k; */
//_COM_SMARTPTR_TYPEDEF(_DotNetClass, __uuidof(_DotNetClass));
//DotNetClass hgh;
//IDotNetInterface pApp; //error C2259: 'ControlBoxWinSoft::IDotNetInterface' : cannot instantiate abstract class
//IReadOnlyProfile pApp; //error C2259: 'ControlBoxWinSoft::IReadOnlyProfile' : cannot instantiate abstract class
//Program pApp;          //error C2065: 'Program' : undeclared identifier
//HRESULT hr = pApp.CreateInstance("ControlBoxWinSoft");

//_COM_SMARTPTR_TYPEDEF(_Form1, __uuidof(_Form1));
//_Form1 ggfgfg;
  //_Program aaa;

 // extern "C" const GUID __declspec()



//_Form1Ptr ipForm;
//hr = ipForm.CreateInstance(__uuidof(_Form1));
//if(FAILED(hr)){    
//   sprintf_s(szHres,46,"Hresult 0x%x",hr);
//   MessageBox (NULL,szHres,"CreateInstance(__uuidof(_Form1)",MB_OK);
//// return;
//  }
//
//
//IDotNetInterfacePtr ipDNI;
//hr = ipDNI.CreateInstance(__uuidof(IDotNetInterface));
//if(FAILED(hr)){    
//   sprintf_s(szHres,46,"Hresult 0x%x",hr);
//   MessageBox (NULL,szHres,"CreateInstance(__uuidof(IDN)",MB_OK);
//// return;
//  }

//_rProgramPtr ipProg;
//hr = ipProg.CreateInstance(__uuidof(_rProgram));
//if(FAILED(hr)){
//   sprintf_s(szHres,46,"Hresult 0x%x",hr);
//   MessageBox (NULL,szHres,"CreateInstance(__uuidof(Progy)",MB_OK);
//// return;
//  }

//_rrppProgramPtr ipProgP(__uuidof(_Program));
//long lResult; //dummy
//lResult = 1;
//ipProgP->myInitialize(&lResult);
//lResult *= 2;

ImyDotNetInterfacePtr ipProg(__uuidof(Program));
long lResult;    //dummy
lResult = 1;
//ipProg->myInitialize();
ipProg->myInitialize(&lResult); //  <= correct
lResult *= 2;

//IDotNetInterfacePtr ipProg(__uuidof(IDotNetInterface));
//long lResult; //dummy
//lResult = 1;
//ipProg->Initialize();
//lResult *= 2;

   //CoUninitialize(); now in WM_DESTROY
  }
  //jzy.Main();
#else

//#import "C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727\mscorlib.tlb" raw_interfaces_only auto_rename
#import "C:\MyProjects\sManagedDLL\sManagedDLL\bin\debug\sManagedDLL.tlb" raw_interfaces_only
using namespace sManagedDLL;
void CallUnManagedModule(void){
  char szHres[46];
  HRESULT hr;

  IcalcPtr ipProgP(__uuidof(Icalc));
  long lResult; //dummy
  lResult = 1;
  ipProgP->myInitialize(&lResult);
  lResult *= 2;

  ImyDotNetInterfacePtr ipProg(__uuidof(ImyDotNetInterface));
  //long lResult; //dummy
  lResult = 1;
  ipProg->myInitialize(&lResult);
  lResult *= 2;

}
#endif
#endif
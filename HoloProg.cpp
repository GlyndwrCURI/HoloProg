#include "stdafx.h"

#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;                                //Current instance
TCHAR szTitle[MAX_LOADSTRING];                  //The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            //The main window class name
HMENU hMenuMain;

char  mes[SIZMES];

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
 UNREFERENCED_PARAMETER(hPrevInstance);
 UNREFERENCED_PARAMETER(lpCmdLine);
 MSG msg;
 HACCEL hAccelTable;
 LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);	    // Initialize global strings
 LoadString(hInstance, IDC_HOLOPROG, szWindowClass, MAX_LOADSTRING);
 if( FindWindow(szWindowClass, NULL) ){
//  if(IDCANCEL == MessageBox(NULL,"The copy you are trying to start will terminate.\n\nThe existing program will continue to run.\n\n Perhaps you're just browing a Folder called Holoprog?","You Cannot Run Two Copies of HoloProg at the same time.",MB_OKCANCEL))
//    MessageBox(NULL,"This is probably because the HoloProg program is already running,\nbut it could be because you are, for example, browsing a\nfolder called HoloProg (which you must close).","A window called 'HoloProg' is already open.",MB_OK);
//I think the above is appropriate when using FindWindow() to find a Window by lpWindowName, but unnecessary when finding a Window by lpClassName: 
    MessageBox(NULL,"The copy you are trying to start will terminate.\n\n","You Cannot Run Two Copies of HoloProg at the same time.",MB_OK|MB_ICONSTOP);
    exit(1);
   }

 MyRegisterClass(hInstance);

 if (!InitInstance (hInstance, nCmdShow))                           // Perform application initialization:
   return FALSE;

 hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HOLOPROG));
 //AfxInitRichEdit2( );
 //AfxInitRichEdit();
 HMODULE reLib = LoadLibrary("riched20.dll");
 if (reLib == NULL)
   MessageBox(NULL,"Failed to load RichEd32.dll",NULL,MB_ICONEXCLAMATION);

 while (GetMessage(&msg, NULL, 0, 0))                               // Main message loop:
   if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     }
 DestroyMenu(hMenuMain);
 if (reLib != NULL)
   FreeLibrary(reLib);
 return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance){

 hMenuMain = LoadMenu(hInst,MAKEINTRESOURCE(IDC_HOLOPROG));

 WNDCLASSEX wcex;
 wcex.cbSize = sizeof(WNDCLASSEX);
 wcex.style         = CS_HREDRAW | CS_VREDRAW;
 wcex.lpfnWndProc   = WndProc;
 wcex.cbClsExtra    = 0;
 wcex.cbWndExtra    = 0;
 wcex.hInstance     = hInstance;
 wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HOLOPROG));
 wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
 //wcex.hbrBackground   = (HBRUSH)(CreateSolidBrush(RGB(0,230,230)));
 //wcex.hbrBackground   = (HBRUSH)(CreateSolidBrush(RGB(150,150,150)));
 //wcex.hbrBackground   = (HBRUSH)(CreateSolidBrush(RGB(190,190,190)));
 //wcex.hbrBackground   = (HBRUSH)(CreateSolidBrush(RGB(210,210,210)));
 //wcex.hbrBackground   = (HBRUSH)(CreateSolidBrush(RGB(205,205,205)));
 wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(215,215,215)));
 wcex.lpszMenuName  = MAKEINTRESOURCE(IDC_HOLOPROG);
 wcex.lpszClassName = szWindowClass;
 wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
 //wcex.hIconSm     = NULL;
 return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
 HWND hWnd;
 hInst = hInstance; // Store instance handle in our global variable
 hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                     144,0,1536,1160, NULL, NULL, hInstance, NULL);//144,0,1536,1017  //0, 70, MAINWINDOW_X_SIZE, MAINWINDOW_Y_SIZE
 if (!hWnd)
    return FALSE;
 ShowWindow(hWnd, nCmdShow);
 UpdateWindow(hWnd);
 return TRUE;
}

void myLoadString(UINT STR_x){
 if ( LoadString(hInst,STR_x,mes,SIZMES) == 0)
    MessageBox (NULL,"Error in myLoadString",NULL,MB_ICONEXCLAMATION|MB_OK);
}
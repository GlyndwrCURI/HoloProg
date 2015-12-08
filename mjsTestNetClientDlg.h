// mjsTestNetClientDlg.h : header file
//
#include "stdafx.h"
#include "mySocket.h"

#pragma once

//#include "CommunicationData.h"
//#include "afxwin.h"
//#include "afxcmn.h"

class CTestNetClientDlg //: public CDialog
{
// Construction
public:
	//CTestNetClientDlg(CWnd* pParent = NULL);	// standard constructor
	//CTestNetClientDlg();	// standard constructor

  //void Setup(GNETCLIENT_SetMessageCallbackPROC lpGNETCLIENT_SetMessageCallback){(lpGNETCLIENT_SetMessageCallback)(MessageCallbackNotify, this);};
// Dialog Data
	//enum { IDD = IDD_TESTNETCLIENT_DIALOG };
	JobListStruct m_JobList[100];
	int m_iTotalJobs;

    CString m_sStatus;
    CString sReturnStatus(void){return this->m_sStatus;};
	static void CALLBACK MessageCallbackNotify(string sMessage, int iMsgSize);
	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

//	CGeolaNetClient m_Client;
  //static void CALLBACK MessageCallbackNotify(LPVOID lpParam, CString sMessage, int iMsgSize);
  //static void CALLBACK MessageCallbackNotify(CString sMessage, int iMsgSize);
	int m_iJobsCounter;


// Implementation
protected:
	//HICON m_hIcon;

	// Generated message map functions
	//virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	//DECLARE_MESSAGE_MAP()
public:
    
	//afx_msg void OnBnClickedButtonConnect();
	//afx_msg void OnBnClickedButtonGetStatus();
	//CListBox m_List;
	//CStatic m_DataField;
	//afx_msg void OnLbnSelchangeList1();
	//afx_msg void OnBnClickedButtonGetJobsList();
	//CEdit m_Status;
	//CIPAddressCtrl m_ServerIPCtrl;
	//afx_msg void OnBnClickedButtonStartPrinting();
	//afx_msg void OnBnClickedButtonStopPrinting();
	//CComboBox m_RepRate;
	//afx_msg void OnBnClickedButtonSetRepRate();
	//afx_msg void OnBnClickedButtonGetRepRate();
	//afx_msg void OnDestroy();
    
};

#pragma once
#include "userinfoshow.h"
#include "Switcher.h"
#include "SelectMac.h"


// CUserAuthDlg 对话框

class CUserAuthDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserAuthDlg)
private:
	CSwitcher *session;
	string room;
	CSelectMac *dlg;
	int selLine;
	int port;

	vector<user_info> v;
	vector<vector<DWORD>> pay_info;
	vector<IpAddress> vIP;
public:
	CUserAuthDlg(CWnd* pParent = NULL,const char* room = NULL,CSwitcher *session = NULL,int port = NULL);   // 标准构造函数
	virtual ~CUserAuthDlg();

// 对话框数据
	enum { IDD = IDD_DLG_USERSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CUserInfoShow m_list;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	int MoveSubWindow(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	virtual BOOL DestroyWindow();
//	virtual void OnCancel();
	virtual BOOL DestroyWindow();
	afx_msg void OnMove(int x, int y);
	int CustomMACDlg(int iSel);
	int RefershUserMAC(int idx,const char *phone, MacAddress & mac,const char *lpUserName = NULL);
};

#pragma once

#include "ListCtrlEx.h"
#include "ListCtrlCellEx.h"
#include "UserAuthDlg.h"
#include "Switcher.h"
#include "BaseInfo.h"

// CSwitcherManager 对话框

class CSwitcherManager : public CDialogEx
{
	DECLARE_DYNAMIC(CSwitcherManager)

public:
	CSwitcherManager(CWnd* pParent = NULL,LPCTSTR ip = NULL,int mgtPort = NULL);   // 标准构造函数
	virtual ~CSwitcherManager();

// 对话框数据
	enum { IDD = IDD_DLG_SWITCHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
private:
	int mgtPort;
	int selLine;
	CUserAuthDlg *dlgAuth;
	IpAddress ip;
	CSwitcher *session;
public:
	virtual BOOL OnInitDialog();
	ListCtrlEx::CListCtrlEx m_list;
//	afx_msg void OnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL globalMAC;
protected:
	afx_msg LRESULT OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClickedCheck1();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnMove(int x, int y);
	int MoveSubWindow(void);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
//	afx_msg void OnBnClickedRadio2();
	int m_safemode;
	afx_msg void OnClickedRadio1();
	afx_msg void OnBnClickedOnekeyconfig();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedReset();
	BOOL m_trap;
	size_t UtilOneKeySet(int setType = NULL);
	int ResetSession(void);
	static size_t DeltaUpdate();
};


// Hoshizora_BetaDlg.h : 头文件
//

#pragma once
#include "BaseInfo.h"
#include "Loading.h"
#include "AdvanceBase.h"
#include "SearchBaseDlg.h"
#include "SwitcherBaseDlg.h"
#include "DeviceMonitor.h"
#include "Settings.h"
#include "afxwin.h"
#include "afxcmn.h"

// CHoshizora_BetaDlg 对话框
class CHoshizora_BetaDlg : public CDialogEx
{
private:
	CSwitcherBaseDlg *page1;
	CLoading page2;
	CSearchBaseDlg *page3;
	CAdvanceBase *page6;
	CLoading page4;
	CDeviceMonitor * page5;
	CSettings *page7;
// 构造
public:
	CHoshizora_BetaDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HOSHIZORA_BETA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedCancel();
	CTabCtrl m_tab;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};

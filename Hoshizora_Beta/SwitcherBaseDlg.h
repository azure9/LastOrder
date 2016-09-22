#pragma once
#include "BaseInfo.h"

// CSwitcherBaseDlg 对话框

class CSwitcherBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSwitcherBaseDlg)
private:
	int mgtPort;
public:
	CSwitcherBaseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSwitcherBaseDlg();

// 对话框数据
	enum { IDD = IDD_BASE_SWITCHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString strIP;
	afx_msg void OnCbnEditchangeRoomlist();
	afx_msg void OnCbnEditchangeIplist();
	afx_msg void OnCbnSelchangeRoomlist();
	CString m_info;
	CString m_port;
	afx_msg void OnCbnSelchangeIplist();
};

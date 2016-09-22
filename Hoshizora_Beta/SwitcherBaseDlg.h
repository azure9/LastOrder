#pragma once
#include "BaseInfo.h"

// CSwitcherBaseDlg �Ի���

class CSwitcherBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSwitcherBaseDlg)
private:
	int mgtPort;
public:
	CSwitcherBaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSwitcherBaseDlg();

// �Ի�������
	enum { IDD = IDD_BASE_SWITCHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

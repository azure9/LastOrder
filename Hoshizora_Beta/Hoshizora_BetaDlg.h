
// Hoshizora_BetaDlg.h : ͷ�ļ�
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

// CHoshizora_BetaDlg �Ի���
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
// ����
public:
	CHoshizora_BetaDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HOSHIZORA_BETA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

#pragma once
#include "afxwin.h"


// CSearchDlg �Ի���

class CSearchBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchBaseDlg)

private:
public:
	CSearchBaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSearchBaseDlg();

// �Ի�������
	enum { IDD = IDD_BASE_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CComboBox m_type;
	virtual BOOL OnInitDialog();
	CString m_keyword;
	afx_msg void OnBnClickedButton2();
//	afx_msg void OnBnClickedOk();
//	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

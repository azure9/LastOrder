#pragma once
#include "afxwin.h"


// CSearchDlg 对话框

class CSearchBaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchBaseDlg)

private:
public:
	CSearchBaseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchBaseDlg();

// 对话框数据
	enum { IDD = IDD_BASE_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

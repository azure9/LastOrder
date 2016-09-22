#pragma once
#include "afxwin.h"


// CAdvanceBase 对话框

class CAdvanceBase : public CDialogEx
{
	DECLARE_DYNAMIC(CAdvanceBase)

private:
	
public:
	CAdvanceBase(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdvanceBase();

// 对话框数据
	enum { IDD = IDD_BASE_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	int m_setType;
};

#pragma once
#include "afxwin.h"


// CEula 对话框

class CEula : public CDialogEx
{
	DECLARE_DYNAMIC(CEula)

public:
	CEula(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEula();

// 对话框数据
	enum { IDD = IDD_EULA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_data;
};

#pragma once


// CLoading 对话框

#include "Resource.h"

class CLoading : public CDialogEx
{
	DECLARE_DYNAMIC(CLoading)

public:
	CLoading(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoading();

// 对话框数据
	enum { IDD = IDD_DEVELOPING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

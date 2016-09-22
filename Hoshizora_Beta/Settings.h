#pragma once
#include "BaseInfo.h"

// CSettings 对话框

class CSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettings();

// 对话框数据
	enum { IDD = IDD_BASE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeScheme();
	virtual BOOL OnInitDialog();
};

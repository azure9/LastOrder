#pragma once
#include "BaseInfo.h"

// CSettings �Ի���

class CSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettings();

// �Ի�������
	enum { IDD = IDD_BASE_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeScheme();
	virtual BOOL OnInitDialog();
};

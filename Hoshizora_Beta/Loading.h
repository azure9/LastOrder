#pragma once


// CLoading �Ի���

#include "Resource.h"

class CLoading : public CDialogEx
{
	DECLARE_DYNAMIC(CLoading)

public:
	CLoading(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoading();

// �Ի�������
	enum { IDD = IDD_DEVELOPING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};

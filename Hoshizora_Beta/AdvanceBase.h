#pragma once
#include "afxwin.h"


// CAdvanceBase �Ի���

class CAdvanceBase : public CDialogEx
{
	DECLARE_DYNAMIC(CAdvanceBase)

private:
	
public:
	CAdvanceBase(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAdvanceBase();

// �Ի�������
	enum { IDD = IDD_BASE_ADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	int m_setType;
};

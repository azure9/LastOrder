#pragma once
#include "afxwin.h"


// CEula �Ի���

class CEula : public CDialogEx
{
	DECLARE_DYNAMIC(CEula)

public:
	CEula(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEula();

// �Ի�������
	enum { IDD = IDD_EULA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_data;
};

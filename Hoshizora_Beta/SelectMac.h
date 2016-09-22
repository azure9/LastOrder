#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include "common.h"

using namespace std;


// CSelectMac �Ի���

class CSelectMac : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectMac)

private:
	int sel;
public:
	CSelectMac(CWnd* pParent = NULL,vector<user_info> *pv = NULL,int sel = 0);   // ��׼���캯��
	virtual ~CSelectMac();

// �Ի�������
	enum { IDD = IDD_SELECT_MAC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	vector<user_info> *pv;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_list;
	virtual void OnCancel();
};

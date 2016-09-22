#pragma once
#include "afxcmn.h"
#include "UserInfoShow.h"


// CSearchDlg �Ի���

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

private:
	vector<user_info> *v;
	vector<vector<DWORD>> *pPayed_info;
	vector<IpAddress> *pIP;
	//�����ʹ��һ�Σ����ߴӲ���������������ָ�롢�����������ʹ�ò�Ҫ��ָ�롢������ʱջ�������������´���
public:
	CSearchDlg(CWnd* pParent = NULL,vector<user_info> *v = NULL,vector<vector<DWORD>> *pPay_info = NULL);   // ��׼���캯��
	CSearchDlg(CWnd* pParent = NULL,vector<IpAddress> *pIP = NULL,vector<user_info> *v = NULL,vector<vector<DWORD>> *pPay_info = NULL);
	virtual ~CSearchDlg();

// �Ի�������
	enum { IDD = IDD_DLG_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	CUserInfoShow m_list;
	virtual BOOL OnInitDialog();
//	afx_msg void OnLvnItemchangedSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnHdnItemdblclickSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam);
};

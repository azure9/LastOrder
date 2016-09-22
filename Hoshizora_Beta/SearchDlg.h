#pragma once
#include "afxcmn.h"
#include "UserInfoShow.h"


// CSearchDlg 对话框

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

private:
	vector<user_info> *v;
	vector<vector<DWORD>> *pPayed_info;
	vector<IpAddress> *pIP;
	//如果仅使用一次，或者从不被析构，可以用指针、如果是子类多次使用不要用指针、否则临时栈变量会析构导致错误
public:
	CSearchDlg(CWnd* pParent = NULL,vector<user_info> *v = NULL,vector<vector<DWORD>> *pPay_info = NULL);   // 标准构造函数
	CSearchDlg(CWnd* pParent = NULL,vector<IpAddress> *pIP = NULL,vector<user_info> *v = NULL,vector<vector<DWORD>> *pPay_info = NULL);
	virtual ~CSearchDlg();

// 对话框数据
	enum { IDD = IDD_DLG_SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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

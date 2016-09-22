// SearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "SearchDlg.h"
#include "SwitcherManager.h"
#include "afxdialogex.h"


// CSearchDlg 对话框

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/,vector<user_info> *v,vector<vector<DWORD>> *pPayed_info)
	: CDialogEx(CSearchDlg::IDD, pParent),
	v(v),pPayed_info(pPayed_info),pIP(nullptr)
{

}

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/,vector<IpAddress> *pIP,vector<user_info> *v,vector<vector<DWORD>> *pPayed_info)
   : CDialogEx(CSearchDlg::IDD, pParent),
   v(v),pPayed_info(pPayed_info),pIP(pIP)
{

}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCH_RESULT, m_list);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CSearchDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_SEARCH_RESULT, &CSearchDlg::OnNMDblclkSearchResult)
	ON_NOTIFY(NM_RCLICK, IDC_SEARCH_RESULT, &CSearchDlg::OnNMRClickSearchResult)
	ON_MESSAGE(WM_ListCtrlEx_CHECK_CHANGED, &CSearchDlg::OnListctrlexCheckChanged)
END_MESSAGE_MAP()


// CSearchDlg 消息处理程序


void CSearchDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	DestroyWindow();
}


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_list.Init();
	if (pIP==nullptr)
	{
		m_list.PrepareUserData(*v,*pPayed_info,NULL,NULL);
	}
	else
	{
		m_list.PrepareUserData(*v,*pPayed_info,*pIP);
	}
	pIP = nullptr;
	v = nullptr;//临时栈变量、初始化完了就释放掉
	pPayed_info = nullptr;
	m_list.RefershGrid();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSearchDlg::OnNMDblclkSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	USES_CONVERSION;
	int port;
	string && ip = bs->GetIPFromDescrS(W2A(m_list.GetItemText(pNMItemActivate->iItem,3)),port);

	CSwitcherManager *dlg;
	try
	{
		dlg = new CSwitcherManager(this,CString(ip.c_str()),port);
		dlg->Create(IDD_DLG_SWITCHER,this);
		dlg->ShowWindow(SW_SHOW);
	}
	catch (...)
	{
		AfxMessageBox(_T("无法管理此台交换机！"));
	}
	
	*pResult = 0;
}

afx_msg LRESULT CSearchDlg::OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam)
{
	return m_list.OnListctrlexCheckChanged(wParam,lParam);
}

void CSearchDlg::OnNMRClickSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_list.OnRclickList1(pNMHDR,pResult);
}
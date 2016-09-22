// SearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "SearchDlg.h"
#include "SwitcherManager.h"
#include "afxdialogex.h"


// CSearchDlg �Ի���

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


// CSearchDlg ��Ϣ�������


void CSearchDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DestroyWindow();
}


BOOL CSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	v = nullptr;//��ʱջ��������ʼ�����˾��ͷŵ�
	pPayed_info = nullptr;
	m_list.RefershGrid();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSearchDlg::OnNMDblclkSearchResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		AfxMessageBox(_T("�޷������̨��������"));
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
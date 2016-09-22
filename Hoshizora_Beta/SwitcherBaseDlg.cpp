// SwitcherBaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "Hoshizora_Beta.h"
#include "SwitcherBaseDlg.h"
#include "afxdialogex.h"
#include "SwitcherManager.h"


// CSwitcherBaseDlg 对话框

IMPLEMENT_DYNAMIC(CSwitcherBaseDlg, CDialogEx)

CSwitcherBaseDlg::CSwitcherBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSwitcherBaseDlg::IDD, pParent),mgtPort(0)
{

}

CSwitcherBaseDlg::~CSwitcherBaseDlg()
{
}

void CSwitcherBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_COMBO1, strIP);
	DDX_Text(pDX, IDC_INFO, m_info);
	DDX_Text(pDX, IDC_PROT, m_port);
}


BEGIN_MESSAGE_MAP(CSwitcherBaseDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSwitcherBaseDlg::OnBnClickedOk)
	ON_CBN_EDITCHANGE(IDC_ROOMLIST, &CSwitcherBaseDlg::OnCbnEditchangeRoomlist)
	ON_CBN_EDITCHANGE(IDC_IPLIST, &CSwitcherBaseDlg::OnCbnEditchangeIplist)
	ON_CBN_SELCHANGE(IDC_ROOMLIST, &CSwitcherBaseDlg::OnCbnSelchangeRoomlist)
	ON_CBN_SELCHANGE(IDC_IPLIST, &CSwitcherBaseDlg::OnCbnSelchangeIplist)
END_MESSAGE_MAP()


// CSwitcherBaseDlg 消息处理程序

void CSwitcherBaseDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (strIP.IsEmpty())
	{
		MessageBox(_T("被管交换机IP不能为空！"),_T("警告"),MB_ICONWARNING);
		return;
	}
	CSwitcherManager* dlg = nullptr;
	try
	{
		dlg = new CSwitcherManager(this,static_cast<LPCTSTR>(strIP),mgtPort);
		dlg->Create(IDD_DLG_SWITCHER,NULL);
		dlg->ShowWindow(SW_SHOW);
	}
	catch (const WCHAR *msg)
	{
		AfxMessageBox(msg);
		delete dlg;
	}
	catch (...)
	{
		AfxMessageBox(L"通信失败！请稍后再试！");
		delete dlg;
	}
}


void CSwitcherBaseDlg::OnCbnEditchangeRoomlist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString roominfo;
	USES_CONVERSION;
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_ROOMLIST));
	pBox->GetWindowText(roominfo);
	string && ip = bs->GetIPFromDescrS(W2A(roominfo),mgtPort);
	if (ip.empty())
	{
		vector<string> &&v = bs->RoomImagination(W2A(roominfo));
		for (int i = pBox->GetCount() - 1; i >= 0; i--)
		{
			pBox->DeleteString(i);
		}
		for_each(v.begin(),v.end(),[&](string & x)
		{
			pBox->AddString(A2W(x.c_str()));
		});
		::SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW))); 
		pBox->ShowDropDown(TRUE);//显示下拉框结果
		pBox = static_cast<CComboBox *>(GetDlgItem(IDC_IPLIST));
		pBox->EnableWindow(true);
		strIP=L"";
		m_port=L"";
		m_info=L"IP地址无效";
		//SetDlgItemText(IDC_SWITCHERTYPE,L"");
		UpdateData(false);
	}
	else
	{
		pBox = static_cast<CComboBox *>(GetDlgItem(IDC_IPLIST));
		pBox->EnableWindow(false);
		m_info = A2W(bs->GetLocationInfoS(ip).c_str());
		strIP = A2W(ip.c_str());
		//SetDlgItemText(IDC_SWITCHERTYPE,A2W(bs->GetType(ip).c_str()));
		m_port = stringf(_T("端口：%d"),mgtPort).c_str();
		UpdateData(false);
	}
}


void CSwitcherBaseDlg::OnCbnEditchangeIplist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ip;
	USES_CONVERSION;
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_IPLIST));
	pBox->GetWindowText(ip);
	vector<string> &&v = bs->IPImaginationS(string(W2A(ip)));
	for (int i = pBox->GetCount() - 1; i >= 0; i--)
	{
		pBox->DeleteString(i);
	}
	for_each(v.begin(),v.end(),[&](string & x)
	{
		pBox->AddString(CString(x.c_str()));
	});
	//	pBox->ShowDropDown(FALSE);//隐藏下拉框结果，为了刷新界面
	m_info = L"";
	GetDlgItem(IDC_INFO)->SetWindowText(L"");
	SetDlgItemText(IDC_SWITCHERTYPE,L"");
	::SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW))); 
	pBox->ShowDropDown(TRUE);//显示下拉框结果
}

void CSwitcherBaseDlg::OnCbnSelchangeRoomlist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString roominfo;
	USES_CONVERSION;
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_ROOMLIST));
	pBox->GetLBText(pBox->GetCurSel(),roominfo);
	string && ip = bs->GetIPFromDescrS(W2A(roominfo),mgtPort);
	if (ip.empty())
	{
	}
	else
	{
		pBox = static_cast<CComboBox *>(GetDlgItem(IDC_IPLIST));
		pBox->EnableWindow(false);
		m_info = CString(bs->GetLocationInfoS(ip).c_str());
		strIP = CString(ip.c_str());
		int scheme = bs->GetDeviceScheme(ip);
		CString strScheme;
		if (scheme!=-1)
		{
			strScheme = bs->LoadScheme(scheme).name.c_str();
		}
		SetDlgItemText(IDC_SWITCHERTYPE,strScheme);
		m_port = stringf(_T("端口：%d"),mgtPort).c_str();
		UpdateData(false);
	}
}


void CSwitcherBaseDlg::OnCbnSelchangeIplist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ip;
	USES_CONVERSION;
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_IPLIST));
	pBox->GetLBText(pBox->GetCurSel(),ip);

	string tIP(W2A(ip));
	string && descr = (bs->GetLocationInfoS(tIP).c_str());
	if (descr.empty())
	{
		m_info = L"无法找到该交换机信息";
	}
	else
	{
		m_info = A2W(descr.c_str());
		mgtPort = 0;
		int scheme = bs->GetDeviceScheme(tIP);
		CString strScheme;
		if (scheme!=-1)
		{
			strScheme = bs->LoadScheme(scheme).name.c_str();
		}
		SetDlgItemText(IDC_SWITCHERTYPE,strScheme);
	}
	strIP = ip;
	UpdateData(false);
}

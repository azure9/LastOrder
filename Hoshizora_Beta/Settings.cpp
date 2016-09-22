// Settings.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Settings.h"
#include "afxdialogex.h"


// CSettings 对话框

IMPLEMENT_DYNAMIC(CSettings, CDialogEx)

CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettings::IDD, pParent)
{

}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettings, CDialogEx)
	ON_CBN_SELCHANGE(IDC_SCHEME, &CSettings::OnCbnSelchangeScheme)
END_MESSAGE_MAP()


// CSettings 消息处理程序


void CSettings::OnCbnSelchangeScheme()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_SCHEME));

	int idx = pBox->GetCurSel()-1;

	bs->SetScheme(idx);
	AfxGetApp()->WriteProfileInt(_T("LastOrder"),_T("Scheme"),idx);

	MessageBox(_T("管理方案已改变"),_T("提示"),MB_ICONINFORMATION);

}


BOOL CSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化

	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_SCHEME));
	pBox->AddString(_T("全部方案"));
	
	auto && rsl = bs->LoadScheme();

	for (size_t idx = 0;idx<rsl.size();++idx)
	{
		pBox->AddString(CString(rsl[idx].name.c_str()));
	}

	pBox->SetCurSel(bs->GetScheme()+1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

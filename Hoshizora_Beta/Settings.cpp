// Settings.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Settings.h"
#include "afxdialogex.h"


// CSettings �Ի���

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


// CSettings ��Ϣ�������


void CSettings::OnCbnSelchangeScheme()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_SCHEME));

	int idx = pBox->GetCurSel()-1;

	bs->SetScheme(idx);
	AfxGetApp()->WriteProfileInt(_T("LastOrder"),_T("Scheme"),idx);

	MessageBox(_T("�������Ѹı�"),_T("��ʾ"),MB_ICONINFORMATION);

}


BOOL CSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	CComboBox * pBox = static_cast<CComboBox *>(GetDlgItem(IDC_SCHEME));
	pBox->AddString(_T("ȫ������"));
	
	auto && rsl = bs->LoadScheme();

	for (size_t idx = 0;idx<rsl.size();++idx)
	{
		pBox->AddString(CString(rsl[idx].name.c_str()));
	}

	pBox->SetCurSel(bs->GetScheme()+1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

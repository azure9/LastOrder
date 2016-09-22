// SelectMac.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "SelectMac.h"
#include "msSQL.h"
#include "afxdialogex.h"


// CSelectMac 对话框

IMPLEMENT_DYNAMIC(CSelectMac, CDialogEx)

CSelectMac::CSelectMac(CWnd* pParent /*=NULL*/,vector<user_info> *pv,int sel)
	: CDialogEx(CSelectMac::IDD, pParent),pv(pv),sel(sel)
{
}

CSelectMac::~CSelectMac()
{
}

void CSelectMac::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
}


BEGIN_MESSAGE_MAP(CSelectMac, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CSelectMac::OnNMDblclkList2)
END_MESSAGE_MAP()


// CSelectMac 消息处理程序

BOOL CSelectMac::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_list.InsertColumn(0,_T("请选择该用户的新MAC地址"),LVCFMT_LEFT,350);

	int id = 0;
	int idx = 0;
	// TODO:  在此添加额外的初始化
	for each (user_info x in (*pv))
	{
		if (!(x.flag & RegedUser))
		{
			m_list.InsertItem(idx,CString(x.macAddress.get_printable()));
			m_list.SetItemData(idx,id);
			++idx;
		}
		++id;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelectMac::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int selMac = m_list.GetItemData(pNMItemActivate->iItem);
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(stringf(_T("您确定要将 %s 的MAC地址改为 %s 么？"),
		CString((*pv)[sel].name.c_str()),CString((*pv)[selMac].macAddress.get_printable())).c_str(),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		if (!UpdateData(TRUE))
		{
			TRACE(traceAppMsg, 0, "UpdateData failed during dialog termination.\n");
			// the UpdateData routine will set focus to correct item
			return;
		}
		
		::PostMessage(this->GetParent()->m_hWnd,SEL_MESSAGE, selMac,0);
	}
	*pResult = 0;
}


void CSelectMac::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	::PostMessage(this->GetParent()->m_hWnd,SEL_MESSAGE, -1,0);
	CDialogEx::OnCancel();
}

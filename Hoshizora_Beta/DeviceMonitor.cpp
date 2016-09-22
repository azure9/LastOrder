// DeviceMonitor.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "DeviceMonitor.h"
#include "common.h"
#include "afxdialogex.h"


// CDeviceMonitor 对话框

IMPLEMENT_DYNAMIC(CDeviceMonitor, CDialogEx)

CDeviceMonitor::CDeviceMonitor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceMonitor::IDD, pParent)
	, m_pluse(60)
{

}

CDeviceMonitor::~CDeviceMonitor()
{
}

void CDeviceMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_pluse);
}


BEGIN_MESSAGE_MAP(CDeviceMonitor, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON7, &CDeviceMonitor::OnBnClickedButton7)
END_MESSAGE_MAP()


// CDeviceMonitor 消息处理程序
DWORD WINAPI MonitorThread(LPVOID lpThreadParameter)
{
	CDeviceMonitor * pDM = (CDeviceMonitor *)lpThreadParameter;
	multimap<MacAddress,IpAddress> diff;
	while (true)
	{
		Sleep(pDM->m_pluse*1000);
		multimap<MacAddress,IpAddress> &&tmp = pDM->r->GetVlanIPMACList();
		for (auto iter = diff.begin();iter!=diff.end();++iter)
		{
			auto it = pDM->table.find(iter->first);
			if (it!=pDM->table.end())
			{
				diff.insert(pair<MacAddress,IpAddress>(it->first,it->second));
			}
		}
		if (diff.size())
		{
			auto iter = diff.begin();
			for (;iter!=diff.end();++iter)
			{
				pDM->m_list.AddString(stringf(L"%s  <->  %s",iter->first.get_printable(),iter->second.get_printable()).c_str());
				pDM->UpdateData(true);
			}
			diff.clear();
			pDM->table = tmp;
		}
	}
}

void CDeviceMonitor::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
// 	UpdateData(false);
// 	if (!table.size())
// 	{
// 		r = new CRouter("10.200.34.254",false);
// 		table = r->GetVlanIPMACList();
// 		DWORD tid;
// 		CreateThread(NULL,NULL,MonitorThread,this,NULL,&tid);
// 	}
// 	else
// 	{
// 		AfxMessageBox(_T("Monitor thread has been started."));
// 	}
}

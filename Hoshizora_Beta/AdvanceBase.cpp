// AdvanceBase.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "BaseInfo.h"
#include "SwitcherManager.h"
#include "Hoshizora_Beta.h"
#include "AdvanceBase.h"
#include "afxdialogex.h"
#include <ppl.h>

using namespace Concurrency;


// CAdvanceBase 对话框

IMPLEMENT_DYNAMIC(CAdvanceBase, CDialogEx)

CAdvanceBase::CAdvanceBase(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdvanceBase::IDD, pParent)
	, m_setType(0)
{

}

CAdvanceBase::~CAdvanceBase()
{
}

void CAdvanceBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_setType);
}


BEGIN_MESSAGE_MAP(CAdvanceBase, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAdvanceBase::OnBnClickedButton1)
END_MESSAGE_MAP()


// CAdvanceBase 消息处理程序

DWORD WINAPI DummyRefersh(LPVOID lParma)
{
	if ((int)lParma==2)
	{
		return CSwitcherManager::DeltaUpdate();
	}
	vector<string> && vIP = bs->GetAllIPS();

	if ((int)lParma==0&&MessageBox(NULL,
		L"您选择的选项常用于批量初始化交换机，并不推荐用此模式作为日常交换机刷新方案，此方式可能造成部分特殊端口用户无法访问网络，请再次确认是否用该模式刷新。",
		L"少女卖萌中",
		MB_ICONASTERISK|MB_YESNO)!=IDYES)
	{
		MessageBox(NULL,L"じゃね～感谢您支持LastOrder！",L"少女卖萌中",MB_ICONINFORMATION);
		return 0;
	}

	parallel_for(size_t(0), size_t(vIP.size()),[&](size_t index)
	{
		CSwitcherManager *session = new CSwitcherManager(NULL,CString(vIP[index].c_str()));
		try
		{
			session->ResetSession();
			session->UtilOneKeySet((int)lParma);
		}
		catch (...)
		{
			//MessageBox(NULL,stringf(_T("交换机 %s 连接失败，请确认交换机状态！"),CString(vIP[index].c_str())).c_str(),L"错误",MB_ICONERROR);
		}
		delete session;
	});

	//MessageBox(NULL,_T("恭喜！已完成所有注册交换机配置。"),_T("少女卖萌中"),MB_ICONINFORMATION);
	return 0;
}

DWORD WINAPI newThread(LPVOID lParma)
{
	CAdvanceBase *dlg = reinterpret_cast<CAdvanceBase *>(lParma);

	vector<string> && vIP = bs->GetAllIPS();

	HWND hList = GetDlgItem(dlg->GetSafeHwnd(),IDC_LIST1);
	SendMessage(hList,LB_RESETCONTENT,0,0);

	extern size_t LastLineID;
	CmsSQL *sql = new CmsSQL;
	LastLineID = sql->GetLastPayedLineID();
	delete sql;

	AfxGetApp()->WriteProfileInt(_T("LastOrder"), _T("LastLineID"), LastLineID);

	parallel_for(size_t(0), size_t(vIP.size()),[&](size_t index)
	{
		CSwitcherManager *session = new CSwitcherManager(NULL,CString(vIP[index].c_str()));
		try
		{
			SendMessage(hList,LB_ADDSTRING,0,
				reinterpret_cast<LPARAM>(stringf(_T("正在配置交换机 %s ..."),CString(vIP[index].c_str())).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

			session->ResetSession();
			SendMessage(hList,LB_ADDSTRING,0,
				reinterpret_cast<LPARAM>(stringf(_T("%s 配置完成！已修改 %d 个项目。"),
				CString(vIP[index].c_str()),session->UtilOneKeySet(dlg->m_setType==0?1:0)).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
		}
		catch (...)
		{
			SendMessage(hList,LB_ADDSTRING,0,
				reinterpret_cast<LPARAM>(stringf(_T("交换机 %s 连接失败，请确认交换机状态！"),CString(vIP[index].c_str())).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
		}
		delete session;
	});

	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("恭喜！已完成所有注册交换机配置。")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	dlg->MessageBox(_T("恭喜！已完成所有注册交换机配置。"),_T("少女卖萌中"),MB_ICONINFORMATION);
	return 0;
}

DWORD WINAPI deltaThread(LPVOID lParma)
{
	CAdvanceBase *dlg = reinterpret_cast<CAdvanceBase *>(lParma);

	HWND hList = GetDlgItem(dlg->GetSafeHwnd(),IDC_LIST1);
	SendMessage(hList,LB_RESETCONTENT,0,0);


	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("差异升级 始まるよ～_(:з」∠)_")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("最近得了一种叫懒癌的不治之症、懒得做界面交互了╮(￣▽￣\")╭ ")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(stringf(_T("恭喜！已完成%d位用户安全数据更新。"),CSwitcherManager::DeltaUpdate()).c_str()));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	dlg->MessageBox(_T("恭喜！已完成用户安全数据差异升级。"),_T("少女卖萌中"),MB_ICONINFORMATION);
	return 0;
}

void CAdvanceBase::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	// add concurrent lock. Don't allow user to refresh list multi thread.
	UpdateData(true);
	DWORD tid;
	if (m_setType==2)
	{
		CreateThread(0,0,deltaThread,this,0,&tid);
		return;
	}
	else if (m_setType==1&&MessageBox(
		L"您选择的选项常用于批量初始化交换机，并不推荐用此模式作为日常交换机刷新方案，此方式可能造成部分特殊端口用户无法访问网络，请再次确认是否用该模式刷新。",
		L"少女卖萌中",
		MB_ICONASTERISK|MB_YESNO)!=IDYES)
	{
		MessageBox(L"じゃね～感谢您支持LastOrder！",L"少女卖萌中",MB_ICONINFORMATION);
		return;
	}
	CreateThread(0,0,newThread,this,0,&tid);
}


BOOL CAdvanceBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

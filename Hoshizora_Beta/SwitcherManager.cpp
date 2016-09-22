// SwitcherManager.cpp : 实现文件
//

#include "stdafx.h"
#include "Loading.h"
#include "UserInfo.h"
#include "Hoshizora_Beta.h"
#include "SwitcherManager.h"
#include "afxtaskdialog.h"
#include "VMProtectSDK.h"

#ifndef _SNMP_PUBLIC_INCLUDE
#define _SNMP_PUBLIC_INCLUDE
#include <winsock2.h>
#include "snmp_pp/snmp_pp.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"libdes.lib")
#pragma comment(lib,"snmp++.lib")

using namespace Snmp_pp;
#endif
#include "afxdialogex.h"

extern COperMonitor* op;


// CSwitcherManager 对话框

IMPLEMENT_DYNAMIC(CSwitcherManager, CDialogEx)

	CSwitcherManager::CSwitcherManager(CWnd* pParent /*=NULL*/,LPCTSTR ip,int mgrPort)
	: CDialogEx(CSwitcherManager::IDD, pParent)
	, globalMAC(FALSE),dlgAuth(nullptr),selLine(0),session(0),mgtPort(mgrPort)
	, m_safemode(0)
	, m_trap(FALSE)
{
	USES_CONVERSION;
	this->ip = W2A(ip);
}

CSwitcherManager::~CSwitcherManager()
{
	delete session;
	delete dlgAuth;
}

void CSwitcherManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Check(pDX, IDC_CHECK1, globalMAC);
	DDX_Radio(pDX, IDC_RADIO1, m_safemode);
	DDX_Check(pDX, IDC_CHECK2, m_trap);
}


BEGIN_MESSAGE_MAP(CSwitcherManager, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CSwitcherManager::OnBnClickedCancel)
	ON_MESSAGE(WM_ListCtrlEx_CHECK_CHANGED, &CSwitcherManager::OnListctrlexCheckChanged)
	ON_BN_CLICKED(IDC_CHECK1, &CSwitcherManager::OnClickedCheck1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSwitcherManager::OnNMDblclkList1)
	ON_WM_MOVING()
	ON_BN_CLICKED(IDC_RADIO1, &CSwitcherManager::OnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSwitcherManager::OnClickedRadio1)
	ON_BN_CLICKED(IDC_ONEKEYCONFIG, &CSwitcherManager::OnBnClickedOnekeyconfig)
	ON_BN_CLICKED(IDC_CHECK2, &CSwitcherManager::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_RESET, &CSwitcherManager::OnBnClickedReset)
END_MESSAGE_MAP()


// CSwitcherManager 消息处理程序


void CSwitcherManager::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	DestroyWindow();
}


BOOL CSwitcherManager::OnInitDialog()
{
	CLoading loading;
	loading.Create(IDD_DLG_LOADING,NULL);
	loading.ShowWindow(SW_SHOW);
	loading.UpdateWindow();//(╯‵□′)╯︵┻━┻卧槽不Update神马也没有坑爹呢么
	auto && rsl = bs->GetCommunity(this->ip.get_printable());
	session = new CSwitcher((char *)this->ip.get_printable(),rsl.first.c_str(),rsl.second.c_str());
	CDialogEx::OnInitDialog();

	SetWindowText(CString("交换机管理：")+ip.get_printable() +"　　"+ bs->GetLocationInfoS(ip.get_printable()).c_str());
	// TODO:  在此添加额外的初始化
	op->SendOper((LPSTR)ip.get_printable(),"SwitcherManager Page");
	if (mgtPort)
	{
		op->SendOper((LPSTR)ip.get_printable(),(LPSTR)stringf("Port %d selected!",mgtPort).c_str());
	}

	m_list.ModifyStyle(0, LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() |LVS_EX_FULLROWSELECT );

	m_list.InsertColumn(0, _T("端口号"), 50, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(1, _T("端口名称"), 125, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(2, _T("端口描述"), 125, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(3, _T("开关"), 50, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(4, _T("MAC安全"), 80, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(5, _T("在线"), 50, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(6, _T("在线人数"), 70, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);

	USES_CONVERSION;
	vector<PortStatus> && port = session->GetPortStatus();int i=0;
	map<int,string>&& m = bs->GetPortListDescrS(ip.get_printable());

	for (auto it = port.begin();it!=port.end();++it)
	{
		++i;
		int nRow = m_list.InsertItem(m_list.GetItemCount(), stringf(_T("%d"),i).c_str());

		m_list.SetItemText(nRow,1,A2W(it->Name.c_str()));
		auto iter = m.find(i);
		if (iter!=m.end())
		{
			m_list.SetItemText(nRow,2,A2W(iter->second.c_str()));
		}
		m_list.SetCellChecked(nRow,3,it->bStatus&PORT_ADMINSTATUS_ON);
		m_list.SetCellChecked(nRow,4,it->bStatus&PORT_MACSECURITY_ON);
		m_list.SetCellChecked(nRow,5,it->bStatus&PORT_OPERSTATUS_ON);
		m_list.SetCellEnabled(nRow,5,false);
		m_list.SetItemText(nRow,6,stringf(_T("%d"),it->cntOnline).c_str());
		if (i==mgtPort)
		{
			m_list.SetLineColor(nRow,0x0000FF);
			m_list.SetItemState(nRow,LVIS_SELECTED, LVIS_SELECTED);
			m_list.EnsureVisible(nRow, FALSE);
		}
	}

	globalMAC = session->GetGlobalMAC();
	if (!globalMAC)
	{
		MessageBox(_T("当前交换机全局MAC安全设置未开启！请注意！"),_T("警告"),MB_ICONWARNING);
	}

	m_trap = session->GetTrapStatus();

	m_safemode = session->GetSafeMode();
	UpdateData(false);
	//	m_list.SetShowProgressPercent(TRUE);
	m_list.SetSupportSort(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

afx_msg LRESULT CSwitcherManager::OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam)
{
	ListCtrlEx::CListCtrlEx* ctl =reinterpret_cast<ListCtrlEx::CListCtrlEx *> (wParam);
	ListCtrlEx::CheckCellMsg* msg =reinterpret_cast<ListCtrlEx::CheckCellMsg *>(lParam);
	switch (msg->m_nColumn)
	{
	case 3:
		if (MessageBox(_T("确认要修改该端口状态么？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			int idxPort = wcstol(m_list.GetItemText(msg->m_nRow,0),NULL,0);
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("Port %d status changed! => %s",idxPort,msg->m_bChecked?"ON":"OFF").c_str());
			if (session->SetPortAdminStatus(idxPort,msg->m_bChecked))
			{
				MessageBox(_T("端口状态设置成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("端口状态设置失败！"),_T("警告"),MB_ICONWARNING);
			}
		}
		else
		{
			ctl->SetCellChecked(msg->m_nRow, msg->m_nColumn, !msg->m_bChecked);
		}
		break;
	case 4:
		if (MessageBox(_T("确认要修改该端口MAC安全么？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			int idxPort = wcstol(m_list.GetItemText(msg->m_nRow,0),NULL,0);
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("Port %d security status changed! => %s",idxPort,msg->m_bChecked?"ON":"OFF").c_str());
			if (session->SetPortSecurityStatus(idxPort,msg->m_bChecked))
			{
				MessageBox(_T("端口MAC安全设置成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("端口MAC安全设置超时，请稍后刷新！"),_T("警告"),MB_ICONWARNING);
			}
		}
		else
		{
			ctl->SetCellChecked(msg->m_nRow, msg->m_nColumn, !msg->m_bChecked);
		}
		break;
	default:
		break;
	}
	return 0;
}


void CSwitcherManager::OnClickedCheck1()
{
	UpdateData(true);
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(_T("您确认需要修改此交换机的全局MAC安全状态么？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		op->SendOper((LPSTR)ip.get_printable(),
			(LPSTR)stringf("Global security status changed! => %s",globalMAC?"ON":"OFF").c_str());
		if (session->SetGlobalMAC(globalMAC))
		{
			MessageBox(_T("交换机全局MAC安全状态设置成功！"),_T("提示"),MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("交换机全局MAC安全设置超时，请稍后刷新！"),_T("警告"),MB_ICONWARNING);
		}
	}
	else
	{
		globalMAC^=1;
		UpdateData(false);
	}
}


void CSwitcherManager::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	selLine = pNMItemActivate->iItem+1;
	if (dlgAuth)
	{
		delete dlgAuth;
		dlgAuth = nullptr;
	}
	USES_CONVERSION;
	int idxPort = wcstol(m_list.GetItemText(pNMItemActivate->iItem,0),NULL,10);
	dlgAuth = new CUserAuthDlg(this,W2A(m_list.GetItemText(pNMItemActivate->iItem,2)),session,idxPort);
	dlgAuth->Create(IDD_DLG_USERSHOW,this);
	MoveSubWindow();
	dlgAuth->ShowWindow(SW_SHOW);
	op->SendOper((LPSTR)ip.get_printable(),(LPSTR)stringf("Port %d manage page open!",idxPort).c_str());
	*pResult = 0;
}


int CSwitcherManager::MoveSubWindow(void)
{
	if (dlgAuth == 0)
	{
		return -1;
	}

	CRect rect;
	m_list.GetSubItemRect(selLine,0,0,rect);
	m_list.ClientToScreen(rect);
	CRect subRect;
	dlgAuth->GetWindowRect(subRect);
	int height = subRect.Height();
	int width = subRect.Width();
	subRect.top = rect.top;
	subRect.bottom = rect.top+height;
	subRect.left = rect.left;
	subRect.right = rect.left+width;
	dlgAuth->MoveWindow(subRect);
	return 0;
}


void CSwitcherManager::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnMoving(fwSide, pRect);
	// TODO: 在此处添加消息处理程序代码
	MoveSubWindow();
}


void CSwitcherManager::OnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	int safe = session->GetSafeMode();
	if (m_safemode!=safe)
	{
		if (MessageBox(_T("您确认需要修改此交换机的MAC安全模式么？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("MAC security action changed! => %s",globalMAC?"Port block":"Data filtering").c_str());
			if (session->SetSafeMode(m_safemode))
			{
				MessageBox(_T("交换机MAC安全模式设置成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("交换机MAC安全模式设置超时，请稍后刷新！"),_T("警告"),MB_ICONWARNING);
			}
		}
		else
		{
			m_safemode = session->GetSafeMode();
			UpdateData(false);
		}
	}

}


void CSwitcherManager::OnBnClickedOnekeyconfig()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(_T("交换机智能配置过程较慢，请确认是否开始智能配置交换机？"),_T("少女卖萌中"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		size_t changecnt = UtilOneKeySet();
		MessageBox(stringf(_T("交换机智能配置已完成！本次智能配置修改了 %d 个项目。"),changecnt).c_str(),
			_T("少女卖萌中"),MB_ICONINFORMATION);
	}
}


void CSwitcherManager::OnBnClickedCheck2()//注册Trap通知……=。=Trap监控暂时交给某程序好了、自己先不写了
{
	// TODO: 在此添加控件通知处理程序代码
	//1.3.6.1.4.1.45.1.6.4.3.4.1.4.1 trap receive ip
	// 	s5AgTrpRcvrNetAddr
	//s5AgRemoteLoginIp	SCALAR	read-only	IpAddress	
	// 
	UpdateData(true);

	if (m_trap)
	{
		if (MessageBox(_T("确定要监控本台交换机么？"),_T("少女卖萌中"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			int status;Vb tmp;Pdu pdu;
			session->Get("1.3.6.1.4.1.45.1.6.4.3.4.1.2.1").get_value(status);

			if (status!=2)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.3.4.1.2.1");
				tmp.set_value(5);
				pdu+=tmp;
			}

			IpAddress &&ip = session->GetLocalIP();
			tmp.set_oid("1.3.6.1.4.1.45.1.6.4.3.4.1.4.1");
			OctetStr str(&ip[0],4);
			tmp.set_value(str);
			pdu+=tmp;

			str = "public";
			tmp.set_oid("1.3.6.1.4.1.45.1.6.4.3.4.1.5.1");
			tmp.set_value(str);
			pdu+=tmp;

			if (session->SetList(pdu))
			{
				MessageBox(_T("实时监控设置成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("实时监控设置超时，请稍后刷新！"),_T("警告"),MB_ICONWARNING);
			}
		}
		else
		{
			m_trap ^=true;
		}
	}
	else
	{
		if (MessageBox(_T("确定要取消监控本台交换机么？"),_T("少女卖萌中"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			Vb tmp;
			tmp.set_oid("1.3.6.1.4.1.45.1.6.4.3.4.1.2.1");
			tmp.set_value(4);
			if (session->Set(tmp))
			{
				MessageBox(_T("实时监控取消成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("实时监控取消超时，请稍后刷新！"),_T("警告"),MB_ICONWARNING);
			}
		}
		else
		{
			m_trap ^=true;
		}
	}
	UpdateData(true);

}


void CSwitcherManager::OnBnClickedReset()//重启交换机
{
	// TODO: 在此添加控件通知处理程序代码
	if (CTaskDialog::IsSupported())
	{
		Vb tmp((Oid)"1.3.6.1.4.1.45.1.6.4.2.1.1.0");
		int result = CTaskDialog::ShowDialog(_T("请选择"),_T("您希望如何重启交换机？"),_T("交换机重启选项"),
			IDS_RESTART,IDS_S_RRESET,TDCBF_CANCEL_BUTTON,TDF_ENABLE_HYPERLINKS|TDF_USE_COMMAND_LINKS,
			_T("重启一般需要耗时2-3分钟，泡杯茶再回来看吧_(:з」∠)_"));
		bool status = false;

		if (result!=IDCANCEL)
		{
			CString str;
			str.LoadString(result);
			USES_CONVERSION;
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("Restart switcher! Operation type: %s",W2A(str)).c_str());
		}

		switch (result)
		{
		case IDS_REBOOT:
			tmp.set_value(4);
			status = session->Set(tmp);
			break;
		case IDS_RESET:
			tmp.set_value(5);
			status = session->Set(tmp);
			break;
		case IDS_RESTART:
			tmp.set_value(3);
			status = session->Set(tmp);
			break;
		case IDS_S_RRESET:
			AfxMessageBox(_T("暂不支持汇聚路由重启。"));
		default:
			return;
		}
		if (status)
		{
			MessageBox(_T("重启指令已发出！"),_T("提示"),MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("重启指令发送失败！"),_T("警告"),MB_ICONWARNING);
		}
	}
	else
		AfxMessageBox(_T("本功能仅支持Windows Vista及以上系统，请您升级您的系统！"));
}

size_t CSwitcherManager::DeltaUpdate()//增量更新 数据表里加入上次更新的最后一行缴费数据数据和上次更新的月份，如果和当前月份不同则自动刷新时使用端口完全更新，否则使用增量更新
{
	OutputDebugString(L"Delta Update");
	extern size_t LastLineID;
	CmsSQL sql;
	vector<pair<string,MacAddress>> v;
	OutputDebugString(L"Query user data.");
	LastLineID = sql.GetNewPayedUser(v,LastLineID,bs->GetScheme()!=-1?bs->LoadScheme(bs->GetScheme()).prefix.c_str():NULL);

	OutputDebugString(L"Prepare refresh list.");

	//best score 0.00059484 all student for a month
	map<pair<int,int>,string> PortMap;

	bs->GetAllPortDescr(PortMap);
	map<string,pair<int,int>> PortDescr;

	for each (auto & var in PortMap)
	{
		PortDescr[var.second] = var.first;
	}
	PortMap.clear();

	map<pair<int,int>,vector<MacAddress>> m;
	for each (auto & x in v)
	{
		int port = 0;
		auto && flag = PortDescr.find(x.first);
		if (flag!=PortDescr.end())
		{
			auto it = m.find(flag->second);
			if (it!=m.end())
			{
				it->second.push_back(move(x.second));
			}
			else
				m[flag->second] = vector<MacAddress>(1,move(x.second));
		}
	}
	v.clear();

	//////////////////////////////////////////////////////////////////////////
	//	map<pair<int,int>,vector<MacAddress>> m
	map<string,int> vIPtmp;
	bs->GetAllIP(vIPtmp);

	map<int,string> vIP;
	for each (auto & var in vIPtmp)
	{
		vIP[var.second] = var.first;
	}
	vIPtmp.clear();
	OutputDebugStringA(stringf("Refresh  security list. Total %d items.",m.size()).c_str());

	{
		CSwitcherManager *smgr = nullptr;
		int idx = 0;

		//for each (auto var in m) //this will be wrong
		for (auto & var : m)
		{
			if (idx!=var.first.first)
			{
				delete smgr;
				smgr = new CSwitcherManager(0,CString(vIP[var.first.first].c_str()));
				OutputDebugStringA(stringf("New session! %s",vIP[var.first.first].c_str()).c_str());
				smgr->ResetSession();
				idx = var.first.first;
			}

			smgr->session->AddMacToPort(var.first.second,var.second);	
		}
		delete smgr;
		smgr = nullptr;
	}

	OutputDebugStringA("Refresh accomplished!");
	AfxGetApp()->WriteProfileInt(_T("LastOrder"), _T("LastLineID"), LastLineID);

	//map<MacAddress,pair<IpAddress,int>> mCurrent;
	//m.find(pair<IpAddress,int>("10.10.200.18",32))->second.push_back(MacAddress(""));
	/*vector<tuple<IpAddress,int,MacAddress>> v;
	for each (tuple<IpAddress,int,MacAddress>&x in v)
	{
		IpAddress &device_ip = get<0>(x);
		int &port = get<1>(x);
		MacAddress &mac = get<2>(x);


	}*/

	size_t refreshCount = m.size();

	if (refreshCount != 0)
	{
		op->SendOper((LPSTR)"localhost","Switcher onekey configuration! Operation type: Delta update.");
	}
	return refreshCount;
}


size_t CSwitcherManager::UtilOneKeySet(int setType)//type 0:All,Type 1:Smart,Type 3:Delta
{
#ifdef Business
	VMProtectBeginVirtualizationLockByKey("CSwitcherManager::UtilOneKeySet hdr");
	int status=0;
	status = VMProtectGetSerialNumberState();
	if (status)
	{
		MessageBox(_T("请不要耍小聪明！"),_T("LastOrder授权系统"),MB_ICONERROR);
		ExitProcess(-1);
	}
#endif // Business
	size_t changecnt = 0;
	//先更新所有端口的安全列表
	multimap<int,MacAddress> && mPortSecu = session->GetPortSecurityList();
	CUserInfo info;
	CPortSet macPort;//Port mac security
	map<int,string> && mPortDescr = bs->GetPortListDescrS(session->GetDeviceIPAddress().get_printable());

#ifdef Business
	VMProtectEnd();
#endif // Business
	for (int i = 1;i<=session->GetPortCnt();++i)
	{
		vector<MacAddress> existMac,stdMac;
		if (!mPortDescr[i].empty())
		{
			macPort.SetPos(i,true);
			vector<user_info> && v = info.GetUserInfo((char *)mPortDescr[i].c_str());
			auto iter = mPortSecu.lower_bound(i);
			if (iter!=mPortSecu.end())
			{
				for (;iter!=mPortSecu.upper_bound(i);++iter)
				{
					existMac.push_back(iter->second);
				}
			}

			for (auto iter = v.begin();iter!=v.end();++iter)
			{
				if (iter->flag&Payed)
				{
					stdMac.push_back(move(iter->macAddress));
				}
			}

			vector<MacAddress>&& newMac = GetDelta(existMac,stdMac);
			vector<MacAddress>&& oldMac = GetDelta(stdMac,existMac);

			changecnt += (oldMac.size()>0?oldMac.size()-session->DeleteMacFromPort(i,oldMac):NULL);
			changecnt += (newMac.size()>0?newMac.size()-session->AddMacToPort(i,newMac):NULL);
		}
		else
		{
			macPort.SetPos(i,false);
		}
	}

	if (setType==0)
	{
		vector<Oid> OidList;
		OidList.push_back("1.3.6.1.4.1.45.1.6.4.6.2.0");//NTP服务器
		OidList.push_back("1.3.6.1.4.1.45.1.6.4.6.1.0");//是否开启时间同步
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.4.0");//日志重写模式
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.11.0");//关闭远程日志//1 for enable 2 for disable
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.13.0");//日志记录等级

		Vb tmp;
		Pdu pdu;

		vector<Vb> && resList = session->GetList(OidList);

		if (OidList.size()==5)
		{
			IpAddress tmpIP;int status;
			resList[0].get_value(tmpIP);
			resList[1].get_value(status);
			if (tmpIP!=session->GetGateway()||status!=2)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.6.2.0");
				tmp.set_value(session->GetGateway());
				session->Set(tmp);//NTP服务器

				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.6.1.0");
				tmp.set_value(2);//时间同步
				session->Set(tmp);

				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.6.5.0");
				tmp.set_value(1);//手动更新时间
				session->Set(tmp);

				changecnt+=3;
			}

			resList[2].get_value(status);
			if (status!=1)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.4.0");
				tmp.set_value(1);//日志重写模式
				pdu+=tmp;

				++changecnt;
			}

			resList[3].get_value(status);
			if (status!=2)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.11.0");
				tmp.set_value(2);//关闭远程日志//1 for enable 2 for disable
				pdu+=tmp;

				++changecnt;
			}

			resList[4].get_value(status);
			if (status!=3)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.13.0");
				tmp.set_value(3);//日志记录等级
				pdu+=tmp;

				++changecnt;
			}
		}

		if (!session->GetGlobalMAC())
		{
			tmp.set_oid("1.3.6.1.4.1.45.1.6.5.3.3.0");//全局MAC安全
			tmp.set_value(1);
			pdu+=tmp;

			++changecnt;
		}

		if (session->GetSafeMode())
		{
			tmp.set_oid("1.3.6.1.4.1.45.1.6.5.3.5.0");//MAC安全模式
			tmp.set_value(6);
			pdu+=tmp;

			++changecnt;
		}

		CPortSet currmacPort(session->Get("1.3.6.1.4.1.45.1.6.5.3.15.0"));//Port mac security

		if (currmacPort!=macPort)
		{
			tmp.set_oid("1.3.6.1.4.1.45.1.6.5.3.15.0");
			tmp.set_value(macPort.GetDesc(),64);

			pdu+=tmp;
			++changecnt;
		}

		session->SetList(pdu);
	}

	//	printf("Setting up remote log...\n");
	// 	char LogServer[] = {"202.204.193.231"};
	// 	snmpSession.Set(".1.3.6.1.4.1.45.1.6.16.1.1.12.0",LogServer,ASN_IPADDRESS);

	if (changecnt != 0)
	{
		op->SendOper((LPSTR)ip.get_printable(),
			(LPSTR)stringf("Switcher onekey configuration! Operation type: %s",setType?"Port security only":"Auto config").c_str());
	}

	return changecnt;
}


int CSwitcherManager::ResetSession()
{
	delete session;
	auto && rsl = bs->GetCommunity(this->ip.get_printable());
	session = new CSwitcher((char *)this->ip.get_printable(),rsl.first.c_str(),rsl.second.c_str());
	return 0;
}

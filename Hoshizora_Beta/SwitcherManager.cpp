// SwitcherManager.cpp : ʵ���ļ�
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


// CSwitcherManager �Ի���

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


// CSwitcherManager ��Ϣ�������


void CSwitcherManager::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DestroyWindow();
}


BOOL CSwitcherManager::OnInitDialog()
{
	CLoading loading;
	loading.Create(IDD_DLG_LOADING,NULL);
	loading.ShowWindow(SW_SHOW);
	loading.UpdateWindow();//(�s�F����)�s��ߩ����Բ۲�Update����Ҳû�пӵ���ô
	auto && rsl = bs->GetCommunity(this->ip.get_printable());
	session = new CSwitcher((char *)this->ip.get_printable(),rsl.first.c_str(),rsl.second.c_str());
	CDialogEx::OnInitDialog();

	SetWindowText(CString("����������")+ip.get_printable() +"����"+ bs->GetLocationInfoS(ip.get_printable()).c_str());
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	op->SendOper((LPSTR)ip.get_printable(),"SwitcherManager Page");
	if (mgtPort)
	{
		op->SendOper((LPSTR)ip.get_printable(),(LPSTR)stringf("Port %d selected!",mgtPort).c_str());
	}

	m_list.ModifyStyle(0, LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() |LVS_EX_FULLROWSELECT );

	m_list.InsertColumn(0, _T("�˿ں�"), 50, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(1, _T("�˿�����"), 125, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(2, _T("�˿�����"), 125, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	m_list.InsertColumn(3, _T("����"), 50, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(4, _T("MAC��ȫ"), 80, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(5, _T("����"), 50, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_list.InsertColumn(6, _T("��������"), 70, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);

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
		MessageBox(_T("��ǰ������ȫ��MAC��ȫ����δ��������ע�⣡"),_T("����"),MB_ICONWARNING);
	}

	m_trap = session->GetTrapStatus();

	m_safemode = session->GetSafeMode();
	UpdateData(false);
	//	m_list.SetShowProgressPercent(TRUE);
	m_list.SetSupportSort(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

afx_msg LRESULT CSwitcherManager::OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam)
{
	ListCtrlEx::CListCtrlEx* ctl =reinterpret_cast<ListCtrlEx::CListCtrlEx *> (wParam);
	ListCtrlEx::CheckCellMsg* msg =reinterpret_cast<ListCtrlEx::CheckCellMsg *>(lParam);
	switch (msg->m_nColumn)
	{
	case 3:
		if (MessageBox(_T("ȷ��Ҫ�޸ĸö˿�״̬ô��"),_T("��ʾ"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			int idxPort = wcstol(m_list.GetItemText(msg->m_nRow,0),NULL,0);
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("Port %d status changed! => %s",idxPort,msg->m_bChecked?"ON":"OFF").c_str());
			if (session->SetPortAdminStatus(idxPort,msg->m_bChecked))
			{
				MessageBox(_T("�˿�״̬���óɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("�˿�״̬����ʧ�ܣ�"),_T("����"),MB_ICONWARNING);
			}
		}
		else
		{
			ctl->SetCellChecked(msg->m_nRow, msg->m_nColumn, !msg->m_bChecked);
		}
		break;
	case 4:
		if (MessageBox(_T("ȷ��Ҫ�޸ĸö˿�MAC��ȫô��"),_T("��ʾ"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			int idxPort = wcstol(m_list.GetItemText(msg->m_nRow,0),NULL,0);
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("Port %d security status changed! => %s",idxPort,msg->m_bChecked?"ON":"OFF").c_str());
			if (session->SetPortSecurityStatus(idxPort,msg->m_bChecked))
			{
				MessageBox(_T("�˿�MAC��ȫ���óɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("�˿�MAC��ȫ���ó�ʱ�����Ժ�ˢ�£�"),_T("����"),MB_ICONWARNING);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (MessageBox(_T("��ȷ����Ҫ�޸Ĵ˽�������ȫ��MAC��ȫ״̬ô��"),_T("��ʾ"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		op->SendOper((LPSTR)ip.get_printable(),
			(LPSTR)stringf("Global security status changed! => %s",globalMAC?"ON":"OFF").c_str());
		if (session->SetGlobalMAC(globalMAC))
		{
			MessageBox(_T("������ȫ��MAC��ȫ״̬���óɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("������ȫ��MAC��ȫ���ó�ʱ�����Ժ�ˢ�£�"),_T("����"),MB_ICONWARNING);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ˴������Ϣ����������
	MoveSubWindow();
}


void CSwitcherManager::OnClickedRadio1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	int safe = session->GetSafeMode();
	if (m_safemode!=safe)
	{
		if (MessageBox(_T("��ȷ����Ҫ�޸Ĵ˽�������MAC��ȫģʽô��"),_T("��ʾ"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			op->SendOper((LPSTR)ip.get_printable(),
				(LPSTR)stringf("MAC security action changed! => %s",globalMAC?"Port block":"Data filtering").c_str());
			if (session->SetSafeMode(m_safemode))
			{
				MessageBox(_T("������MAC��ȫģʽ���óɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("������MAC��ȫģʽ���ó�ʱ�����Ժ�ˢ�£�"),_T("����"),MB_ICONWARNING);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (MessageBox(_T("�������������ù��̽�������ȷ���Ƿ�ʼ�������ý�������"),_T("��Ů������"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		size_t changecnt = UtilOneKeySet();
		MessageBox(stringf(_T("������������������ɣ��������������޸��� %d ����Ŀ��"),changecnt).c_str(),
			_T("��Ů������"),MB_ICONINFORMATION);
	}
}


void CSwitcherManager::OnBnClickedCheck2()//ע��Trap֪ͨ����=��=Trap�����ʱ����ĳ������ˡ��Լ��Ȳ�д��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//1.3.6.1.4.1.45.1.6.4.3.4.1.4.1 trap receive ip
	// 	s5AgTrpRcvrNetAddr
	//s5AgRemoteLoginIp	SCALAR	read-only	IpAddress	
	// 
	UpdateData(true);

	if (m_trap)
	{
		if (MessageBox(_T("ȷ��Ҫ��ر�̨������ô��"),_T("��Ů������"),MB_ICONQUESTION|MB_YESNO)==IDYES)
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
				MessageBox(_T("ʵʱ������óɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("ʵʱ������ó�ʱ�����Ժ�ˢ�£�"),_T("����"),MB_ICONWARNING);
			}
		}
		else
		{
			m_trap ^=true;
		}
	}
	else
	{
		if (MessageBox(_T("ȷ��Ҫȡ����ر�̨������ô��"),_T("��Ů������"),MB_ICONQUESTION|MB_YESNO)==IDYES)
		{
			Vb tmp;
			tmp.set_oid("1.3.6.1.4.1.45.1.6.4.3.4.1.2.1");
			tmp.set_value(4);
			if (session->Set(tmp))
			{
				MessageBox(_T("ʵʱ���ȡ���ɹ���"),_T("��ʾ"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("ʵʱ���ȡ����ʱ�����Ժ�ˢ�£�"),_T("����"),MB_ICONWARNING);
			}
		}
		else
		{
			m_trap ^=true;
		}
	}
	UpdateData(true);

}


void CSwitcherManager::OnBnClickedReset()//����������
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (CTaskDialog::IsSupported())
	{
		Vb tmp((Oid)"1.3.6.1.4.1.45.1.6.4.2.1.1.0");
		int result = CTaskDialog::ShowDialog(_T("��ѡ��"),_T("��ϣ�����������������"),_T("����������ѡ��"),
			IDS_RESTART,IDS_S_RRESET,TDCBF_CANCEL_BUTTON,TDF_ENABLE_HYPERLINKS|TDF_USE_COMMAND_LINKS,
			_T("����һ����Ҫ��ʱ2-3���ӣ��ݱ����ٻ�������_(:�١���)_"));
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
			AfxMessageBox(_T("�ݲ�֧�ֻ��·��������"));
		default:
			return;
		}
		if (status)
		{
			MessageBox(_T("����ָ���ѷ�����"),_T("��ʾ"),MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("����ָ���ʧ�ܣ�"),_T("����"),MB_ICONWARNING);
		}
	}
	else
		AfxMessageBox(_T("�����ܽ�֧��Windows Vista������ϵͳ��������������ϵͳ��"));
}

size_t CSwitcherManager::DeltaUpdate()//�������� ���ݱ�������ϴθ��µ����һ�нɷ��������ݺ��ϴθ��µ��·ݣ�����͵�ǰ�·ݲ�ͬ���Զ�ˢ��ʱʹ�ö˿���ȫ���£�����ʹ����������
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
		MessageBox(_T("�벻ҪˣС������"),_T("LastOrder��Ȩϵͳ"),MB_ICONERROR);
		ExitProcess(-1);
	}
#endif // Business
	size_t changecnt = 0;
	//�ȸ������ж˿ڵİ�ȫ�б�
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
		OidList.push_back("1.3.6.1.4.1.45.1.6.4.6.2.0");//NTP������
		OidList.push_back("1.3.6.1.4.1.45.1.6.4.6.1.0");//�Ƿ���ʱ��ͬ��
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.4.0");//��־��дģʽ
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.11.0");//�ر�Զ����־//1 for enable 2 for disable
		OidList.push_back("1.3.6.1.4.1.45.1.6.16.1.1.13.0");//��־��¼�ȼ�

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
				session->Set(tmp);//NTP������

				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.6.1.0");
				tmp.set_value(2);//ʱ��ͬ��
				session->Set(tmp);

				tmp.set_oid("1.3.6.1.4.1.45.1.6.4.6.5.0");
				tmp.set_value(1);//�ֶ�����ʱ��
				session->Set(tmp);

				changecnt+=3;
			}

			resList[2].get_value(status);
			if (status!=1)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.4.0");
				tmp.set_value(1);//��־��дģʽ
				pdu+=tmp;

				++changecnt;
			}

			resList[3].get_value(status);
			if (status!=2)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.11.0");
				tmp.set_value(2);//�ر�Զ����־//1 for enable 2 for disable
				pdu+=tmp;

				++changecnt;
			}

			resList[4].get_value(status);
			if (status!=3)
			{
				tmp.set_oid("1.3.6.1.4.1.45.1.6.16.1.1.13.0");
				tmp.set_value(3);//��־��¼�ȼ�
				pdu+=tmp;

				++changecnt;
			}
		}

		if (!session->GetGlobalMAC())
		{
			tmp.set_oid("1.3.6.1.4.1.45.1.6.5.3.3.0");//ȫ��MAC��ȫ
			tmp.set_value(1);
			pdu+=tmp;

			++changecnt;
		}

		if (session->GetSafeMode())
		{
			tmp.set_oid("1.3.6.1.4.1.45.1.6.5.3.5.0");//MAC��ȫģʽ
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

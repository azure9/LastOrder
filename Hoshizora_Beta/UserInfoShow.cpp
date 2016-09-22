#include "stdafx.h"
#include "UserInfoShow.h"
#include "VMProtectSDK.h"

IMPLEMENT_DYNAMIC(CUserInfoShow, CListCtrlEx)

extern COperMonitor *op;

CUserInfoShow::CUserInfoShow()
	:r(nullptr),s(nullptr)
{
}


CUserInfoShow::~CUserInfoShow(void)
{
}

int CUserInfoShow::PrepareUserData(vector<user_info>& v,vector<vector<DWORD>>& pay_info,CSwitcher *s,int idxPort)
{
	this->v = v;
	this->pay_info = pay_info;
	this->s = s;
	this->idxPort = idxPort;

	if (s)
	{
		vector<MacAddress> vmac;

		for (auto iter = this->v.begin();iter!=this->v.end();++iter)
		{
			vmac.push_back(iter->macAddress);
		}

		IpAddress && rIP = s->GetGateway();
		auto && rsl = bs->GetCommunity(rIP.get_printable());
		CRouter r(rIP.get_printable(),rsl.first.c_str(),rsl.second.c_str());
		vIP = r.GetIPFromMac(vmac);
	}
	else
	{
		vector<CRouter *> vR;
		vector<CSwitcher *> vS;
		for_each(this->v.begin(),this->v.end(),[&](user_info & x)
		{
			int port;
			string &&ip = bs->GetIPFromDescrS(x.local,port);
			if (ip.size()&&port!=-1)
			{
				auto iterS = find(vS.begin(),vS.end(),ip);
				if (iterS==vS.end())
				{
					auto &&rsl = bs->GetCommunity(ip.c_str());
					vS.push_back(new CSwitcher(ip.c_str(),rsl.first.c_str(),rsl.second.c_str()));
					iterS = vS.end()-1;
				}

				if ((*iterS)->IsMacOnline(x.macAddress))
				{
					x.flag|=Online;
				}
				if ((*iterS)->IsMacSecuOnPort(x.macAddress,port))
				{
					x.flag|=OnSwitcher;
				}

				string gateway = (*iterS)->GetGateway().get_printable();
				auto iterR = find(vR.begin(),vR.end(),gateway);
				if (iterR==vR.end())
				{
					auto && rsl = bs->GetCommunity(gateway.c_str());
					vR.push_back(new CRouter(gateway.c_str(),rsl.first.c_str(),rsl.second.c_str()));
					iterR = vR.end()-1;
				}

				vIP.push_back((*iterR)->GetIPFromMac(x.macAddress)[0]);
			}
		});

		for (auto iter = vS.begin();iter!=vS.end();++iter)
		{
			delete *iter;
			*iter = nullptr;
		}

		for (auto iter = vR.begin();iter!=vR.end();++iter)
		{
			delete *iter;
			*iter = nullptr;
		}

		vR.clear();vS.clear();
	}
	return 0;
}


int CUserInfoShow::PrepareUserData(vector<user_info>& v,vector<vector<DWORD>>& pay_info,vector<IpAddress>& vIP)
{
	this->vIP = move(vIP);
	this->pay_info = move(pay_info);
	this->v = move(v);
	return 0;
}

int CUserInfoShow::FetchUserData(vector<user_info>& v,vector<vector<DWORD>>& pay_info,vector<IpAddress>& vIP)
{
	vIP = move(this->vIP);
	pay_info = move(this->pay_info);
	v = move(this->v);
	return 0;
}


int CUserInfoShow::RefershGrid(void)
{
#ifdef Business
	VMProtectBeginVirtualizationLockByKey("CUserInfoShow::RefershGrid");
	int status=0;
	status = VMProtectGetSerialNumberState();
	if (status)
	{
		MessageBox(_T("请不要耍小聪明！"),_T("LastOrder授权系统"),MB_ICONERROR);
		ExitProcess(-1);
	}
#endif // Business
	size_t pos=0;
	USES_CONVERSION;
	IpAddress invaildIP("0.0.0.0");
	for_each(v.begin(),v.end(),[&](user_info & x)
	{
		int nRow = InsertItem(GetItemCount(),A2W(x.userid.c_str()));
		SetItemText(nRow,1,A2W(x.name.c_str()));
		SetItemText(nRow,2,A2W(x.tel.c_str()));
		SetItemText(nRow,3,A2W(x.local.c_str()));
		SetItemText(nRow,4,A2W(x.macAddress.get_printable()));
		if (pos<vIP.size())
		{
			if (vIP[pos]!=invaildIP)
			{
				SetItemText(nRow,5,A2W(vIP[pos].get_printable()));
			}
			else
			{
				SetItemText(nRow,5,L"未获取到IP");
			}
		}
		SetCellChecked(nRow,6,x.flag&Online);
		SetCellEnabled(nRow,6,false);
		SetCellChecked(nRow,7,!(x.flag&OnSwitcher));
		if (x.flag&RegedUser)
		{
			if (x.flag&Payed)
			{
				if (!(x.flag&OnSwitcher))
				{
					SetLineColor(nRow,0x00FF00);//green - payed new user
				}
			}
			else
			{
				//payed
				if (x.flag&OnSwitcher)
				{
					SetLineColor(nRow,0xFF0000);//blue - non-payed old user
				}
				else
				{
					SetLineColor(nRow,0xC9AEFF);//pink - non-payed new user
				}
				
			}
		}
		else if (x.flag&OnSwitcher)
		{
			SetLineColor(nRow,0xFF0000);//blue old user
		}
		else
		{
			SetLineColor(nRow,0x0000FF);//red illegal user
		}
		++pos;
	});

#ifdef Business
	VMProtectEnd();
#endif // Business
	return 0;
}


vector<user_info>* CUserInfoShow::GetUserData(void)
{
	return &v;
}


void CUserInfoShow::Init(void)
{
#ifdef Business
	VMProtectBeginVirtualizationLockByKey("CUserInfoShow::Init");
	int status=0;
	status = VMProtectGetSerialNumberState();
	if (status)
	{
		MessageBox(_T("请不要耍小聪明！"),_T("LastOrder授权系统"),MB_ICONERROR);
		ExitProcess(-1);
	}
#endif // Business
	ModifyStyle(0, LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	SetExtendedStyle(GetExtendedStyle() |LVS_EX_FULLROWSELECT);

	InsertColumn(0, _T("用户名"), 80, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	InsertColumn(1, _T("真实姓名"), 80, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByString);
	InsertColumn(2, _T("电话"), 70, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByDigit);
	InsertColumn(3, _T("接入点"), 60, ListCtrlEx::Normal, LVCFMT_LEFT, ListCtrlEx::SortByString);
	InsertColumn(4, _T("MAC"), 100, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByString);
	InsertColumn(5, _T("IP"), 80, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByString);
	InsertColumn(6, _T("在线"), 36, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	InsertColumn(7, _T("屏蔽"), 36, ListCtrlEx::CheckBox, LVCFMT_CENTER, ListCtrlEx::SortByString);

	SetSupportSort(TRUE);

#ifdef Business
	VMProtectEnd();
#endif // Business
}

LRESULT CUserInfoShow::OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam)
{
	ListCtrlEx::CListCtrlEx* ctl =reinterpret_cast<ListCtrlEx::CListCtrlEx *> (wParam);
	ListCtrlEx::CheckCellMsg* msg =reinterpret_cast<ListCtrlEx::CheckCellMsg *>(lParam);
	USES_CONVERSION;
	switch (msg->m_nColumn)
	{
	case 7://屏蔽
		{
			string strMAC(W2A(GetItemText(msg->m_nRow,4)));
			MacAddress selMac(strMAC.c_str());
			auto iter = find(v.begin(),v.end(),selMac);
			if (iter!=v.end())
			{
				if (iter->flag & OnSwitcher)
				{
					if (MessageBox(_T("确认要屏蔽该用户么？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
					{
						bool status = false;
						if (s)
						{
							op->SendOper((LPSTR)s->GetDeviceIPAddress().get_printable(),
								(LPSTR)stringf("Delete MAC from port! Port: %d, MAC: %s, Name: %s",
								idxPort,strMAC.c_str(),W2A(GetItemText(msg->m_nRow,1))).c_str());
							if (!s->DeleteMacFromPort(idxPort,vector<MacAddress>(1,selMac)))
							{
								iter->flag&=~OnSwitcher;
								status = true;
							}
						}
						else
						{
							int port;
							string && ip = bs->GetIPFromDescrS(W2A(GetItemText(msg->m_nRow,3)),port);
							if (!ip.empty())
							{
								auto && rsl = bs->GetCommunity(ip.c_str());
								CSwitcher ss(ip.c_str(),rsl.first.c_str(),rsl.second.c_str());

								op->SendOper((LPSTR)ip.c_str(),
									(LPSTR)stringf("Delete MAC from port! Port: %d, MAC: %s, Name: %s",
									port,strMAC.c_str(),W2A(GetItemText(msg->m_nRow,1))).c_str());

								if (!ss.DeleteMacFromPort(port,vector<MacAddress>(1,selMac)))
								{
									iter->flag&=~OnSwitcher;
									status = true;
								}
							}
						}
						status?MessageBox(_T("用户屏蔽成功！"),_T("提示"),MB_ICONINFORMATION):
							MessageBox(_T("用户屏蔽失败！"),_T("警告"),MB_ICONWARNING);
					}
					else
					{
						ctl->SetCellChecked(msg->m_nRow, msg->m_nColumn, !msg->m_bChecked);
					}
				}
				else
				{
					if (MessageBox(_T("确认要将该用户加入安全列表？"),_T("提示"),MB_ICONQUESTION|MB_YESNO)==IDYES)
					{
						bool status = false;
						if (s)
						{
							op->SendOper((LPSTR)s->GetDeviceIPAddress().get_printable(),
								(LPSTR)stringf("Add MAC to port! Port: %d, MAC: %s, Name: %s",
								idxPort,strMAC.c_str(),W2A(GetItemText(msg->m_nRow,1))).c_str());
							if (!s->AddMacToPort(idxPort,vector<MacAddress>(1,selMac)))
							{
								iter->flag|=OnSwitcher;
								status = true;
							}
						}
						else
						{
							int port;
							string && ip = bs->GetIPFromDescrS(W2A(GetItemText(msg->m_nRow,3)),port);

							if (!ip.empty())
							{
								op->SendOper((LPSTR)ip.c_str(),
									(LPSTR)stringf("Add MAC to port! Port: %d, MAC: %s, Name: %s",
									port,strMAC.c_str(),W2A(GetItemText(msg->m_nRow,1))).c_str());

								auto && rsl = bs->GetCommunity(ip.c_str());
								CSwitcher ss(ip.c_str(),rsl.first.c_str(),rsl.second.c_str());

								if (!ss.AddMacToPort(port,vector<MacAddress>(1,selMac)))
								{
									iter->flag|=OnSwitcher;
									status = true;
								}
							}

						}
						status?MessageBox(_T("用户成功加入安全列表！"),_T("提示"),MB_ICONINFORMATION):
							MessageBox(_T("用户加入安全列表失败！"),_T("警告"),MB_ICONWARNING);
					}
					else
					{
						ctl->SetCellChecked(msg->m_nRow, msg->m_nColumn, !msg->m_bChecked);
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

void CUserInfoShow::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int x = pNMItemActivate->iItem;
	CMenu menu;
	menu.CreatePopupMenu();
	if (GetItemText(pNMItemActivate->iItem,3).IsEmpty())
	{
		menu.AppendMenu(MF_STRING,WM_NULL,_T("无当前用户付费信息！"));
	}
	else
	{
		menu.AppendMenu(MF_STRING,WM_NULL,
			stringf(_T("用户 %s 付费月份"),CString(v[x].name.c_str())).c_str());

		menu.AppendMenu(MF_SEPARATOR);


		for (auto iter = pay_info[x].begin();iter!=pay_info[x].end();++iter)
		{
			basic_string<TCHAR> && descr = stringf(_T("%04d年%02d月"),*iter>>8,*iter&0xFF);
			menu.AppendMenu(MF_STRING,WM_NULL,descr.c_str());
		}
	}
	CPoint pt;
	GetCursorPos(&pt);
	menu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
	menu.DestroyMenu();
	
	*pResult = 0;
}

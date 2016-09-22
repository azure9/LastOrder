// UserAuthDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Loading.h"
#include "UserInfo.h"
#include "Hoshizora_Beta.h"
#include "UserAuthDlg.h"
#include "afxdialogex.h"
#include "InputBox.h"


// CUserAuthDlg 对话框
extern COperMonitor* op;
IMPLEMENT_DYNAMIC(CUserAuthDlg, CDialogEx)

CUserAuthDlg::CUserAuthDlg(CWnd* pParent /*=NULL*/,const char* room,CSwitcher *session,int port)
	: CDialogEx(CUserAuthDlg::IDD, pParent),
	room(room),session(session),port(port)
{

}

CUserAuthDlg::~CUserAuthDlg()
{
}

void CUserAuthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CUserAuthDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CUserAuthDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CUserAuthDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_ListCtrlEx_CHECK_CHANGED, &CUserAuthDlg::OnListctrlexCheckChanged)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CUserAuthDlg::OnRclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CUserAuthDlg::OnDblclkList1)
//	ON_WM_MOVING()
	ON_WM_MOVE()
END_MESSAGE_MAP()


// CUserAuthDlg 消息处理程序


void CUserAuthDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	delete dlg;
	dlg = nullptr;
	CDialogEx::OnCancel();
}


void CUserAuthDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(_T("您确认需要更新本端口的安全数据么？"),_T("少女卖萌中"),MB_ICONQUESTION|MB_YESNO)==IDYES)
	{
		op->SendOper((LPSTR)session->GetDeviceIPAddress().get_printable(),
			(LPSTR)stringf("Refersh port secirty! Port: %d,Room: %s",port,room.c_str()).c_str());
		vector<user_info> *p = m_list.GetUserData();
		vector<MacAddress> payedUser;
		vector<MacAddress> illegalUser;

		for (auto iter = p->begin();iter!=p->end();++iter)
		{
			if (iter->name.empty())
			{
				continue;
			}

			if (iter->local!=room)
			{
				illegalUser.push_back(iter->macAddress);
				continue;
			}

			if (iter->flag&Payed)
			{
				if (iter->flag&OnSwitcher)
				{
					continue;
				}
				else
				{
					payedUser.push_back(iter->macAddress);//new user
					continue;
				}
			}
			else
			{
				if (iter->flag&OnSwitcher)
				{
					illegalUser.push_back(iter->macAddress);//user to delete
					continue;
				}
			}
		}
		if ((payedUser.size()>0?!(session->AddMacToPort(port,payedUser)):true)&&//运算优先级给跪……?:优先于&&运算Orz
			(illegalUser.size()>0?!(session->DeleteMacFromPort(port,illegalUser)):true))
		{
			MessageBox(_T("端口安全数据更新成功！"),_T("少女卖萌中"),MB_ICONINFORMATION);
		}
		else
		{
			MessageBox(_T("端口安全数据更新失败！"),_T("少女卖萌中"),MB_ICONWARNING);
		}
	}
}


BOOL CUserAuthDlg::OnInitDialog()
{
	CLoading loading;
	loading.Create(IDD_DLG_LOADING,NULL);
	loading.ShowWindow(SW_SHOW);
	loading.UpdateWindow();//(╯‵□′)╯︵┻━┻卧槽不Update神马也没有坑爹呢么
	CDialogEx::OnInitDialog();
	CUserInfo info;

	vector<user_info> && v = info.GetUserInfo(room.c_str());

	vector<MacAddress>&& mac = session->GetPortSecurityList(port);

	for (auto iter = mac.begin();iter!=mac.end();++iter)
	{
		auto it = find(v.begin(),v.end(),*iter);
		if (it!=v.end())
		{
			it->flag|=OnSwitcher;
			if (this->session->IsMacOnline(*iter))//重新判断下、因为第二次不会刷新全局Bridge表
			{
				it->flag|=Online;
			}
		}
		else
		{
			v.push_back(user_info("过期用户","","","",*iter,OnSwitcher,0));
			it = v.end() - 1;
			if (this->session->IsMacOnline(*iter))//重新判断下、因为第二次不会刷新全局Bridge表
			{
				it->flag|=Online;
			}
		}
	}

	vector<vector<DWORD>> pay_info;

	info.FillUserInfo(v,pay_info);
	//存在Mac Flood 攻击的时候查数据库太慢了、而且那么多地址都是废的、所以只查前面的就可以了

	multimap<int,MacAddress>&& onlineLst = session->GetBridge();
	auto iter = onlineLst.lower_bound(port);
	if (iter!=onlineLst.end())
	{
		for (;iter!=onlineLst.upper_bound(port);++iter)
		{
			auto it = find(v.begin(),v.end(),iter->second);
			if (it==v.end())
			{
				v.push_back(user_info("非法用户","","","",iter->second,Online,0));//我们只为交钱的用户提供更好的服务_(:з」∠)_
			}
		}
	}

	m_list.Init();
	m_list.PrepareUserData(v,pay_info,session,port);
	m_list.RefershGrid();
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CUserAuthDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(this->GetParent())
	{
		::PostMessage(this->GetParent()->m_hWnd,WM_SYSCOMMAND, SC_MOVE|0x0002,0);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

afx_msg LRESULT CUserAuthDlg::OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam)
{
	return m_list.OnListctrlexCheckChanged(wParam,lParam);
}

void CUserAuthDlg::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_list.OnRclickList1(pNMHDR,pResult);
}


void CUserAuthDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	selLine = pNMItemActivate->iItem+1;

	if (!v.size())
	{
		m_list.FetchUserData(v,pay_info,vIP);
	}

	switch (pNMItemActivate->iSubItem)
	{
	case 3://room
		{
			CInputBox ibox(this->GetSafeHwnd());
			if (ibox.DoModal(_T("提示"),
				stringf(_T("请在指定 %s 的新接入点\n示例：润杰1-504\n请保证和原格式一样否则用户将无法上网！"),
				CString(v[pNMItemActivate->iItem].name.c_str())).c_str())) 
			{
				if (_tcslen(ibox.Text))
				{
					if (!_tcschr(ibox.Text,_T('-')))
					{
						MessageBox(_T("接入点格式错误，请输入有效的接入点！"),_T("错误"),MB_ICONERROR);
						break;
					}
					USES_CONVERSION;
					CmsSQL sql;
					if (!sql.CheckTimeLegal(time(0)))
					{
						MessageBox(_T("系统时间错误，为防止潜在的危害，LastOrder拒绝启动！"),_T("错误"),MB_ICONERROR);
						ExitProcess(-1);
					}
					if (sql.UpdateUserRoom(v[pNMItemActivate->iItem].idx,W2A(ibox.Text)))
					{
						op->SendOper((LPSTR)session->GetDeviceIPAddress().get_printable(),
							(LPSTR)stringf("Change user registed room! Port: %d, Name: %s, Room: %s => %s",
							port,v[pNMItemActivate->iItem].name.c_str(),v[pNMItemActivate->iItem].local.c_str(),
							W2A(ibox.Text)).c_str());
						if (!session->DeleteMacFromPort(port,
							vector<MacAddress>(1,v[pNMItemActivate->iItem].macAddress)))
						{
							MessageBox(_T("注册信息更新成功！请手动刷新目标端口安全！"),_T("提示"),MB_ICONINFORMATION);
						}
						else
							MessageBox(_T("注册信息更新成功！请手动刷新端口安全！"),_T("警告"),MB_ICONWARNING);						
					}
					else
						MessageBox(_T("注册信息更新失败，请您检查网络！"),_T("错误"),MB_ICONERROR);
				}
			}
		}
		break;
	case 4://MAC
		{
			delete dlg;
			dlg = nullptr;

			int id = -1;
			int idx = 0;
			int count = 0;
			// TODO:  在此添加额外的初始化
			for each (user_info x in v)
			{
				if (!(x.flag & RegedUser))
				{
					id = idx;
					++count;
				}
				++idx;
			}

			if (count==1)
			{
				if (pNMItemActivate->iItem!=id)
				{
					if (MessageBox(
						stringf(
						_T("您确定要将 %s 的MAC地址改为 %s 么？"),
						CString((v)[pNMItemActivate->iItem].name.c_str()),
						CString((v)[id].macAddress.get_printable())).c_str(),
						_T("提示"),MB_ICONQUESTION|MB_YESNO
						)==IDYES)
					{
						RefershUserMAC(v[pNMItemActivate->iItem].idx,
							v[pNMItemActivate->iItem].tel.c_str(),v[id].macAddress,
							v[pNMItemActivate->iItem].name.c_str());
					}
					else
					{
						CustomMACDlg(pNMItemActivate->iItem);
					}
				}
			}
			else if (pNMItemActivate->iItem<v.size()&&id!=-1)
			{
				dlg = new CSelectMac(this,&v,pNMItemActivate->iItem);

				dlg->Create(IDD_SELECT_MAC,this);
				MoveSubWindow();
				dlg->ShowWindow(SW_SHOW);
			}
			else
			{
				CustomMACDlg(pNMItemActivate->iItem);
			}
		}
		break;
	}

	*pResult = 0;
}

int CUserAuthDlg::MoveSubWindow(void)
{
	if (dlg == 0)
	{
		return -1;
	}

	CRect rect;
	m_list.GetSubItemRect(selLine,0,0,rect);
	m_list.ClientToScreen(rect);
	CRect subRect;
	dlg->GetWindowRect(subRect);
	int height = subRect.Height();
	int width = subRect.Width();
	subRect.top = rect.top;
	subRect.bottom = rect.top+height;
	subRect.left = rect.left;
	subRect.right = rect.left+width;
	dlg->MoveWindow(subRect);
	return 0;
}


BOOL CUserAuthDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == SEL_MESSAGE)
	{
		int pos = (int)m_list.GetFirstSelectedItemPosition()-1;
		if (pMsg->wParam == -1)
		{
			CustomMACDlg(pos);
		}
		else
		{
			RefershUserMAC(v[pos].idx,v[pos].tel.c_str(),v[pMsg->wParam].macAddress,v[pos].name.c_str());
		}
		delete dlg;
		dlg = nullptr;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CUserAuthDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	delete dlg;
	dlg = nullptr;
	return CDialogEx::DestroyWindow();
}


void CUserAuthDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	MoveSubWindow();
	// TODO: 在此处添加消息处理程序代码
}


int CUserAuthDlg::CustomMACDlg(int iSel)
{
	if (iSel<v.size()&&v[iSel].flag & RegedUser)
	{
		CInputBox ibox(this->GetSafeHwnd());
		if (ibox.DoModal(_T("提示"),
			stringf(_T("请在下方输入 %s 的新MAC地址\n示例：00-00-00-00-00-00"),CString(v[iSel].name.c_str())).c_str())) 
		{
			USES_CONVERSION;
			MacAddress mac(W2A(ibox.Text));
			if (mac.valid())
			{
				RefershUserMAC(v[iSel].idx,v[iSel].tel.c_str(),mac,v[iSel].name.c_str());
			}
			else
				MessageBox(_T("MAC地址格式错误，请输入有效的MAC地址！"),_T("错误"),MB_ICONERROR);
		}
	}
	return 0;
}


int CUserAuthDlg::RefershUserMAC(int idx,const char *phone, MacAddress & mac,const char *lpUserName)
{
//	OutputDebugString(stringf(_T("idx:%d\nphone:%s\nnew MAC:%s"),idx,CString(phone),CString(mac.get_printable())).c_str());
	CmsSQL sql;
	if (!sql.CheckTimeLegal(time(0)))
	{
		MessageBox(_T("系统时间错误，为防止潜在的危害，LastOrder即将关闭！"),_T("错误"),MB_ICONERROR);
		ExitProcess(-1);
	}
	string && destMAC = stringf("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
		mac[0],mac[1],mac[2],
		mac[3],mac[4],mac[5]);
	if (sql.UpdateUserMac(idx,phone,destMAC.c_str()))
	{
		op->SendOper((LPSTR)session->GetDeviceIPAddress().get_printable(),
			(LPSTR)stringf("Change user registed MAC! Port: %d, Name: %s, => MAC: %s",
			port,lpUserName,destMAC.c_str()).c_str());
		auto iter = find(v.begin(),v.end(),idx);
		if (iter!=v.end())
		{
			if (session->DeleteMacFromPort(port,vector<MacAddress>(1,iter->macAddress)) && 
				session->AddMacToPort(port,vector<MacAddress>(1,mac)))
			{
				MessageBox(_T("注册信息更新成功！端口安全刷新成功！"),_T("提示"),MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(_T("注册信息更新成功！请手动刷新端口安全！"),_T("警告"),MB_ICONWARNING);
			}
			iter->macAddress = mac;
		}
	}
	else
	{
		MessageBox(_T("注册信息更新失败，请您检查网络！"),_T("错误"),MB_ICONERROR);
	}
	return 0;
}

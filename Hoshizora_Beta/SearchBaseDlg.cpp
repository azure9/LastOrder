// SearchDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Loading.h"
#include "UserInfo.h"
#include "UserInfoShow.h"
#include "SearchBaseDlg.h"
#include "SearchDlg.h"
#include "afxdialogex.h"


// CSearchDlg �Ի���

IMPLEMENT_DYNAMIC(CSearchBaseDlg, CDialogEx)

CSearchBaseDlg::CSearchBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchBaseDlg::IDD, pParent)
	, m_keyword(_T(""))
{

}

CSearchBaseDlg::~CSearchBaseDlg()
{
}

void CSearchBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE, m_type);
	DDX_Text(pDX, IDC_KEYWORD, m_keyword);
}


BEGIN_MESSAGE_MAP(CSearchBaseDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CSearchBaseDlg::OnBnClickedButton1)
//	ON_CBN_SELCHANGE(IDC_TYPE, &CSearchBaseDlg::OnCbnSelchangeType)
	ON_BN_CLICKED(IDC_BUTTON2, &CSearchBaseDlg::OnBnClickedButton2)
//	ON_BN_CLICKED(IDC_OK, &CSearchBaseDlg::OnBnClickedOk)
//ON_BN_CLICKED(IDOK, &CSearchBaseDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSearchDlg ��Ϣ�������


void CSearchBaseDlg::OnBnClickedButton1()
{
	CLoading loading;
	loading.Create(IDD_DLG_LOADING,NULL);
	loading.ShowWindow(SW_SHOW);
	loading.UpdateWindow();//(�s�F����)�s��ߩ����Բ۲�Update����Ҳû�пӵ���ô
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if (m_keyword.IsEmpty())
	{
		AfxMessageBox(_T("��ѯ�ؼ��ֲ���Ϊ�գ�"));
		return;
	}
	USES_CONVERSION;
	switch (m_type.GetCurSel())
	{
	case 0:
		{
			int iScheme = bs->GetScheme();
			SchemeContent empty(string("All"),string(""),string(""));
			auto && sc = iScheme == -1?empty:bs->LoadScheme(iScheme);
			string && cond = stringf(" LIKE '%%%s%%'",W2A(m_keyword));
			string && condPrefix = sc.prefix.empty()?string(""):stringf("����ص� LIKE '%s%%' and ",sc.prefix.c_str());
			string && queryStr = 
				stringf("%s(�û���%s or MAC��ַ%s or ����ص�%s or ��ʵ����%s or �绰%s)",
				condPrefix.c_str(),
				cond.c_str(),cond.c_str(),cond.c_str(),cond.c_str(),cond.c_str());
			CUserInfo info;
			vector<vector<DWORD>> pay_info;
			vector<user_info> && v = info.GetUserInfo(queryStr.c_str(),pay_info,CUSTOM);

			CSearchDlg *dlg = new CSearchDlg(this,&v,&pay_info);
			dlg->Create(IDD_DLG_SEARCH,this);
			dlg->ShowWindow(SW_SHOW);
		}
		break;
	case 1:
		{
			GenAddress addr(W2A(m_keyword));
			switch (addr.get_type())
			{
			case Address::type_ip:
				AfxMessageBox(_T("IP��ַ��֧�ֱ���������"));
				break;
			case Address::type_mac:
				{
					CUserInfo info;
					vector<vector<DWORD>> pay_info;
					vector<user_info> v;
					v.push_back(user_info("","","","",addr,0,0));
					info.FillUserInfo(v,pay_info,SEARCH);

					CSearchDlg *dlg = new CSearchDlg(this,&v,&pay_info);
					dlg->Create(IDD_DLG_SEARCH,this);
					dlg->ShowWindow(SW_SHOW);
				}
				break;
			default:
				AfxMessageBox(_T("��ѯ��ַ��Ч��"));
				return;
			}
		}
		break;
	default:
		break;
	}

}


BOOL CSearchBaseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_type.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSearchBaseDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CLoading loading;
	loading.Create(IDD_DLG_LOADING,NULL);
	loading.ShowWindow(SW_SHOW);
	loading.UpdateWindow();//(�s�F����)�s��ߩ����Բ۲�Update����Ҳû�пӵ���ô
	switch (m_type.GetCurSel())
	{
	case 0:
		{
			AfxMessageBox(_T("�û�������Ϣ��֧�ֱ���������"));
		}
		break;
	case 1:
		{
			UpdateData(true);
			USES_CONVERSION;
			GenAddress addr(W2A(m_keyword));
			switch (addr.get_type())
			{
			case Address::type_ip:
				{
					vector<user_info> vInfo;
					MacAddress mac;
					IpAddress ip = static_cast<IpAddress>(addr);

					auto && vIPR = bs->GetAllIPR();
					for (auto iter = vIPR.begin();iter!=vIPR.end();++iter)
					{
						auto && rsl = bs->GetCommunity(iter->c_str());
						CRouter sr(iter->c_str(),rsl.first.c_str(),rsl.second.c_str());

						mac = sr.GetMacFromIP(ip);
						if (mac.valid())
						{
							break;
						}
					}

					if (mac.valid())
					{
						int idxPort = -1;
						CSwitcher *ss = nullptr;
						vector<string> && v = bs->IPImagination(stringf("%d.%d.%d",ip[0],ip[1],ip[2]),0);
						sort(v.begin(),v.end(),greater<string>());

						bool bFindS = false;
						try
						{
							for (auto iter = v.begin();iter!=v.end();delete ss, ss= nullptr, ++iter)
							{
								auto && rsl = bs->GetCommunity(iter->c_str());
								ss = new CSwitcher(iter->c_str(),rsl.first.c_str(),rsl.second.c_str());
								idxPort = ss->IsMacOnline(mac);
								if (idxPort&&ss->GetPortCnt()-2>idxPort)
								{
									bFindS = true;
									break;
								}
							}
						}
						catch (...)
						{
						}


						if (bFindS)
						{
							DWORD flag = 0;
							flag|=Online;
							if (ss->IsMacSecuOnPort(mac,idxPort))
							{
								flag|=OnSwitcher;
							}
							string &&room = bs->GetPortDescrS(ip.get_printable(),idxPort);
							vInfo.push_back(user_info("�����û�","","",
								(char *)(room.empty()?ip.get_printable():room.c_str()),mac,flag,0));

							delete ss;
							ss = nullptr;
						}
						else
						{
							vInfo.push_back(user_info("δ֪�û�","","","",mac,0,0));
						}

						CUserInfo info;
						vector<vector<DWORD>> pay_info;
						info.FillUserInfo(vInfo,pay_info,SEARCH);

						vector<IpAddress> vIP;
						vIP.push_back(move(static_cast<IpAddress>(addr)));

						CSearchDlg *dlg = new CSearchDlg(this,&vIP,&vInfo,&pay_info);
						dlg->Create(IDD_DLG_SEARCH,this);
						dlg->ShowWindow(SW_SHOW);
					}
					else
					{
						AfxMessageBox(_T("δ�ҵ�ָ��IP��"));
					}
				}
				break;
			case Address::type_mac:
				{
					vector<string> && v = bs->GetAllGateway();
					
					vector<IpAddress> vR;
					vector<user_info> vInfo;
					for (auto iter = v.begin();iter!=v.end();++iter)
					{
						IpAddress ssIP = iter->c_str();
						{
							int idxPort = -1;
							CSwitcher *ss = nullptr;

							vector<string> && vlanSIP = bs->IPImagination(stringf("%d.%d.%d",ssIP[0],ssIP[1],ssIP[2]),0);
							sort(vlanSIP.begin(),vlanSIP.end(),greater<string>());

							bool bFindS = false;
							try
							{
								for (auto iter = vlanSIP.begin();iter!=vlanSIP.end();delete ss, ss= nullptr, ++iter)
								{
									auto && rsl = bs->GetCommunity(ssIP.get_printable());
									ss = new CSwitcher(ssIP.get_printable(),rsl.first.c_str(),rsl.second.c_str());
									idxPort = ss->IsMacOnline(static_cast<MacAddress>(addr));
									if (idxPort&&ss->GetPortCnt()-2>idxPort)
									{
										bFindS = true;
										break;
									}
									else if (iter == vlanSIP.begin())
									{
										break;
									}
								}
							}
							catch (...)
							{
							}
							
							
							if (bFindS)
							{
								DWORD flag = 0;
								//����Switcher����IP
								vR.push_back(move(ss->GetGateway()));
								flag|=Online;
								if (ss->IsMacSecuOnPort(static_cast<MacAddress>(addr),idxPort))
								{
									flag|=OnSwitcher;
								}
								string &&room = bs->GetPortDescrS(ssIP.get_printable(),idxPort);
								vInfo.push_back(user_info("","",
									vR[vR.size()-1].get_printable(),
									(room.empty()?ssIP.get_printable():room.c_str()),
									static_cast<MacAddress>(addr),flag,0));
							}

							delete ss;
							ss = nullptr;
						}
					}

					vector<IpAddress> vsumIP;

					for (auto iter = vR.begin();iter!=vR.end();++iter)
					{
						auto && rsl = bs->GetCommunity(iter->get_printable());
						CRouter sr(iter->get_printable(),rsl.first.c_str(),rsl.second.c_str());
						vector<IpAddress>&& vIP = sr.GetIPFromMac(static_cast<MacAddress>(addr));
						auto iterS = find(vInfo.begin(),vInfo.end(),(char *)iter->get_printable());
						if (iterS!=vInfo.end()&&
							vIP.size()>1)
						{
							vInfo.insert(iterS,vIP.size()-1,*iterS);
						}
						if (vIP[0]!="0.0.0.0")
						{
							vsumIP.insert(vsumIP.end(),vIP.begin(),vIP.end());
						}
					}
					if (!vsumIP.size())
					{
						vsumIP.push_back("0.0.0.0");
					}

					CUserInfo info;
					vector<vector<DWORD>> pay_info;
					info.FillUserInfo(vInfo,pay_info,SEARCH);

					CSearchDlg *dlg = new CSearchDlg(this,&vsumIP,&vInfo,&pay_info);
					dlg->Create(IDD_DLG_SEARCH,this);
					dlg->ShowWindow(SW_SHOW);
				}
				break;
			default:
				AfxMessageBox(_T("��ѯ��ַ��Ч��"));
				return;
			}
		}
		break;
	default:
		break;
	}
}


BOOL CSearchBaseDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN
		&& pMsg->wParam == VK_RETURN
		&& pMsg->hwnd != GetDlgItem(IDOK)->GetSafeHwnd())
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

// AdvanceBase.cpp : ʵ���ļ�
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


// CAdvanceBase �Ի���

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


// CAdvanceBase ��Ϣ�������

DWORD WINAPI DummyRefersh(LPVOID lParma)
{
	if ((int)lParma==2)
	{
		return CSwitcherManager::DeltaUpdate();
	}
	vector<string> && vIP = bs->GetAllIPS();

	if ((int)lParma==0&&MessageBox(NULL,
		L"��ѡ���ѡ�����������ʼ���������������Ƽ��ô�ģʽ��Ϊ�ճ�������ˢ�·������˷�ʽ������ɲ�������˿��û��޷��������磬���ٴ�ȷ���Ƿ��ø�ģʽˢ�¡�",
		L"��Ů������",
		MB_ICONASTERISK|MB_YESNO)!=IDYES)
	{
		MessageBox(NULL,L"����͡���л��֧��LastOrder��",L"��Ů������",MB_ICONINFORMATION);
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
			//MessageBox(NULL,stringf(_T("������ %s ����ʧ�ܣ���ȷ�Ͻ�����״̬��"),CString(vIP[index].c_str())).c_str(),L"����",MB_ICONERROR);
		}
		delete session;
	});

	//MessageBox(NULL,_T("��ϲ�����������ע�ύ�������á�"),_T("��Ů������"),MB_ICONINFORMATION);
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
				reinterpret_cast<LPARAM>(stringf(_T("�������ý����� %s ..."),CString(vIP[index].c_str())).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

			session->ResetSession();
			SendMessage(hList,LB_ADDSTRING,0,
				reinterpret_cast<LPARAM>(stringf(_T("%s ������ɣ����޸� %d ����Ŀ��"),
				CString(vIP[index].c_str()),session->UtilOneKeySet(dlg->m_setType==0?1:0)).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
		}
		catch (...)
		{
			SendMessage(hList,LB_ADDSTRING,0,
				reinterpret_cast<LPARAM>(stringf(_T("������ %s ����ʧ�ܣ���ȷ�Ͻ�����״̬��"),CString(vIP[index].c_str())).c_str()));
			SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
		}
		delete session;
	});

	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("��ϲ�����������ע�ύ�������á�")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	dlg->MessageBox(_T("��ϲ�����������ע�ύ�������á�"),_T("��Ů������"),MB_ICONINFORMATION);
	return 0;
}

DWORD WINAPI deltaThread(LPVOID lParma)
{
	CAdvanceBase *dlg = reinterpret_cast<CAdvanceBase *>(lParma);

	HWND hList = GetDlgItem(dlg->GetSafeHwnd(),IDC_LIST1);
	SendMessage(hList,LB_RESETCONTENT,0,0);


	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("�������� ʼ�ޤ�衫_(:�١���)_")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);
	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(_T("�������һ�ֽ������Ĳ���֢֮�����������潻���˨r(������\")�q ")));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	SendMessage(hList,LB_ADDSTRING,0,
		reinterpret_cast<LPARAM>(stringf(_T("��ϲ�������%dλ�û���ȫ���ݸ��¡�"),CSwitcherManager::DeltaUpdate()).c_str()));
	SendMessage(hList,WM_VSCROLL, SB_BOTTOM, 0);

	dlg->MessageBox(_T("��ϲ��������û���ȫ���ݲ���������"),_T("��Ů������"),MB_ICONINFORMATION);
	return 0;
}

void CAdvanceBase::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// add concurrent lock. Don't allow user to refresh list multi thread.
	UpdateData(true);
	DWORD tid;
	if (m_setType==2)
	{
		CreateThread(0,0,deltaThread,this,0,&tid);
		return;
	}
	else if (m_setType==1&&MessageBox(
		L"��ѡ���ѡ�����������ʼ���������������Ƽ��ô�ģʽ��Ϊ�ճ�������ˢ�·������˷�ʽ������ɲ�������˿��û��޷��������磬���ٴ�ȷ���Ƿ��ø�ģʽˢ�¡�",
		L"��Ů������",
		MB_ICONASTERISK|MB_YESNO)!=IDYES)
	{
		MessageBox(L"����͡���л��֧��LastOrder��",L"��Ů������",MB_ICONINFORMATION);
		return;
	}
	CreateThread(0,0,newThread,this,0,&tid);
}


BOOL CAdvanceBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

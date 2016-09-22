
// Hoshizora_BetaDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "Hoshizora_Beta.h"
#include "Hoshizora_BetaDlg.h"
#include "VMProtectSDK.h"


extern CString timestamp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHoshizora_BetaDlg �Ի���



CHoshizora_BetaDlg::CHoshizora_BetaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHoshizora_BetaDlg::IDD, pParent),
	page1(new CSwitcherBaseDlg(this)),
	page3(new CSearchBaseDlg(this)),
	page5(new CDeviceMonitor(this)),
	page6(new CAdvanceBase(this)),
	page7(new CSettings(this))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHoshizora_BetaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

BEGIN_MESSAGE_MAP(CHoshizora_BetaDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDCANCEL, &CHoshizora_BetaDlg::OnBnClickedCancel)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CHoshizora_BetaDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CHoshizora_BetaDlg ��Ϣ�������

BOOL CHoshizora_BetaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_tab.InsertItem(0,_T("����������"));
	m_tab.InsertItem(1,_T("·�ɹ���"));
	m_tab.InsertItem(2,_T("��Ϣ����"));
	m_tab.InsertItem(3,_T("Trap����"));
	m_tab.InsertItem(4,_T("�豸���"));
	m_tab.InsertItem(5,_T("�߼�"));
	m_tab.InsertItem(6,_T("����"));

	//��������ҳ��ҳ 
	page1->Create(IDD_BASE_SWITCHER,GetDlgItem(IDC_TAB1));
	page2.Create(IDD_BASE_ROUTER,GetDlgItem(IDC_TAB1));
	page3->Create(IDD_BASE_SEARCH,GetDlgItem(IDC_TAB1));
	page4.Create(IDD_BASE_TRAP,GetDlgItem(IDC_TAB1));
	page5->Create(IDD_BASE_MONITOR,GetDlgItem(IDC_TAB1));
	page6->Create(IDD_BASE_ADVANCE,GetDlgItem(IDC_TAB1));
	page7->Create(IDD_BASE_SETTINGS,GetDlgItem(IDC_TAB1));

	//����ҳ���λ����m_tab�ؼ���Χ�� 
	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.top+=24; 
	rect.bottom-=4; 
	rect.left+=4; 
	rect.right-=4; 
	page1->MoveWindow(&rect);
	page1->ShowWindow(TRUE);
	page2.MoveWindow(&rect);
	page3->MoveWindow(&rect);
	page4.MoveWindow(&rect);
	page5->MoveWindow(&rect);
	page6->MoveWindow(&rect);
	page7->MoveWindow(&rect);
	m_tab.SetCurSel(0);

	SetDlgItemText(IDC_BUILD,CString("��������: ")+timestamp);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CHoshizora_BetaDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHoshizora_BetaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHoshizora_BetaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHoshizora_BetaDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int CurSel; 
	CurSel=m_tab.GetCurSel();
	int status=0;

#ifdef Business
	status = VMProtectGetSerialNumberState();
#endif // Business
	switch(CurSel) 
	{ 
	case 0:
		page1->ShowWindow(TRUE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(FALSE); 
		break; 
	case 1: 
		page1->ShowWindow(FALSE);
		page2.ShowWindow(TRUE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(FALSE); 
		break; 
	case 2: 
		page1->ShowWindow(FALSE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(status==0?TRUE:FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(FALSE);  
		break; 
	case 3:
		page1->ShowWindow(FALSE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(TRUE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(FALSE); 
		break;
	case 4:
		page1->ShowWindow(FALSE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(TRUE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(FALSE); 
		break;
	case 5:
		page1->ShowWindow(FALSE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(status==0?TRUE:FALSE); 
		page7->ShowWindow(FALSE); 
		break;
	case 6:
		page1->ShowWindow(FALSE);
		page2.ShowWindow(FALSE); 
		page3->ShowWindow(FALSE); 
		page4.ShowWindow(FALSE); 
		page5->ShowWindow(FALSE); 
		page6->ShowWindow(FALSE); 
		page7->ShowWindow(TRUE); 
		break;
	default:
		break;
	} 
	*pResult = 0;
}

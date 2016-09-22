
// Hoshizora_BetaDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CHoshizora_BetaDlg 对话框



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


// CHoshizora_BetaDlg 消息处理程序

BOOL CHoshizora_BetaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_tab.InsertItem(0,_T("交换机管理"));
	m_tab.InsertItem(1,_T("路由管理"));
	m_tab.InsertItem(2,_T("信息检索"));
	m_tab.InsertItem(3,_T("Trap监听"));
	m_tab.InsertItem(4,_T("设备监控"));
	m_tab.InsertItem(5,_T("高级"));
	m_tab.InsertItem(6,_T("设置"));

	//建立属性页各页 
	page1->Create(IDD_BASE_SWITCHER,GetDlgItem(IDC_TAB1));
	page2.Create(IDD_BASE_ROUTER,GetDlgItem(IDC_TAB1));
	page3->Create(IDD_BASE_SEARCH,GetDlgItem(IDC_TAB1));
	page4.Create(IDD_BASE_TRAP,GetDlgItem(IDC_TAB1));
	page5->Create(IDD_BASE_MONITOR,GetDlgItem(IDC_TAB1));
	page6->Create(IDD_BASE_ADVANCE,GetDlgItem(IDC_TAB1));
	page7->Create(IDD_BASE_SETTINGS,GetDlgItem(IDC_TAB1));

	//设置页面的位置在m_tab控件范围内 
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

	SetDlgItemText(IDC_BUILD,CString("编译日期: ")+timestamp);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHoshizora_BetaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHoshizora_BetaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHoshizora_BetaDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
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

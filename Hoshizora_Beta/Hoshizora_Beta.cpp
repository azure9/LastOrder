
// Hoshizora_Beta.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Hoshizora_BetaDlg.h"
#include "Eula.h"
#include "Splash.h"
#include "msSQL.h"

#include "VMProtectSDK.h"


#define VERSION_DESCR _T("2.1")
size_t LastLineID = 0;
DWORD time_magic = 0;


Snmp *snmp = nullptr;
CString timestamp;
CBaseInfo *bs;
COperMonitor* op;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI DummyRefersh(LPVOID lParma);
// CHoshizora_BetaApp

BEGIN_MESSAGE_MAP(CHoshizora_BetaApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHoshizora_BetaApp 构造

CHoshizora_BetaApp::CHoshizora_BetaApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CHoshizora_BetaApp 对象

CHoshizora_BetaApp theApp;

__forceinline char *read_serial(const char *fname)
{
	FILE *f;
	if (0 != fopen_s(&f, fname, "rb")) return NULL;
	fseek(f, 0, SEEK_END);
	int s = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = new char[s + 1];
	fread(buf, s, 1, f);
	buf[s] = 0;
	fclose(f);
	return buf;
}

char *GetComplierDate(PIMAGE_DOS_HEADER pDH)
{
	//	PIMAGE_DOS_HEADER pDH = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS32 pNtH = (PIMAGE_NT_HEADERS32)((char *)pDH+pDH->e_lfanew);
	return ctime((time_t *)&pNtH->FileHeader.TimeDateStamp);
}

// CHoshizora_BetaApp 初始化
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#ifdef Business
#pragma optimize( "g", off )
#endif // Business
BOOL CHoshizora_BetaApp::InitInstance()
{
	VMProtectSerialNumberData sd = {0};
	SetRegistryKey(_T("Moeware"));
	op = new COperMonitor;
#ifdef Business
	int status = GetProfileInt(_T("LastOrder"),_T("FirstRun"),0);
	switch (status)
	{
	case 0:
		{
			CEula dlg;
			if (dlg.DoModal()==IDOK)
			{
				WriteProfileInt(_T("LastOrder"),_T("FirstRun"),1);
				break;
			}
			else
				return 0;
		}
		break;
	case 1:
		break;
	}
	VMProtectBeginUltra("Startup check");

	char *serial = read_serial("LastOrder.key");

	if (serial)
	{
		int res = VMProtectSetSerialNumber(serial);
		delete [] serial;

		VMProtectGetSerialNumberData(&sd, sizeof(sd));

		if (res)
		{
			op->SendOper("localhost","Invaild License!");
			MessageBox(NULL,_T("您当前使用的授权无效！"),_T("LastOrder授权验证"),MB_ICONERROR);
			return 0;
		}
		else if(!VMProtectIsValidImageCRC())
		{
			op->SendOper("localhost","Image checksum wrong!");
			MessageBox(NULL,_T("由于检测到您违反了EULA，LastOrder拒绝运行！"),_T("LastOrder授权验证"),MB_ICONERROR);
			return 0;
		}
	}
	else
	{
		op->SendOper("localhost","Demo run!");
		MessageBox(NULL,_T("LastOrder检测不到您的授权文件，将以Demo模式运行。\nDemo模式存在功能限制，推荐使用正版。"),_T("LastOrder授权验证"),MB_ICONINFORMATION);
		sd.dtExpire.wYear = 9999;sd.dtExpire.bMonth = 12; sd.dtExpire.bDay=31;
		wcscpy(sd.wUserName,L"demo");
		VMProtectSetSerialNumber(NULL);
	}
	
	VMProtectEnd();
#else
	wcscpy(sd.wUserName,L"x64 developer");
	sd.dtExpire.wYear = 9999;sd.dtExpire.bMonth = 12; sd.dtExpire.bDay=31;
#endif // Business

	if (GetFileAttributes(_T("data.db"))==-1)
	{
		op->SendOper("localhost","Datebase missing!");
		MessageBox(NULL,_T("本地数据库不存在！"),_T("程序初始化失败"),MB_ICONERROR);
		return FALSE;
	}
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;  
	ULONG_PTR gdiplusToken;  
	
	if(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok)  
	{  
		MessageBox(NULL, _T("Gdiplus Startup failed!"), _T("程序初始化失败"), MB_ICONERROR);
		return FALSE;
	}  

	timestamp = GetComplierDate((PIMAGE_DOS_HEADER)GetModuleHandle(NULL));
	CSplash *loading = new CSplash;

	wstring title(0x400,0);
	size_t cbBaseMsg = 0;
	{
		CString message;
		message.Format(_T("授权用户：%s\n授权到期时间：%d年%d月%d日\n\n\n当前版本：%s\n编译日期：%s"),
			sd.wUserName,
			sd.dtExpire.wYear,sd.dtExpire.bMonth,sd.dtExpire.bDay,
			VERSION_DESCR,timestamp);
		title = message;
		cbBaseMsg = message.GetLength();
	}

	loading->Create((WCHAR *)title.c_str());
	loading->ShowWindow(SW_SHOW);
	loading->UpdateWindow();//(╯‵□′)╯︵┻━┻卧槽不Update神马也没有坑爹呢么

	{
		int status;
		Snmp::socket_startup();
		snmp = new Snmp(status,0,false);
		bs = new CBaseInfo("data.db");
	}

	LastLineID = GetProfileInt(_T("LastOrder"), _T("LastLineID"), 0);

	//////////////////////////////////////////////////////////////////////////
	//	Database update
	{
#ifdef Business
		VMProtectBeginUltra("CheckTime");
#endif // Business
		CmsSQL* sql = nullptr;
		try
		{
			sql = new CmsSQL;
			if (!sql->CheckTimeLegal(time(0)))
			{
				op->SendOper("localhost","Invalid date! ");
				MessageBox(NULL,_T("系统时间与服务器不一致，为防止潜在的危害，LastOrder拒绝启动！请您与服务器校对时间。"),_T("错误"),MB_ICONERROR);
				ExitProcess(-1);
			}
		}
		catch (...)
		{
			op->SendOper("localhost","Cannot connect DB server!");
			MessageBox(NULL,_T("无法与数据库建立连接，LastOrder将以上次数据库启动！"),_T("错误"),MB_ICONERROR);
			goto cleanup;
		}

		title += _T("\n\n正在更新本地数据库……");
		loading->Invalidate();
		loading->UpdateWindow();

		vector<SchemeContent> & vSC = bs->LoadScheme();
		vector<vector<pair<string,string>>> v;

#ifdef Business
		VMProtectEnd();
#endif // Business

		for (size_t idx = 0;idx<vSC.size();++idx)
		{
			vector<pair<string,string>> vSchemeData;
			sql->GetDeviceData(vSchemeData,vSC[idx].prefix.c_str());

			v.push_back(move(vSchemeData));
		}

		delete sql;

		bs->UpdateAllSchemeDB(v);
		Sleep(500);
	}
cleanup:
	//////////////////////////////////////////////////////////////////////////
	bs->SetScheme(GetProfileInt(_T("LastOrder"),_T("Scheme"),-1));
	time_t ltime;
	time( &ltime );
	tm *today;
	today = localtime( &ltime );
	time_magic = (today->tm_year+1900)<<8|today->tm_mon+1;

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	int argc = 0;
	LPWSTR *lpszArgv = NULL;
	LPTSTR szCmdLine = GetCommandLine(); //获取命令行参数；
	lpszArgv = CommandLineToArgvW(szCmdLine, &argc); //拆分命令行参数字符串；
	if (argc >= 2) {
		wstring cmd(lpszArgv[1]);
		if (cmd[0]==L'/'&&tolower(cmd[1])==L'r')
		{
			int TypeId = 2;//default delta update
			if (cmd.size()>2)
			{
				TypeId = cmd[2]-'0';
			}
			if (TypeId<3)
			{
				op->SendOper("localhost",(LPSTR)stringf("Command refersh! Scheme id:%d",bs->GetScheme()).c_str());
				_tcscpy(&title[cbBaseMsg],_T("\n\n正在刷新端口数据……"));
				loading->Invalidate();
				loading->UpdateWindow();
				DummyRefersh((LPVOID)TypeId);
				delete bs;
				return FALSE;
			}
		}
		MessageBox(NULL,L"未知指令，请联系开发团队！",L"错误",MB_ICONERROR);
		return FALSE;
	}


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
//	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CHoshizora_BetaDlg dlg;
	m_pMainWnd = &dlg;

	_tcscpy(&title[cbBaseMsg],_T("\n\n初始化完毕。等待启动……"));
	loading->Invalidate();
	loading->UpdateWindow();

	Sleep(1000);

	delete loading;
	op->SendOper("localhost",(LPSTR)stringf("LastOrder %ls init complete!",VERSION_DESCR).c_str());
	INT_PTR nResponse = dlg.DoModal();
	Gdiplus::GdiplusShutdown(gdiplusToken);
	delete bs;
	delete snmp;
	Snmp::socket_cleanup();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
#ifdef Business
#pragma optimize( "g", on )
#endif // Business
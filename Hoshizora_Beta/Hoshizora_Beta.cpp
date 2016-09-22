
// Hoshizora_Beta.cpp : ����Ӧ�ó��������Ϊ��
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


// CHoshizora_BetaApp ����

CHoshizora_BetaApp::CHoshizora_BetaApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CHoshizora_BetaApp ����

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

// CHoshizora_BetaApp ��ʼ��
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
			MessageBox(NULL,_T("����ǰʹ�õ���Ȩ��Ч��"),_T("LastOrder��Ȩ��֤"),MB_ICONERROR);
			return 0;
		}
		else if(!VMProtectIsValidImageCRC())
		{
			op->SendOper("localhost","Image checksum wrong!");
			MessageBox(NULL,_T("���ڼ�⵽��Υ����EULA��LastOrder�ܾ����У�"),_T("LastOrder��Ȩ��֤"),MB_ICONERROR);
			return 0;
		}
	}
	else
	{
		op->SendOper("localhost","Demo run!");
		MessageBox(NULL,_T("LastOrder��ⲻ��������Ȩ�ļ�������Demoģʽ���С�\nDemoģʽ���ڹ������ƣ��Ƽ�ʹ�����档"),_T("LastOrder��Ȩ��֤"),MB_ICONINFORMATION);
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
		MessageBox(NULL,_T("�������ݿⲻ���ڣ�"),_T("�����ʼ��ʧ��"),MB_ICONERROR);
		return FALSE;
	}
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;  
	ULONG_PTR gdiplusToken;  
	
	if(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok)  
	{  
		MessageBox(NULL, _T("Gdiplus Startup failed!"), _T("�����ʼ��ʧ��"), MB_ICONERROR);
		return FALSE;
	}  

	timestamp = GetComplierDate((PIMAGE_DOS_HEADER)GetModuleHandle(NULL));
	CSplash *loading = new CSplash;

	wstring title(0x400,0);
	size_t cbBaseMsg = 0;
	{
		CString message;
		message.Format(_T("��Ȩ�û���%s\n��Ȩ����ʱ�䣺%d��%d��%d��\n\n\n��ǰ�汾��%s\n�������ڣ�%s"),
			sd.wUserName,
			sd.dtExpire.wYear,sd.dtExpire.bMonth,sd.dtExpire.bDay,
			VERSION_DESCR,timestamp);
		title = message;
		cbBaseMsg = message.GetLength();
	}

	loading->Create((WCHAR *)title.c_str());
	loading->ShowWindow(SW_SHOW);
	loading->UpdateWindow();//(�s�F����)�s��ߩ����Բ۲�Update����Ҳû�пӵ���ô

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
				MessageBox(NULL,_T("ϵͳʱ�����������һ�£�Ϊ��ֹǱ�ڵ�Σ����LastOrder�ܾ������������������У��ʱ�䡣"),_T("����"),MB_ICONERROR);
				ExitProcess(-1);
			}
		}
		catch (...)
		{
			op->SendOper("localhost","Cannot connect DB server!");
			MessageBox(NULL,_T("�޷������ݿ⽨�����ӣ�LastOrder�����ϴ����ݿ�������"),_T("����"),MB_ICONERROR);
			goto cleanup;
		}

		title += _T("\n\n���ڸ��±������ݿ⡭��");
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

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	int argc = 0;
	LPWSTR *lpszArgv = NULL;
	LPTSTR szCmdLine = GetCommandLine(); //��ȡ�����в�����
	lpszArgv = CommandLineToArgvW(szCmdLine, &argc); //��������в����ַ�����
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
				_tcscpy(&title[cbBaseMsg],_T("\n\n����ˢ�¶˿����ݡ���"));
				loading->Invalidate();
				loading->UpdateWindow();
				DummyRefersh((LPVOID)TypeId);
				delete bs;
				return FALSE;
			}
		}
		MessageBox(NULL,L"δָ֪�����ϵ�����Ŷӣ�",L"����",MB_ICONERROR);
		return FALSE;
	}


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
//	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CHoshizora_BetaDlg dlg;
	m_pMainWnd = &dlg;

	_tcscpy(&title[cbBaseMsg],_T("\n\n��ʼ����ϡ��ȴ���������"));
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
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
#ifdef Business
#pragma optimize( "g", on )
#endif // Business
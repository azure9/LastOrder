#include "stdafx.h"
#include "OperMonitor.h"
#include "assert.h"
#pragma comment(lib,"Iphlpapi.lib")

#define REPORT_SERVER "202.204.193.190"//"10.10.200.18"//"202.204.193.190"
#define SERVER_PORT 9999

unsigned char ToHex(unsigned char x)   
{   
	return  x > 9 ? x + 55 : x + 48;   
}  

unsigned char FromHex(unsigned char x)   
{   
	unsigned char y;  
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;  
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;  
	else if (x >= '0' && x <= '9') y = x - '0';  
	else assert(0);  
	return y;  
}  

std::string UrlEncode(const std::string& str)  
{  
	std::string strTemp = "";  
	size_t length = str.length();  
	for (size_t i = 0; i < length; i++)  
	{  
		if (isalnum((unsigned char)str[i]) ||   
			(str[i] == '-') ||  
			(str[i] == '_') ||   
			(str[i] == '.') ||   
			(str[i] == '~'))  
			strTemp += str[i];  
		else if (str[i] == ' ')  
			strTemp += "+";  
		else  
		{  
			strTemp += '%';  
			strTemp += ToHex((unsigned char)str[i] >> 4);  
			strTemp += ToHex((unsigned char)str[i] % 16);  
		}  
	}  
	return strTemp;  
}  

wstring GBK2UNI(const char* pGBK, long nBytesCount)
{
	DWORD dwMinSize = MultiByteToWideChar(936, 0, pGBK, nBytesCount, NULL, 0);
	wstring strUNI(dwMinSize,0);
	if (MultiByteToWideChar(936, 0, (LPCSTR)pGBK, nBytesCount, (WCHAR*)strUNI.c_str(), dwMinSize) > 0)
	{
		return (strUNI);
	}
	return (strUNI);
}

string UNI2UTF8(const WCHAR* pUNI, long nBytesCount)
{
	DWORD dwMinSize = WideCharToMultiByte(CP_UTF8, 0, pUNI, nBytesCount, NULL, 0,0,0);
	string strGBK(max(dwMinSize,nBytesCount),0);

	if (WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pUNI, dwMinSize, (char *)strGBK.c_str(), dwMinSize,0,0) > 0)
	{
		return strGBK;
	}
	return strGBK;
}



COperMonitor::COperMonitor(void)
{
#ifndef kDays
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	if (ERROR_BUFFER_OVERFLOW==nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
	}

	string && currentIP = GetIP();

	PIP_ADAPTER_INFO pSave = pIpAdapterInfo;

	if (ERROR_SUCCESS==nRel)
	{
		//输出网卡信息
		while (pIpAdapterInfo)
		{
			//cout<<"网卡名称："<<pIpAdapterInfo->AdapterName<<endl;
			//cout<<"网卡描述："<<pIpAdapterInfo->Description<<endl;
			//cout<<"网卡MAC地址："<<pIpAdapterInfo->Address;
			string && tempMac = stringf("%02x-%02x-%02x-%02x-%02x-%02x",
				pIpAdapterInfo->Address[0],pIpAdapterInfo->Address[1],pIpAdapterInfo->Address[2],
				pIpAdapterInfo->Address[3],pIpAdapterInfo->Address[4],pIpAdapterInfo->Address[5]);
			//cout<<"网卡IP地址如下："<<endl;
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
			do 
			{
				if (pIpAddrString->IpAddress.String==currentIP)
				{
					mac = tempMac;
					goto cleanup;
				}
				//cout<<pIpAddrString->IpAddress.String<<endl;
				pIpAddrString=pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//释放内存空间
cleanup:
	if (pSave)
	{
		delete pSave;
	}
#endif
}


COperMonitor::~COperMonitor(void)
{
}

void COperMonitor::SendOper(LPSTR lpDestIP,LPSTR lpOper)
{
#ifndef kDays
	HINTERNET hInternet = InternetOpenA("LastOder Reporter", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		return;
	}

	HINTERNET hConnection = InternetConnectA(hInternet,REPORT_SERVER, SERVER_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);
	if (hConnection==NULL)
	{
		InternetCloseHandle(hInternet);
		return;
	}

	HINTERNET hRequest = HttpOpenRequestA(hConnection, "POST", "LastOrder/monitor",NULL,NULL,NULL,INTERNET_FLAG_RELOAD|INTERNET_KEEP_ALIVE_ENABLED,0);
	if (hRequest==NULL)
	{
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hInternet);
		return;
	}

	wstring && tmpcontent = GBK2UNI(lpOper,strlen(lpOper));

	string && content = stringf("dest=%s&mac=%s&oper=%s",lpDestIP,mac.c_str(),UrlEncode(UNI2UTF8(tmpcontent.c_str(),tmpcontent.size()).c_str()).c_str());
	string hdr("Content-Type: application/x-www-form-urlencoded");

	BOOL rc = HttpSendRequestA(hRequest, hdr.c_str(), hdr.size(),(LPVOID)content.c_str(), content.size());

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);
#else
	OutputDebugStringA(lpDestIP);
	OutputDebugStringA(lpOper);
#endif
}

string COperMonitor::GetIP()
{
#ifndef kDays
	HINTERNET hInternet = InternetOpenA("LastOder Reporter", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		return "0.0.0.0";
	}

	HINTERNET hConnection = InternetConnectA(hInternet,REPORT_SERVER, SERVER_PORT,NULL,NULL,INTERNET_SERVICE_HTTP,0,0);
	if (hConnection==NULL)
	{
		InternetCloseHandle(hInternet);
		return "0.0.0.0";
	}

	HINTERNET hRequest = HttpOpenRequestA(hConnection, "GET", "LastOrder",NULL,NULL,NULL,INTERNET_FLAG_RELOAD|INTERNET_KEEP_ALIVE_ENABLED,0);
	if (hRequest==NULL)
	{
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hInternet);
		return "0.0.0.0";
	}

	BOOL rc = HttpSendRequest(hRequest, NULL, 0,NULL, NULL);

	string retVal;

	if(rc) // here rc = 0 and GetLastError() returns 12152
	{
		DWORD availDataLen;
		DWORD readCount = 0;

		InternetQueryDataAvailable(hRequest, &availDataLen, 0, 0);

		char *lpLatestVer = new char[availDataLen+1];
		memset(lpLatestVer,0,availDataLen+1);
		char *lpCurr = lpLatestVer;
		while(0 < availDataLen)
		{
			InternetReadFile(hRequest, lpCurr, availDataLen, &readCount);
			availDataLen -= readCount;
			lpCurr+=readCount;
		}

		retVal = lpLatestVer;
		delete lpLatestVer;
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hInternet);

	return retVal;
#endif
}
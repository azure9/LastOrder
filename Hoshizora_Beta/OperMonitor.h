#pragma once
#include <WinInet.h>


class COperMonitor
{
	string mac;
public:
	COperMonitor(void);
	~COperMonitor(void);

	void SendOper(LPSTR lpDestIP,LPSTR lpOper);

	string GetIP();
};


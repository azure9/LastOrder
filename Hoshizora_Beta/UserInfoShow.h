#pragma once
#include "listctrlex.h"
#include "BaseInfo.h"
#include "common.h"
#include "Switcher.h"
#include "Router.h"

using namespace ListCtrlEx;


// typedef enum _CreationFlag
// {
// 	SWITCHER_SESSION = 1,
// 	ROUTER_SESSION,
// 	BASE_INFO = 4
// }CreationFlag;
class CUserInfoShow :
	public CListCtrlEx
{
	DECLARE_DYNAMIC(CUserInfoShow)
private:
	CSwitcher *s;
	CRouter *r;
	int idxPort;
	vector<user_info> v;
	vector<vector<DWORD>> pay_info;
	vector<IpAddress> vIP;
public:
	CUserInfoShow(void);
	~CUserInfoShow(void);
	int PrepareUserData(vector<user_info>& v, vector<vector<DWORD>>& pay_info,
		CSwitcher *s = NULL ,int idxPort = NULL);
	int PrepareUserData(vector<user_info>& v,vector<vector<DWORD>>& pay_info,vector<IpAddress>& vIP);
	int FetchUserData(vector<user_info>& v,vector<vector<DWORD>>& pay_info,vector<IpAddress>& vIP);
	int RefershGrid(void);
	vector<user_info>* GetUserData(void);
	void Init(void);
	LRESULT OnListctrlexCheckChanged(WPARAM wParam, LPARAM lParam);
	void OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};


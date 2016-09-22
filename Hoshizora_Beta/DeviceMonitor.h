#pragma once
#include <map>
#include "Router.h"
#include "afxwin.h"

using namespace std;


// CDeviceMonitor 对话框

class CDeviceMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceMonitor)

public:
	CDeviceMonitor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeviceMonitor();
	multimap<MacAddress,IpAddress> table;
	CRouter *r;

// 对话框数据
	enum { IDD = IDD_BASE_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton7();
	CListBox m_list;
	int m_pluse;
};

#pragma once
#include <map>
#include "Router.h"
#include "afxwin.h"

using namespace std;


// CDeviceMonitor �Ի���

class CDeviceMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(CDeviceMonitor)

public:
	CDeviceMonitor(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDeviceMonitor();
	multimap<MacAddress,IpAddress> table;
	CRouter *r;

// �Ի�������
	enum { IDD = IDD_BASE_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton7();
	CListBox m_list;
	int m_pluse;
};

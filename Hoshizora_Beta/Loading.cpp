// Loading.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Loading.h"
#include "afxdialogex.h"


// CLoading �Ի���

IMPLEMENT_DYNAMIC(CLoading, CDialogEx)

CLoading::CLoading(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoading::IDD, pParent)
{

}

CLoading::~CLoading()
{
}

void CLoading::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoading, CDialogEx)
//	ON_BN_CLICKED(IDC_BUTTON1, &CLoading::OnBnClickedButton1)
END_MESSAGE_MAP()


// CLoading ��Ϣ�������
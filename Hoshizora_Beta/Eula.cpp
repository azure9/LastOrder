// Eula.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Eula.h"
#include "afxdialogex.h"


// CEula �Ի���

IMPLEMENT_DYNAMIC(CEula, CDialogEx)

CEula::CEula(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEula::IDD, pParent)
	, m_data(_T(""))
{

}

CEula::~CEula()
{
}

void CEula::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EULA, m_data);
}


BEGIN_MESSAGE_MAP(CEula, CDialogEx)
END_MESSAGE_MAP()


// CEula ��Ϣ�������

const TCHAR * eula = _T("EULA\r\n\
1.����Demo���û����������Ա�����������ⷴ���롢�ƽ⡢�Լ��ٷ�����\r\n�����û�����Щ����Υ���ġ�\r\n\
\r\n\
2.����������ڿ�ʼ������ͻ��ṩ�걸���ۺ����������Ȩ�۸���顣\r\n������ҵ�����չ����۸���̸��\r\n\
\r\n\
3.�����û����򿪷�������������󣬿����������ʵ������Գ���������Ӧ��\r\n������\r\n\
\r\n\
4.��Э�����Ȩ�鿪�������С������ܱ�Э��ʹ������Ѿ������˱��������ʱ\r\n�����ӵ�����Э�顣\r\n\
\r\n\
\r\n\
\r\n\
\r\n*������Դ��ڲ����ڡ��Ժ󽫼����������ú��ֹ�������Ӧ���ӵ����绷����\r\n\
\r\nPowered By Azure[LCG]\r\n");

BOOL CEula::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_data = eula + CString(__TIMESTAMP__);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

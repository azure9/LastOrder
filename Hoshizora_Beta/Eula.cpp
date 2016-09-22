// Eula.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Eula.h"
#include "afxdialogex.h"


// CEula 对话框

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


// CEula 消息处理程序

const TCHAR * eula = _T("EULA\r\n\
1.对于Demo版用户，允许您对本软件进行任意反编译、破解、以及再发布。\r\n正版用户做这些则是违法的。\r\n\
\r\n\
2.本软件从现在开始对正版客户提供完备的售后服务，正版授权价格待议。\r\n接受商业性质收购、价格面谈。\r\n\
\r\n\
3.正版用户可向开发者提出功能需求，开发者则根据实际情况对程序作出相应的\r\n升级。\r\n\
\r\n\
4.本协议解释权归开发者所有。您接受本协议就代表您已经接受了本软件更新时\r\n所附加的其他协议。\r\n\
\r\n\
\r\n\
\r\n\
\r\n*本软件仍处于测试期、以后将加入智能配置和手工配置适应复杂的网络环境。\r\n\
\r\nPowered By Azure[LCG]\r\n");

BOOL CEula::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_data = eula + CString(__TIMESTAMP__);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

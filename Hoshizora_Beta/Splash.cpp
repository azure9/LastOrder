// Splash.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Splash.h"
#include "afxdialogex.h"
#include "CGdiPlusBitmap.h"



// CSplash �Ի���

IMPLEMENT_DYNAMIC(CSplash, CWnd)

	CSplash::CSplash()
{
	splash = new CGdiPlusBitmapResource(IDB_PNG1,_T("PNG"));
}

CSplash::~CSplash()
{
	delete splash;
}

BEGIN_MESSAGE_MAP(CSplash, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSplash ��Ϣ�������
void CSplash::Create(WCHAR *lpMessage)
{
	this->lpMessage = lpMessage;
	CreateEx(0,  
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),  
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, splash->m_pBitmap->GetWidth(),splash->m_pBitmap->GetHeight(), NULL , NULL);  
	SetWindowLong(m_hWnd,   
		GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	CenterWindow();
}

void CSplash::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
	HBITMAP hbmp;
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(&dc);  

	splash->m_pBitmap->GetHBITMAP(Gdiplus::Color(0,0,0,0),&hbmp);  
	HGDIOBJ oldBmp = dcMemory.SelectObject(hbmp);  

	// ����Ĵ���ֻ��Ϊ����MemoryDC�ϻ���Щ��������������������ӿؼ�  
	Gdiplus::Graphics grap(dcMemory.GetSafeHdc());  
	Gdiplus::SolidBrush brush(Gdiplus::Color(180,0x66,0xCC,0xFF));  
	Gdiplus::FontFamily fontFamily(L"simhei");  
	Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);  
	grap.DrawString(lpMessage, wcslen(lpMessage), &font, Gdiplus::PointF(250,200), NULL, &brush);  

	// ��ȡ��ĻDC  
	HDC hdcScreen = ::GetDC(NULL);  

	// ����alpha�ںϹ���  
	BLENDFUNCTION blend = { 0 };  
	blend.BlendOp = AC_SRC_OVER;  
	blend.SourceConstantAlpha = 255;  
	blend.AlphaFormat = AC_SRC_ALPHA;   

	// ����layered����  
	CRect rectWindow;  
	GetWindowRect(&rectWindow);  
	::UpdateLayeredWindow(  
		m_hWnd, hdcScreen, &rectWindow.TopLeft(), &rectWindow.Size(), dcMemory.GetSafeHdc(), &CPoint(0,0), RGB(0, 0, 0), &blend, ULW_ALPHA);  
	// ע�⣺��xp�£�ULW_ALPHA | ULW_COLORKEYͬʱʹ�û��޷���ʾ����  

	// �ʵ�������  
	dcMemory.SelectObject(oldBmp);  
	::DeleteObject(hbmp);  
	::ReleaseDC(NULL, hdcScreen);
}


void CSplash::OnTimer(UINT_PTR nIDEvent)  
{  
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ  
	//CWnd::OnTimer(nIDEvent);  
	DestroyWindow(); //���ٳ�ʼ���洰��  
}  
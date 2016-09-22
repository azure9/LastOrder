// Splash.cpp : 实现文件
//

#include "stdafx.h"
#include "Hoshizora_Beta.h"
#include "Splash.h"
#include "afxdialogex.h"
#include "CGdiPlusBitmap.h"



// CSplash 对话框

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


// CSplash 消息处理程序
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
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	HBITMAP hbmp;
	CDC dcMemory;
	dcMemory.CreateCompatibleDC(&dc);  

	splash->m_pBitmap->GetHBITMAP(Gdiplus::Color(0,0,0,0),&hbmp);  
	HGDIOBJ oldBmp = dcMemory.SelectObject(hbmp);  

	// 这里的代码只是为了在MemoryDC上绘制些东西，或者在这里绘制子控件  
	Gdiplus::Graphics grap(dcMemory.GetSafeHdc());  
	Gdiplus::SolidBrush brush(Gdiplus::Color(180,0x66,0xCC,0xFF));  
	Gdiplus::FontFamily fontFamily(L"simhei");  
	Gdiplus::Font font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);  
	grap.DrawString(lpMessage, wcslen(lpMessage), &font, Gdiplus::PointF(250,200), NULL, &brush);  

	// 获取屏幕DC  
	HDC hdcScreen = ::GetDC(NULL);  

	// 设置alpha融合规则  
	BLENDFUNCTION blend = { 0 };  
	blend.BlendOp = AC_SRC_OVER;  
	blend.SourceConstantAlpha = 255;  
	blend.AlphaFormat = AC_SRC_ALPHA;   

	// 更新layered窗口  
	CRect rectWindow;  
	GetWindowRect(&rectWindow);  
	::UpdateLayeredWindow(  
		m_hWnd, hdcScreen, &rectWindow.TopLeft(), &rectWindow.Size(), dcMemory.GetSafeHdc(), &CPoint(0,0), RGB(0, 0, 0), &blend, ULW_ALPHA);  
	// 注意：在xp下，ULW_ALPHA | ULW_COLORKEY同时使用会无法显示窗口  

	// 适当的清理  
	dcMemory.SelectObject(oldBmp);  
	::DeleteObject(hbmp);  
	::ReleaseDC(NULL, hdcScreen);
}


void CSplash::OnTimer(UINT_PTR nIDEvent)  
{  
	// TODO: 在此添加消息处理程序代码和/或调用默认值  
	//CWnd::OnTimer(nIDEvent);  
	DestroyWindow(); //销毁初始画面窗口  
}  
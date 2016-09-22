#pragma once
#include "CGdiPlusBitmap.h"
#include <gdiplus.h>  
using namespace Gdiplus;  
#pragma comment(lib, "gdiplus.lib") 


// CSplash ¶Ô»°¿ò

class CSplash : public CWnd
{
	DECLARE_DYNAMIC(CSplash)  
public:  
	CSplash();  
	virtual ~CSplash();  
protected:
	DECLARE_MESSAGE_MAP()  
	WCHAR *lpMessage;
	CGdiPlusBitmapResource* splash;
public:
	void Create(WCHAR *lpMessage);  
	afx_msg void OnPaint();  
	afx_msg void OnTimer(UINT_PTR nIDEvent); 
};

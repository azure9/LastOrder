
// Hoshizora_Beta.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHoshizora_BetaApp:
// �йش����ʵ�֣������ Hoshizora_Beta.cpp
//

class CHoshizora_BetaApp : public CWinApp
{
public:
	CHoshizora_BetaApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHoshizora_BetaApp theApp;
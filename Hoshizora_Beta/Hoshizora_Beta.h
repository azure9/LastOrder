
// Hoshizora_Beta.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CHoshizora_BetaApp:
// 有关此类的实现，请参阅 Hoshizora_Beta.cpp
//

class CHoshizora_BetaApp : public CWinApp
{
public:
	CHoshizora_BetaApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CHoshizora_BetaApp theApp;
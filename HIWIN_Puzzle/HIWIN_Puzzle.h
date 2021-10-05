
// HIWIN_Puzzle.h: HIWIN_Puzzle 應用程式的主要標頭檔
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // 主要符號


// CHIWINPuzzleApp:
// 查看 HIWIN_Puzzle.cpp 以了解此類別的實作
//

class CHIWINPuzzleApp : public CWinApp
{
public:
	CHIWINPuzzleApp() noexcept;


// 覆寫
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 程式碼實作
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHIWINPuzzleApp theApp;

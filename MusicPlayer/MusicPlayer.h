
// MusicPlayer.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CMusicPlayerApp:
// Сведения о реализации этого класса: MusicPlayer.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
	CMusicPlayerApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CMusicPlayerApp theApp;

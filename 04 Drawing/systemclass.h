////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "inputclass.h"
#include "graphicsclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass(const SystemClass&) = delete;
	SystemClass& operator=(const SystemClass&) = delete;

	SystemClass();
	~SystemClass();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows();
	void ShutdownWindows();

private:
	bool		m_fullscreen =		false;
	UINT		m_xResolution =		0;
	UINT		m_yResolution =		0;
	LPCWSTR		m_applicationName =	L"04 Drawing";
	HINSTANCE	m_hinstance;
	HWND		m_hwnd;

	InputClass*		m_Input =		nullptr;
	GraphicsClass*	m_Graphics =	nullptr;
};

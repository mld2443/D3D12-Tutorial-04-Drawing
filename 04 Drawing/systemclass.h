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

	SystemClass();
	~SystemClass();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	bool InitializeWindows(UINT&, UINT&);
	void ShutdownWindows();

private:
	LPCWSTR		m_applicationName =	L"04 Drawing";
	HINSTANCE	m_hinstance;
	HWND		m_hwnd;

	InputClass*		m_Input =		nullptr;
	GraphicsClass*	m_Graphics =	nullptr;
};

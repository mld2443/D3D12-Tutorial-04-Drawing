////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "inputclass.h"
#include "engineclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
    SystemClass(const SystemClass &) = delete;
    SystemClass & operator=(const SystemClass &) = delete;

    SystemClass();
    ~SystemClass();

    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows();
    void ShutdownWindows();

private:
    bool      m_fullscreen      = false;
    UINT      m_xResolution     = 1024u;
    UINT      m_yResolution     = 768u;
    LPCWSTR   m_applicationName = L"04 Drawing";
    HINSTANCE m_hinstance       = NULL;
    HWND      m_hwnd            = NULL;

    std::unique_ptr<InputClass>  m_Input  = nullptr;
    std::unique_ptr<EngineClass> m_Engine = nullptr;
};

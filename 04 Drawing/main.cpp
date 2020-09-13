////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "systemclass.h"


int WINAPI WinMain(__in HINSTANCE hInstance,
                   __in_opt HINSTANCE hPrevInstance,
                   __in PSTR pScmdline,
                   __in int iCmdshow)
{
    // Create and initialize the system object.
    std::unique_ptr<SystemClass> System;
    try
    {
        System = std::make_unique<SystemClass>();
    }
    catch (std::exception& e)
    {
        ShowCursor(true);
        MessageBoxA(NULL, e.what(), "Initialization Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Run the system
    try
    {
        System->Run();
    }
    catch (std::exception& e)
    {
        ShowCursor(true);
        MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONERROR);
        return 2;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    {
        // Acquire DXGI debug controller.
        ComPtr<IDXGIDebug1> debugInterface;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debugInterface.GetAddressOf()))))
        {
            debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }
#endif // DX12_ENABLE_DEBUG_LAYER

    return 0;
}

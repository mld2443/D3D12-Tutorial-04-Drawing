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
    std::unique_ptr<SystemClass> System;


    // Create and initialize the system object.
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

    return 0;
}

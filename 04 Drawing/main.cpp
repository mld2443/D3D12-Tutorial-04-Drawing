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
    unique_ptr<SystemClass> System;


    // Create and initialize the system object.
    try
    {
        System = make_unique<SystemClass>();
    }
    catch (exception& e)
    {
        MessageBoxA(NULL, e.what(), "Initialization Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Run the system
    try
    {
        System->Run();
    }
    catch (exception& e)
    {
        MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONERROR);
        return 2;
    }

    return 0;
}

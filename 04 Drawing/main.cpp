////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;


	try
	{
		// Create and initialize the system object.
		System = new SystemClass();

		// Run the system
		System->Run();
	}
	catch (MessageBoxType exception)
	{
		MessageBox(nullptr, exception.message, exception.title, exception.type);
		return 1;
	}

	// Shutdown and release the system object.
	SAFE_DELETE(System);

	return 0;
}

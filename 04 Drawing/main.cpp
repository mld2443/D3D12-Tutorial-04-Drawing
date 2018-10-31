////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "systemclass.h"


int WINAPI WinMain(__in HINSTANCE hInstance,
				   __in_opt HINSTANCE hPrevInstance,
				   __in PSTR pScmdline,
				   __in int iCmdshow)
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
		MessageBox(NULL, exception.message, exception.title, exception.type);
		return 1;
	}

	// Shutdown and release the system object.
	SAFE_DELETE(System);

	return 0;
}

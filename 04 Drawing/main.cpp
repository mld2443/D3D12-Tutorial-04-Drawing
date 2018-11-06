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


	try
	{
		// Create and initialize the system object.
		System = make_unique<SystemClass>();

		// Run the system
		System->Run();
	}
	catch (MessageBoxType exception)
	{
		MessageBox(NULL, exception.message, exception.title, exception.type);
		return 1;
	}

	return 0;
}

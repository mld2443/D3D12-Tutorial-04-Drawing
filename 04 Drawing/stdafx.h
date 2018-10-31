////////////////////////////////////////////////////////////////////////////////
// Filename: stdafx.h
////////////////////////////////////////////////////////////////////////////////
#pragma once
#pragma message("COMPILING PRECOMPILED HEADER")


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////

#define WIN32_LEAN_AND_MEAN


/////////////
// LINKING //
/////////////

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


//////////////
// INCLUDES //
//////////////

// Windows
#include <windows.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <directxmath.h>

// C++ Standard Library
#include <string>
#include <vector>


////////////////////
// ERROR HANDLING //
////////////////////

struct MessageBoxType {
	LPCWSTR message;
	LPCWSTR title;
	UINT type;
};


/////////////////////
// MACRO FUNCTIONS //
/////////////////////

#define SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = nullptr; }
#define SAFE_RELEASE(ptr) if (ptr) { ptr->Release(); ptr = nullptr; }
#define THROW_MESSAGE(message, title) throw MessageBoxType{ message, title, MB_OK }
#define THROW_IF_FAILED(hr, message, title) if (FAILED(hr)) { THROW_MESSAGE(message, title); }
#define THROW_IF_FALSE(cond, message, title) if (!cond) { THROW_MESSAGE(message, title); }
#define THROW_IF_TRUE(cond, message, title) if (cond) { THROW_MESSAGE(message, title); }


///////////////
// CONSTANTS //
///////////////

#define FRAME_BUFFER_COUNT 2
#define PI_180 0.0174532925f


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////

static void* g_ApplicationHandle = nullptr;


//////////////////////
// USING DIRECTIVES //
//////////////////////

using namespace DirectX;
using namespace std;

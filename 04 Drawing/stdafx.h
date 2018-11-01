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
#include <wrl/client.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <directxmath.h>

// C++ Standard Library
#include <string>
#include <vector>


///////////////
// CONSTANTS //
///////////////

// Defines the number of back buffers and other variable length resources.
#define FRAME_BUFFER_COUNT 2

// The value of pi divided by 180.  Used to convert degrees to radians.
#define PI_180 0.0174532925f


//////////////////////
// USING DIRECTIVES //
//////////////////////

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

// Global function callback for windows messages.
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// Global pointer to our SystemClass instantiation.  Used to redirect windows messages to SystemClass.
static void* g_ApplicationHandle = nullptr;


////////////////////
// ERROR HANDLING //
////////////////////

// Struct representing the arguments to a MessageBox call.
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
#define THROW_MESSAGE(message, title) throw MessageBoxType{ message, title, MB_OK | MB_ICONERROR }
#define THROW_IF_FAILED(hr, message, title) if (FAILED(hr)) { THROW_MESSAGE(message, title); }
#define THROW_IF_TRUE(cond, message, title) if (cond) { THROW_MESSAGE(message, title); }
#define THROW_IF_FALSE(cond, message, title) THROW_IF_TRUE(!cond, message, title)
#define BYTE_ALIGNED_WIDTH(type, target) (sizeof(type) + target) & ~target

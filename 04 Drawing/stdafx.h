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

#define SAFE_SHUTDOWN(ptr) if (ptr) { ptr->Shutdown(); delete ptr; ptr = nullptr; }
#define SAFE_RELEASE(ptr) if (ptr) { ptr->Release(); ptr = nullptr; }
#define THROW_MESSAGE(message, title) throw MessageBoxType{ message, title, MB_OK }
#define THROW_IF_FAILED(hr, message, title) if (FAILED(hr)) { THROW_MESSAGE(message, title); }
#define THROW_IF_FALSE(cond, message, title) if (!cond) { THROW_MESSAGE(message, title); }
#define THROW_IF_TRUE(cond, message, title) if (cond) { THROW_MESSAGE(message, title); }


///////////////
// CONSTANTS //
///////////////

constexpr auto PI = 3.141592654f;
constexpr auto PI_180 = 0.0174532925f;


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////

static void* g_ApplicationHandle = nullptr;
constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr UINT FRAME_BUFFER_COUNT = 2;


//////////////////////
// USING DIRECTIVES //
//////////////////////

using namespace DirectX;

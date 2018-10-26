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


/////////////////////
// MACRO FUNCTIONS //
/////////////////////

#define SAFE_SHUTDOWN(p) if (p) { p->Shutdown(); delete p; p = nullptr; }
#define SAFE_RELEASE(p) if (p) { p->Release(); p = nullptr; }


///////////////
// CONSTANTS //
///////////////

constexpr auto PI = 3.141592654f;
constexpr auto PI_180 = 0.0174532925f;
constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr UINT FRAME_BUFFER_COUNT = 2;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////

static void* g_ApplicationHandle = 0;


//////////////////////
// USING DIRECTIVES //
//////////////////////

using namespace DirectX;

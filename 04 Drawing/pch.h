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
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>


///////////////
// CONSTANTS //
///////////////

// Defines the number of back buffers and other variable length resources.
#define FRAME_BUFFER_COUNT 2u

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


/////////////////////
// MACRO FUNCTIONS //
/////////////////////

#define THROW_IF_FAILED(hr, message) if (FAILED(hr)) { throw runtime_error(message); }
#define THROW_IF_TRUE(cond, message) if (cond) { throw runtime_error(message); }
#define THROW_IF_FALSE(cond, message) if (!cond) { throw runtime_error(message); }
#define BYTE_ALIGNED_WIDTH(type, target) (sizeof(type) + target) & ~target

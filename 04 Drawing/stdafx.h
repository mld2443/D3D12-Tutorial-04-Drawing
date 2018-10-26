////////////////////////////////////////////////////////////////////////////////
// Filename: stdafx.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


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


///////////////
// CONSTANTS //
///////////////
#define PI 3.141592654f
#define PI_180 0.0174532925f


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const UINT FRAME_BUFFER_COUNT = 2;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

using namespace DirectX;

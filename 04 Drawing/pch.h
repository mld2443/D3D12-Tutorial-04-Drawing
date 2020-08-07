////////////////////////////////////////////////////////////////////////////////
// Filename: stdafx.h
////////////////////////////////////////////////////////////////////////////////
#pragma once
#pragma message("COMPILING PRECOMPILED HEADER")


/////////////////////////////
// PREPROCESSOR DIRECTIVES //
/////////////////////////////

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
#include <directxmath.h>
#include <dxgi1_4.h>

// C++ Standard Library
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>


///////////////
// CONSTANTS //
///////////////

// Defines the number of back buffers and other variable length resources.
constexpr uint32_t FRAME_BUFFER_COUNT = 2u;

// The value of pi divided by 180.  Used to convert degrees to radians.
constexpr float PI_180 = 0.0174532925f;


//////////////////////
// USING DIRECTIVES //
//////////////////////

using namespace DirectX;
using namespace Microsoft::WRL;


/////////////////////
// MACRO FUNCTIONS //
/////////////////////

#define THROW_IF_FAILED(hr, message) if (FAILED(hr)) { throw std::runtime_error(message); }
#define THROW_IF_TRUE(cond, message) if (cond) { throw std::runtime_error(message); }
#define THROW_IF_FALSE(cond, message) if (!cond) { throw std::runtime_error(message); }
#define BYTE_ALIGNED_WIDTH(type, target) (sizeof(type) + target) & ~target

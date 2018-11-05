////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"
#include "d3dclass.h"
//#include "triangleclass.h"
#include "quadclass.h"
//#include "colorcontextclass.h"
#include "instancecontextclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: EngineClass
////////////////////////////////////////////////////////////////////////////////
class EngineClass
{
public:
	EngineClass() = delete;
	EngineClass(const EngineClass&) = delete;
	EngineClass& operator=(const EngineClass&) = delete;

	EngineClass(HWND, UINT, UINT, bool);
	~EngineClass();

	void Frame();

private:
	void Render();

private:
	bool m_vsyncEnabled =	true;

	unique_ptr<CameraClass>				m_Camera =			nullptr;
	unique_ptr<D3DClass>				m_Direct3D =		nullptr;
	unique_ptr<InstanceContextClass>	m_InstanceContext =	nullptr;
	unique_ptr<QuadClass>				m_Quads =			nullptr;
	//unique_ptr<ColorContextClass>		m_ColorContext =	nullptr;
	//unique_ptr<TriangleClass>			m_Triangle =		nullptr;
};

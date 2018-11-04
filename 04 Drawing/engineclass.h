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
//#include "colorpipelineclass.h"
#include "instancepipelineclass.h"


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

	unique_ptr<CameraClass>				m_Camera =		nullptr;
	unique_ptr<D3DClass>				m_Direct3D =	nullptr;
	unique_ptr<InstancePipelineClass>	m_Pipeline =	nullptr;
	unique_ptr<QuadClass>				m_Geometry =	nullptr;
	//unique_ptr<ColorPipelineClass>	m_Pipeline =	nullptr;
	//unique_ptr<TriangleClass>			m_Geometry =	nullptr;
};

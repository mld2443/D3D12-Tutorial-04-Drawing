////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"
#include "d3dclass.h"
#include "pipelineclass.h"
#include "instancecontextclass.h"
#include "quadclass.h"


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
	unique_ptr<PipelineClass>		m_Pipeline =	nullptr;
	unique_ptr<InstanceContextClass>	m_Context =		nullptr;
	unique_ptr<GeometryInterface>		m_Geometry =	nullptr;
};

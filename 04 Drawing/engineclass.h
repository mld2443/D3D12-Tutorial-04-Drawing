////////////////////////////////////////////////////////////////////////////////
// Filename: engineclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"
#include "d3dclass.h"
#include "triangleclass.h"
#include "solopipelineclass.h"


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

	D3DClass*			m_Direct3D =	nullptr;
	CameraClass*		m_Camera =		nullptr;
	TriangleClass*		m_Geometry =	nullptr;
	SoloPipelineClass*	m_Pipeline =	nullptr;
};

////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
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
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass() = delete;
	GraphicsClass(const GraphicsClass&) = delete;
	GraphicsClass& operator=(const GraphicsClass&) = delete;

	GraphicsClass(HWND, UINT, UINT, bool);
	~GraphicsClass();

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

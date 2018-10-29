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

	GraphicsClass(HWND, UINT, UINT);
	~GraphicsClass();

	void Frame();

private:
	void Render();

private:
	D3DClass*			m_Direct3D =	nullptr;
	CameraClass*		m_Camera =		nullptr;
	GeometryInterface*	m_Geometry =	nullptr;
	PipelineInterface*	m_Pipeline =	nullptr;
};

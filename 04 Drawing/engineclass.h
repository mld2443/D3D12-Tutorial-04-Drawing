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
class EngineClass : private D3DClass
{
public:
    EngineClass(HWND, UINT, UINT, bool);
    ~EngineClass();

    void Frame();

private:
    void Render();

private:
    const bool m_vsyncEnabled = true;

    std::unique_ptr<CameraClass>          m_Camera   = nullptr;
    std::unique_ptr<PipelineClass>        m_Pipeline = nullptr;
    std::unique_ptr<InstanceContextClass> m_Context  = nullptr;
    std::unique_ptr<GeometryInterface>    m_Geometry = nullptr;
};

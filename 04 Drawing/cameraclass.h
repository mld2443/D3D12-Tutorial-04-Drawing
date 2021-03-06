////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
    CameraClass(UINT, UINT, float, float = 0.1f, float = 1'000.0f);
    ~CameraClass() = default;

    UINT GeXResolution();
    UINT GetYResolution();
    float GetScreenNear();
    float GetScreenFar();
    XMMATRIX & GetViewMatrix();
    XMMATRIX & GetProjectionMatrix();

    void SetFieldOfViewInDegrees(float);
    void SetPosition(float, float, float);
    void SetRotationInDegrees(float, float, float);
    void SetLookDirection(float, float, float);

    void Render();

private:
    void UpdateProjectionMatrix();

private:
    UINT  m_xResolution, m_yResolution;
    float m_fieldOfView, m_aspectRatio;
    float m_screenNear, m_screenFar;

    XMVECTOR m_position      = XMVectorZero();
    XMVECTOR m_rotation      = XMVectorZero();
    XMVECTOR m_lookDirection = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    XMMATRIX m_viewMatrix       = XMMatrixIdentity();
    XMMATRIX m_projectionMatrix = XMMatrixIdentity();
};

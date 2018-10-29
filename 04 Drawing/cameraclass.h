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
	CameraClass() = delete;
	CameraClass(const CameraClass&) = delete;
	CameraClass& operator=(const CameraClass&) = delete;

	CameraClass(float, float, float = 0.1f, float = 1000.0f);
	~CameraClass() = default;

	void SetFieldOfView(float);
	void SetPosition(float, float, float);
	void SetRotationInDegrees(float, float, float);
	void SetLookDirection(float, float, float);

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	void Render();

private:
	void UpdateProjectionMatrix();

private:
	float		m_fieldOfView, m_aspectRatio;
	float		m_screenNear, m_screenFar;

	XMVECTOR	m_position =			XMVectorZero();
	XMVECTOR	m_rotation =			XMVectorZero();
	XMVECTOR	m_lookDirection =		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	XMMATRIX	m_viewMatrix =			XMMatrixIdentity();
	XMMATRIX	m_projectionMatrix =	XMMatrixIdentity();
};

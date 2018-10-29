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
	CameraClass(const CameraClass&) = delete;
	CameraClass& operator=(const CameraClass&) = delete;

	CameraClass() = default;
	~CameraClass() = default;

	void SetPosition(float, float, float);
	void SetLookDirection(float, float, float);
	void SetRotationInDegrees(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLookDirection();
	XMFLOAT3 GetRotation();
	XMMATRIX GetViewMatrix();

	void Render();

private:
	XMFLOAT3	m_position =		XMFLOAT3();
	XMFLOAT3	m_lookDirection =	XMFLOAT3();
	XMFLOAT3	m_rotation =		XMFLOAT3();
	XMMATRIX	m_viewMatrix =		XMMatrixIdentity();
};

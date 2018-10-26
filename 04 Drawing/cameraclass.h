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
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetLookDirection(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLookDirection();
	XMFLOAT3 GetRotation();

	void Render();
	XMMATRIX GetViewMatrix();

private:
	XMFLOAT3	m_position;
	XMFLOAT3	m_lookDirection;
	XMFLOAT3	m_rotation;
	XMMATRIX	m_viewMatrix;
};

////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "cameraclass.h"


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void CameraClass::SetLookDirection(float x, float y, float z)
{
	m_lookDirection = XMFLOAT3(x, y, z);
}


void CameraClass::SetRotationInDegrees(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x * PI_180, y * PI_180, z * PI_180);
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}


XMFLOAT3 CameraClass::GetLookDirection()
{
	return m_lookDirection;
}


XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}


XMMATRIX CameraClass::GetViewMatrix()
{
	return m_viewMatrix;
}


void CameraClass::Render()
{
	XMVECTOR cameraUp, position, lookAt;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards relative to the camera.
	cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup the position of the camera in the world.
	position = XMLoadFloat3(&m_position);

	// Setup where the camera is looking by default.
	lookAt = XMLoadFloat3(&m_lookDirection);

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	cameraUp = XMVector3TransformCoord(cameraUp, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, cameraUp);
}

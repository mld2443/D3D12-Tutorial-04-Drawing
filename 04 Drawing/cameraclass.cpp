////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "cameraclass.h"


CameraClass::CameraClass(UINT xResolution, UINT yResolution, float fieldOfView, float screenNear, float screenFar) :
	m_xResolution(xResolution),
	m_yResolution(yResolution),
	m_fieldOfView(fieldOfView * PI_180),
	m_aspectRatio((float)xResolution / (float)yResolution),
	m_screenNear(screenNear),
	m_screenFar(screenFar)
{
	UpdateProjectionMatrix();
}


UINT CameraClass::GeXResolution()
{
	return m_xResolution;
}


UINT CameraClass::GetYResolution()
{
	return m_yResolution;
}


float CameraClass::GetScreenNear()
{
	return m_screenNear;
}


float CameraClass::GetScreenFar()
{
	return m_screenFar;
}


XMMATRIX CameraClass::GetViewMatrix()
{
	return m_viewMatrix;
}


XMMATRIX CameraClass::GetProjectionMatrix()
{
	return m_projectionMatrix;
}


void CameraClass::SetFieldOfViewInDegrees(float fieldOfView)
{
	m_fieldOfView = fieldOfView * PI_180;

	UpdateProjectionMatrix();
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_position = XMVectorSet(x, y, z, 0.0f);
}


void CameraClass::SetRotationInDegrees(float x, float y, float z)
{
	m_rotation = XMVectorSet(x * PI_180, y * PI_180, z * PI_180, 0.0f);
}


void CameraClass::SetLookDirection(float x, float y, float z)
{
	m_lookDirection = XMVector3Normalize(XMVectorSet(x, y, z, 0.0f));
}


void CameraClass::Render()
{
	XMVECTOR upDirection, lookAt;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards relative to the camera.
	upDirection = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup where the camera is looking by default.
	lookAt = m_lookDirection;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYawFromVector(m_rotation);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	upDirection = XMVector3TransformCoord(upDirection, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = m_position + m_lookDirection;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(m_position, lookAt, upDirection);
}


void CameraClass::UpdateProjectionMatrix()
{
	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_screenNear, m_screenFar);
}

#include "Camera.h"

void Camera::RecalculateProjectionMatrix(float screenWidth, float screenHeight) //1920.0f / 1080.0f;
{
	/*float aspectRatio = screenWidth / screenHeight;
	float yScale = (1.0f / (float)tan(m_FOV * DegToRad / 2)) * aspectRatio;
	float xScale = yScale / aspectRatio;
	float frustumLength = m_farPlane - m_nearPlane;*/
	//m_projectionMatrix.values[0 + 0 * 4] = xScale;												// 0
	//m_projectionMatrix.values[1 + 1 * 4] = yScale;												// 5
	//m_projectionMatrix.values[2 + 2 * 4] = -((m_farPlane + m_nearPlane) / frustumLength);		// 10
	//m_projectionMatrix.values[2 + 3 * 4] = -1;													// 11
	//m_projectionMatrix.values[3 + 2 * 4] = -((2 * m_nearPlane * m_farPlane) / frustumLength);	// 14
	//m_projectionMatrix.values[3 + 3 * 4] = 0;													// 15


	//m_projectionMatrix.values[0 + 0 * 4] = xScale;												// 0 // x
	//m_projectionMatrix.values[1 + 1 * 4] = yScale;												// 5 // y
	//m_projectionMatrix.values[2 + 2 * 4] = -((m_farPlane + m_nearPlane) / frustumLength);		// 10 // z
	//m_projectionMatrix.values[3 + 2 * 4] = -1;													// 11 // z
	//m_projectionMatrix.values[2 + 3 * 4] = -((2 * m_nearPlane * m_farPlane) / frustumLength);	// 14 // w
	//m_projectionMatrix.values[3 + 3 * 4] = 0;													// 15 // w

	float aspectRatio = screenHeight / screenWidth;
	float fovRad = 1.0f / tan(m_FOV * .5f / 180 * 3.14159f);

	/*float yScale = (1.0f / (float)tan(m_FOV * DegToRad / 2)) * aspectRatio;
	float xScale = yScale / aspectRatio;*/
	float frustumLength = m_farPlane - m_nearPlane;

	m_projectionMatrix.values[0 + 0 * 4] = aspectRatio * fovRad;												// 0 // x
	m_projectionMatrix.values[1 + 1 * 4] = fovRad;												// 5 // y
	m_projectionMatrix.values[2 + 2 * 4] = m_farPlane / frustumLength;		// 10 // z
	m_projectionMatrix.values[3 + 2 * 4] = (-m_farPlane * m_nearPlane) / frustumLength;													// 11 // z
	m_projectionMatrix.values[2 + 3 * 4] = 1;	// 14 // w
	m_projectionMatrix.values[3 + 3 * 4] = 0;													// 15 // w
}

void Camera::RecalculateViewMatrix()
{
	m_viewMatrix.SetIdentity();
	Vector3 up = { 0, 1, 0 };
	Vector3 target = { 0, 0, 1 };
	// rotate
	// x
	Matrix4 matCameraRot = Matrix4::RotationX(m_transform.Rotation().x * DegToRad);
	target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
	// y
	matCameraRot = Matrix4::RotationY(m_transform.Rotation().y * DegToRad);
	target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
	// x
	//matCameraRot = Matrix4::RotationX(m_transform.Rotation().x * DegToRad);
	//target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
	// z
	matCameraRot = Matrix4::RotationZ(m_transform.Rotation().z * DegToRad);
	target = Matrix4::MultiplyMatrixVector3(target, matCameraRot);
	// offset with camera position
	target.x += m_transform.Position.x; target.y += m_transform.Position.y; target.z += m_transform.Position.z;
	//Vector3 target = m_transform.Position + {0, 0, 1};
	Matrix4 matCamera = Matrix4::PointAt(m_transform.Position, target, up);
	//m_viewMatrix.Translate({ -m_transform.Position.x, -m_transform.Position.y, -m_transform.Position.z });

	/*Vector3 target = m_transform.Forward();
	target.x += m_transform.Position.x; target.y += m_transform.Position.y; target.z += m_transform.Position.z;
	Matrix4 matCamera = Matrix4::PointAt(m_transform.Position, target, {0, 0, 1});*/
	m_viewMatrix = Matrix4::Matrix_QuickInverse(matCamera);
}
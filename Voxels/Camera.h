#pragma once
#include "Vector3.h"


struct Camera
{
private:
	Matrix4 m_viewMatrix;
	Matrix4 m_projectionMatrix;
public:
	float m_FOV = 90;
	float m_nearPlane = .1f;
	float m_farPlane = 5000.0f;
	Transform m_transform;
	Camera()
	{
		//RecalculateProjectionMatrix();
	}
	void RecalculateProjectionMatrix(float screenWidth, float screenHeight);
	void RecalculateViewMatrix();
	const Matrix4& ViewMatrix() const { return m_viewMatrix; }
	const Matrix4& ProjectionMatrix() const { return m_projectionMatrix; }

};
#pragma once
#include "../../Vector3.h"
class Entity
{
private:
	bool m_hasMesh;
public:
	Transform m_transform;
	// mesh -
	Entity(Vector3 position, bool hasMesh) : m_hasMesh(hasMesh)
	{
		m_transform.Position.x = position.x;
		m_transform.Position.y = position.y;
		m_transform.Position.z = position.z;
	}
	void virtual Update() = 0;
};
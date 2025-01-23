#pragma once
#include "../Vector3.h"

struct Vertex
{
public:
	Vector3 Position; // use offsetof to get offset of different properties :)
	Vector2 Uv;
};

struct ChunkVertex
{
public:
	//Vector3 Position;
	int positionPacked;
	Vector2 Uv;
	int m_Color;
	//int m_blockIndex;
	//ChunkVertex() {}
	ChunkVertex(Vector3 pos, Vector2 uv, int col, int blockIndex) : Uv(uv), m_Color(col)
	{
		int x = int(pos.x);
		int y = int(pos.y);
		int z = int(pos.z);
		positionPacked = (x << 0) | (y << 10) | (z << 20);
	}

	inline void SetColor(int r, int g, int b)
	{
		m_Color = GetColor(r, g, b);
	}
	static int GetColor(int r, int g, int b);
};
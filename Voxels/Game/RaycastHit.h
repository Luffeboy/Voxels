#pragma once
#include "../Vector3.h"
#include "../ChunkData.h"
struct RaycastHit
{
public:
	bool hit;
	PositionAndChunk pos;
	RaycastHit(bool hit = false) : hit(hit), pos(), m_Next(0) {}
	RaycastHit(bool hit, PositionAndChunk pos, uint8_t next) : hit(hit), pos(pos), m_Next(next)
	{}
	RaycastHit(bool hit, PositionAndChunk pos, Vector3Int nextTo) : hit(hit), pos(pos), m_Next(0)
	{
		m_Next |= (nextTo.x == 1 ? 1 : 0) << 0;
		m_Next |= (nextTo.x == -1 ? 1 : 0) << 1;

		m_Next |= (nextTo.y == 1 ? 1 : 0) << 2;
		m_Next |= (nextTo.y == -1 ? 1 : 0) << 3;

		m_Next |= (nextTo.z == 1 ? 1 : 0) << 4;
		m_Next |= (nextTo.z == -1 ? 1 : 0) << 5;
	}
	PositionAndChunk NextToPosition()
	{
		PositionAndChunk newPos = pos;
		newPos.Position.x += (m_Next >> 0) & 1;
		newPos.Position.x -= (m_Next >> 1) & 1;
		if (newPos.Position.x < 0)
		{
			newPos.Position.x += ChunkWidth;
			newPos.Chunk.x--;
		}
		else if (newPos.Position.x >= ChunkWidth)
		{
			newPos.Position.x -= ChunkWidth;
			newPos.Chunk.x++;
		}

		newPos.Position.y += (m_Next >> 2) & 1;
		newPos.Position.y -= (m_Next >> 3) & 1;
		if (newPos.Position.y < 0)
		{
			newPos.Position.y += ChunkHeight;
			newPos.Chunk.y--;
		}
		else if (newPos.Position.y >= ChunkHeight)
		{
			newPos.Position.y -= ChunkHeight;
			newPos.Chunk.y++;
		}

		newPos.Position.z += (m_Next >> 4) & 1;
		newPos.Position.z -= (m_Next >> 5) & 1;
		if (newPos.Position.z < 0)
		{
			newPos.Position.z += ChunkWidth;
			newPos.Chunk.z--;
		}
		else if (newPos.Position.z >= ChunkWidth)
		{
			newPos.Position.z -= ChunkWidth;
			newPos.Chunk.z++;
		}
		return newPos;
	}
private:
	uint8_t m_Next;
};
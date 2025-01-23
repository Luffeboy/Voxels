#include "Vertex.h"

int ChunkVertex::GetColor(int r, int g, int b)
{
	return r << 8 * 0 | g << 8 * 1 | b << 8 * 2;
}
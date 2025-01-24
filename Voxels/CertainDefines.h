#pragma once
#define DegToRad 0.0174533
constexpr int ChunkWidth = 64;
constexpr int ChunkHeight = ChunkWidth;
constexpr float BlockSize = 1.0f;
constexpr float BlockTexturesPerRow = 6.0f;
constexpr int BlocksInTexture = 5;
#define xyzToChunkIndex(x, y, z) (z) + (y) * ChunkWidth + (x) * (ChunkWidth * ChunkHeight)
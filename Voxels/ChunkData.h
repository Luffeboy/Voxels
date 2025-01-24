#pragma once
#include <vector>
#include "Graphics/Vertex.h"
#include "IncludeOpenGL.h"
#include "Game/WorldGeneration/PerlinNoise.h"
#include "CertainDefines.h"

enum BlockType
{
	Air = 0,
	Dirt,
	Stone,
	Sand,
	Glass,
	Water,

	Count
};
//bool TransparentBlockDefinitions[BlockType::Count] =
//{
//	true, // air
//	false, // dirt
//	false, // stone
//	false, // sand
//	true, // glass
//	true, // water
//};

class Renderer;

struct ChunkData
{
private:
	static bool TransparentBlockDefinitions[BlockType::Count];
	unsigned int m_buffer;
	unsigned int m_indexBuffer;
	unsigned int m_transparentBuffer;
	unsigned int m_transparentIndexBuffer;

	mutable size_t m_vertexCount = 0;
	mutable size_t m_indeciesCount = 0;
	mutable size_t m_transparentVertexCount = 0;
	mutable size_t m_transparentIndeciesCount = 0;

	Vector3 m_offset;
	Vector3Int m_offsetInt;
	Vector3 m_offsetForRendering;
public: // per object stuff
	BlockType* Blocks;
	//std::vector<Vertex> verticies;
	ChunkData() { std::cout << "ChunkData default constructor called... why?" << std::endl; }

	ChunkData(Vector3 offset) : Blocks(new BlockType[ChunkWidth * ChunkHeight * ChunkWidth]), m_vertexCount(0), m_indeciesCount(0), m_offset(offset), m_offsetInt(offset.ToVector3Int())
	{
		m_offsetForRendering = m_offset;
		m_offsetForRendering.x *= ChunkWidth * BlockSize;
		m_offsetForRendering.y *= ChunkHeight * BlockSize;
		m_offsetForRendering.z *= ChunkWidth * BlockSize;
		//Blocks = new BlockType[ChunkWidth * ChunkHeight * ChunkWidth];
		glGenBuffers(1, &m_buffer);
		glGenBuffers(1, &m_indexBuffer);

		glGenBuffers(1, &m_transparentBuffer);
		glGenBuffers(1, &m_transparentIndexBuffer);

		// get save data?

		// create it if no save data exists
		double noiseMultiplier = 100;
		//double stoneLine = 0;
		double waterLine = 3;
		//double sandLine = waterLine + 3;
		double dirtHeight = 3;
		double sandHeight = 3;
		//double waterHeight = 3;
		Vector3Int noiseOffset = { (int)m_offsetForRendering.x, (int)m_offsetForRendering.y, (int)m_offsetForRendering.z };

		for (int x = 0; x < ChunkWidth; x++)
			for (int z = 0; z < ChunkWidth; z++)
			{
				double noiseValue = PerlinNoise::Noise(x + noiseOffset.x, z + noiseOffset.z) * noiseMultiplier - noiseOffset.y;
				for (int y = 0; y < ChunkHeight; y++)
				{
					int yValue = noiseOffset.y + y;
					BlockType block = BlockType::Air;
					bool hasBlock = yValue <= noiseValue;
					if (hasBlock)
					{
						block = BlockType::Dirt;
						//if (yValue >= noiseValue - dirtHeight)
						//	block = BlockType::Dirt;
						//else if (yValue >= noiseValue - dirtHeight - sandHeight)
						//	block = BlockType::Sand;
						//else if (yValue < waterLine)
						//	block = BlockType::Water;
						//else
						//	block = BlockType::Stone;
					}
					Blocks[xyzToChunkIndex(x, y, z)] = block;
				}
			}
		// remove all sides
		//for (int x = 0; x < ChunkWidth; x++)
		//	for (int z = 0; z < ChunkWidth; z++)
		//	{
		//		Blocks[xyzToChunkIndex(x, 0, z)] = BlockType::Air;
		//		Blocks[xyzToChunkIndex(x, (ChunkHeight - 1), z)] = BlockType::Air;
		//	}
		//for (int y = 0; y < ChunkHeight; y++)
		//	for (int z = 0; z < ChunkWidth; z++)
		//	{
		//		Blocks[xyzToChunkIndex(0, y, z)] = BlockType::Air;
		//		Blocks[xyzToChunkIndex((ChunkWidth - 1), y, z)] = BlockType::Air;
		//		Blocks[xyzToChunkIndex(z, y, 0)] = BlockType::Air;
		//		Blocks[xyzToChunkIndex(z, y, (ChunkWidth - 1))] = BlockType::Air;
		//	}
		
		//for (int x = 0; x < ChunkWidth; x+= 3)
		//	for (int z = x; z < ChunkWidth; z++)
		//		for (int y = z; y < ChunkHeight; y++)
		//			Blocks[xyzToChunkIndex(x, y, z)] = BlockType::Air;


		// check if the chunk should get data from a neighboring chunk


		// get data from somewhere else, instead of this
		//for (int x = 0; x < ChunkWidth; x++)
		//	for (int y = 0; y < ChunkHeight; y++)
		//		for (int z = 0; z < ChunkWidth; z++)
		//		Blocks[xyzToChunkIndex(x, y, z)] = BlockType::Dirt;

		//srand(100);
		//for (int x = 0; x < ChunkWidth; x++)
		//	for (int z = 0; z < ChunkWidth; z++)
		//	{
		//		int dirtHeight = 1 + (int)(((double)rand() / (RAND_MAX)) * (ChunkHeight - 1));
		//		for (int y = dirtHeight / 2; y < dirtHeight; y++)
		//			Blocks[xyzToChunkIndex(x, y, z)] = BlockType::Dirt;
		//		for (int y = 0; y < dirtHeight / 2; y++)
		//			Blocks[xyzToChunkIndex(x, y, z)] = BlockType::Stone;
		//	}

		//for (int x = ChunkWidth / 2 - 5; x < ChunkWidth / 2 + 5; x++)
		//	for (int z = ChunkWidth / 2 - 5; z < ChunkWidth / 2 + 5; z++)
		//		for (int y = 1; y < ChunkHeight - 1; y++)
		//			Blocks[xyzToChunkIndex(x, y, z)] = BlockType::Glass;

		//
		//CreateMesh();
	}
	~ChunkData() 
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_buffer);
		glDeleteBuffers(1, &m_indexBuffer);
		glDeleteBuffers(1, &m_transparentBuffer);
		glDeleteBuffers(1, &m_transparentIndexBuffer);
		delete[](Blocks);
	}
	void CreateMesh();

	inline unsigned int Buffer() const { return m_buffer; }
	inline unsigned int TransparentBuffer() const { return m_transparentBuffer; }
	inline unsigned int IndexBuffer() const { return m_indexBuffer; }
	inline unsigned int TransparentIndexBuffer() const { return m_transparentIndexBuffer; }

	inline size_t VertexCount() const { return m_vertexCount; }
	inline size_t IndeciesCount() const { return m_indeciesCount; }
	inline size_t TransparentVertexCount() const { return m_transparentVertexCount; }
	inline size_t TransparentIndeciesCount() const { return m_transparentIndeciesCount; }

	inline const Vector3& Offset() const { return m_offset; }
	inline const Vector3& OffsetForRendering() const { return m_offsetForRendering; }
	static inline bool IsTransparent(BlockType block)
	{
		return TransparentBlockDefinitions[block];
		//return block == BlockType::Air || block == BlockType::Glass || block == BlockType::Water;
	}

	BlockType GetBlock(Vector3Int pos);
	void DeleteBlock(const Vector3Int& pos);
	inline BlockType GetBlockInChunk(Vector3Int pos)
	{
		return Blocks[xyzToChunkIndex(pos.x, pos.y, pos.z)];
	}

	static void SetAttribArray()
	{
		unsigned int stride = sizeof(ChunkVertex);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(offsetof(ChunkVertex, Uv)));

		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void*)(offsetof(ChunkVertex, m_Color)));
	}
	static void UnsetAttribArray()
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
};







//struct ChunkData // store vetecies and indicies on cpu, then upload it to GPU each time... not very good :(
//{
//private:
//	mutable size_t m_vertexCount;
//	mutable size_t m_indeciesCount;
//	mutable size_t m_transparentVertexCount;
//	mutable size_t m_transparentIndeciesCount;
//
//	Vector3 m_offset;
//	Vector3 m_offsetForRendering;
//
//	ChunkVertex* m_normalVertecies;
//	ChunkVertex* m_transparentVertecies;
//	unsigned int* m_normalIndicies;
//	unsigned int* m_transparentIndicies;
//	 
//	//std::vector<ChunkVertex> m_normalVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	//std::vector<ChunkVertex> m_transparentVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	//std::vector<unsigned int> m_normalIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	//std::vector<unsigned int> m_transparentIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//public: // per object stuff
//	BlockType* Blocks;
//	//std::vector<Vertex> verticies;
//	ChunkData() { std::cout << "ChunkData default constructor called... why?" << std::endl; }
//
//	ChunkData(Vector3 offset) : Blocks(new BlockType[ChunkWidth * ChunkHeight * ChunkWidth]), m_vertexCount(0), m_indeciesCount(0)
//	{
//		m_offset = offset;
//		m_offsetForRendering = m_offset;
//		m_offsetForRendering.x *= ChunkWidth * BlockSize;
//		m_offsetForRendering.y *= ChunkHeight * BlockSize;
//		m_offsetForRendering.z *= ChunkWidth * BlockSize;
//
//		m_normalVertecies = new ChunkVertex[1];
//		m_transparentVertecies = new ChunkVertex[1];
//		m_normalIndicies = new unsigned int[1];
//		m_transparentIndicies = new unsigned int[1];
//
//		// get save data?
//
//		// create it if no save data exists
//		double noiseMultiplier = 100;
//		double stoneLine = 0;
//		double waterLine = 3;
//		double sandLine = waterLine + 3;
//		Vector3Int noiseOffset = { (int)m_offsetForRendering.x, (int)m_offsetForRendering.y, (int)m_offsetForRendering.z };
//
//		for (int x = 0; x < ChunkWidth; x++)
//			for (int z = 0; z < ChunkWidth; z++)
//			{
//				double noiseValue = PerlinNoise::Noise(x + noiseOffset.x, z + noiseOffset.z) * noiseMultiplier - noiseOffset.y;
//				for (int y = 0; y < ChunkHeight; y++)
//				{
//					int yValue = noiseOffset.y + y;
//					BlockType block = BlockType::Air;
//					bool hasBlock = yValue <= noiseValue;
//					if (hasBlock)
//					{
//						if (yValue < stoneLine)
//							block = BlockType::Stone;
//						else if (yValue < waterLine)
//							block = BlockType::Water;
//						else if (yValue < sandLine)
//							block = BlockType::Sand;
//						else
//							block = BlockType::Dirt;
//					}
//					Blocks[xyzToChunkIndex(x, y, z)] = block;
//				}
//			}
//
//		for (int x = 0; x < ChunkWidth; x++)
//			for (int y = 0; y < ChunkHeight; y++)
//				for (int z = 0; z < ChunkWidth; z++)
//					Blocks[xyzToChunkIndex(x, y, z)] = Dirt;
//	}
//	~ChunkData()
//	{
//		delete[](m_normalVertecies);
//		delete[](m_normalIndicies);
//		delete[](m_transparentVertecies);
//		delete[](m_transparentIndicies);
//		delete[](Blocks);
//	}
//	// also some sort of data buffer on the GPU
//	void CreateMesh();
//	//void CreateAndSaveMesh();
//	/// <summary>
//	/// uploads mesh data to gpu, and keeps the vertexAttribpointers bound.
//	/// </summary>
//	void UploadSavedNormalMeshToGPU() const;
//	void UploadSavedTransparentMeshToGPU() const;
//	/*inline unsigned int Buffer() const;
//	inline size_t VertexCount() const;*/
//
//	inline size_t VertexCount() const { return m_vertexCount; }
//	inline size_t IndeciesCount() const { return m_indeciesCount; }
//	inline size_t TransparentVertexCount() const { return m_transparentVertexCount; }
//	inline size_t TransparentIndeciesCount() const { return m_transparentIndeciesCount; }
//
//	inline const Vector3& Offset() const { return m_offset; }
//	inline const Vector3& OffsetForRendering() const { return m_offsetForRendering; }
//	static inline bool IsTransparent(BlockType block)
//	{
//		return block == BlockType::Air || block == BlockType::Glass || block == BlockType::Water;
//	}
//};

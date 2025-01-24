#include "ChunkData.h"
#include <iostream>
#include "Graphics/Renderer.h"
#include "Time.h"
#include "Game/World.h"

bool ChunkData::TransparentBlockDefinitions[BlockType::Count] =
{
	true, // air
	false, // dirt
	false, // stone
	false, // sand
	true, // glass
	true, // water
};

BlockType ChunkData::GetBlock(Vector3Int pos)
{
	ChunkData* chunk = this;
	Vector3Int offset = m_offsetInt;
	// check for chunk borders
	// x
	if (pos.x < 0)
	{
		pos.x += ChunkWidth;
		offset.x--;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	if (pos.x >= ChunkWidth)
	{
		pos.x -= ChunkWidth;
		offset.x++;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	// y
	if (pos.y < 0)
	{
		pos.y += ChunkHeight;
		offset.y--;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	if (pos.y >= ChunkHeight)
	{
		pos.y -= ChunkHeight;
		offset.y++;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	// z
	if (pos.z < 0)
	{
		pos.z += ChunkWidth;
		offset.z--;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	if (pos.z >= ChunkWidth)
	{
		pos.z -= ChunkWidth;
		offset.z++;
		chunk = World::ActiveWorld()->GetChunk(offset);
	}
	return chunk->Blocks[xyzToChunkIndex(pos.x, pos.y, pos.z)];
}

void ChunkData::DeleteBlock(const Vector3Int& pos)
{
	Blocks[xyzToChunkIndex(pos.x, pos.y, pos.z)] = BlockType::Air;
}


void ChunkData::CreateMesh()
{
	{
		const World& world = *World::ActiveWorld(); // for getting adjecent chunks blocks
		// check adjecent chunks are loaded in
		world.LoadChunk({ (m_offsetInt.x - 1), (m_offsetInt.y) , (m_offsetInt.z) });
		world.LoadChunk({ (m_offsetInt.x + 1), (m_offsetInt.y) , (m_offsetInt.z) });
		world.LoadChunk({ (m_offsetInt.x), (m_offsetInt.y - 1) , (m_offsetInt.z) });
		world.LoadChunk({ (m_offsetInt.x), (m_offsetInt.y + 1) , (m_offsetInt.z) });
		world.LoadChunk({ (m_offsetInt.x), (m_offsetInt.y) , (m_offsetInt.z - 1) });
		world.LoadChunk({ (m_offsetInt.x), (m_offsetInt.y) , (m_offsetInt.z + 1) });
	}
	//Time::Timer initCreating("init");
	std::vector<ChunkVertex> normalVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
	std::vector<ChunkVertex> transparentVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
	normalVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6 * sizeof(ChunkVertex));
	transparentVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6 * sizeof(ChunkVertex));
	std::vector<unsigned int> normalIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
	std::vector<unsigned int> transparentIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
	normalIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 6 * 6);
	transparentIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 6 * 6);
	//initCreating.Stop();
	
	//Vector3 forward = { 0, 0, 1 };
	//Vector3 right = { 1, 0, 0 };
	//Vector3 up = { 0, 1, 0 };
	//Time::Timer meshCreating("CreateMesh");
	// try to create full block, just for fun :)
	//float uv;
	for (int x = 0; x < ChunkWidth; x++)
		for (int y = 0; y < ChunkHeight; y++)
			for (int z = 0; z < ChunkWidth; z++)
			{
				//int index = xyzToChunkIndex(x, y, z);
				BlockType block = Blocks[xyzToChunkIndex(x, y, z)];
				if (block == BlockType::Air)
					continue;
				bool isTransparent = IsTransparent(block);
				std::vector<ChunkVertex>& vertecies = isTransparent ? transparentVertecies : normalVertecies;
				std::vector<unsigned int>& indicies = isTransparent ? transparentIndicies : normalIndicies;
				float startX = float(x) * BlockSize;
				float startY = float(y) * BlockSize;
				float startZ = float(z) * BlockSize;
				float endX = startX + BlockSize;
				float endY = startY + BlockSize;
				float endZ = startZ + BlockSize;
				float uvXSize = 1.0f / BlockTexturesPerRow;
				float uvYSize = 1.0f / BlocksInTexture;
				float uvYStart = float(block - 1) * uvYSize; // - 1, since air doesn't have a texture :)

				// front s +z
				BlockType frontBlock = GetBlock({ x, y, z + 1 });
				if ((!isTransparent && IsTransparent(frontBlock)) || (isTransparent && frontBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 2;
					vertecies.push_back({ {startX, startY, endZ}, {uvXStart			 , uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX,   endY, endZ}, {uvXStart			 , uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX, startY, endZ}, {uvXStart + uvXSize, uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX,   endY, endZ}, {uvXStart + uvXSize, uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(0 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
				}

				// back n -z
				BlockType backBlock = GetBlock({ x, y, z - 1 });
				if ((!isTransparent && IsTransparent(backBlock)) || (isTransparent && backBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 3;
					vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX,   endY, startZ}, {uvXStart + uvXSize, uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(0 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
				}

				// up +y
				BlockType upBlock = GetBlock({ x, y + 1, z });
				if ((!isTransparent && IsTransparent(upBlock)) || (isTransparent && upBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 0;
					vertecies.push_back({ {startX,  endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX,  endY,   endZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX,  endY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX,  endY, startZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(2 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
				}
				// down -y
				BlockType downBlock = GetBlock({ x, y - 1, z });
				if ((!isTransparent && IsTransparent(downBlock)) || (isTransparent && downBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 1;
					vertecies.push_back({ {  endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX, startY,   endZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(2 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
				}

				// right e +x
				BlockType rightBlock = GetBlock({ x + 1, y, z });
				if ((!isTransparent && IsTransparent(rightBlock)) || (isTransparent && rightBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 4;
					vertecies.push_back({ { endX, startY, startZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ { endX,   endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ { endX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ { endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(2 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
				}
				// left w -x
				BlockType leftBlock = GetBlock({ x - 1, y, z });
				if ((!isTransparent && IsTransparent(leftBlock)) || (isTransparent && leftBlock == BlockType::Air))
				{
					int indeciesStart = vertecies.size();
					float uvXStart = uvXSize * 5;
					vertecies.push_back({ {startX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });

					indicies.push_back(2 + indeciesStart);
					indicies.push_back(1 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
					indicies.push_back(3 + indeciesStart);
					indicies.push_back(2 + indeciesStart);
					indicies.push_back(0 + indeciesStart);
				}
			}
	//meshCreating.Stop();
	
	//// front s
	//vertecies.push_back({ {-.5, -.5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
	//vertecies.push_back({ {-.5,  .5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
	//vertecies.push_back({ { .5, -.5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
	//vertecies.push_back({ { .5,  .5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });

	//indicies.push_back(0 + 4 * 0);
	//indicies.push_back(1 + 4 * 0);
	//indicies.push_back(2 + 4 * 0);
	//indicies.push_back(1 + 4 * 0);
	//indicies.push_back(3 + 4 * 0);
	//indicies.push_back(2 + 4 * 0);

	//// back n
	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });

	//indicies.push_back(0 + 4 * 1);
	//indicies.push_back(1 + 4 * 1);
	//indicies.push_back(2 + 4 * 1);
	//indicies.push_back(0 + 4 * 1);
	//indicies.push_back(2 + 4 * 1);
	//indicies.push_back(3 + 4 * 1);

	//// right e
	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
	//vertecies.push_back({ { .5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
	//vertecies.push_back({ { .5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });

	//indicies.push_back(2 + 4 * 2);
	//indicies.push_back(1 + 4 * 2);
	//indicies.push_back(0 + 4 * 2);
	//indicies.push_back(3 + 4 * 2);
	//indicies.push_back(2 + 4 * 2);
	//indicies.push_back(0 + 4 * 2);

	//// left w
	//vertecies.push_back({ {-.5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
	//vertecies.push_back({ {-.5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });

	//indicies.push_back(2 + 4 * 3);
	//indicies.push_back(1 + 4 * 3);
	//indicies.push_back(0 + 4 * 3);
	//indicies.push_back(3 + 4 * 3);
	//indicies.push_back(2 + 4 * 3);
	//indicies.push_back(0 + 4 * 3);

	//// up
	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
	//vertecies.push_back({ {-.5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
	//vertecies.push_back({ { .5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });

	//indicies.push_back(2 + 4 * 4);
	//indicies.push_back(1 + 4 * 4);
	//indicies.push_back(0 + 4 * 4);
	//indicies.push_back(3 + 4 * 4);
	//indicies.push_back(2 + 4 * 4);
	//indicies.push_back(0 + 4 * 4);

	//// down
	//vertecies.push_back({ { .5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
	//vertecies.push_back({ {-.5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });

	//indicies.push_back(2 + 4 * 5);
	//indicies.push_back(1 + 4 * 5);
	//indicies.push_back(0 + 4 * 5);
	//indicies.push_back(3 + 4 * 5);
	//indicies.push_back(2 + 4 * 5);
	//indicies.push_back(0 + 4 * 5);

	//Time::Timer uploadToGPU("Copy verticies");
	unsigned int stride = sizeof(ChunkVertex);
	// opague
	m_vertexCount = normalVertecies.size();
	ChunkVertex* normalVertexData = m_vertexCount == 0 ? nullptr : (ChunkVertex*)malloc(sizeof(ChunkVertex) * m_vertexCount);
	for (int i = 0; i < m_vertexCount; i++)
		normalVertexData[i] = normalVertecies[i];
	m_indeciesCount = normalIndicies.size();
	unsigned int* normalIndiciesArray = m_indeciesCount == 0 ? nullptr : new unsigned int[m_indeciesCount];
	for (int i = 0; i < m_indeciesCount; i++)
		normalIndiciesArray[i] = normalIndicies[i];
	//if (m_vertexCount != 0)
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	//	glBufferData(GL_ARRAY_BUFFER, m_vertexCount * (sizeof(ChunkVertex)), &normalVertecies[0], GL_STATIC_DRAW);
	//	//glBufferData(GL_ARRAY_BUFFER, m_vertexCount * (sizeof(ChunkVertex)), normalVertexData, GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeciesCount * sizeof(unsigned int), (const void*)&normalIndicies.front(), GL_STATIC_DRAW);
	//	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeciesCount * sizeof(unsigned int), (const void*)normalIndiciesArray, GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}

	// transparent

	m_transparentVertexCount = transparentVertecies.size();
	ChunkVertex* transparentVertexData = m_transparentVertexCount == 0 ? nullptr : (ChunkVertex*)malloc(sizeof(ChunkVertex) * m_transparentVertexCount);
	for (int i = 0; i < m_transparentVertexCount; i++)
		transparentVertexData[i] = transparentVertecies[i];

	m_transparentIndeciesCount = transparentIndicies.size();
	unsigned int* transparentIndiciesArray = m_transparentIndeciesCount == 0 ? nullptr : new unsigned int[m_transparentIndeciesCount];
	for (int i = 0; i < m_transparentIndeciesCount; i++)
		transparentIndiciesArray[i] = transparentIndicies[i];

	//if (m_transparentVertexCount != 0)
	//{
	//	glBindBuffer(GL_ARRAY_BUFFER, m_transparentBuffer);
	//	glBufferData(GL_ARRAY_BUFFER, m_transparentVertexCount * (sizeof(ChunkVertex)), (const void*)&transparentVertecies.front(), GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndexBuffer);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndeciesCount * sizeof(unsigned int), (const void*)&transparentIndicies[0], GL_STATIC_DRAW);

	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//}
	
	//uploadToGPU.Stop();
	//Time::Timer addToMeshQueueTimer("Add to mesh queue");
	World::ActiveWorld()->AddChunkMeshToQueue(m_offsetInt,
		normalVertexData, m_vertexCount, normalIndiciesArray, m_indeciesCount,
		transparentVertexData, m_transparentVertexCount, transparentIndiciesArray, m_transparentIndeciesCount);
	//addToMeshQueueTimer.Stop();
	//for (int x = 0; x < ChunkWidth; x++)
	//	for (int y = 0; y < ChunkHeight; y++)
	//	{
	//		BlockType& block = Blocks[xyToChunkIndex(x, y)];
	//		if (block == BlockType::Air)
	//			continue;
	//		// we have a block
	//	}

	// for the time being
	//vertecies.emplace_back({})
}

//void ChunkData::CreateAndSaveMesh()
//{
//	m_normalVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 4);
//	m_transparentVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 4);
//	m_normalIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6);
//	m_transparentIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6);
//	//initCreating.Stop();
//
//	//Vector3 forward = { 0, 0, 1 };
//	//Vector3 right = { 1, 0, 0 };
//	//Vector3 up = { 0, 1, 0 };
//	//Time::Timer meshCreating("CreateMesh");
//	// try to create full block, just for fun :)
//	//float uv;
//	for (int x = 0; x < ChunkWidth; x++)
//		for (int y = 0; y < ChunkHeight; y++)
//			for (int z = 0; z < ChunkWidth; z++)
//			{
//				//int index = xyzToChunkIndex(x, y, z);
//				BlockType block = Blocks[xyzToChunkIndex(x, y, z)];
//				switch (block)
//				{
//				case BlockType::Air:
//					continue;
//				case BlockType::Dirt:
//				case BlockType::Sand:
//					break;
//				}
//				bool isTransparent = IsTransparent(block);
//				std::vector<ChunkVertex>& vertecies = isTransparent ? m_transparentVertecies : m_normalVertecies;
//				std::vector<unsigned int>& indicies = isTransparent ? m_transparentIndicies : m_normalIndicies;
//				float startX = float(x) * BlockSize;
//				float startY = float(y) * BlockSize;
//				float startZ = float(z) * BlockSize;
//				float endX = startX + BlockSize;
//				float endY = startY + BlockSize;
//				float endZ = startZ + BlockSize;
//				float uvXSize = 1.0f / BlockTexturesPerRow;
//				float uvYSize = 1.0f / BlocksInTexture;
//				float uvYStart = float(block - 1) * uvYSize; // - 1, since air doesn't have a texture :)
//
//				// front s +z
//				if (z == ChunkWidth - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, y, (z + 1))])) || (isTransparent && Blocks[xyzToChunkIndex(x, y, (z + 1))] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 2;
//					vertecies.push_back({ {startX, startY, endZ}, {uvXStart			 , uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX,   endY, endZ}, {uvXStart			 , uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX, startY, endZ}, {uvXStart + uvXSize, uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX,   endY, endZ}, {uvXStart + uvXSize, uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//				}
//
//				// back n -z
//				if (z == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, y, (z - 1))])) || (isTransparent && Blocks[xyzToChunkIndex(x, y, (z - 1))] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 3;
//					vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX,   endY, startZ}, {uvXStart + uvXSize, uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//				}
//
//				// up +y
//				if (y == ChunkHeight - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, (y + 1), z)])) || (isTransparent && Blocks[xyzToChunkIndex(x, (y + 1), z)] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 0;
//					vertecies.push_back({ {startX,  endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX,  endY,   endZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX,  endY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX,  endY, startZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//				}
//				// down -y
//				if (y == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, (y - 1), z)])) || (isTransparent && Blocks[xyzToChunkIndex(x, (y - 1), z)] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 1;
//					vertecies.push_back({ {  endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX, startY,   endZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//				}
//
//				// right e +x
//				if (x == ChunkWidth - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex((x + 1), y, z)])) || (isTransparent && Blocks[xyzToChunkIndex((x + 1), y, z)] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 4;
//					vertecies.push_back({ { endX, startY, startZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ { endX,   endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ { endX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ { endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//				}
//				// left w -x
//				if (x == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex((x - 1), y, z)])) || (isTransparent && Blocks[xyzToChunkIndex((x - 1), y, z)] == BlockType::Air))
//				{
//					int indeciesStart = vertecies.size();
//					float uvXStart = uvXSize * 5;
//					vertecies.push_back({ {startX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//					vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(1 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//					indicies.push_back(3 + indeciesStart);
//					indicies.push_back(2 + indeciesStart);
//					indicies.push_back(0 + indeciesStart);
//				}
//			}
//}
//
//void ChunkData::UploadSavedNormalMeshToGPU() const
//{
//	unsigned int stride = sizeof(ChunkVertex);
//	// opague
//
//	m_vertexCount = m_normalVertecies.size();
//	m_indeciesCount = m_normalIndicies.size();
//	if (m_vertexCount != 0)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
//		glBufferData(GL_ARRAY_BUFFER, m_vertexCount * (sizeof(ChunkVertex)), &m_normalVertecies[0], GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));
//
//		glEnableVertexAttribArray(2);
//		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2)));
//
//		glEnableVertexAttribArray(3);
//		glVertexAttribIPointer(3, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2) + sizeof(int)));
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeciesCount * sizeof(unsigned int), (const void*)&m_normalIndicies.front(), GL_STATIC_DRAW);
//	}
//}
//
//
//void ChunkData::UploadSavedTransparentMeshToGPU() const
//{
//	unsigned int stride = sizeof(ChunkVertex);
//	// transparent
//
//	m_transparentVertexCount = m_transparentVertecies.size();
//	m_transparentIndeciesCount = m_transparentIndicies.size();
//	if (m_transparentVertexCount != 0)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_transparentBuffer);
//		glBufferData(GL_ARRAY_BUFFER, m_transparentVertexCount * (sizeof(ChunkVertex)), (const void*)&m_transparentVertecies.front(), GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));
//
//		glEnableVertexAttribArray(2);
//		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2)));
//
//		glEnableVertexAttribArray(3);
//		glVertexAttribIPointer(3, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2) + sizeof(int)));
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndexBuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndeciesCount * sizeof(unsigned int), (const void*)&m_transparentIndicies[0], GL_STATIC_DRAW);
//	}
//}






//unsigned int ChunkData::Buffer() const { return m_buffer; }
//size_t ChunkData::VertexCount() const { return m_vertexCount; }

//void ChunkData::CreateMesh()
//{
//	//Time::Timer initCreating("init");
//	std::vector<ChunkVertex> normalVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	std::vector<ChunkVertex> transparentVertecies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	normalVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 4);
//	transparentVertecies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 4);
//	std::vector<unsigned int> normalIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	std::vector<unsigned int> transparentIndicies;// (ChunkWidth * ChunkHeight * ChunkWidth);
//	normalIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6);
//	transparentIndicies.reserve(ChunkWidth * ChunkHeight * ChunkWidth * 4 * 6);
//	//initCreating.Stop();
//
//	//Vector3 forward = { 0, 0, 1 };
//	//Vector3 right = { 1, 0, 0 };
//	//Vector3 up = { 0, 1, 0 };
//	//Time::Timer meshCreating("CreateMesh");
//	// try to create full block, just for fun :)
//	//float uv;
//	if (true)
//		for (int x = 0; x < ChunkWidth; x++)
//			for (int y = 0; y < ChunkHeight; y++)
//				for (int z = 0; z < ChunkWidth; z++)
//				{
//					//int index = xyzToChunkIndex(x, y, z);
//					BlockType block = Blocks[xyzToChunkIndex(x, y, z)];
//					switch (block)
//					{
//					case BlockType::Air:
//						continue;
//					case BlockType::Dirt:
//					case BlockType::Sand:
//						break;
//					}
//					bool isTransparent = IsTransparent(block);
//					std::vector<ChunkVertex>& vertecies = isTransparent ? transparentVertecies : normalVertecies;
//					std::vector<unsigned int>& indicies = isTransparent ? transparentIndicies : normalIndicies;
//					float startX = float(x) * BlockSize;
//					float startY = float(y) * BlockSize;
//					float startZ = float(z) * BlockSize;
//					float endX = startX + BlockSize;
//					float endY = startY + BlockSize;
//					float endZ = startZ + BlockSize;
//					float uvXSize = 1.0f / BlockTexturesPerRow;
//					float uvYSize = 1.0f / BlocksInTexture;
//					float uvYStart = float(block - 1) * uvYSize; // - 1, since air doesn't have a texture :)
//
//					// front s +z
//					if (z == ChunkWidth - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, y, (z + 1))])) || (isTransparent && Blocks[xyzToChunkIndex(x, y, (z + 1))] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 2;
//						vertecies.push_back({ {startX, startY, endZ}, {uvXStart			 , uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX,   endY, endZ}, {uvXStart			 , uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX, startY, endZ}, {uvXStart + uvXSize, uvYStart + uvYSize }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX,   endY, endZ}, {uvXStart + uvXSize, uvYStart			  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//					}
//
//					// back n -z
//					if (z == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, y, (z - 1))])) || (isTransparent && Blocks[xyzToChunkIndex(x, y, (z - 1))] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 3;
//						vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX,   endY, startZ}, {uvXStart + uvXSize, uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//					}
//
//					// up +y
//					if (y == ChunkHeight - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, (y + 1), z)])) || (isTransparent && Blocks[xyzToChunkIndex(x, (y + 1), z)] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 0;
//						vertecies.push_back({ {startX,  endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX,  endY,   endZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX,  endY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX,  endY, startZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//					}
//					// down -y
//					if (y == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex(x, (y - 1), z)])) || (isTransparent && Blocks[xyzToChunkIndex(x, (y - 1), z)] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 1;
//						vertecies.push_back({ {  endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX, startY,   endZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {  endX, startY, startZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//					}
//
//					// right e +x
//					if (x == ChunkWidth - 1 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex((x + 1), y, z)])) || (isTransparent && Blocks[xyzToChunkIndex((x + 1), y, z)] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 4;
//						vertecies.push_back({ { endX, startY, startZ}, {uvXStart		  , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ { endX,   endY, startZ}, {uvXStart		  , uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ { endX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		  }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ { endX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//					}
//					// left w -x
//					if (x == 0 || (!isTransparent && IsTransparent(Blocks[xyzToChunkIndex((x - 1), y, z)])) || (isTransparent && Blocks[xyzToChunkIndex((x - 1), y, z)] == BlockType::Air))
//					{
//						int indeciesStart = vertecies.size();
//						float uvXStart = uvXSize * 5;
//						vertecies.push_back({ {startX, startY,   endZ}, {uvXStart + uvXSize, uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX,   endY,   endZ}, {uvXStart + uvXSize, uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX,   endY, startZ}, {uvXStart		   , uvYStart		   }, ChunkVertex::GetColor(255, 255, 255), (int)block });
//						vertecies.push_back({ {startX, startY, startZ}, {uvXStart		   , uvYStart + uvYSize}, ChunkVertex::GetColor(255, 255, 255), (int)block });
//
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(1 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//						indicies.push_back(3 + indeciesStart);
//						indicies.push_back(2 + indeciesStart);
//						indicies.push_back(0 + indeciesStart);
//					}
//				}
//	//meshCreating.Stop();
//
//	//// front s
//	//vertecies.push_back({ {-.5, -.5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
//	//vertecies.push_back({ {-.5,  .5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
//	//vertecies.push_back({ { .5, -.5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
//	//vertecies.push_back({ { .5,  .5, .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 0) });
//
//	//indicies.push_back(0 + 4 * 0);
//	//indicies.push_back(1 + 4 * 0);
//	//indicies.push_back(2 + 4 * 0);
//	//indicies.push_back(1 + 4 * 0);
//	//indicies.push_back(3 + 4 * 0);
//	//indicies.push_back(2 + 4 * 0);
//
//	//// back n
//	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
//	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
//	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
//	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 0) });
//
//	//indicies.push_back(0 + 4 * 1);
//	//indicies.push_back(1 + 4 * 1);
//	//indicies.push_back(2 + 4 * 1);
//	//indicies.push_back(0 + 4 * 1);
//	//indicies.push_back(2 + 4 * 1);
//	//indicies.push_back(3 + 4 * 1);
//
//	//// right e
//	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
//	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
//	//vertecies.push_back({ { .5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
//	//vertecies.push_back({ { .5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 0, 255) });
//
//	//indicies.push_back(2 + 4 * 2);
//	//indicies.push_back(1 + 4 * 2);
//	//indicies.push_back(0 + 4 * 2);
//	//indicies.push_back(3 + 4 * 2);
//	//indicies.push_back(2 + 4 * 2);
//	//indicies.push_back(0 + 4 * 2);
//
//	//// left w
//	//vertecies.push_back({ {-.5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
//	//vertecies.push_back({ {-.5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
//	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
//	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 255, 0) });
//
//	//indicies.push_back(2 + 4 * 3);
//	//indicies.push_back(1 + 4 * 3);
//	//indicies.push_back(0 + 4 * 3);
//	//indicies.push_back(3 + 4 * 3);
//	//indicies.push_back(2 + 4 * 3);
//	//indicies.push_back(0 + 4 * 3);
//
//	//// up
//	//vertecies.push_back({ {-.5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
//	//vertecies.push_back({ {-.5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
//	//vertecies.push_back({ { .5,  .5,  .5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
//	//vertecies.push_back({ { .5,  .5, -.5}, {0, 0}, ChunkVertex::GetColor(0, 255, 255) });
//
//	//indicies.push_back(2 + 4 * 4);
//	//indicies.push_back(1 + 4 * 4);
//	//indicies.push_back(0 + 4 * 4);
//	//indicies.push_back(3 + 4 * 4);
//	//indicies.push_back(2 + 4 * 4);
//	//indicies.push_back(0 + 4 * 4);
//
//	//// down
//	//vertecies.push_back({ { .5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
//	//vertecies.push_back({ {-.5, -.5,  .5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
//	//vertecies.push_back({ {-.5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
//	//vertecies.push_back({ { .5, -.5, -.5}, {0, 0}, ChunkVertex::GetColor(255, 0, 255) });
//
//	//indicies.push_back(2 + 4 * 5);
//	//indicies.push_back(1 + 4 * 5);
//	//indicies.push_back(0 + 4 * 5);
//	//indicies.push_back(3 + 4 * 5);
//	//indicies.push_back(2 + 4 * 5);
//	//indicies.push_back(0 + 4 * 5);
//
//	//Time::Timer uploadToGPU("Upload to gpu");
//
//
//	unsigned int stride = sizeof(ChunkVertex);
//	// opague
//
//	m_vertexCount = normalVertecies.size();
//	m_indeciesCount = normalIndicies.size();
//	if (m_vertexCount != 0)
//	{
//		delete[](m_normalVertecies);
//		m_normalVertecies = new ChunkVertex[m_vertexCount]; //memccpy(m_normalVertecies, &normalVertecies[0], 0, m_vertexCount * stride);
//		for (int i = 0; i < m_vertexCount; i++)
//			m_normalVertecies[i] = normalVertecies[i];
//		delete[](m_normalIndicies);
//		m_normalIndicies = new unsigned int[m_indeciesCount];
//		for (int i = 0; i < m_indeciesCount; i++)
//			m_normalIndicies[i] = normalIndicies[i];
//	}
//
//	// transparent
//	m_transparentVertexCount = transparentVertecies.size();
//	m_transparentIndeciesCount = transparentIndicies.size();
//	if (m_transparentVertexCount != 0)
//	{
//		delete[](m_transparentVertecies);
//		m_transparentVertecies = new ChunkVertex[m_transparentVertexCount]; //memccpy(m_normalVertecies, &normalVertecies[0], 0, m_vertexCount * stride);
//		for (int i = 0; i < m_transparentVertexCount; i++)
//			m_transparentVertecies[i] = transparentVertecies[i];
//		delete[](m_transparentIndicies);
//		m_transparentIndicies = new unsigned int[m_transparentIndeciesCount];
//		for (int i = 0; i < m_transparentIndeciesCount; i++)
//			m_transparentIndicies[i] = transparentIndicies[i];
//	}
//}
//void ChunkData::UploadSavedNormalMeshToGPU() const
//{
//	unsigned int stride = sizeof(ChunkVertex);
//	// opague
//	//if (m_vertexCount != 0)
//	{
//		//glBindBuffer(GL_ARRAY_BUFFER, buffer);
//		glBufferData(GL_ARRAY_BUFFER, m_vertexCount * (sizeof(ChunkVertex)), m_normalVertecies, GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));
//
//		glEnableVertexAttribArray(2);
//		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2)));
//
//		glEnableVertexAttribArray(3);
//		glVertexAttribIPointer(3, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2) + sizeof(int)));
//
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indeciesCount * sizeof(unsigned int), (const void*)m_normalIndicies, GL_STATIC_DRAW);
//	}
//}
//void ChunkData::UploadSavedTransparentMeshToGPU() const
//{
//	unsigned int stride = sizeof(ChunkVertex);
//	// transparent
//	//if (m_transparentVertexCount != 0)
//	{
//		//glBindBuffer(GL_ARRAY_BUFFER, m_transparentBuffer);
//		glBufferData(GL_ARRAY_BUFFER, m_transparentVertexCount * (sizeof(ChunkVertex)), (const void*)m_transparentVertecies, GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));
//
//		glEnableVertexAttribArray(2);
//		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2)));
//
//		glEnableVertexAttribArray(3);
//		glVertexAttribIPointer(3, 1, GL_INT, stride, (const void*)(sizeof(Vector3) + sizeof(Vector2) + sizeof(int)));
//
//		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndexBuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_transparentIndeciesCount * sizeof(unsigned int), (const void*)m_transparentIndicies, GL_STATIC_DRAW);
//	}
//}


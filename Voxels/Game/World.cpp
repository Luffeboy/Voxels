#include "World.h"
#include <thread>

World* World::m_ActiveWorld = new World();

void World::LoadChunkWithMeshNow(const Vector3Int& position) const
{
	LoadChunk(position);
	int64_t key = position.GetValueForCompairing();
	m_Chunks[key].CreateMesh();
}

void World::LoadChunkMeshAsync(const Vector3Int& position) const
{
	int64_t key = position.GetValueForCompairing();
	ActiveWorld()->QueueChunkForMeshGeneration(key);
	//int64_t id = ActiveWorld()->_nextThreadId++;
	//m_chunkGenerationThreads.emplace_back(id, World::CreateChunkMesh, key);
}
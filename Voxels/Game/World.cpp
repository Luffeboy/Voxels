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
	/*ActiveWorld()->*/QueueChunkForMeshGeneration(key);
	//int64_t id = ActiveWorld()->_nextThreadId++;
	//m_chunkGenerationThreads.emplace_back(id, World::CreateChunkMesh, key);
}

void World::UnloadChunk(const Vector3Int& position) const
{
	std::lock_guard<std::mutex> guard(m_loadChunksMutex);
	int64_t key = position.GetValueForCompairing();
	auto it = m_Chunks.find(key);
	if (it != m_Chunks.end())
		m_Chunks.erase(key);
}
void World::UnloadChunkRegion(const Vector3Int& start, const Vector3Int& end) const
{
	for (int x = start.x; x <= end.x; x++)
		for (int y = start.y; y <= end.y; y++)
			for (int z = start.z; z <= end.z; z++)
				UnloadChunk({ x, y, z });
}
BlockType World::GetBlockData(const Vector3& pos) const
{
    PositionAndChunk pac = { pos };

    return  World::ActiveWorld()->GetChunk(pac.Chunk)->GetBlockInChunk(pac.Position);
}
RaycastHit World::Raycast(const Vector3& startPos, const Vector3& rayDir, float maxDist) const
{
    if (rayDir.x == 0 && rayDir.y == 0 && rayDir.z == 0)
    {
        std::cout << "Tried to raycast, with a direction of 0" << std::endl;
        return {};
    }
    // Make sure the rayDir is normalized
    Vector3 dir = rayDir.Normalized();

    // Start at the position of the ray
    Vector3 currentPos = startPos;

    // Step sizes for each axis
    Vector3 stepSize;
    stepSize.x = (dir.x > 0) ? 1.0f : -1.0f;
    stepSize.y = (dir.y > 0) ? 1.0f : -1.0f;
    stepSize.z = (dir.z > 0) ? 1.0f : -1.0f;
    uint8_t nextTo = 0;
    uint8_t nextToX = (dir.x > 0) ? (1 << 0) : (1 << 1);
    uint8_t nextToY = (dir.y > 0) ? (1 << 2) : (1 << 3);
    uint8_t nextToZ = (dir.z > 0) ? (1 << 4) : (1 << 5);

    // Calculate the initial tMax values for each axis (how far we need to go to hit the next voxel boundary)
    Vector3 tMax;
    tMax.x = (dir.x > 0) ? (std::floor(currentPos.x) + 1.0f - currentPos.x) / dir.x : (std::floor(currentPos.x) - currentPos.x) / dir.x;
    tMax.y = (dir.y > 0) ? (std::floor(currentPos.y) + 1.0f - currentPos.y) / dir.y : (std::floor(currentPos.y) - currentPos.y) / dir.y;
    tMax.z = (dir.z > 0) ? (std::floor(currentPos.z) + 1.0f - currentPos.z) / dir.z : (std::floor(currentPos.z) - currentPos.z) / dir.z;

    // Calculate the initial tDelta values for each axis (the time increment for each axis)
    Vector3 tDelta;
    tDelta.x = std::abs(1.0f / dir.x);
    tDelta.y = std::abs(1.0f / dir.y);
    tDelta.z = std::abs(1.0f / dir.z);

    // Initialize the distance traveled along the ray
    float distanceTraveled = 0.0f;
    // Loop to step through the grid
    while (distanceTraveled < maxDist) {
        // Check the current voxel's data (use GetBlockData)
        BlockType block = GetBlockData(currentPos);
        if (block != BlockType::Air) {
            // If block data indicates a hit, return the corresponding hit info
            return RaycastHit(true, { currentPos }, nextTo);
        }

        // Advance to the next voxel boundary
        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                currentPos.x += stepSize.x;
                tMax.x += tDelta.x;
                nextTo = nextToX;
            }
            else {
                currentPos.z += stepSize.z;
                tMax.z += tDelta.z;
                nextTo = nextToZ;
            }
        }
        else {
            if (tMax.y < tMax.z) {
                currentPos.y += stepSize.y;
                tMax.y += tDelta.y;
                nextTo = nextToY;
            }
            else {
                currentPos.z += stepSize.z;
                tMax.z += tDelta.z;
                nextTo = nextToZ;
            }
        }

        // Update the distance traveled
        distanceTraveled += std::min({ tMax.x, tMax.y, tMax.z });
    }

    // If no hit was found within maxDist
    return RaycastHit(false, { currentPos }, 0);  // Returning default (no-hit) result
}

void World::RemoveBlock(const PositionAndChunk& pos) const
{
    auto* chunk = GetChunk(pos.Chunk);
    chunk->DeleteBlock(pos.Position);
    // regenerate mesh
    LoadChunkMeshAsync(pos.Chunk);
    // check for borders
}
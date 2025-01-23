#pragma once
#include <map>
#include "../Vector3.h"
#include "../ChunkData.h"
#include <mutex>
#include <thread>
#include <queue>

constexpr int MaxChunkGenerationThreads = 3;
class World
{
private:
	mutable std::map<int64_t, ChunkData> m_Chunks;
	mutable std::mutex m_loadChunksMutex;
	static World* m_ActiveWorld;

private:
	struct ChunkPositionAndMeshData
	{
		Vector3Int pos;

		ChunkVertex* normalMeshData = nullptr;
		size_t normalMeshVertexCount;
		unsigned int* normalIndicies = nullptr;
		size_t normalIndiciesCount;

		ChunkVertex* transparentMeshData = nullptr;
		size_t transparentMeshVertexCount;
		unsigned int* transparentIndicies = nullptr;
		size_t transparentIndiciesCount;

		ChunkPositionAndMeshData(Vector3Int pos,
			ChunkVertex* normalMeshData, size_t normalMeshVertexCount, unsigned int* normalIndicies, size_t normalIndiciesCount,
			ChunkVertex* transparentMeshData, size_t transparentMeshVertexCount, unsigned int* transparentIndicies, size_t transparentIndiciesCount) :
			pos(pos),
			normalMeshData(normalMeshData), normalMeshVertexCount(normalMeshVertexCount), normalIndicies(normalIndicies), normalIndiciesCount(normalIndiciesCount),
			transparentMeshData(transparentMeshData), transparentMeshVertexCount(transparentMeshVertexCount), transparentIndicies(transparentIndicies), transparentIndiciesCount(transparentIndiciesCount)
		{}

		~ChunkPositionAndMeshData()
		{
			if (normalMeshData != nullptr) delete[](normalMeshData);
			if (normalIndicies != nullptr) delete[](normalIndicies);
			if (transparentMeshData != nullptr) delete[](transparentMeshData);
			if (transparentIndicies != nullptr) delete[](transparentIndicies);
		}
	};
public:
	void LoadChunk(const Vector3Int& position) const
	{
		std::lock_guard<std::mutex> guard(m_loadChunksMutex);
		int64_t key = position.GetValueForCompairing();
		auto it = m_Chunks.find(key);
		if (it == m_Chunks.end())
		{
			m_Chunks.emplace(key, Vector3((float)position.x, (float)position.y, (float)position.z));
		}
	}
	void LoadChunkWithMesh(const Vector3Int& position) const
	{
		LoadChunk(position);
		LoadChunkMeshAsync(position);
		//int64_t key = position.GetValueForCompairing();
		//m_Chunks[key].CreateMesh();
	}
	void LoadChunkWithMeshNow(const Vector3Int& position) const;
	void LoadChunkRegionWithMesh(const Vector3Int& start, const Vector3Int& end) const
	{
		//Time::Timer initialLoadChunkTimer("initial load chunks");
		for (int x = start.x; x <= end.x; x++)
			for (int y = start.y; y <= end.y; y++)
				for (int z = start.z; z <= end.z; z++)
					LoadChunk({x, y, z});
		//initialLoadChunkTimer.Stop();

		//Time::Timer queueForAsyncMeshCreationTimer("queue chunks for mesh creation");
		for (int x = start.x; x <= end.x; x++)
			for (int y = start.y; y <= end.y; y++)
				for (int z = start.z; z <= end.z; z++)
				{
					LoadChunkMeshAsync({ x, y, z });
					//Vector3Int pos = { x, y, z };
					//int64_t key = pos.GetValueForCompairing();
					//m_Chunks[key].CreateMesh();
				}
		//queueForAsyncMeshCreationTimer.Stop();
	}

	static const World* ActiveWorld() { return m_ActiveWorld; }
	const std::map<int64_t, ChunkData>& Chunks() const { return m_Chunks; }
	inline ChunkData* GetChunk(const Vector3Int& position) const // assumes the chunk is allready loaded in
	{
		int64_t key = position.GetValueForCompairing();
		if (m_Chunks.find(key) == m_Chunks.end())
			LoadChunk(position);
		return &m_Chunks[key];
	}

	void UpdateQueuedMeshDataToGPU() const
	{
		std::lock_guard<std::mutex> guard(m_chunkMeshQueueMutex);
		if (m_chunkMeshDataReadyToBeUploaded.size() != 0)
		{
			while (m_chunkMeshDataReadyToBeUploaded.size() != 0)
			{
				{
					//Time::Timer uploadToGPUTimer("upload to gpu");
					const ChunkPositionAndMeshData& chunkPositionAndMeshData = *m_chunkMeshDataReadyToBeUploaded[m_chunkMeshDataReadyToBeUploaded.size() - 1];
					int64_t key = chunkPositionAndMeshData.pos.GetValueForCompairing();
					const ChunkData& chunk = m_Chunks[key];

					// normal
					glBindBuffer(GL_ARRAY_BUFFER, chunk.Buffer());
					glBufferData(GL_ARRAY_BUFFER, chunkPositionAndMeshData.normalMeshVertexCount * (sizeof(ChunkVertex)), chunkPositionAndMeshData.normalMeshData, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.IndexBuffer());
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkPositionAndMeshData.normalIndiciesCount * sizeof(unsigned int), chunkPositionAndMeshData.normalIndicies, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);


					// transparent
					glBindBuffer(GL_ARRAY_BUFFER, chunk.TransparentBuffer());
					glBufferData(GL_ARRAY_BUFFER, chunkPositionAndMeshData.transparentMeshVertexCount * (sizeof(ChunkVertex)), chunkPositionAndMeshData.transparentMeshData, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.TransparentIndexBuffer());
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkPositionAndMeshData.transparentIndiciesCount * sizeof(unsigned int), chunkPositionAndMeshData.transparentIndicies, GL_STATIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					//uploadToGPUTimer.Stop();
				}

				//Time::Timer deleteVertexDataTimer("delete vertex data");
				delete(m_chunkMeshDataReadyToBeUploaded[m_chunkMeshDataReadyToBeUploaded.size() - 1]);
				m_chunkMeshDataReadyToBeUploaded.erase(m_chunkMeshDataReadyToBeUploaded.begin() + m_chunkMeshDataReadyToBeUploaded.size() - 1);
				//deleteVertexDataTimer.Stop();
			}
			DeleteMarkedThreads();
		}
	}
	void AddChunkMeshToQueue(const Vector3Int& pos,
		ChunkVertex* normalMeshData, size_t normalMeshVertexCount, unsigned int* normalIndicies, size_t normalIndiciesCount,
		ChunkVertex* transparentMeshData, size_t transparentMeshVertexCount, unsigned int* transparentIndicies, size_t transparentIndiciesCount) const
	{
		std::lock_guard<std::mutex> guard(m_chunkMeshQueueMutex);
		m_chunkMeshDataReadyToBeUploaded.push_back(new ChunkPositionAndMeshData(pos, normalMeshData, normalMeshVertexCount, normalIndicies, normalIndiciesCount,
			transparentMeshData, transparentMeshVertexCount, transparentIndicies, transparentIndiciesCount));
	}
private:
	struct ThreadWithId
	{
		public:
			int64_t id;
			std::thread t;
			ThreadWithId(int64_t id, void (*methodPointer)(int64_t, int64_t), int64_t methodParameter) : id(id), t(methodPointer, methodParameter, id) {}
	};
	mutable int m_currentChunkGenerationThreads = 0;
	void LoadChunkMeshAsync(const Vector3Int& position) const;
	mutable std::mutex m_chunkMeshQueueMutex;
	mutable std::vector<ChunkPositionAndMeshData*> m_chunkMeshDataReadyToBeUploaded;
	mutable std::vector<ThreadWithId> m_chunkGenerationThreads;
	mutable std::mutex m_chunkGenerationThreadsMarkedForDeletionMutex;
	mutable std::vector<int64_t> m_chunkGenerationThreadsMarkedForDeletion;
	mutable int64_t _nextThreadId = 0;

	static void CreateChunkMesh(int64_t key, int64_t id)
	{
		//Time::Timer createMeshTimer("create mesh");
		ActiveWorld()->m_Chunks[key].CreateMesh();
		//createMeshTimer.Stop();
		//Time::Timer markForDeletionTimer("mark for deletion");
		ActiveWorld()->MarkThreadForDeletion(id);
		//markForDeletionTimer.Stop();
	}
	void MarkThreadForDeletion(int id) const
	{
		std::lock_guard<std::mutex> guard(m_chunkGenerationThreadsMarkedForDeletionMutex);
		m_chunkGenerationThreadsMarkedForDeletion.push_back(id);
	}
	void DeleteMarkedThreads() const
	{
		std::lock_guard<std::mutex> guard(m_chunkGenerationThreadsMarkedForDeletionMutex);
		for (int i = 0; i < m_chunkGenerationThreadsMarkedForDeletion.size(); i++)
		{
			for (int j = 0; j < m_chunkGenerationThreads.size(); j++)
			{
				if (m_chunkGenerationThreadsMarkedForDeletion[i] == m_chunkGenerationThreads[j].id)
				{
					m_chunkGenerationThreads[j].t.join();
					m_chunkGenerationThreads.erase(m_chunkGenerationThreads.begin() + j);
					m_currentChunkGenerationThreads--;
				}
			}
		}
		while (m_meshGenerationQueue.size() != 0)
		{
			if (DispatchMeshGenerationThread(m_meshGenerationQueue.front()))
			{
				m_meshGenerationQueue.pop();
			}
			else break;
		}
	}
	mutable std::queue<int64_t> m_meshGenerationQueue;
	void QueueChunkForMeshGeneration(int64_t key) const
	{
		if (!DispatchMeshGenerationThread(key))
			m_meshGenerationQueue.push(key);
	}
	bool DispatchMeshGenerationThread(int64_t key) const
	{
		if (m_currentChunkGenerationThreads < MaxChunkGenerationThreads)
		{
			m_currentChunkGenerationThreads++;
			int64_t id = ActiveWorld()->_nextThreadId++;
			m_chunkGenerationThreads.emplace_back(id, World::CreateChunkMesh, key);
			return true;
		}
		return false;
	}

public:
	~World() // just gonna join all the threads, before closing the program
	{
		for (int i = 0; i < m_chunkGenerationThreads.size(); i++)
		{
			m_chunkGenerationThreads[i].t.join();
		}
	}
};
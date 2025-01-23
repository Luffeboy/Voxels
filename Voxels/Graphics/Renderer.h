#pragma once
#include "../IncludeOpenGL.h"
#include "Shader.h"
#include "../Camera.h"
#include "Texture.h"
#include "../Game/World.h"

struct ChunkData;

class Renderer
{
private:
	Camera* m_camera = nullptr;
	Shader ChunkShader;
	Shader TextShader;
	std::vector<Texture*> m_textures;
	unsigned int ChunkBuffer;
	unsigned int ChunkIndexBuffer;
	unsigned int TextBuffer;
public:

	Renderer();
	~Renderer();
	void LoadShaders();
	inline void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
	inline void SetCamera(Camera* camera) 
	{
		m_camera = camera; 
		m_camera->RecalculateProjectionMatrix(1920.0f, 1080.0f);
	}
	inline void PreRender()
	{
		m_camera->RecalculateViewMatrix();
		ChunkShader.Bind();
		ChunkShader.SetUniformMat4("viewMatrix", m_camera->ViewMatrix());
		ChunkShader.Unbind();
	}
	void DrawTriangle();
	void DrawChunk(const ChunkData& chunk);
	void DrawWorld(const World& chunk);
	void DrawText(std::string& text, Vector2 topLeft, Color col = {1, 1, 1, 1}, float textSize = .01, float spacing = .01f);
};
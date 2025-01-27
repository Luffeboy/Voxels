#include "Renderer.h"
//#include "../Vector3.h"
#include "Vertex.h"
#include "Shader.h"
#include <iostream>
#include "../ChunkData.h"
#include "../Camera.h"
#include "TextureReader.h"

Renderer::Renderer()
{
}
Renderer::~Renderer()
{
	glDeleteBuffers(1, &ChunkBuffer);
	glDeleteBuffers(1, &TextBuffer);
}
void Renderer::LoadShaders()
{
	glGenBuffers(1, &ChunkBuffer);
	glGenBuffers(1, &ChunkIndexBuffer);
	// text related stuff
	TextShader.Load("res/shaders/Text.shader");
	TextShader.Bind();
	TextShader.SetUniform1i("u_Texture", 0);
	TextShader.Unbind();
	glGenBuffers(1, &TextBuffer);

	// load textures
	m_textures.push_back(TextureReader::ReadImageFromFileHeap("res/textures/TextTextures.data"));
	m_textures.push_back(TextureReader::ReadImageFromFilePNGHeap("res/textures/BlockTextures.png"));

	// check textures
	// ...
	// 
	// load chunk Shader
	ChunkShader.Load("res/shaders/Chunk.shader");
	ChunkShader.Bind();
	ChunkShader.SetUniformMat4("projectionMatrix", m_camera->ProjectionMatrix());
	ChunkShader.SetUniform1f("texWidth", m_textures[1]->GetWidth());
	ChunkShader.SetUniform1f("texHeight", m_textures[1]->GetHeight());
	ChunkShader.Unbind();

}
//
//void Renderer::DrawTriangle()
//{
//	// shader
//	Shader shader = Shader("res/shaders/Basic.shader");
//	shader.Bind();
//	const int vertexCount = 3;
//	Vertex data[vertexCount] = {
//		{{-.5, -.5, 0}, { 0, 0}},
//		{{ 0,   .5, 0}, {.5, 1}},
//		{{ .5, -.5, 0}, { 1, 0}}
//	};
//	unsigned int buffer;
//	glGenBuffers(1, &buffer);
//	glBindBuffer(GL_ARRAY_BUFFER, buffer);
//	glBufferData(GL_ARRAY_BUFFER, vertexCount * (sizeof(Vertex)), data, GL_STATIC_DRAW);
//
//	unsigned int stride = sizeof(Vertex);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));
//
//	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//
//	glDeleteBuffers(1, &buffer);
//	shader.Unbind();
//}
void Renderer::DrawTriangle()
{
	// shader
	TextShader.Bind();
	TextShader.SetUniform4f("u_Color", 1, 1, 1, 1);
	m_textures[1]->Bind();
	const int vertexCount = 4;
	const int indiciesCount = 6;
	//Vertex data[vertexCount] = {
	//	{{-.5, -.5, 0}, { 0, 0}},
	//	{{ 0,   .5, 0}, {.5, 1}},
	//	{{ .5, -.5, 0}, { 1, 0}}
	//	/*{{-.5, -.5, 0}, { 0, 0}, ChunkVertex::GetColor(0, 0, 255)},
	//	{{ 0,   .5, 0}, {.5, 1}, ChunkVertex::GetColor(0, 0, 255)},
	//	{{ .5, -.5, 0}, { 1, 0}, ChunkVertex::GetColor(0, 0, 255)}*/
	//};
	// square

	Vertex data[vertexCount] = {
		{{ -1, -1, 0 }, { 0, 1 }}, // 0 1
		{{  1, -1, 0 }, { 1, 1 }}, // 1 1
		{{ -1,  1, 0 }, { 0, 0 }}, // 0 0
		{{  1,  1, 0 }, { 1, 0 }}  // 1 0
	};
	unsigned int indicies[indiciesCount] = {
		0,	1,	2,
		1,	3,	2
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);


	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), data, GL_STATIC_DRAW);

	unsigned int stride = sizeof(Vertex);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));

	glDrawElements(GL_TRIANGLES, indiciesCount, GL_UNSIGNED_INT, (const void*)&indicies[0]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_textures[1]->Unbind();
	TextShader.Unbind();

	glDeleteBuffers(1, &buffer);
}
void Renderer::DrawChunk(const ChunkData& chunk)
{
	ChunkShader.Bind();
	ChunkShader.SetUniform3f("positionOffset", chunk.OffsetForRendering());
	m_textures[1]->Bind();

	unsigned int stride = sizeof(ChunkVertex);
	// opauge

	if (chunk.VertexCount() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, chunk.Buffer());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.IndexBuffer());
		chunk.SetAttribArray();
		//glDrawArrays(GL_TRIANGLES, 0, (unsigned int)chunk.VertexCount()); // old, outdated, one might even say depricated.
		glDrawElements(GL_TRIANGLES, (unsigned int)chunk.IndeciesCount(), GL_UNSIGNED_INT, nullptr);
		chunk.UnsetAttribArray();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}



	// transparent
	if (chunk.TransparentVertexCount() != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, chunk.TransparentBuffer());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.TransparentIndexBuffer());
		chunk.SetAttribArray();
		//glDrawArrays(GL_TRIANGLES, 0, (unsigned int)chunk.VertexCount()); // old, outdated, one might even say depricated.
		glDrawElements(GL_TRIANGLES, (unsigned int)chunk.TransparentIndeciesCount(), GL_UNSIGNED_INT, nullptr);
		chunk.UnsetAttribArray();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	m_textures[1]->Unbind();
	ChunkShader.Unbind();
}


void Renderer::DrawWorld(const World& world)
{
	glBindBuffer(GL_ARRAY_BUFFER, ChunkBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ChunkIndexBuffer);
	auto& chunks = world.Chunks();
	//ChunkData::SetAttribArray();
	for (auto& chunk : chunks)
	{
		DrawChunk(chunk.second);
	}
	//ChunkData::UnsetAttribArray();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::DrawText(std::string& text, Vector2 topLeft, Color col, float textSize, float spacing)
{
	TextShader.Bind();
	TextShader.SetUniform4f("u_Color", col.r, col.g, col.b, col.a);
	m_textures[0]->Bind();

	std::vector<Vertex> verticies;
	verticies.reserve(text.length() * 4);
	std::vector<unsigned int> indicies;
	indicies.reserve(text.length() * 6);
	for (int i = 0; i < text.length(); i++)
	{
		float uvX = 0.0f;
		float uvXEnd = 0.0f;
		float uvY = .0f;
		float uvYEnd = 0.0f;
		if (text[i] >= 'a' && text[i] <= 'z')
		{
			uvX = (text[i] - 'a') / 26.0f; // 26 os the amount of charecters in the aplhabet... surprising, I know
			uvXEnd = uvX + 1.0f / 26.0f;
			uvYEnd = .5f;
		}
		else if (text[i] >= 'A' && text[i] <= 'Z')
		{
			uvX = (text[i] - 'A') / 26.0f;
			uvXEnd = uvX + 1.0f / 26.0f;
			uvYEnd = .5f;
		}
		else if (text[i] >= '0' && text[i] <= '9')
		{
			uvX = (text[i] - '0') / 26.0f;
			uvXEnd = uvX + 1.0f / 26.0f;
			uvY = .5f;
			uvYEnd = 1.0f;
		}

		int vertexCount = verticies.size();
		float xStart = topLeft.x + i * (textSize + spacing);
		verticies.emplace_back(Vertex{ { xStart           , topLeft.y           , 0 }, { uvX   , uvYEnd } }); // 0 1
		verticies.emplace_back(Vertex{ { xStart + textSize, topLeft.y           , 0 }, { uvXEnd, uvYEnd } }); // 1 1
		verticies.emplace_back(Vertex{ { xStart           , topLeft.y + textSize, 0 }, { uvX   , uvY    } }); // 0 0
		verticies.emplace_back(Vertex{ { xStart + textSize, topLeft.y + textSize, 0 }, { uvXEnd, uvY    } }); // 1 0
		//verticies.emplace_back(Vertex{ { xStart           , topLeft.y           , 0 }, { 0   , 1 } }); // 0 1
		//verticies.emplace_back(Vertex{ { xStart + textSize, topLeft.y           , 0 }, { 1, 1 } }); // 1 1
		//verticies.emplace_back(Vertex{ { xStart           , topLeft.y + textSize, 0 }, { 0   , 0    } }); // 0 0
		//verticies.emplace_back(Vertex{ { xStart + textSize, topLeft.y + textSize, 0 }, { 1, 0    } }); // 1 0

		indicies.push_back(vertexCount + 0);
		indicies.push_back(vertexCount + 1);
		indicies.push_back(vertexCount + 2);

		indicies.push_back(vertexCount + 1);
		indicies.push_back(vertexCount + 3);
		indicies.push_back(vertexCount + 2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, TextBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), (const void*) & verticies[0], GL_STATIC_DRAW);

	unsigned int stride = sizeof(Vertex);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(sizeof(Vector3)));

	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, (const void*)&indicies[0]);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	m_textures[0]->Unbind();
	TextShader.Unbind();
}



//void Renderer::DrawChunk(const ChunkData& chunk)
//{
//	ChunkShader.Bind();
//	ChunkShader.SetUniform3f("positionOffset", chunk.OffsetForRendering());
//	m_textures[1]->Bind();
//
//	chunk.UploadSavedNormalMeshToGPU();
//	glDrawElements(GL_TRIANGLES, (unsigned int)chunk.IndeciesCount(), GL_UNSIGNED_INT, nullptr);
//
//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(2);
//	glDisableVertexAttribArray(3);
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	chunk.UploadSavedTransparentMeshToGPU();
//	glDrawElements(GL_TRIANGLES, (unsigned int)chunk.TransparentIndeciesCount(), GL_UNSIGNED_INT, nullptr);
//
//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glDisableVertexAttribArray(2);
//	glDisableVertexAttribArray(3);
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	m_textures[1]->Unbind();
//	ChunkShader.Unbind();
//}

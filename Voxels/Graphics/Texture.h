#pragma once
#include <string>

struct Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height;
	bool m_heapAllocatedData;
public:
	Texture(const std::string& filePath, unsigned char* localBuffer, int width, int height, bool heapAllocatedData = false);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight()const { return m_Height; }
};
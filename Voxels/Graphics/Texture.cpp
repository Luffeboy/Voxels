#include "Texture.h"
#include "../IncludeOpenGL.h"
#include <iostream>
//#include <windows.h> // for checking it gets the correct data

Texture::Texture(const std::string& filePath, unsigned char* localBuffer, int width, int height, bool heapAllocatedData)
	:m_RendererID(0), m_FilePath(filePath), m_LocalBuffer(localBuffer), m_Width(width), m_Height(height), m_heapAllocatedData(heapAllocatedData)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);


	/*for (int y = 0; y < m_Height; y++)
		for (int x = 0; x < m_Width; x++)
			std::cout << int(m_LocalBuffer[(x + width * y) * 4]);
	std::cout << std::endl;*/
	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 8 * 4; x++)
		{
			int num = int(localBuffer[x * 4 + y * (width * 4)]);
			if (num == 0)
			{
				SetConsoleTextAttribute(hConsole, 8);
				std::cout << "000" << " ";
			}
			else
			{
				SetConsoleTextAttribute(hConsole, 6);
				std::cout << num << " ";
			}
		}
		std::cout << std::endl;
	}*/
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
	if (m_heapAllocatedData)
		delete[] m_LocalBuffer;
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

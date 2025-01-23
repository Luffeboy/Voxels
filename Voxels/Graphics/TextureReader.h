#pragma once
#include "../IncludeOpenGL.h"
#include "Texture.h"

class TextureReader
{
public:
	static Texture ReadImageFromFile(const std::string& filePath);
	static Texture* ReadImageFromFileHeap(const std::string& filePath);
	static Texture* ReadImageFromFileLPFHeap(const std::string& filePath);
	static Texture* ReadImageFromFilePNGHeap(const std::string& filePath);
};
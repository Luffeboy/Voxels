//#define _CRT_SECURE_NO_DEPRECATE
#include "TextureReader.h"
#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "../External/stb_image.h"

Texture TextureReader::ReadImageFromFile(const std::string& filePath)
{
	// read file
	std::fstream stream(filePath);
	stream.seekg(0);
	// get width and height of the image do note the
	// first line should have the width of the image, the second should have the height
	int width;
	//std::cout << sizeof(int);
	stream.read((char*)&width, sizeof(int));
	int height;
	stream.read((char*)&height, sizeof(int));
	// read the bytes from the file and into the buffer
	int amountOfBytes = width * height * sizeof(char) / 8;

	//std::cout << width << ":" << height << ", bytes: " << amountOfBytes << std::endl;
	unsigned char* data = (unsigned char*)alloca(amountOfBytes);
	stream.read((char*)data, amountOfBytes);
	stream.close();
	// expand the data, to include colors and such

	unsigned char* pixelData = (unsigned char*)alloca(width * height * sizeof(char) * 4);
	for (int i = 0; i < amountOfBytes; i++)
	{
		for (unsigned char j = 0; j < 8; j++)
		{
			int col = (data[i] >> (7 - j)) & 1;
			unsigned char colorAmount = (col == 0) ? 0 : 255;
			pixelData[i * 8 * 4 + j * 4 + 0] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 1] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 2] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 3] = colorAmount;
		}
	}

	return { filePath, pixelData, width, height };
}
Texture* TextureReader::ReadImageFromFileHeap(const std::string& filePath)
{
	// read file
	std::fstream stream(filePath);
	stream.seekg(0);
	// get width and height of the image do note the
	// first line should have the width of the image, the second should have the height
	int width;
	//std::cout << sizeof(int);
	stream.read((char*)&width, sizeof(int));
	int height;
	stream.read((char*)&height, sizeof(int));
	// read the bytes from the file and into the buffer
	int amountOfBytes = width * height * sizeof(char) / 8;

	//std::cout << width << ":" << height << ", bytes: " << amountOfBytes << std::endl;
	unsigned char* data = (unsigned char*)alloca(amountOfBytes);
	stream.read((char*)data, amountOfBytes);
	stream.close();
	// expand the data, to include colors and such

	unsigned char* pixelData = (unsigned char*)alloca(width * height * sizeof(char) * 4);
	for (int i = 0; i < amountOfBytes; i++)
	{
		for (unsigned char j = 0; j < 8; j++)
		{
			int col = (data[i] >> (7 - j)) & 1;
			unsigned char colorAmount = (col == 0) ? 0 : 255;
			pixelData[i * 8 * 4 + j * 4 + 0] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 1] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 2] = colorAmount;
			pixelData[i * 8 * 4 + j * 4 + 3] = colorAmount;
		}
	}

	return new Texture(filePath, pixelData, width, height);
}
Texture* TextureReader::ReadImageFromFileLPFHeap(const std::string& filePath)
{
	// read file
	std::fstream stream(filePath);
	if (!stream.good())
	{
		std::cout << "couldn't open file: " << filePath << std::endl;
		return nullptr;
	}
	stream.seekg(0);
	// get width and height of the image do note the
	// first line should have the width of the image, the second should have the height
	int width;
	stream.read((char*)&width, sizeof(int));
	int height;
	stream.read((char*)&height, sizeof(int));
	// read the amount of different colors in the image
	unsigned char colorCount;
	stream.read((char*)&colorCount, sizeof(unsigned char));

	unsigned char junk; // idk why these two lines are nessesary :(
	stream.read((char*)&junk, sizeof(unsigned char)); //         :(

	//colorCount *= 4; // change from "count" to "size"
	unsigned char* colors = (unsigned char*)alloca(colorCount * 4);
	stream.read((char*)colors, colorCount * 4);
	//for (int i = 0; i < colorCount; i++) std::cout << "Color " << i << ": " << colors[i] << std::endl;
	//for (int i = 0; i < colorCount; i++) // for debugging ---------------------------------------------------------------------------------------
	//{
	//	std::cout << int(colors[i * 4]) << " : " << (int)colors[i * 4 + 1] << " : " << (int)colors[i * 4 + 2] << " : " << (int)colors[i * 4 + 3] << std::endl;
	//}
	// read the indicies of each color
	int amountOfBytes = width * height * sizeof(char);
	
	// read the bytes from the file and into the buffer
	//stream.seekg(sizeof(int) * 2 + sizeof(unsigned char) + colorCount * sizeof(unsigned char) + 1);
	unsigned char* indicies = (unsigned char*)alloca(amountOfBytes); // malloc(amountOfBytes);?
	stream.read((char*)indicies, amountOfBytes);
	stream.close();

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
			std::cout << int(indicies[(x + width * y)]) << " ";
		std::cout << std::endl;
	}

	//std::cout << "bytes: " << amountOfBytes << std::endl;
	//for (int i = 0; i < amountOfBytes; i++) std::cout << "Color " << i << ": " << '0' + indicies[i] << std::endl;
	// convert from the compressed version to a non-compressed version
	unsigned char* data = new unsigned char[amountOfBytes * 4];
	for (int i = 0; i < amountOfBytes; i++)
	{
		int j = i * 4;
		//int colIndex0 = indicies[i] * 4 + 0;
		//int colIndex1 = indicies[i] * 4 + 1;
		//int colIndex2 = indicies[i] * 4 + 2;
		//int colIndex3 = indicies[i] * 4 + 3;

		//int col0 = colors[indicies[i] * 4 + 0];
		//int col1 = colors[indicies[i] * 4 + 1];
		//int col2 = colors[indicies[i] * 4 + 2];
		//int col3 = colors[indicies[i] * 4 + 3];
		data[j + 0] = colors[indicies[i] * 4 + 0];
		data[j + 1] = colors[indicies[i] * 4 + 1];
		data[j + 2] = colors[indicies[i] * 4 + 2];
		data[j + 3] = colors[indicies[i] * 4 + 3];
	}
	//for (int i = 0; i < amountOfBytes * 4; i++) std::cout << "Color " << i << ": " << (int)data[i] << std::endl;


	return new Texture(filePath, data, width, height);
}


Texture* TextureReader::ReadImageFromFilePNGHeap(const std::string& filePath)
{
	int width, height, channels;
	unsigned char* data = (unsigned char*)stbi_load(filePath.c_str(), &width, &height, &channels, 4);
	if (!data)
		return nullptr;
	return new Texture(filePath, data, width, height, true);

}
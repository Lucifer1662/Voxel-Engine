
#include "Buffer.h"
#include <glm/glm.hpp>
#include <bitset>

struct TextureRegion {
	glm::vec2 bottomLeft;
	glm::vec2 topRight;

};

//front 0
//right 1
//back 2
//left 3
//top 4
//bottom 5

constexpr auto FRONT = 1;
constexpr auto RIGHT = 2;
constexpr auto BACK = 4;
constexpr auto LEFT = 8;
constexpr auto TOP = 16;
constexpr auto BOTTOM = 32;


struct Block {
	TextureRegion uvFaces[6];
	void setTextureSidesRegion(std::bitset<6> faces, const glm::vec2& bottomLeft, const glm::vec2& topRight);
};

struct Blocks {
	Buffer<GL_SHADER_STORAGE_BUFFER> blocksGPU;
	void BlockTypesLoad();
	void BlockTypesUnload();
};





class Chunk {
public:
	using BlockType = int;

	Buffer<GL_ARRAY_BUFFER> blockSelfData;
	Buffer<GL_SHADER_STORAGE_BUFFER> blockGlobablData;
	glm::vec3 position;

	Chunk(int seed, glm::vec3 position);
	void Render();

	static const int width = 10;
	static const int height = 1;
	static int Volume() { return Chunk::width * Chunk::width * Chunk::height; }
	
};


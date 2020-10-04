#include "Chunk.h"
#include <vector>
#include <bitset>

const int Chunk::width;
const int Chunk::height;


void Blocks::BlockTypesLoad() {
	Block d = { {{{0,0},{0,1}}} };

	std::vector<Block> blocks = std::vector<Block>(3);
	blocks[0].setTextureSidesRegion(FRONT | LEFT | RIGHT | BACK | TOP | BOTTOM, { 0,0 }, { 1,1 });
	blocks[1].setTextureSidesRegion(FRONT | LEFT | RIGHT | BACK | TOP | BOTTOM, { 0,0 }, { 0.5f,0.5f });
	blocks[2].setTextureSidesRegion(FRONT | LEFT | RIGHT | BACK | TOP | BOTTOM, { 0.5f,0.5f }, { 1,1 });
	blocksGPU.SetData(blocks.data(), sizeof(Block) * blocks.size());
	blocksGPU.BindBufferBase(1);
}

void Blocks::BlockTypesUnload() {
	
}

Chunk::Chunk(int seed, glm::vec3 position) {
	this->position = position;

	std::vector<glm::vec3> positions;
	positions.reserve(Chunk::Volume());

	std::vector<BlockType> blockTypes;
	blockTypes.reserve(Chunk::Volume());

	

	for (size_t y = 0; y < Chunk::height; y++)
	{
		for (size_t x = 0; x < Chunk::width; x++)
		{
			for (size_t z = 0; z < Chunk::width; z++)
			{
				positions.emplace_back(x+position.x,y+position.y,z+position.z);
				blockTypes.emplace_back(rand()%3);
				//blockTypes.emplace_back(x % 3);
			}
		}
	}

	blockSelfData.SetData(positions.data(), sizeof(glm::vec3)*positions.size());


	blockGlobablData.SetData(blockTypes.data(), sizeof(BlockType)*blockTypes.size());
	blockGlobablData.BindBufferBase(3);
}

void Chunk::Render()
{
	blockSelfData.Bind();
	blockGlobablData.Bind();
	glDrawArrays(GL_POINTS, 0, Chunk::Volume());

}

void Block::setTextureSidesRegion(std::bitset<6> faces, const glm::vec2& bottomLeft, const glm::vec2& topRight)
{
	for (int i = 0; i < 6; i++) {
		if(faces.test(i))
			uvFaces[i] = { bottomLeft, topRight };
	}
}

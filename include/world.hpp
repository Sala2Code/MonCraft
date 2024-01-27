#ifndef WORLD_HPP
#define WOLRD_HPP

bool isBlockInView(Frustum &frustum, glm::vec3 &vecObj);

void setupWorldMap(
		Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		std::array<int, chunkLoadSize> &listIndex
    );

int getTexture(std::string name);
int getTextureBlock(int biome, int distSurf, glm::vec3 pos);
int setup_grass(std::string name, int distSurf, glm::vec2 seedRandom);


#endif
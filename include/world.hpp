#ifndef WORLD_HPP
#define WOLRD_HPP

#include "struct.hpp"

void getCoordChunk(int nbChunk, int &chunkCoX, int &chunkCoY);
int getNumChunk(int X, int Y, int divide);
int getIndexChunk(int chunk, int &actualChunk, std::array<int, chunkLoadSize> &listIndex);

void updateChunkUpDown(
		int shiftChunk,
        std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<BlockMap, chunkArea> &tempHeightMap,
        std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		int coordActX
    );
// void updateChunkLeftRight(
// 		int shiftIndex,
// 		int shiftChunk,
// 		std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
//         std::array<BlockMap, chunkArea> &tempHeightMap,
// 		std::array<int, chunkLoadSize> &chunkLoad,
// 		std::array<int, chunkLoadSize> &tempLoad,
// 		std::array<BlockMap, chunkArea> &tempHeight,
// 		std::array<int, 2> &borderChunkMin
// 	);

void loadNewChunk(
        Player &player,
        std::array<int, chunkLoadSize> &chunkLoad,
        std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<BlockMap, chunkArea> &tempHeightMap,
        std::array<std::array<BlockMap, chunkArea>, 2*chunkView+1> &tempHeight,
        std::array<int, 2*chunkView+1> &tempLoad,
        glm::vec3 &playerBeforePos,
        std::array<int, 2> &borderChunkMin
    );

void displayMap(
		Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<BlockMap, chunkArea> &tempHeightMap,
		std::array<std::array<BlockMap, chunkArea>, 2*chunkView+1> &tempHeight,
		std::array<int, 2*chunkView+1> &tempLoad,
		std::array<int, 2> &borderChunkMin,
		BlockShader &blockShader,
        std::array<int, chunkLoadSize> listIndex,
        std::vector<BlockInfo> &worldMap
    );


bool isBlockInView(float playerX, float playerZ, float chunkX, float chunkZ, float jx, float jz, float playerAngleView, float tolView, bool thirdView);

#endif
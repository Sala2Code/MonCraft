#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "struct.hpp"
#include "player.hpp"


void getCoordChunk(int nbChunk, int &chunkCoX, int &chunkCoY);
int getNumChunk(int chunkCoX, int chunkCoY, int divide);
int getIndexChunk(int chunk, int actualChunk, std::array<int, chunkLoadSize> &listIndex);

void updateChunkUpDown(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		Player player
	);

void updateChunkUpDown_Tree(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	);

void updateChunkUpDown_Fol(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	);




void updateChunkLeftRight(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		Player player
	);

void updateChunkLeftRight_Tree(
		int shiftChunk,
		std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	);

void updateChunkLeftRight_Fol(
		int shiftChunk,
		std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	);

bool loadNewChunk(
        Player &player,
        std::array<int, chunkLoadSize> &chunkLoad,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
        std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,

        glm::vec3 &playerBeforePos,
        std::array<int, 2> &borderChunkMin
    );

#endif
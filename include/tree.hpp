#ifndef TREE_HPP
#define TREE_HPP

int getIndexTreeWood(Tree tree, int y, int z, int x);
int getIndexTreeLeaves(Tree tree, int y, int z, int x);

std::unique_ptr<Tree> generateTree(glm::vec3 pos);
float rateTree(int biome);
void setupTreeMap(Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		std::array<int, chunkLoadSize> &listIndex
);

#endif
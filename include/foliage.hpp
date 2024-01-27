#ifndef FOLIAGE_HPP
#define FOLIAGE_HPP

float rateFol(int biome);
void setupFolMap(		
    Player &player,
    std::array<int, chunkLoadSize> &chunkLoad,
    std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
    std::array<int, 2> &borderChunkMin,
    std::array<int, chunkLoadSize> &listIndex
);

#endif
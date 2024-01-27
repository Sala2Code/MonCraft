#ifndef INITCHUNK_HPP
#define INITCHUNK_HPP

void initChunkLoad(std::array<int, chunkLoadSize> &chunkLoad);
void initChunkHeightMap(
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
    std::array<int, chunkLoadSize> &listIndex
);
void initChunkTreeMap(
    std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
    std::array<int, chunkLoadSize> &listIndex,
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap
);

void initChunkFolMap(
    std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
    std::array<int, chunkLoadSize> &listIndex,
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap
);

#endif
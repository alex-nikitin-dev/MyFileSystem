#pragma once
#include <cstdint>

class BlockSize
{
	uint64_t _realSize{};
	uint64_t _normalizedSize{};
	//This ensures that a new block size will be divided by 
	//minimum file size without a remainder and it'll be able to include
	//the required size for structures
	static uint64_t GetSuitableBlockSize(uint64_t size, uint64_t minFileSize);
public:
	BlockSize(uint64_t realSize, uint64_t minFileSize);
	void SetRealSize(uint64_t realSize, uint64_t minFileSize);
	uint64_t GetRealSize() const;
	uint64_t GetNormalizedSize() const;
	uint64_t GetAdditionalSize() const;
};

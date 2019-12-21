#pragma once
#include "BlockSize.h"
#include <cstdint>

class SuperBlock
{
public:
	SuperBlock()= default;
	SuperBlock(const char* fileSystemName, uint64_t  minBlockSize, uint64_t  fileSystemSize, uint64_t  minFileSize, int addressSize);
	const char* GetFileSystemName() const;
	uint64_t GetFileSystemSize() const;
	uint64_t GetMinBlockSize() const;
	uint64_t GetMinFileSize() const;
	BlockSize GetSuperBlockSize() const;
	BlockSize GetBmpIndexDescriptorSize() const;
	BlockSize GetIndexDescriptorsBlockSize() const;
	BlockSize GetBmpDataBlockSize() const;
	uint64_t GetDataBlockSize() const;

	void WriteToFile(const char* fileName) const;
	static SuperBlock ReadFromFile(const char* filename);
	uint64_t GetStartBMPIndexDescriptors() const;
	uint64_t GetStartIndexDescriptorsBlock() const;
	uint64_t GetStartBMPDataBlocks() const;
	uint64_t GetStartDataBlocks() const;
	int GetAddressSize() const;
	uint64_t GetCountOfAddressesInOneBlock() const;
private:
	
	char _fileSystemName[50]{};
	uint64_t _fileSystemSize{};
	uint64_t _minBlockSize{};
	uint64_t _minFileSize{};
	int _addressSize{};

	BlockSize _superBlockSize{0,0};
	BlockSize _bmpIndexDescriptorSize{0,0};
	BlockSize _IndexDescriptorsBlockSize{0,0};
	BlockSize _bmpDataBlockSize{0,0};
	uint64_t _dataBlockSize{};
};

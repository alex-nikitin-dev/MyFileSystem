#include "superblock.h"
#include <fstream>
#include "indexdescriptor.h"
SuperBlock::SuperBlock(const char* fileSystemName, uint64_t  minBlockSize, uint64_t  fileSystemSize, uint64_t  minFileSize, int addressSize)
	:SuperBlock()
{
	strcpy_s(_fileSystemName, fileSystemName);
	_fileSystemSize = fileSystemSize;
	_minBlockSize = minBlockSize;
	_minFileSize = minFileSize;
	_addressSize = addressSize;
	_superBlockSize.SetRealSize(sizeof(SuperBlock), _minFileSize);
	_bmpIndexDescriptorSize.SetRealSize(_fileSystemSize / _minFileSize, _minFileSize);
	_IndexDescriptorsBlockSize.SetRealSize(sizeof(IndexDescriptor) * _bmpIndexDescriptorSize.GetRealSize(), _minFileSize);
	auto sumSizesOfBlockUpper = _superBlockSize.GetNormalizedSize();
	sumSizesOfBlockUpper += _bmpIndexDescriptorSize.GetNormalizedSize();
	sumSizesOfBlockUpper += _IndexDescriptorsBlockSize.GetNormalizedSize();
	const auto sizeOfBmpDataAndDataBlock = _fileSystemSize - sumSizesOfBlockUpper;
	const auto sizeOfBmpDataBlock = sizeOfBmpDataAndDataBlock / _minBlockSize - 4 * std::ceil(
		sizeOfBmpDataAndDataBlock / static_cast<double>(_minFileSize * _minBlockSize));
	_bmpDataBlockSize.SetRealSize(sizeOfBmpDataBlock, _minFileSize);
	sumSizesOfBlockUpper += _bmpDataBlockSize.GetNormalizedSize();
	_dataBlockSize = _fileSystemSize - sumSizesOfBlockUpper;
}
void SuperBlock::WriteToFile(const char * fileName) const
{
	std::ofstream fs(fileName, std::ios::binary | std::ios::trunc);
	if (fs.is_open())
	{
		fs.write(reinterpret_cast<const char*>(this), sizeof(SuperBlock));
		fs.close();
	}
	else
	{
		throw std::logic_error("cannot write the file");
	}
}

SuperBlock SuperBlock::ReadFromFile(const char * filename)
{
	std::ifstream fs(filename, std::ios::binary);
	SuperBlock superBlock;
	if (fs.is_open())
	{
		fs.read(reinterpret_cast<char*>(&superBlock), sizeof(SuperBlock));
		fs.close();
	}
	else
	{
		throw std::logic_error("can not open the file");
	}
	return superBlock;
}

uint64_t SuperBlock::GetStartBMPIndexDescriptors() const
{
	return GetSuperBlockSize().GetNormalizedSize();
}
uint64_t SuperBlock::GetStartIndexDescriptorsBlock() const
{
	return GetStartBMPIndexDescriptors() +
		GetBmpIndexDescriptorSize().GetNormalizedSize();
}
uint64_t SuperBlock::GetStartBMPDataBlocks() const
{
	return GetStartIndexDescriptorsBlock() +
		GetIndexDescriptorsBlockSize().GetNormalizedSize();
}
uint64_t SuperBlock::GetStartDataBlocks() const
{
	return GetStartBMPDataBlocks() +
		GetBmpDataBlockSize().GetNormalizedSize();
}
int SuperBlock::GetAddressSize() const
{
	return _addressSize;
}
uint64_t SuperBlock::GetCountOfAddressesInOneBlock() const
{
	return GetMinBlockSize() / GetAddressSize();
}
const char* SuperBlock::GetFileSystemName() const
{
	return _fileSystemName;
}

uint64_t SuperBlock::GetFileSystemSize() const
{
	return _fileSystemSize;
}

uint64_t SuperBlock::GetMinBlockSize() const
{
	return _minBlockSize;
}

uint64_t SuperBlock::GetMinFileSize() const
{
	return _minFileSize;
}

BlockSize SuperBlock::GetSuperBlockSize() const
{
	return _superBlockSize;
}

BlockSize SuperBlock::GetBmpIndexDescriptorSize() const
{
	return _bmpIndexDescriptorSize;
}

BlockSize SuperBlock::GetIndexDescriptorsBlockSize() const
{
	return _IndexDescriptorsBlockSize;
}

BlockSize SuperBlock::GetBmpDataBlockSize() const
{
	return _bmpDataBlockSize;
}

uint64_t SuperBlock::GetDataBlockSize() const
{
	return _dataBlockSize;
}


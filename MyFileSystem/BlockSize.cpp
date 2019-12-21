#include "BlockSize.h"
#include <cmath>
#include <stdexcept>

uint64_t BlockSize::GetSuitableBlockSize(uint64_t size, uint64_t minFileSize)
{
	return ceil(size / static_cast<double>(minFileSize)) * minFileSize;
}

BlockSize::BlockSize(uint64_t realSize, uint64_t minFileSize)
{
	SetRealSize(realSize,  minFileSize);
}

void BlockSize::SetRealSize(uint64_t realSize, uint64_t minFileSize)
{
	_realSize = realSize;
	_normalizedSize = GetSuitableBlockSize(realSize, minFileSize);
}

uint64_t BlockSize::GetRealSize() const
{
	return _realSize;
}

uint64_t BlockSize::GetNormalizedSize() const
{
	return _normalizedSize;
}

uint64_t BlockSize::GetAdditionalSize() const
{
	return _normalizedSize - _realSize;
}

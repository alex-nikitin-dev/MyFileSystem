#include "filesystem1.h"
#include "indexdescriptor.h"
MyFileSystem::MyFileSystem(const char* fileName, const char* FileSystemName, uint64_t sizeOfFileSystem, uint64_t sizeOfBlock, int addressSize)
	:_minFileSize(sizeOfBlock * 4), _fileName(fileName)
{
	_superBlock = SuperBlock(FileSystemName, sizeOfBlock, sizeOfFileSystem, _minFileSize, addressSize);
}
MyFileSystem MyFileSystem::Open(const char * fileName)
{
	MyFileSystem myFileSystem;
	myFileSystem._superBlock = SuperBlock::ReadFromFile(fileName);
	myFileSystem._minFileSize = myFileSystem._superBlock.GetMinFileSize();
	myFileSystem._fileName = fileName;
	return myFileSystem;
}
uint64_t  MyFileSystem::GetFreeBitInBMPIndexDescriptors(std::fstream &fs, uint64_t startIndexInTheMap) const
{
	return GetFreeBitInTheMap(fs, startIndexInTheMap, _superBlock.GetStartBMPIndexDescriptors(), _superBlock.GetBmpIndexDescriptorSize().GetRealSize());
}
uint64_t MyFileSystem::GetFreeBitInBMPDataBlocks(std::fstream &fs, uint64_t startIndexInTheMap) const
{
	return GetFreeBitInTheMap(fs, startIndexInTheMap, _superBlock.GetStartBMPDataBlocks(), _superBlock.GetBmpDataBlockSize().GetRealSize());
}

uint64_t  MyFileSystem::GetFreeBitInTheMap(std::fstream &fs, uint64_t startIndexInTheMap, uint64_t startIndexOfByteOfTheMapInTheFile, uint64_t realSizeOfTheMap)
{
	if (fs.is_open())
	{
		fs.seekg(startIndexOfByteOfTheMapInTheFile + startIndexInTheMap);
		char buf;
		for (size_t i = startIndexInTheMap; i < realSizeOfTheMap; i++)
		{
			fs.read(&buf, sizeof(char));
			if (static_cast<int>(buf) == 0)
			{
				return i;
			}
		}
	}
	else
	{
		throw std::logic_error("cannot read file");
	}
	throw std::logic_error("GetFreeBitInTheMap error: cannot find any free bit");
}


uint64_t MyFileSystem::GetFreeBitInTheMap(uint64_t startIndexOfByteOfTheMapInTheFile, uint64_t realSizeOfTheMap) const
{
	std::fstream fs(_fileName, std::ios::binary | std::ios::in);
	return GetFreeBitInTheMap(fs, 0, startIndexOfByteOfTheMapInTheFile, realSizeOfTheMap);
}

IndexDescriptor MyFileSystem::GetIndexDescriptorByIndex(uint64_t index) const
{
	std::ifstream fs(_fileName, std::ios::binary);
	IndexDescriptor result{};
	if (fs.is_open())
	{
		fs.seekg(_superBlock.GetStartIndexDescriptorsBlock() + index * sizeof IndexDescriptor);
		fs.read(reinterpret_cast<char*>(&result), sizeof IndexDescriptor);
	}
	else
	{
		throw std::logic_error("can't read file");
	}
	return result;
}

uint64_t MyFileSystem::GetIndexOfAnyFreeIndexDescriptor() const
{
	return  GetFreeBitInTheMap(_superBlock.GetStartBMPIndexDescriptors(), _superBlock.GetIndexDescriptorsBlockSize().GetRealSize());
}

void MyFileSystem::ReadBytesFromDataBlock(std::fstream& fs, uint64_t indexDataBlock, uint64_t offsetInDataBlock, char* data, uint64_t sizeOfData = 1) const
{
	ReadBytesFrom(fs, _superBlock.GetStartDataBlocks(), indexDataBlock, offsetInDataBlock, data, sizeOfData);
}

void MyFileSystem::ReadBytesFrom(std::fstream& fs, uint64_t offsetFileBlock, uint64_t indexDataBlock, uint64_t offsetInDataBlock, char* data, uint64_t sizeOfData) const
{
	if (fs.is_open())
	{
		fs.seekg(offsetFileBlock + indexDataBlock * _superBlock.GetMinBlockSize() + offsetInDataBlock);
		fs.read(data, sizeOfData);
	}
	else
	{
		throw std::logic_error("can't open the file");
	}
}
void MyFileSystem::SetIndexDataBit(std::fstream &fs, uint64_t indexOfTheBit, bool isTaken) const
{
	SetBitInMap(fs, _superBlock.GetStartBMPDataBlocks(), indexOfTheBit, isTaken);
}

void MyFileSystem::SetIndexDescriptorBit(std::fstream &fs, uint64_t indexOfTheBit, bool isTaken) const
{
	SetBitInMap(fs, _superBlock.GetStartBMPIndexDescriptors(), indexOfTheBit, isTaken);
}

void MyFileSystem::SetBitInMap(std::fstream &fs, uint64_t offsetTheMap, uint64_t indexOfTheBit, bool isTaken)
{
	if (fs.is_open())
	{
		fs.seekg(offsetTheMap + indexOfTheBit);
		char data;
		isTaken ? data = 1 : data = 0;
		fs.write(&data, sizeof(char));
	}
	else
	{
		throw std::logic_error("can't open the file");
	}
}

void MyFileSystem::WriteIndexDescriptorAndSetBit(std::fstream &fs, uint64_t indexOfDescriptor, IndexDescriptor* theDescriptor) const
{
	if (fs.is_open())
	{
		fs.seekg(_superBlock.GetStartIndexDescriptorsBlock() + indexOfDescriptor * sizeof IndexDescriptor);
		fs.write(reinterpret_cast<const char*>(theDescriptor), sizeof IndexDescriptor);
		SetIndexDescriptorBit(fs, indexOfDescriptor, true);
	}
	else
	{
		throw std::logic_error("can't open the file");
	}
}
void MyFileSystem::WriteBytesToDataBlock(std::fstream &fs, uint64_t indexDataBlock, uint64_t offsetInDataBlock, const char* data, uint64_t sizeOfData = 1) const
{
	WriteBytesTo(fs, _superBlock.GetStartDataBlocks(), indexDataBlock, offsetInDataBlock, data, sizeOfData);
}

void MyFileSystem::WriteBytesTo(std::fstream &fs, uint64_t offsetFileBlock, uint64_t indexDataBlock, uint64_t offsetInDataBlock, const char* data, uint64_t sizeOfData) const
{
	if (fs.is_open())
	{
		fs.seekg(offsetFileBlock + indexDataBlock * _superBlock.GetMinBlockSize() + offsetInDataBlock);
		fs.write(data, sizeOfData);
	}
	else
	{
		throw std::logic_error("can't open the file");
	}
}
uint64_t MyFileSystem::GetFreeBitInBMPDataBlocksAndSaveAddress(std::fstream &fs, uint64_t addressBlockIndex, size_t offsetInAddressBlock) const
{
	const auto result = GetFreeBitInBMPDataBlocks(fs, addressBlockIndex + 1);
	const auto addressForWrite = result + 1;
	WriteBytesToDataBlock(fs, addressBlockIndex, offsetInAddressBlock*_superBlock.GetAddressSize(),
		(char*)&addressForWrite, sizeof _superBlock.GetAddressSize());
	return result;
}

void MyFileSystem::AddZerosToAddressInBMPDataBlocks(std::fstream &fs, uint64_t addressBlockIndex, size_t offsetInAddressBlock) const
{
	auto zero = new char[_superBlock.GetAddressSize()]{ 0 };
	for (size_t i = offsetInAddressBlock * _superBlock.GetAddressSize(); i < _superBlock.GetMinBlockSize(); i += _superBlock.GetAddressSize())
	{
		WriteBytesToDataBlock(fs, addressBlockIndex, i, zero, sizeof _superBlock.GetAddressSize());
	}
	delete[]zero;
	zero = nullptr;
}

bool MyFileSystem::WriteFile(const char* fileName, const char* dataToWrite, uint64_t sizeOfDataToWrite) const
{
	ThrowExceptionIfTheFileExists(fileName);
	const auto indexOfFreeIndexDescriptor = GetIndexOfAnyFreeIndexDescriptor();
	auto theDescriptor = GetIndexDescriptorByIndex(indexOfFreeIndexDescriptor);
	theDescriptor.SetFileName(fileName);
	theDescriptor.fileSize = sizeOfDataToWrite;

	const auto countOfAddresses = _superBlock.GetCountOfAddressesInOneBlock();
	uint64_t indexByteInDataToWrite = 0;
	std::fstream fs(_fileName, std::ios::binary | std::ios::out | std::ios::in);
	if (fs.is_open())
	{
		const auto firstLevelAddressBlockIndex = GetFreeBitInBMPDataBlocks(fs, 0);
		SetIndexDataBit(fs, firstLevelAddressBlockIndex, true);

		theDescriptor.blockNumber = firstLevelAddressBlockIndex;
		WriteIndexDescriptorAndSetBit(fs, indexOfFreeIndexDescriptor, &theDescriptor);

		for (size_t i = 0; i < countOfAddresses; i++)
		{//enumerate addresses in the first level address block
			if (indexByteInDataToWrite < sizeOfDataToWrite)
			{
				const auto secondLevelAddressBlockIndex = GetFreeBitInBMPDataBlocksAndSaveAddress(fs, firstLevelAddressBlockIndex, i);
				SetIndexDataBit(fs, secondLevelAddressBlockIndex, true);
				for (size_t j = 0; j < countOfAddresses; j++)
				{//enumerate addresses in the second level address block
					if (indexByteInDataToWrite < sizeOfDataToWrite)
					{
						const auto thirdLevelAddressBlockIndex = GetFreeBitInBMPDataBlocksAndSaveAddress(fs, secondLevelAddressBlockIndex, j);
						SetIndexDataBit(fs, thirdLevelAddressBlockIndex, true);
						for (size_t k = 0; k < countOfAddresses; k++)
						{//enumerate addresses in the third level address block
							if (indexByteInDataToWrite < sizeOfDataToWrite)
							{
								const auto dataBlockIndex = GetFreeBitInBMPDataBlocksAndSaveAddress(fs, thirdLevelAddressBlockIndex, k);
								SetIndexDataBit(fs, dataBlockIndex, true);
								for (size_t dataByteIndex = 0; dataByteIndex < _superBlock.GetMinBlockSize() && indexByteInDataToWrite < sizeOfDataToWrite; dataByteIndex++)
								{
									WriteBytesToDataBlock(fs, dataBlockIndex, dataByteIndex, reinterpret_cast<const char*>(&dataToWrite[indexByteInDataToWrite++]));
								}
							}
							else
							{
								AddZerosToAddressInBMPDataBlocks(fs, thirdLevelAddressBlockIndex, k);
							}
						}
					}
					else
					{
						AddZerosToAddressInBMPDataBlocks(fs, secondLevelAddressBlockIndex, j);
					}
				}
			}
			else
			{
				AddZerosToAddressInBMPDataBlocks(fs, firstLevelAddressBlockIndex, i);
			}
		}
	}
	else
	{
		throw std::logic_error("can't open the file");
	}
	return true;
}

bool MyFileSystem::GetBitStateInTheMap(std::fstream &fs, uint64_t mapOffsetInTheFile, uint64_t indexOfTheBit)
{
	if (fs.is_open())
	{
		fs.seekg(mapOffsetInTheFile + indexOfTheBit);
		char buf;
		fs.read(&buf, 1);
		return static_cast<int>(buf) == 1;
	}
	throw std::logic_error("cannot read file");
}

bool MyFileSystem::IsBitInBMPIndexDescriptorsTaken(std::fstream &fs, uint64_t indexOfTheBit) const
{
	return GetBitStateInTheMap(fs, _superBlock.GetStartBMPIndexDescriptors(), indexOfTheBit);
}
bool  MyFileSystem::IsBitInBMPDataBlockTaken(std::fstream &fs, uint64_t indexOfTheBit) const
{
	return GetBitStateInTheMap(fs, _superBlock.GetStartBMPDataBlocks(), indexOfTheBit);
}

IndexDescriptor MyFileSystem::GetFileDescriptor(const char* fileName) const
{
	uint64_t index;
	if (!GetFileDescriptorIndex(fileName, index)) throw std::logic_error("cannot find the file");
	return GetIndexDescriptorByIndex(index);
}

uint64_t  MyFileSystem::GetFileSize(const char* fileName) const
{
	return GetFileDescriptor(fileName).fileSize;
}

bool MyFileSystem::GetFileDescriptorIndex(const char* fileName, uint64_t& index) const
{
	std::fstream fs(_fileName, std::ios::binary | std::ios::in);
	if (fs.is_open())
	{
		for (uint64_t i = 0; i < _superBlock.GetBmpIndexDescriptorSize().GetRealSize(); i++)
		{
			if (IsBitInBMPIndexDescriptorsTaken(fs, i))
			{
				const auto fileDescriptor = GetIndexDescriptorByIndex(i);
				if (strcmp(fileName, fileDescriptor.fileName) == 0)
				{
					index = i;
					return true;
				}
			}
		}
	}
	else
	{
		throw std::logic_error("cannot read file");
	}

	return false;
}

bool MyFileSystem::IsFileExist(const char* fileName) const
{
	uint64_t index;
	return GetFileDescriptorIndex(fileName, index);
}

void MyFileSystem::ThrowExceptionIfTheFileExists(const char * fileName) const
{
	if (IsFileExist(fileName))
	{
		throw std::logic_error("There is already a file with the very name is this location");
	}
}

void MyFileSystem::ReadFile(const char * fileName, char* buffer, uint64_t sizeOfBuffer) const
{
	uint64_t indexOfDescriptor;
	if (!GetFileDescriptorIndex(fileName, indexOfDescriptor))
	{
		throw std::logic_error("There isn't a file with the very name is this location");
	}

	const auto fileDescriptor = GetIndexDescriptorByIndex(indexOfDescriptor);
	if (fileDescriptor.fileSize != sizeOfBuffer)
	{
		throw std::logic_error("Size of buffer must have the very size as the file");
	}
	const auto countOfAddresses = _superBlock.GetCountOfAddressesInOneBlock();
	const auto firstLevelAddressBlockIndex = fileDescriptor.blockNumber;
	std::fstream fs(_fileName, std::ios::in | std::ios::binary);
	if (!fs.is_open()) throw std::logic_error("cannot read the file");
	uint64_t indexForWrite = 0;
	for (size_t i = 0; i < countOfAddresses; i++)
	{
		uint64_t secondLevelAddressBlockIndex = 0;
		ReadBytesFromDataBlock(fs, firstLevelAddressBlockIndex, i*_superBlock.GetAddressSize(), (char*)(&secondLevelAddressBlockIndex), _superBlock.GetAddressSize());
		secondLevelAddressBlockIndex--;
		for (size_t j = 0; j < countOfAddresses; j++)
		{
			uint64_t thirdLevelAddressBlockIndex = 0;
			ReadBytesFromDataBlock(fs, secondLevelAddressBlockIndex, j*_superBlock.GetAddressSize(), (char*)(&thirdLevelAddressBlockIndex), _superBlock.GetAddressSize());
			thirdLevelAddressBlockIndex--;
			for (size_t k = 0; k < countOfAddresses; k++)
			{
				uint64_t dataBlockIndex = 0;
				ReadBytesFromDataBlock(fs, thirdLevelAddressBlockIndex, k*_superBlock.GetAddressSize(), (char*)(&dataBlockIndex), _superBlock.GetAddressSize());
				dataBlockIndex--;
				for (uint64_t dataByteIndex = 0; dataByteIndex < _superBlock.GetMinBlockSize(); dataByteIndex++)
				{
					char oneDataByte;
					ReadBytesFromDataBlock(fs, dataBlockIndex, dataByteIndex, &oneDataByte);
					buffer[indexForWrite++] = oneDataByte;
					if (indexForWrite >= sizeOfBuffer)
					{
						return;
					}
				}
			}
		}
	}
}

void MyFileSystem::Format() const
{
	_superBlock.WriteToFile(_fileName);
	std::ofstream fs(_fileName, std::ios::binary | std::ios::app);
	if (fs.is_open())
	{
		auto sizeToWrite = _superBlock.GetSuperBlockSize().GetAdditionalSize();
		sizeToWrite += _superBlock.GetBmpIndexDescriptorSize().GetNormalizedSize();
		sizeToWrite += _superBlock.GetIndexDescriptorsBlockSize().GetNormalizedSize();
		sizeToWrite += _superBlock.GetBmpDataBlockSize().GetNormalizedSize();
		sizeToWrite += _superBlock.GetDataBlockSize();
		char zero = 0;
		for (uint64_t i = 0; i < sizeToWrite; i++)
		{
			fs.write(&zero, sizeof(char));
		}
		fs.close();
	}
}

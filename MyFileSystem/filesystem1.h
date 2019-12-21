#pragma once
#include "superblock.h"
#include <fstream>
#include "indexdescriptor.h"

class MyFileSystem
{
	SuperBlock _superBlock;
	uint64_t _minFileSize{};
	const char* _fileName{};
public:
	MyFileSystem() = default;
	MyFileSystem(const char* fileName, const char* FileSystemName, uint64_t sizeOfFileSystem,
		uint64_t sizeOfBlock=1024, int addressSize=4);

	static MyFileSystem Open(const char* fileName);
	void Format() const;
	bool WriteFile(const char* fileName, const char* dataToWrite, uint64_t sizeOfDataToWrite) const;
	void ReadFile(const char * fileName, char* buffer, uint64_t sizeOfBuffer) const;
	uint64_t GetFileSize(const char* fileName) const;
	bool IsFileExist(const char* fileName) const;

private:
	uint64_t GetFreeBitInBMPIndexDescriptors(std::fstream &fs, uint64_t startIndexInTheMap) const;
	uint64_t GetFreeBitInBMPDataBlocks(std::fstream &fs, uint64_t startIndexInTheMap) const;
	static uint64_t GetFreeBitInTheMap(std::fstream &fs, uint64_t startIndexInTheMap, uint64_t startIndexOfByteOfTheMapInTheFile, uint64_t realSizeOfTheMap);
	uint64_t GetFreeBitInTheMap(uint64_t startIndexOfByteOfTheMapInTheFile, uint64_t realSizeOfTheMap) const;
	IndexDescriptor GetIndexDescriptorByIndex(uint64_t index) const;
	uint64_t GetIndexOfAnyFreeIndexDescriptor() const;
	void ReadBytesFromDataBlock(std::fstream& fs, uint64_t indexDataBlock, uint64_t offsetInDataBlock, char* data, uint64_t sizeOfData) const;
	void ReadBytesFrom(std::fstream& fs, uint64_t offsetFileBlock, uint64_t indexDataBlock, uint64_t offsetInDataBlock, char* data, uint64_t sizeOfData) const;
	void SetIndexDataBit(std::fstream &fs, uint64_t indexOfTheBit, bool isTaken) const;
	void SetIndexDescriptorBit(std::fstream &fs, uint64_t indexOfTheBit, bool isTaken) const;
	static void SetBitInMap(std::fstream &fs, uint64_t offsetTheMap, uint64_t indexOfTheBit, bool isTaken);
	void WriteIndexDescriptorAndSetBit(std::fstream &fs, uint64_t indexOfDescriptor, IndexDescriptor* theDescriptor) const;
	void WriteBytesToDataBlock(std::fstream &fs, uint64_t indexDataBlock, uint64_t offsetInDataBlock, const char* data, uint64_t sizeOfData) const;
	void WriteBytesTo(std::fstream &fs, uint64_t offsetFileBlock, uint64_t indexDataBlock, uint64_t offsetInDataBlock, const char* data, uint64_t sizeOfData) const;
	uint64_t GetFreeBitInBMPDataBlocksAndSaveAddress(std::fstream &fs, uint64_t addressBlockIndex, size_t offsetInAddressBlock) const;
	void AddZerosToAddressInBMPDataBlocks(std::fstream &fs, uint64_t addressBlockIndex, size_t offsetInAddressBlock) const;
	static bool GetBitStateInTheMap(std::fstream &fs, uint64_t mapOffsetInTheFile, uint64_t indexOfTheBit);
	bool IsBitInBMPIndexDescriptorsTaken(std::fstream &fs, uint64_t indexOfTheBit) const;
	bool IsBitInBMPDataBlockTaken(std::fstream &fs, uint64_t indexOfTheBit) const;
	IndexDescriptor GetFileDescriptor(const char* fileName) const;
	bool GetFileDescriptorIndex(const char* fileName, uint64_t& index) const;
	void ThrowExceptionIfTheFileExists(const char * fileName) const;
};

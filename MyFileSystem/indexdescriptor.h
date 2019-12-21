#pragma once
#include <cstring>
struct IndexDescriptor
{
	char fileName[25];
	uint64_t blockNumber;
	uint64_t fileSize;
	
	void SetFileName(const char * name)
	{
		strcpy_s(fileName, name);
	}
};
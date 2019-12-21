#include<iostream>
#include "filesystem1.h"
#include <string>
#include <functional>
#include <vector>
using namespace std;
void ShowData(char* data,uint64_t size,bool isInt)
{
	for (uint64_t i = 0; i < size; i++)
	{
		if (isInt)
			cout << static_cast<int>(data[i]);
		else
			cout << data[i];
	}
	cout << endl << endl;
}

vector<char> ReadFile(MyFileSystem& fs, const char * fname)
{
	vector<char> result;
	const auto fileSize = fs.GetFileSize(fname);
	result.resize(fileSize);
	fs.ReadFile(fname, result.data(), fileSize);
	return  result;
}

void ReadFileAndShow(MyFileSystem& fs, const char * fname, bool isInt)
{
	const auto fileSize = fs.GetFileSize(fname);
	auto dataToRead = new char[fileSize];

	fs.ReadFile(fname, dataToRead, fileSize);

	cout << "file " << fname <<":"<< endl;
	ShowData(dataToRead, fileSize, isInt);
	delete[]dataToRead;
	dataToRead = nullptr;
}
int main()
{
	//MyFileSystem myFileSystem("fileSystem.bin", "myFileSystem", 1024 * 16, 128);
	//myFileSystem.Format();
	//const auto sizeOfData = 32 * 128 + 1; //32 blocks(by 128) + 1= 33 blocks - to show 2d level address
	//const auto dataToWrite = new char[sizeOfData];
	//const char * fileName1 = "test.bin";
	//const char * fileName2 = "secondFile.bin";
	//const char * fileName3 = "message.txt";

	//for (int i = 0; i < sizeOfData; i++)dataToWrite[i] = 5;
	//if (myFileSystem.WriteFile(fileName1, dataToWrite, sizeOfData))
	//{
	//	cout << "File" << fileName1 << " has written successfully" << endl;
	//}

	//for (int i = 0; i < sizeOfData; i++) dataToWrite[i] = 8;
	//if (myFileSystem.WriteFile(fileName2, dataToWrite, sizeOfData))
	//{
	//	cout << "File" << fileName2 << " has written successfully" << endl;
	//}
	//string message = "This is a test message in the file";
	//if (myFileSystem.WriteFile(fileName3, message.c_str(), message.length()))
	//{
	//	cout << "File" << fileName3 << " has written successfully" << endl;
	//}

	//ReadFileAndShow(myFileSystem, fileName1, true);
	//ReadFileAndShow(myFileSystem, fileName2, true);
	//ReadFileAndShow(myFileSystem, fileName3, false);

	vector<string> photosPaths{ "1.jpg","2.jpg","3.jpg" };
	MyFileSystem myFileSystem2("fileSystem2.bin", "myFileSystem", 1024 * 1024 * 100, 1024);
	myFileSystem2.Format();


	for(auto &photosPath: photosPaths)
	{
		ifstream fs(photosPath, ios::binary);
		vector<char> bufferForPhoto;
		if (fs.is_open()) {
			bufferForPhoto.assign(istreambuf_iterator<char>(fs), {});
		}
		fs.close();


		myFileSystem2.WriteFile(photosPath.c_str(), &bufferForPhoto[0], bufferForPhoto.size());
		auto vectorForPhoto = ReadFile(myFileSystem2, photosPath.c_str());

		ofstream os("read_" + photosPath, ios::binary | ios::trunc);
		if (os.is_open())
		{
			for (char& i : vectorForPhoto)
			{
				os.write(&i, 1);
			}
		}
		os.close();
	}
	
	return 0;
}
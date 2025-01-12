#include"virtual_disk.h"
fstream virtualDisk::Disk;
using namespace std;


void virtualDisk::CREATEorOPEN_Disk(const string& path)
{
	virtualDisk::Disk.open(path, ios::in | ios::out | ios::binary);

	if (!Disk.is_open())
	{
		Disk.open(path, ios::out | ios::binary | ios::trunc);
		Disk.close();
		Disk.open(path, ios::in | ios::out | ios::binary);


		if (!Disk) {
			cout << "Error: Failed to create or open the virtual disk." << endl;
		}
	
	}

}




void virtualDisk::writeCluster(const vector<char>& cluster, int clusterIndex)
{
	Disk.seekp(clusterIndex * 1024, ios::beg);
	Disk.write(cluster.data(), 1024);
	Disk.flush();          //to syncronize data with the storage device
}
vector<char> virtualDisk::readCluster(int clusterIndex)
{
	Disk.seekg(clusterIndex * 1024, ios::beg);
	vector<char> bytes(1024);
	Disk.read(bytes.data(), 1024);
	return bytes;
}

int virtualDisk::getFreeSpace()
{

	/*
it seeks the get pointer zero steps from the end of file means that the
pointer
will be put at the end of file and moves zero or no steps
*/
	Disk.seekg(0, ios::end);
	/*
	tellg() returns the position of get pointer which represents number of bytes
	from the beginning of the file to that position and we cast the value to int
	and substract it from 1024*1024 (a value that reprsents our virtual disk
	size, 1MB)
	*/
	return (1024 * 1024) - static_cast<int>(Disk.tellg());
}

bool virtualDisk::isNew()
{
	// Seek to the end of the file
	Disk.seekg(0, std::ios::end);

	// Get the position of the get pointer (number of bytes in the file)
	int size = static_cast<int>(Disk.tellg());

	// Check if the size is zero (indicating the disk is newly created)
	return (size == 0);
}


void virtualDisk::closeTheDisk()
{
	if (Disk.is_open())
	{
		Disk.close();
		cout << "The virtual disk has been safely closed." << endl;
	}
	else
	{
		cout << "The virtual disk is already closed." << endl;
	}
}

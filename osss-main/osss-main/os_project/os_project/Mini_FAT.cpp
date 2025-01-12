
#include "Mini_FAT.h"
#include "Converter.h"
#include "virtual_disk.h"
int Mini_FAT::FAT[1024] = { 0 }; // Initialize all elements to 0

void Mini_FAT::initialize_FAT()
{
	for (int i = 0; i < 1024; i++)
	{
		if (i == 0 || i == 4)
		{
			FAT[i] = -1;
		}
		else if (i > 0 && i <= 3)
		{
			FAT[i] = i + 1;
		}
		else
		{
			FAT[i] = 0;
		}
	}

}


void Mini_FAT::printFAT()
{
	cout << "FAT has the following: ";
	for (int i = 0; i < 1024; i++)
		cout << "FAT[" << i << "] = " << Mini_FAT::FAT[i] << endl;
}



int Mini_FAT::getAvailableCluster()
{
	for (int i = 0; i < 1024; i++)
	{
		if (Mini_FAT::FAT[i] == 0)
			return i;
	}
	return -1;//our disk is full
}



int Mini_FAT::getAvailableClusters()
{
	int s = 0;
	for (int i = 0; i < 1024; i++)
	{
		if (Mini_FAT::FAT[i] == 0)
			s++;
	}
	return s;
}

int Mini_FAT::getFreeSize()
{
	return Mini_FAT::getAvailableClusters() * 1024;
}

int Mini_FAT::getClusterStatus(int clusterIndex)
{
	if (clusterIndex >= 0 && clusterIndex < 1024)
		return Mini_FAT::FAT[clusterIndex];
	else
		return -1;
}
// Retrieve the next cluster in the chain for a given cluster index

void Mini_FAT::setClusterPointer(int clusterIndex, int status)
{
	if (clusterIndex >= 0 && clusterIndex < 1024 && status >= 0 && status < 1024)
		Mini_FAT::FAT[clusterIndex] = status;
}


void Mini_FAT::writeFAT()
{
	vector<char> FATBYTES = Converter::intArrayToByteArray(Mini_FAT::FAT, 1024);
	vector<vector<char>> ls = Converter::splitBytes(FATBYTES);
	for (int i = 0; i < ls.size(); i++)
	{
		virtualDisk::writeCluster(ls[i], i + 1);
	}
}

void Mini_FAT::readFAT()
{
	vector<char> ls;
	for (int i = 1; i <= 4; i++)
	{
		vector<char> b = virtualDisk::readCluster(i);
		ls.insert(ls.end(), b.begin(), b.end());
	}
	Converter::byteArrayToIntArray(Mini_FAT::FAT, ls);
}


vector<char> Mini_FAT::CreateSuperBlock()
{
	vector<char> superBlock(1024, 0);
	return superBlock;
}


void Mini_FAT::initialize_Or_Open_FileSystem(string name)
{
	virtualDisk::CREATEorOPEN_Disk(name);
	if (virtualDisk::isNew())
	{
		vector<char> superBlock = Mini_FAT::CreateSuperBlock();
		virtualDisk::writeCluster(superBlock, 0);
		Mini_FAT::initialize_FAT();
		Mini_FAT::writeFAT();
	}
	else
	{


			Mini_FAT::readFAT();
	}
}


int Mini_FAT::getClusterPointer(int clusterIndex) {
	// Validate the cluster index range
	if (clusterIndex >= 0 && clusterIndex < 1024) {
		return Mini_FAT::FAT[clusterIndex]; // Return the value from the FAT table
	}
	else {
		cout << "Error: Invalid cluster index " << clusterIndex << "." << endl;
		return -1; // Return -1 for invalid indices
	}
}


void Mini_FAT::CloseTheSystem()
{
	Mini_FAT::writeFAT();
	virtualDisk::closeTheDisk();
}
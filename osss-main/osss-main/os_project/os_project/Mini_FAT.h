#pragma once
#include"virtual_disk.h"

class Mini_FAT
{
public:

	static int FAT[1024];

	static void initialize_FAT();
	
	static void printFAT();
	static int getAvailableCluster();
	static int getAvailableClusters();
	int getClusterStatus(int clusterIndex);
	static vector<char> CreateSuperBlock();
	
	static void writeFAT();
	static void readFAT();
	static void CloseTheSystem();
	
	static int getClusterPointer(int clusterIndex);
	static void initialize_Or_Open_FileSystem(string name);
	
	static void setClusterPointer(int clusterIndex, int pointer);
	
	static int getFreeSize();

private:


};
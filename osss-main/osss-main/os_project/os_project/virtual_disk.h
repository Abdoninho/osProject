#pragma once
#include <fstream>
#include<iostream>
#include <string>
#include<vector>
using namespace std;
class virtualDisk
{
public:
    static void CREATEorOPEN_Disk(const string& path);
    static void writeCluster(const vector<char>& cluster, int clusterIndex);
    static vector<char> readCluster(int clusterIndex);
    static bool isNew();
    static int getFreeSpace();
    static void closeTheDisk();

private:
    static fstream Disk;

};


#pragma once
#include "DirectoryEntry.h"
#include <vector>
using namespace std;

class Directory : public Directory_Entry {
public:

   
    Directory* parent; // Pointer to parent directory
    vector<Directory_Entry> DirOrFiles; // Children
    

    Directory(string name, char dir_attr, int dir_firstCluster, Directory* pa);

    Directory_Entry GetDirectory_Entry();
    string getFullPath();
    int getMySizeOnDisk();
    bool canAddEntry(Directory_Entry d);
    void emptyMyClusters();
    void writeDirectory();
    string getDrive();
    void readDirectory();
    void addEntry(Directory_Entry d);
    void removeEntry(Directory_Entry d);
    void deleteDirectory();
    void updatecontent(Directory_Entry OLD, Directory_Entry NEW);
    int searchDirectory(string name);
};

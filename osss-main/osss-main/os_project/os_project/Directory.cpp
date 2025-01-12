#include "Directory.h"
#include "virtual_disk.h"
#include "Mini_FAT.h"
#include <algorithm>
#include <iostream>
#include "Converter.h"
#include "DirectoryEntry.h"
#include "parser.h"
// Constructor initializes a Directory object with its name, attributes, first cluster, and parent directory
// Constructor: ده الكونستركتور اللي بيعمل إنشاء للدليل (Directory) وبيستلم:
// - الاسم (name)
// - النوع (attributes) زي فايل أو فولدر
// - أول كلستر بيبدأ منه (firstCluster)
// - والأب (Parent directory)
Directory::Directory(string name, char dir_attr, int dir_firstCluster, Directory* pa)
    : Directory_Entry (name, dir_attr, dir_firstCluster), parent(pa) 
{

}
                                                                

// Returns the Directory_Entry object of this directory
// GetDirectory_Entry: دي دالة بتجهز نسخة من الدليل ده كـ Directory_Entry عشان تستخدمه


Directory_Entry Directory::GetDirectory_Entry() {
    Directory_Entry me(dir_name, dir_attr, dir_firstCluster);
    for (size_t i = 0; i < 12; i++)
    {
        me.dir_empty[i] = dir_empty[i];
}
    me.dir_fileSize = dir_fileSize;
    return me;

}



// getMySizeOnDisk: بتحسب الحجم اللي مستخدمه الدليل على القرص




// Checks if a new entry can be added to the directory without exceeding the 1MB disk limit
// canAddEntry: بتشوف لو في مساحة كفاية لإضافة مدخل جديد في الدليل.

int Directory::getMySizeOnDisk()
{
    int size = 0;
    if (dir_firstCluster != 0)
    {
        int cluster = dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        do
        {
            size++;
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);
    }
    return size;
}



bool Directory::canAddEntry(Directory_Entry d)
{
    bool can = false;
    int neededSize = (DirOrFiles.size() + 1) * 32;
    int neededCluster = neededSize / 1024;
    int rem = neededSize % 1024;
    if (rem > 0) neededCluster++;
    neededCluster += d.dir_fileSize / 1024;
    int rem1 = d.dir_fileSize % 1024;
    if (rem1 > 0) neededCluster++;
    if (getMySizeOnDisk() + Mini_FAT::getAvailableClusters() >= neededCluster)
        can = true;
    return can;
}


// Clears all clusters associated with this directory by releasing them in the Mini_FAT
void Directory::emptyMyClusters() {
    if (this->dir_firstCluster != 0) {
        int cluster = this->dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        if (cluster == 5 && next == 0)
            return;
        do {
            Mini_FAT::setClusterPointer(cluster, 0);
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT:: getClusterPointer(cluster);
        } while (cluster != -1);
    }
}

// writeDirectory: اهم فنكشن   

// Reads the directory data from the virtual disk and populates the DirOrFiles vector with entries
void Directory::readDirectory() {
    if (this->dir_firstCluster != 0)
    {
        DirOrFiles.clear();
        int cluster = this->dir_firstCluster;
        int next = Mini_FAT::getClusterPointer(cluster);
        if (cluster == 5 && next == 0)
            return;
        vector<char> ls;
        do
        {
            vector<char> clusterData = virtualDisk::readCluster(cluster);
            ls.insert(ls.end(), clusterData.begin(), clusterData.end());
            cluster = next;
            if (cluster != -1)
                next = Mini_FAT::getClusterPointer(cluster);
        } while (cluster != -1);

        DirOrFiles=Converter::BytesToDirectory_Entries(ls);
    }

}

void Directory::removeEntry(Directory_Entry d)
{
    readDirectory();
    int index = searchDirectory(d.dir_name);
    if (index != -1)
    {
        DirOrFiles.erase(DirOrFiles.begin() + index);
        writeDirectory();
    }
}

void Directory::addEntry(Directory_Entry d)
{
    DirOrFiles.push_back(d);
    writeDirectory();
}


// Deletes the directory by releasing its clusters and clearing its contents
void Directory::deleteDirectory()
{
    emptyMyClusters();
    if (this->parent != nullptr)
    {
        this->parent->removeEntry(GetDirectory_Entry());
    }
}

// Updates an existing entry with new content and writes changes to disk
void Directory::updatecontent(Directory_Entry OLD, Directory_Entry New)
{
    readDirectory();
    int index = searchDirectory(OLD.dir_name);
    if (index != -1)
    {
        DirOrFiles[index] = New;
        writeDirectory();
    }
}
// Searches the directory for an entry by name and returns its index, or -1 if not found
int Directory::searchDirectory(string name)
{
    readDirectory();
    string searchName = name;
    if (searchName.length() > 11)
    {
        if (searchName.find('-') != string::npos)
        {
            string s = searchName.substr(0, 7);
            string ss = searchName.substr(searchName.rfind(".") + 1, 3);
            searchName = s + "." + ss;
        }
        else
        {
            searchName = searchName.substr(0, 11);
        }
    }
    for (int i = 0; i < DirOrFiles.size(); i++)
    {
        if (DirOrFiles[i].dir_name == searchName)
            return i;
    }
    return -1;
}



    void Directory::writeDirectory()
    {
        Directory_Entry A = this->GetDirectory_Entry();
        if (!this->DirOrFiles.empty())
        {
            vector<char> dirsOrFilesBytes = Converter::Directory_EntriesToBytes(this->DirOrFiles);
            vector<vector<char>> bytesList = Converter::splitBytes(dirsOrFilesBytes);
            int clusterFATIndex;
            if (this->dir_firstCluster != 0)
            {
                this->emptyMyClusters();
                clusterFATIndex = Mini_FAT::getAvailableCluster();
                this->dir_firstCluster = clusterFATIndex;
            }
            else
            {
                clusterFATIndex = Mini_FAT::getAvailableCluster();
                if (clusterFATIndex != 0)
                    this->dir_firstCluster = clusterFATIndex;
            }
            int lastCluster = -1;
            for (int i = 0; i < bytesList.size(); i++)
            {
                if (clusterFATIndex != -1)
                {
                    virtualDisk::writeCluster(bytesList[i], clusterFATIndex);
                    Mini_FAT::setClusterPointer(clusterFATIndex, -1);
                    if (lastCluster != -1)
                        Mini_FAT::setClusterPointer(lastCluster, clusterFATIndex);
                    lastCluster = clusterFATIndex;
                    clusterFATIndex = Mini_FAT::getAvailableCluster();
                }
            }
        }
        if (this->DirOrFiles.empty())
        {
            if (dir_firstCluster != 0)
                this->emptyMyClusters();
            if (parent != nullptr)
                this->dir_firstCluster = 0;
        }
        Directory_Entry B = this->GetDirectory_Entry();
        if (this->parent != nullptr)
        {
            this->parent->updatecontent(A, B);
        }

        Mini_FAT::writeFAT();
    }
    string Directory::getDrive() 
    {
        if (getName().length() == 2 && getName()[1] == ':') {
            string drive = getName().substr(0, 1);
            transform(drive.begin(), drive.end(), drive.begin(), ::toupper);
            return drive;
        }
        else {
            // Traverse up to find the root drive
            const Directory* current = this;
            while (current->parent != nullptr) {
                current = current->parent;
                if (current->getName().length() == 2 && current->getName()[1] == ':') {
                    string drive = current->getName().substr(0, 1);
                    transform(drive.begin(), drive.end(), drive.begin(), ::toupper);
                    return drive;
                }
            }
            return ""; // No drive found
        }
    }

    string Directory::getFullPath() 
    {
        if (parent == nullptr)
        {
            // Root directory using getDrive()
            string drive = getDrive();
            if (drive.empty()) {
                cout << "Error: Drive letter not found for root directory.\n";
                return "";
            }
            return drive + ":\\";
        }
        else
        {
            string parentPath = parent->getFullPath();

            // Ensure parentPath ends with a backslash
            if (!parentPath.empty() && parentPath.back() != '\\')
                parentPath += "\\";

            string currentName = getName();

            // Validate currentName to prevent malformed paths
            if (currentName.empty()) {
                cout << "Error: Current directory name is empty.\n";
                return parentPath; // Return parent path as a fallback
            }

            return parentPath + currentName;
        }
    }
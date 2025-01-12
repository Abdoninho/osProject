#pragma once
#include <string>
using namespace std;
/** Represents a directory or file entry with metadata like name, attributes, starting cluster, and file size. */
class Directory_Entry
{
public:

    /** The name of the directory/file (8 characters for the name + 3 characters for the extension). */
    char dir_name[11];

    /** The attribute byte (0x10 for directory, 0x00 for file). */
    char dir_attr;

    /** Reserved bytes for alignment or future use. */
    char dir_empty[12];

    /** The index of the first cluster where the data starts. */
    int dir_firstCluster;

    /** The size of the file in bytes (0 for directories). */
    int dir_fileSize;


    Directory_Entry();

    /** Initializes a Directory_Entry with name, attribute, and first cluster index. */
    Directory_Entry(string name, char attr, int firstCluster);

    /** Cleans the name by keeping only alphanumeric characters and underscores. */
    static string cleanTheName(string s);

    /** Assigns a file name (8 chars) and extension (3 chars) to dir_name, padding with blanks. */
    void assignFileName(string name, string extension);

    /** Assigns a directory name (up to 11 chars) to dir_name, padding with blanks. */
    void assignDir_Name(string name);

    string  getName() const; // Retrieves the trimmed name of the directory or file
};

#include "DirectoryEntry.h"
#include <algorithm>
#include <cctype>
#include <cstring>

using namespace std; // Using std namespace for convenience
Directory_Entry::Directory_Entry()
    : dir_attr(0), dir_firstCluster(0), dir_fileSize(0) {
    memset(dir_name, ' ', sizeof(dir_name));
    memset(dir_empty, 0, sizeof(dir_empty));
}

// Constructor to initialize a Directory_Entry object
Directory_Entry::Directory_Entry(string name, char attr, int firstCluster)
    : dir_attr(attr), dir_firstCluster(firstCluster), dir_fileSize(0)
{
    memset(dir_name, ' ', sizeof(dir_name)); // Initialize dir_name with spaces
    memset(dir_empty, 0, sizeof(dir_empty)); // Initialize reserved bytes with zeros

    if (dir_attr == 0x10) { // If it's a directory (0x10 means directory)
        assignDir_Name(name);  // Assign directory name
    }
    else {  // If it's a file
        size_t dotPos = name.find_last_of('.');  // Find the last dot to separate name and extension
        if (dotPos != string::npos) {  // If the file has an extension
            string fname = name.substr(0, dotPos);  // Get the file name part
            string ext = name.substr(dotPos + 1);  // Get the file extension part
            assignFileName(fname, ext);  // Assign the file name and extension
        }
        else {
            assignFileName(name, "");  // If no extension, assign the file name without extension
        }
    }
}

// Cleans the file/directory name to include only alphanumeric characters and underscores
string Directory_Entry::cleanTheName(string s)
{

    string result;
    size_t len = s.length();
    for (size_t i = 0; i < len; ++i) {
        char c = s[i];
        if (isalnum(static_cast<unsigned char>(c)) || c == '_') {  // Allow alphanumeric and underscores
            result += c;
        }
        // Allow ':' only if it's the last character (for drive letters)
        else if (c == ':' && i == len - 1) {
            result += c;
        }
        // Ignore all other characters
    }
    return result;  // Return the cleaned name
    
 /*   string result;
    for (char c : s) {
        if (isalnum(static_cast<unsigned char>(c)) || c == '_') {  // Allow alphanumeric and underscores
            result += c;
        }
    }
    return result;  // Return the cleaned name*/
}

// Assigns a file name and extension to the dir_name array (8 characters for name, 3 for extension)
void Directory_Entry::assignFileName(string name, string extension)
{
    string cleanName = cleanTheName(name);  // Clean the file name
    string cleanExt = cleanTheName(extension);  // Clean the extension

    // Truncate or pad the name to ensure it's 8 characters long
    string fname = cleanName.substr(0, 8);
    fname.resize(8, ' ');  // Pad with spaces if the name is shorter than 8 characters

    // Truncate or pad the extension to ensure it's 3 characters long
    string fext = cleanExt.substr(0, 3);
    fext.resize(3, ' ');  // Pad with spaces if the extension is shorter than 3 characters

    // Copy the name and extension into dir_name (first 8 for name, next 3 for extension)
    memcpy(dir_name, fname.c_str(), 8);
    memcpy(dir_name + 8, fext.c_str(), 3);
}

// Assigns a directory name to the dir_name array (up to 11 characters)
void Directory_Entry::assignDir_Name(string name)
{
    string cleanName = cleanTheName(name);  // Clean the directory name

    // Truncate or pad the name to ensure it's 11 characters long
    string dname = cleanName.substr(0, min(static_cast<size_t>(11), cleanName.length()));      //to prevent substr from throwing an exception if cleanName is shorter than 11 characters.
    dname.resize(11, ' ');  // Pad with spaces if the name is shorter than 11 characters

    // Copy the directory name into dir_name
    memcpy(dir_name, dname.c_str(), 11);
}

string Directory_Entry::getName() const
{
    // Extract the name from the dir_name array and trim trailing spaces
    string name(dir_name, 11); // Take the first 11 characters
    size_t spacePos = name.find(' '); // Find the first trailing space
    if (spacePos != string::npos)
    {
        name = name.substr(0, spacePos); // Remove trailing spaces
    }
    return name;
}
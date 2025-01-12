#include <iostream>
#include <sstream>
#include <string>
#include "virtual_disk.h"
#include "Mini_FAT.h"
#include "Directory.h"
#include "DirectoryEntry.h"
#include "File_Entry.h"
#include "tokenizer.h"
#include "parser.h"
#include "commands.h"
#include "Converter.h"
#include <vector>
using namespace std;




int main()
{

    Mini_FAT::initialize_Or_Open_FileSystem("virtual_disk.dat"); //initialize or open virtual disk and fat
    Directory root("C:", 0x10, 0, nullptr);  //create the root dir
    
    Directory* currentDir = &root;
    Commands cmdProcessor(&currentDir);
    
    bool isRunning = true;
    while (isRunning) {
        cout << "easy_shell^-^ ";
        string input;
        cout << currentDir->getFullPath() << " >> ";
        getline(cin, input);
        cmdProcessor.processCommand(input, isRunning);
    }

    Mini_FAT::CloseTheSystem();    // Close the system properly

    return 0;
}
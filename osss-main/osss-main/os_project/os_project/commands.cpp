#include "commands.h"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;

// Constructor
Commands::Commands(Directory** currentDirPtr)
    : currentDirectoryPtr(currentDirPtr) {
    // Populate the help dictionary
    commandHelp["help"] = {
        "Provides help information for commands.",
        "Usage:\n"
        "  help\n"
        "  help [command]\n\n"
        "Description:\n"
        "  Displays help for all commands or detailed help for a specific command."
    };
    commandHelp["pwd"] = {
        "Prints the current working directory.",
        "Usage:\n  pwd\n\nDescription:\n  Shows the absolute path of the current directory."
    };
    commandHelp["quit"] = {
        "Quit the shell",
        "Usage:\n  quit\n\nDescription:\n  Quit the shell "
    };
    commandHelp["cls"] = {
        "clear the screen",
        "Usage:\n  cls\n\nDescription:\n" "clear the screan"
            
    };

    commandHelp["copy"] = {
    "copy one or more files to another location",
    "Usage:\n  copy [source]\nor\ncopy[source] [destination]  \n\nDescription:\n copy one or more files to another location"

    };


    commandHelp["dir"] = {
       "List the content of the directory",
       "Usage:\n"
       "  dir\n"
       "  dir [directory]\n\n"
       "Description:\n"
       "  List the content of the directory."
    };



    // Add more commands as needed
}

// Show general help for all commands
void Commands::showGeneralHelp() {
    cout << "Available Commands:\n";
    for (const auto& cmd : commandHelp) {
        cout << "  " << cmd.first << " - " << cmd.second.first << "\n";
    }
}

// Show detailed help for a specific command
void Commands::showCommandHelp(const string& command) {
    auto it = commandHelp.find(command);
    if (it != commandHelp.end()) {
        cout << it->second.second << "\n";
    }
    else {
        cout << "Error: Command '" << command << "' is not supported.\n";
    }
}


void Commands::printWorkingDirectory() {


    cout << "Current Directory: " << (*currentDirectoryPtr)->getFullPath() << "\n";

}


Directory* Commands::MoveToDirectory(const std::string& path) {
    if (path.empty()) {
        return *currentDirectoryPtr;
    }

    Directory* dir = nullptr;
    std::vector<std::string> parts;
    std::string delimiter;

    // Determine path delimiter based on input
    if (path.find('\\') != std::string::npos) {
        delimiter = "\\";
    }
    else {
        delimiter = "/";
    }

    // Split the path into parts
    size_t pos = 0;
    size_t found;
    std::string token;
    std::string tempPath = path;

    while ((found = tempPath.find(delimiter)) != std::string::npos) {
        token = tempPath.substr(0, found);
        if (!token.empty()) {
            parts.push_back(token);
        }
        tempPath.erase(0, found + delimiter.length());
    }
    if (!tempPath.empty()) {
        parts.push_back(tempPath);
    }

    // Determine if the path is absolute (starts with drive letter)
    bool isAbsolute = false;
    size_t firstColon = path.find(':');
    if (firstColon != std::string::npos && firstColon == 1) { // e.g., "C:\..."
        isAbsolute = true;
    }

    // Start from root if absolute, else from current directory
    if (isAbsolute) {
        // Assuming the root directory is the current directory's ancestor with no parent
        Directory* root = *currentDirectoryPtr;
        while (root->parent != nullptr) {
            root = root->parent;
        }
        dir = root;
    }
    else {
        dir = *currentDirectoryPtr;
    }

    // Traverse each part
    for (const auto& part : parts) {
        dir->readDirectory(); // Ensure directory entries are loaded
        int index = dir->searchDirectory(part);
        if (index == -1) {
            // Directory not found
            return nullptr;
        }
        Directory_Entry entry = dir->DirOrFiles[index];
        if (!(entry.dir_attr & 0x10)) {
            // Not a directory
            return nullptr;
        }
        // Create a new Directory object for the subdirectory
        Directory* subDir = new Directory(entry.getName(), entry.dir_attr, entry.dir_firstCluster, dir);
        subDir->readDirectory(); // Load its contents
        dir = subDir;
    }

    return dir;
}


void Commands::mkDir(const std::string& argument) {
   /* if (argument.empty()) {
        std::cout << "Error: 'md' command requires a directory name or path.\n";
        std::cout << "Usage:\n  md [directory]\n";
        return;
    }*/

    std::string inputPath = argument;
    Directory* targetParentDir = nullptr;
    std::string newDirName;

    // Check if the inputPath is a full path
    bool isFullPath = (inputPath.find(':') != std::string::npos) ||
        (inputPath.find('\\') != std::string::npos) ||
        (inputPath.find('/') != std::string::npos);

    if (isFullPath) {
        // Full Path Handling
        size_t lastBackslash = inputPath.find_last_of('\\');
        size_t lastSlash = inputPath.find_last_of('/');

        size_t separator = std::string::npos;
        if (lastBackslash != std::string::npos && lastSlash != std::string::npos) {
            separator = std::max(lastBackslash, lastSlash);
        }
        else if (lastBackslash != std::string::npos) {
            separator = lastBackslash;
        }
        else if (lastSlash != std::string::npos) {
            separator = lastSlash;
        }

        if (separator != std::string::npos) {
            newDirName = inputPath.substr(separator + 1);
            std::string parentPath = inputPath.substr(0, separator);

            // Navigate to the parent directory using MoveToDirectory
            targetParentDir = MoveToDirectory(parentPath);

            if (targetParentDir == nullptr) {
                std::cout << "Error: Parent path '" << parentPath << "' does not exist.\n";
                return;
            }
        }
        else {
            // Path does not contain any separators, treat it as relative
            targetParentDir = *currentDirectoryPtr;
            newDirName = inputPath;
        }
    }
    else {
        // Relative Path Handling
        targetParentDir = *currentDirectoryPtr;
        newDirName = inputPath;
    }

    // Read the contents of the target parent directory
    targetParentDir->readDirectory();

    // Check if the directory already exists
    if (targetParentDir->searchDirectory(newDirName) != -1) {
        std::cout << "Error: Directory '" << newDirName << "' already exists.\n";
        return;
    }

    // Create a new Directory_Entry for the new directory
    Directory_Entry newDirEntry(newDirName, 0x10, 0); // dir_attr=0x10 (directory), dir_firstCluster=0 (empty), dir_fileSize=0

    // Check if there's enough space to add the new directory entry
    if (!targetParentDir->canAddEntry(newDirEntry)) {
        std::cout << "Error: Not enough space to create directory.\n";
        return;
    }

    // Add the new directory entry to the parent directory
    targetParentDir->addEntry(newDirEntry);

    std::cout << "Directory '" << newDirName << "' created successfully.\n";
}


void Commands::Cls() {
    system("cls");
}


void Commands::processCommand(const std::string& input, bool& isRunning) {
    // Add command to history
    if (!input.empty()) {
        commandHistory.push_back(input);
    }

    // Tokenize the input
    std::vector<std::string> tokens = Tokenizer::tokenize(input);

    if (tokens.empty()) {
        return; // No command entered
    }

    // Parse tokens into command and arguments
    Command cmd = Parser::parse(tokens);

    // Convert command name to lowercase for case-insensitive comparison
    std::transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(), ::tolower);

    // Handle the commands
    if (cmd.name == "help") {
        if (cmd.arguments.empty()) {
            showGeneralHelp();
        }
        else if (cmd.arguments.size() == 1) {
            showCommandHelp(cmd.arguments[0]);
        }
        else {
            std::cout << "Error: Invalid syntax for help command.\n";
            std::cout << "Usage:\n  help\n  help [command]\n";
        }
    }
    else if (cmd.name == "pwd") {
        if (cmd.arguments.empty()) {
            printWorkingDirectory();
        }
        else {
            std::cout << "Error: Invalid syntax for pwd command.\n";
            std::cout << "Usage:\n  pwd\n";
        }
    }
    else if (cmd.name == "quit") {
        if (cmd.arguments.empty()) {
            isRunning = false;
        }
        else {
            std::cout << "Error: Invalid syntax for quit command.\n";
            std::cout << "Usage:\n  quit\n";
        }
    }
    else if (cmd.name == "cls") {
        if (cmd.arguments.empty()) {
            Cls();
        }
        else {
            std::cout << "Error: Invalid syntax for cls command.\n";
            std::cout << "Usage:\n  cls\n";
        }
    }
    else if (cmd.name == "md")
    {
        if (cmd.arguments.size() == 1)
        {
            mkDir(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for md command.\n";
            cout << "Usage: md [directory_name]\n";
        }
    }
    else {
        std::cout << "Error: Command '" << cmd.name << "' is not supported.\n";
    }
}













// Process commands
//void Commands::processCommand(const string& input, bool& isRunning) {
//    // Add command to history
//    if (!input.empty()) {
//        commandHistory.push_back(input);
//    }
//
//    // Tokenize the input
//    vector<string> tokens = Tokenizer::tokenize(input);
//
//    if (tokens.empty()) {
//        return; // No command entered
//    }
//
//    // Parse tokens into command and arguments
//    Command cmd = Parser::parse(tokens);
//
//    // Convert command name to lowercase for case-insensitive comparison
//    transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(), ::tolower);
//
//    // Handle the commands
//    if (cmd.name == "help") {
//        if (cmd.arguments.empty()) {
//            showGeneralHelp();
//        }
//        else if (cmd.arguments.size() == 1) {
//            showCommandHelp(cmd.arguments[0]);
//        }
//        else {
//            cout << "Error: Invalid syntax for help command.\n";
//            cout << "Usage:\n  help\n  help [command]\n";
//        }
//    }
//   
//    else if (cmd.name == "pwd") {
//        // Example for handling 'pwd' command
//        cout << "Current Directory: " << (*currentDirectoryPtr)->getFullPath() << "\n";
//    }
//
//    else if (cmd.name == "quit") {
//        // Exit the shell
//        isRunning = false;
//    }
//
//
//
//    else {
//        cout << "Error: Command '" << cmd.name << "' is not supported.\n";
//    }
//}

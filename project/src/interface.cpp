#include "interface.h"
#include "index.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void IndexBuilder::printDir(std::string dir, std::string pt) {
    DIR *dr = opendir(dir.c_str());
    struct dirent *entry;
    struct stat statbuf;
    
    if (!dr)
        return;
    
    chdir(dir.c_str());
    
    while(entry = readdir(dr)) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
                continue;
            printDir(entry->d_name, pt+"/"+entry->d_name);
            path.push_back(pt+"/"+entry->d_name+"0");
        } 
        else
            path.push_back(pt+"/"+entry->d_name+"1");
        name.push_back(entry->d_name);
    }
    chdir("..");
    closedir(dr);
}

void IndexBuilder::buildIndex(const std::string& dirPath, const std::string& filename){
    std::ofstream os = std::ofstream(filename, std::ofstream::binary);
    
    lookup(dirPath);    
    Index* id = new Index(name);
    id->save(os);
    id->addPath(os, path);
    
    os.close();
    delete id;
    std::cout << "Index built.\n";
}

void IndexBuilder::lookup(const std::string& dirPath){
    printDir(dirPath, dirPath);
    
    std::cout << "Lookup is over.\n";
}

void getAns(const std::string& substr, const std::string& filename, const int type){
    if (FILE *file = fopen(filename.c_str(), "r"))
        fclose(file);
    else
        throw InterfaceExeption("Error opening index file.");
    
    std::ifstream is = std::ifstream(filename, std::ifstream::binary);
    is.seekg(0, std::ios::beg);
    Index* id = new Index(is);
    
    std::unordered_set<unsigned int> pos;
    int st = id->getState(substr);
    if (st != -1)
        id->getAllPos(st, substr.length(), pos);
        
    for(auto it: pos)
        id->printStr(it, type);
    
    delete id;
    is.close();
}

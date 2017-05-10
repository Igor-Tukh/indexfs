#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <vector>

class InterfaceExeption: public std::exception {
    public:
        const char * what () const throw (){
            return message.c_str();
        }
        
        InterfaceExeption(const std::string &s) : message(s) {}
    private:
        std::string message;
};

class IndexBuilder{
    public:
        void buildIndex(const std::string& dirPath, const std::string& filename);
        void printDir(std::string dir, std::string path);
        void lookup(const std::string& dirPath);
    private:
        std::vector<std::string> name;
        std::vector<std::string> path;
};

void getAns(const std::string& substr, const std::string& filename, const int type = -1);

#endif

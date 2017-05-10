#include "interface.h"

#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <exception>

class ArgumentsParseExeption: public std::exception {
    public:
        const char * what () const throw (){
            return message.c_str();
        }
        
        ArgumentsParseExeption(const std::string &s) : message(s) {}
    private:
        std::string message;
};


int main(int argc, char** argv){
    try{
        if (argc < 4 || argc > 4){
            if (argc != 2 || strcmp(argv[1], "-h") * strcmp(argv[1], "--help"))
                throw ArgumentsParseExeption("Wrong number of arguments");
        }
        if (!strcmp(argv[1], "-b")){
            IndexBuilder* ib = new IndexBuilder;
            ib->buildIndex(argv[2], argv[3]);
            delete ib;
        } else if (!strcmp(argv[1], "-s"))    
            getAns(argv[2], argv[3]);
        else if (!strcmp(argv[1], "-sf"))    
            getAns(argv[2], argv[3], 1);
        else if (!strcmp(argv[1], "-sd"))     
            getAns(argv[2], argv[3], 0);
        else        
            throw ArgumentsParseExeption("Unidentified parameter");
    }
    catch(ArgumentsParseExeption& ape){
        std::cout << ape.what() << "\n";
        std::cout << "indexfs [-h|--help|[-b dirpath indexname]|[-s substring indexname]|[-sf substring indexname]|[-sd substring indexname]]\n";
        std::cout << "-b = build, -s = search, -sf = search files, -sd = search directories\n";
        return 1;
    } catch(InterfaceExeption& ie){
        std::cout << ie.what() << "\n";
        std::cout << "indexfs [-h|--help|[-b dirpath indexname]|[-s substring indexname]|[-sf substring indexname]|[-sd substring indexname]]\n";
        std::cout << "-b = build, -s = search, -sf = search files, -sd = search directories\n";
        return 1;
    }
    return 0;
}

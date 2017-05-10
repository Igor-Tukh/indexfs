#ifndef SUFFIX_AUTOMATON
#define SUFFIX_AUTOMATON

#include <vector>
#include <map>
#include <string> 
#include <iostream>

const unsigned int ALPHABET_SIZE = 256;


class SuffixAutomaton{
    friend class Index;
    public:
        struct State{
            int32_t link, len, firstpos;
            bool isClone; // Useless or optimization?
            int next[ALPHABET_SIZE] = {0};
            std::vector<int> revlink;
            
            State(int32_t ln, int32_t fp, int32_t lk = 0):link(lk), firstpos(fp), len(ln), isClone(0){}
        };
        
        void init();
        void addChar(unsigned char c);
        void addRevLinks();
        ~SuffixAutomaton();
        size_t getSize() const;
        int32_t getStateSize(SuffixAutomaton::State* st) const;
    
    private:
        std::vector<State*> states;
        int last;
};


#endif

#ifndef INDEX_H
#define INDEX_H

#include <vector>
#include <unordered_set>
#include <string>
#include <iostream>

#include "suffixAutomaton.h"

class Index{
    public:
        Index(const std::vector<std::string>& v);
        Index(std::istream& is);
        ~Index();
        void save(std::ostream& os) const;
        SuffixAutomaton::State* readState(size_t ind) const;
        bool containsSubstr(const std::string& s) const;
        int getState(const std::string& s) const;
        int getSubstrFirstPos(const std::string& s) const;
        void getAllPos(int state, int strlength, std::unordered_set<unsigned int>& pos) const;
        size_t getNumber(int ind) const;
        void addPath(std::ostream& os, const std::vector<std::string>& v) const;
        void printStr(size_t ind, const int typ) const;
    private:
        SuffixAutomaton* sa;
        std::int32_t totalLen;
        std::size_t strCnt;
        std::vector<std::int32_t> lens;
        std::istream* in;
};

#endif

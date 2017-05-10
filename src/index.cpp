#include "index.h"

Index::Index(const std::vector<std::string>& v){
    sa = new SuffixAutomaton;
    sa->init();
    
    strCnt = v.size();
    totalLen = 0;
    for(auto it: v){
        lens.push_back(it.length());
        for(int i = 0; i < it.length(); i++)
            sa->addChar((unsigned char)it[i]);
        sa->addChar((unsigned char)'/'); // / - sep
        
        totalLen += it.length() + 1;
    }
    
    sa->addRevLinks();
}

Index::Index(std::istream& is){
    sa = new SuffixAutomaton;
    size_t sz = 0;
    is.read((char*)&sz, sizeof(size_t));
    is.read((char*)&strCnt, sizeof(size_t));
    is.read((char*)&totalLen, sizeof(int32_t));
    
    in = &is;    
    sa->states = std::vector<SuffixAutomaton::State*> (sz, NULL);
}

Index::~Index(){
    delete sa;
}

void Index::save(std::ostream& os) const{
    //size(sa),strCnt,totalLen
    size_t sz = sa->getSize();
    os.write((char*)&sz, sizeof(size_t));
    os.write((char*)&strCnt, sizeof(size_t));
    os.write((char*)&totalLen, sizeof(int32_t));
    //line for ind -> str number 
    size_t sep = strCnt;
    for(size_t i = 0; i < lens.size(); i++){
        for(int j = 0; j < lens[i]; j++)
            os.write((char*) &i, sizeof(size_t));
        os.write((char*) &sep, sizeof(size_t));
    }
    //precalc offset for states
    std::vector<int32_t> offsets;
    int32_t last = 0;
    for(size_t i = 0; i < sz; i++){
        os.write((char*)&last, sizeof(int32_t));
        last += sa->getStateSize(sa->states[i]);
    }
    os.write((char*) &last, sizeof(int32_t));
    //write states
    size_t cnt = 0;
    for(size_t i = 0; i < sz; i++){
        os.write((char*) &(sa->states[i]->len), sizeof(int32_t));
        os.write((char*) &(sa->states[i]->link), sizeof(int32_t));
        os.write((char*) &(sa->states[i]->firstpos), sizeof(int32_t));
        
        cnt = 0;
        for(size_t j = 0; j < ALPHABET_SIZE; j++)
            cnt += (sa->states[i]->next[j] != 0);
        
        os.write((char*) &cnt, sizeof(size_t));
        size_t sz = sa->states[i]->revlink.size();
        os.write((char*) &sz, sizeof(size_t));
        char cl = (char) sa->states[i]->isClone;
        os.write((char*) &cl, sizeof(char));
        
        for(int j = 0; j < ALPHABET_SIZE; j++)
            if (sa->states[i]->next[j] != 0){
                os.write((char*) &j, sizeof(int));
                os.write((char*) &(sa->states[i]->next[j]), sizeof(int));
            }
        
        for(auto it: sa->states[i]->revlink)
            os.write((char*) &it, sizeof(int));
    }    
}

SuffixAutomaton::State* Index::readState(size_t ind) const{
    if (!sa->states[ind]){
        int32_t ln, lk, fp, off;
        in->seekg(sizeof(size_t) * 2 + sizeof(int32_t) + totalLen * sizeof(size_t) + ind * sizeof(int32_t), std::ios::beg);
        in->read((char*) &off, sizeof(int32_t));
        in->seekg(sizeof(size_t) * 2 + sizeof(int32_t) + totalLen * sizeof(size_t) + (sa->getSize() + 1) * sizeof(int32_t) + off, std::ios::beg);
        in->read((char*) &ln, sizeof(int32_t));
        in->read((char*) &lk, sizeof(int32_t));
        in->read((char*) &fp, sizeof(int32_t));
        sa->states[ind] = new SuffixAutomaton::State(ln, fp, lk);
        size_t cnt, sz;
        char c;
        in->read((char*) &cnt, sizeof(size_t));
        in->read((char*) &sz, sizeof(size_t));
        in->read(&c, sizeof(char));
        sa->states[ind]->isClone = (bool) c;
        
        for(size_t i = 0; i < cnt; i++){
            int c;
            in->read((char*)&c, sizeof(int));
            in->read((char*)&(sa->states[ind]->next[c]), sizeof(int));
        }
        
        int tmp;
        for(size_t i = 0; i < sz; i++){
            in->read((char*)&tmp, sizeof(int));
            sa->states[ind]->revlink.push_back(tmp);
        }
    }
    return sa->states[ind];
}

bool Index::containsSubstr(const std::string& s) const{
    return (getState(s) != -1);
}

int Index::getState(const std::string& s) const{
    int cur = 0;
    int ind = 0;
    
    for(; ind < s.length() && readState(cur)->next[(unsigned char)s[ind]]; ind++)
        cur = sa->states[cur]->next[(unsigned char)s[ind]];
        
    if (ind != s.length())
        return -1;
        
    return cur;
}

int Index::getSubstrFirstPos(const std::string& s) const{
    int pos = getState(s);
    
    if (pos == -1)
        return -1;
    
    return readState(pos)->firstpos - s.length() + 1;
}


void Index::getAllPos(int state, int strlength, std::unordered_set<unsigned int>& pos) const{
    //if (!readState(state)->isClone)
    pos.insert(getNumber(readState(state)->firstpos - strlength + 1));
    
    
    for(auto it: sa->states[state]->revlink)
        getAllPos(it, strlength, pos);   
}

size_t Index::getNumber(int ind) const{
    in->seekg(2 * sizeof(size_t) + sizeof(int32_t) + ind * sizeof(size_t), std::ios::beg);
    size_t ans;
    in->read((char*) &ans, sizeof(size_t));
    return ans;
}


void Index::addPath(std::ostream& os, const std::vector<std::string>& v) const{
    int32_t last = 0;
    for(auto it: v){
        os.write((char*) &last, sizeof(int32_t));
        size_t len = it.length();
        os.write((char*) &len, sizeof(size_t));
        last += it.length();
    }
    for(auto it: v)
        for(int i = 0; i < it.length(); i++)
            os.write((char*) &(it[i]), sizeof(char));
}

void Index::printStr(size_t ind, const int type) const{
    in->seekg(sizeof(size_t) * 2 + sizeof(int32_t) + totalLen * sizeof(size_t) + (sa->getSize()) * sizeof(int32_t), std::ios::beg);
    int32_t all;
    in->read((char*) &all, sizeof(int32_t));
    in->seekg(all + ind * (sizeof(size_t) + sizeof(int32_t)), std::ios::cur);
    int32_t off;
    size_t len;
    in->read((char*) &off, sizeof(int32_t));
    in->read((char*) &len, sizeof(size_t));
    in->seekg(sizeof(size_t) * 2 + sizeof(int32_t) + totalLen * sizeof(size_t) + (sa->getSize() + 1) * sizeof(int32_t) + all + strCnt * (sizeof(size_t) + sizeof(int32_t)) + off, std::ios::beg);
    char c;
    in->seekg(sizeof(char) * (len - 1), std::ios::cur);
    in->read((char*) &c, sizeof(char));
    if (type == -1 || type == c - '0'){
        in->seekg(-len * sizeof(char), std::ios::cur);
        for(size_t i = 0; i < len - 1; i++){
            in->read((char*) &c, sizeof(char));
            std::cout << c;
        }
        std::cout << "\n";
    }
}

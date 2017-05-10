#include "suffixAutomaton.h"
#include "index.h"

void SuffixAutomaton::init(){
    last = 0;
    states.push_back(new State(0, -1, -1));
}

void SuffixAutomaton::addChar(unsigned char c){
    states.push_back(new State(states[last]->len + 1, states[last]->len));
    
    int cur = states.size() - 1;
    
    int p = last;
    
    for(; p != -1 && !states[p]->next[c]; p = states[p]->link)
        states[p]->next[c] = cur;
    
    if (p == -1)
        states[cur]->link = 0;
    else{
        int q = states[p]->next[c];
        if (states[q]->len == states[p]->len + 1)
            states[cur]->link = q;
        else{
            states.push_back(new State(states[p]->len + 1, states[q]->firstpos, states[q]->link));
            states[q]->isClone = 1;
            std::copy(std::begin(states[q]->next), std::end(states[q]->next), std::begin(states[states.size() - 1]->next));
            
            for (; p != -1 && states[p]->next[c] == q; p = states[p]->link)
				states[p]->next[c] = states.size() - 1;
		    
		    states[q]->link = states.size() - 1;
		    states[cur]->link = states.size() - 1;
        }
    }
    
    last = cur;
}

void SuffixAutomaton::addRevLinks(){
    for(int i = 0; i < states.size(); i++)
        if (states[i]->link != -1)
            states[states[i]->link]->revlink.push_back(i);
}

SuffixAutomaton::~SuffixAutomaton(){
    for(auto it: states)
        delete it;
    
    states.clear();
}

size_t SuffixAutomaton::getSize() const{
    return states.size();
}


int32_t SuffixAutomaton::getStateSize(SuffixAutomaton::State* st) const{
    int32_t ans = 3 * sizeof(int32_t) + 2 * sizeof(size_t) + sizeof(char);
    //bool->char
    //+count(next)
    for(int i = 0; i < ALPHABET_SIZE; i++)
        if (st->next[i] != 0)
            ans += 2 * sizeof(int);
    //+count(revlink)
    ans += sizeof(int) * st->revlink.size();
    return ans;
}

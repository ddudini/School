#ifndef Memory_h
#define Memory_h
#include <cmath>
#include <vector>
#include <iterator>
#include <string>
#include <iostream>
using namespace std;

class Frame {
public:
    int size;
    int address;
    int allid;
    int pid;

    
    Frame(int s,int add, int i){
        size = s;
        address = add;
        allid = i;
        pid = -1;

    }
};

class Memory{
public:
    vector <Frame> mem;
    int total_size;
    int frame_size;
    int total_allid;
    
    Memory(){}
    Memory(int t_size, int frame);
    
    string printMem();
    int memoryAllocation(int pid, int pageNum);
    int memsize(int pageNum);
    
};


#endif /* Memory_h */


#ifndef MemorySystem_h
#define MemorySystem_h
#include <cmath>
#include <vector>
#include <iterator>
#include <string>
#include <iostream>
#include <algorithm>
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
    void printFrame(){
        cout << "[ADD: " <<address << " |ALLID: " << allid << "|pid: " << pid << " |size: " << size <<"\n";
    }
};

class MemorySystem{
public:
    vector <Frame> memory;
    vector < vector <Frame> > free_list;
    int max_order;
    int frame_size;
    int total_allid;
    
    MemorySystem(){};
    MemorySystem(int pmemSize, int frame_size_);
    string printMemotyStatus();
    int memoryDeAllocation(int pid, int allid);
    int memoryAllocation(int pid, int pageSize);
    void memoryAllocation2(int pid, int address, int size);
    int findSize(int size);
    void printFreeList();
};


#endif /* MemorySystem_h */


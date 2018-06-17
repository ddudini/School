

#ifndef MemorySystem_hpp
#define MemorySystem_hpp

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
    bool printflag;
    
    Frame(int s,int add, int i){
        size = s;
        address = add;
        allid = i;
        pid = -1;
        printflag = false;
        
    }
    Frame(int s,int add, int i, int pid_){
        size = s;
        address = add;
        allid = i;
        pid = pid_;
        printflag = false;
        
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
    int pmem_size;
    int frame_size;

    vector <pair <int, int> > LRU;
    
    MemorySystem(){};
    MemorySystem(int pmemSize, int frame_size_);
    string printMemoryStatus();
    void memoryDeAllocationByLRU(int cycle, vector <pair <int, int> > &lrulist);
    void memoryDeAllocation(int cycle, int pid, int allid);
    void memoryAllocation(int cycle, int pid, int pageSize, int allid, vector <pair <int, int> > &lrulist);
    void memoryAllocation2(int cycle, int pid, int allid,int address, int size);
    int findSize(int size);
    void memoryAcess(int cycle, int id, int aid);
    void printFreeList();
    bool isBuddy(int num);
    void deleteFromLru(int pid, int allid);
};




#endif /* MemorySystem_hpp */

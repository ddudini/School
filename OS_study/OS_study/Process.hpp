//
//  Process.hpp
//  OS_study
//
//  Created by Sujin Han on 2018. 5. 28..
//  Copyright © 2018년 ddudini. All rights reserved.
//

#ifndef Process_hpp
#define Process_hpp
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <set>
using namespace std;
class PageEntry{
public:
    int allid;
    int valid;
    PageEntry(){};
    PageEntry(int a, int b){allid = a; valid = b;};
};

class Process {
public:
    int pid;
    string name;
    string file;
    int totalInst;
    int sleepCycle;
    int CpuCycle;
    vector <int> opcode;
    vector <int> argument;
    int index;
    
    int total_allid;
    set <int> valid_allid;
    vector <PageEntry> PTB;
    
    
    Process(){};
    Process(int id, string n, int size, int vmemSize);
    Process(const Process &p);
    string printProcess();
    string printProcessToFile(int cycle);
    int getOpcode();
    int getArgument();
    bool updateIndex();
    void printVmem(int cycle);
    int memoryAllocation(int cycle, int size);
    bool memoryDeAllocation(int cycle, int allid);
    void memAcess(int cycle, int allid, int &pageFaultSize);
    void meomoryDeAllocationAll();
};
#endif /* Process_hpp */

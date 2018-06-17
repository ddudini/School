//
//  Process.h
//  Assignment3
//
//  Created by Sujin Han on 2018. 5. 22..
//  Copyright © 2018년 ddudini. All rights reserved.
//

#ifndef Process_h
#define Process_h
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class PageEntry {
public:
    int page_start;
    int page_end;
    int allID;
    int valid;
    
    PageEntry(int start, int end, int all, int vd){
        page_start = start;
        page_end = end;
        allID = all;
        valid = vd;
    }
};


class Process {
public:
    int pid;
    int codeIndex;
    int sleep_cycle;
    int cycle;
    int totalInst;
    string name;
    vector <PageEntry> PageTable;
    int ** inst;
    
    
    Process();
    Process(int id_, string f);
    Process(int id_, string f, vector <vector <int> > &inst_, int totalinst);
    void readfile(string filename);
    
    int getOpcode();
    int getArgument();
    int updateInst();
    void sleep(int cycle, int num);
    void updateCycle(int c);
    void printProcessStatus();

};


#endif /* Process_h */

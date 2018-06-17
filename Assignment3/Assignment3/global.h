#ifndef global_h
#define global_h

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <cstdio>
#include "Process.h"

using namespace std;

#define MEMALLOCATION 0
#define MEMACESS 1
#define MEMRELEASE 2
#define NOMEM 3
#define SLEEP 4
#define IOWAITS 5

string root_dir = "/Users/sujinhan/Desktop/OS/Assignment3/example/";

ofstream sysout(root_dir+"system2.txt");
ofstream scheout(root_dir+"scheduler2.txt");
Process *cur_proc;
vector<Process> process_list;
vector<Process> RRQ;
vector<Process> IOQ;
vector<Process> SleepQ;
vector<Process>::iterator it;

int cycle = 0, codeIndex = 1, total_pid = 0, ctq = 0;
int totalEventNum, timeQuantum, vmemSize, pmemSize, pageSize, feedFeeq, feedSize;

vector< vector <string> > instruction;

void printSystem();
void printSchedular();


#endif /* global_h */

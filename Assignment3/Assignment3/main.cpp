

#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include "Process.h"
#include <iterator>
#include <vector>
#include <fstream>
#include "Process.h"
#include "MemorySystem.h"

using namespace std;

string root_dir = "/Users/sujinhan/Desktop/OS/Assignment3/example/";
ofstream sysout(root_dir+"system2.txt");
ofstream scheout(root_dir+"scheduler2.txt");

#define MEMALLOCATION   0
#define MEMACESS        1
#define MEMRELEASE      2
#define NOMEM           3
#define SLEEP           4
#define IOWAITS         5

vector <Process> process_list;
vector <Process>::iterator cur;
int cur_pid;
vector<Process>::iterator it;
vector <int> RRQ;
vector <int> IOQ;
vector <int> SleepQ;

int cycle = 0, codeIndex = 1, total_pid = 0, ctq, feedCycle = 0;
int totalEventNum, timeQuantum, vmemSize, pmemSize, pageSize, feedFeeq, feedSize;

vector< vector <string> > instruction;

void checkSleepQ(int cycle);
void getProcessFromRRQ(int feedSize, int feedFreq);
void getProcessFromRRQ2(bool flag);
void giveCpuCycle(int type, int feedSize);
void printSchedular(int cycle);
void printSystem(int cycle);
void decreaseCpuCycle();

int main(int argc, char ** argv){
    ifstream fin(root_dir + "input.txt");
    
    fin >> totalEventNum >> timeQuantum >> vmemSize >> pmemSize >> pageSize >> feedFeeq >> feedSize;
    
    while (totalEventNum >= 0){
        string temp;
        getline(fin, temp);
        istringstream iss(temp);
        vector<string> wordsVector{istream_iterator<string>{iss}, istream_iterator<string>{}};
        instruction.push_back(wordsVector);
        totalEventNum--;
    }
    
    for(int i=0; i< instruction.size();i++){
        for(int j=0; j<instruction[i].size();j++){
            cout << instruction[i][j];
        }
        cout << "\n";
    }
    
    cur_pid = -1;
    for( ; cycle < 30 ; cycle++){
        //일정 주기마다의 cycle 분배
        if( cycle == feedCycle){
            for(int i=0;i<3;i++)
                giveCpuCycle(i,feedSize);
            if (cur_pid != -1) cur->cycle = feedSize;
            feedCycle+=feedFeeq;
        }
        else decreaseCpuCycle();
//        ○ Sleep된 프로세스의 검사
//        checkSleepQ(cycle);
//        ○ Input으로 주어진 IO 작업의 시행
        //○ Input으로 주어진 Process 생성 작업의 시행
        if(cycle == stoi(instruction[codeIndex][0]) && instruction[codeIndex][1] != "INPUT"){
            string name = instruction[codeIndex][1];
            Process *nprocess = new Process(total_pid, name);
            nprocess->printProcessStatus();
            process_list.push_back(*nprocess);
            RRQ.push_back(nprocess->pid);
            total_pid++;
            codeIndex++;
        }
        
        //○ 현재 수행중인 Process가 없을 경우 스케줄러에서 수행할 Process 획득 (이 시점에 잔여 time quantum이 없는지 확인)
        if(ctq == timeQuantum) {
            RRQ.push_back(cur->pid);
            cur_pid = -1;
        }
        if(cur_pid  == -1)
            getProcessFromRRQ(feedSize, feedFeeq);
        
        //○ system.txt 정보 출력
        printSystem(cycle);
       
        //○ Process 명령 수행
        if(cur_pid == -1 ) continue;
        
        cur->updateInst();

        ctq++;
        /*
         
         */
    }
}
void decreaseCpuCycle(){
    for(int i= 0 ;  i < RRQ.size() ; i++){
        process_list[RRQ[i]].cycle--;
    }
    for(int i= 0 ;  i < IOQ.size() ; i++){
        process_list[IOQ[i]].cycle--;
    }
    for(int i= 0 ;  i < SleepQ.size() ; i++){
        process_list[SleepQ[i]].cycle--;
    }
}
void getProcessFromRRQ2(){
    for(int i=0 ; i < RRQ.size() ; i++){
        int pid = RRQ[i];
        if(process_list[pid].cycle == 0) {
            continue;
        }
        cur = process_list.begin()+pid;
        cur_pid = pid;
        RRQ.erase(RRQ.begin()+i);
        break;
    }
}
void getProcessFromRRQ(int feedSize, int feedFreq){

    if (RRQ.empty()) return;
    
    getProcessFromRRQ2();
    
    if(cur_pid == -1){
        giveCpuCycle(0, feedSize);
        feedCycle+=feedFreq;
        getProcessFromRRQ2();
    }
    ctq = 0;
}
void giveCpuCycle(int type, int feedSize){
    switch (type) {
        case 0:
            for(int i= 0 ;  i < RRQ.size() ; i++){
                process_list[RRQ[i]].updateCycle(feedSize);
            }
            break;
        case 1:
            for(int i= 0 ;  i < IOQ.size() ; i++){
                process_list[IOQ[i]].updateCycle(feedSize);
            }
            break;
        case 2:
            for(int i= 0 ;  i < SleepQ.size() ; i++){
                process_list[SleepQ[i]].updateCycle(feedSize);
            }
            break;
            
        default:
            break;
    }
}
void printSchedular(int cycle){
    scheout << cycle << "\t" << cur->pid << "\t" << cur->name << " \n";
}
void printSystem(int cycle){
    string temp;
    //Line 1 : CPU cycle, 실행되고 있는 Process의 ID, 실행되고 있는 Code의 정보
    sysout <<cycle << " Cycle: ";
    cout <<cycle << " Cycle: ";
    if(cur_pid != -1) {
        int opcode = cur->getOpcode();
        int argument = cur->getArgument();
        temp = "Process#" +to_string(cur->pid)+" running code "+ cur->name+" line "+ to_string(cur->codeIndex)+"(op "+ to_string(opcode) +", arg" + to_string(argument) +")\n";
    }
    else
        temp = " \n";
    sysout << temp;
    cout << temp;
    //    Line 2 : Run queue(RR queue) 현황
    sysout << "RunQueue: ";
    cout << "RunQueue: ";
    if (RRQ.empty())
        sysout << "Empty\n";
    else{
        for(int i= 0 ;  i < RRQ.size() ; i++){
            temp = to_string(process_list[RRQ[i]].pid) +"(" + process_list[RRQ[i]].name+") ";
            sysout << temp;
            cout << temp;
        }
    }
    //      Line 3 : Sleep List 현황
    sysout << "SleepList: ";
    cout << "SleepList: ";
    if (SleepQ.empty())
        sysout << "Empty\n";
    else{
        for(int i= 0 ;  i < IOQ.size() ; i++){
            temp = to_string(process_list[IOQ[i]].pid) +"(" + process_list[IOQ[i]].name+")\n";
            sysout << temp;
            cout << temp;
        }
    }
    //    Line 4 : IOWait List 현황
    sysout << "IOWait List: ";
    cout << "IOWait List: ";
    if (IOQ.empty())
        sysout << "Empty\n";
    else{
        for(int i = 0 ;  i < SleepQ.size() ; i++){
            temp = to_string(process_list[SleepQ[i]].pid) +"(" + process_list[SleepQ[i]].name+")\n";
            sysout << temp;
            cout << temp;
        }
    }
    cout <<"\n";
    //    Line 5 : 현재 Physical Memory 현황
    //    Line 6 : 현재 Physical Memory LRU 현황
    
}

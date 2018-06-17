
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <iterator>
#include <vector>
#include <fstream>
#include "Process.hpp"
#include "MemorySystem.hpp"


using namespace std;

#define MEMALLOCATION   0
#define MEMACESS        1
#define MEMRELEASE      2
#define NOMEM           3
#define SLEEP           4
#define IOWAITS         5

vector<int> RRQ;
vector<int> IOQ;
vector<int> SleepQ;
vector<Process>::iterator it;
int cur_proc = -1;
vector<Process> process_list;
MemorySystem phymem;

int cycle = 0, codeIndex = 1, total_pid = 0, ctq = 0, feedCycle = 0, opcode = -1, argument = -1;
int totalEventNum, timeQuantum, vmemSize, pmemSize, pageSize, feedFreq, feedSize;
vector< vector <string> > instruction;

//outputfile
string root_dir = "/Users/sujinhan/Desktop/OS/OS_study/";

int find(int pid);
void endProcess(int pid);
void printQueue(int type);
void printSystem(int cycle);
void printProcess(int pid);
void printMemoey(int pid, int allid, int type);


int main(int argc, char ** argv){
    ifstream fin("/Users/sujinhan/Desktop/OS/OS_study/OS_study/input.txt");
    
    fin >> totalEventNum >> timeQuantum >> vmemSize >> pmemSize >> pageSize >> feedFreq >> feedSize;
    
    for(int i=0 ; i < totalEventNum ; i++){
        string temp;
        getline(fin, temp);
        istringstream iss(temp);
        vector<string> wordsVector{istream_iterator<string>{iss},
            istream_iterator<string>{}};
        instruction.push_back(wordsVector);
    }
    /*
    for(int i=0; i< instruction.size();i++){
        for(int j=0; j<instruction[i].size();j++){
            cout << instruction[i][j];
        }
        cout << "\n";
    }*/
    
    phymem = MemorySystem(pmemSize, pageSize);
    
    for(;;cycle++){
        if(cycle == 50){
            cout <<"stop";
        }
        if(codeIndex >= totalEventNum) {
            if(RRQ.empty() && SleepQ.empty() && IOQ.empty())
                break;
        }
        //        ○ 일정 주기마다의 cycle 분배
        if(cycle == feedCycle){
            for(it = process_list.begin() ; it != process_list.end() ; it++){
                it->CpuCycle = feedSize;
            }
            feedCycle += feedFreq;
        }

        //        ○ Sleep된 프로세스의 검사
        for(int i = 0 ; i < SleepQ.size() ; i++){
            int id = SleepQ[i];
            if(cycle == process_list[find(id)].sleepCycle){
                process_list[find(id)].sleepCycle = -1;
                RRQ.push_back(id);
                SleepQ.erase(SleepQ.begin()+i);
                break;
            }
        }
       
        if(codeIndex < totalEventNum){
             //        ○ Input으로 주어진 IO 작업의 시행
            if(stoi(instruction[codeIndex][0]) == cycle && instruction[codeIndex][1] == "INPUT" ){
                int inputpid = stoi(instruction[codeIndex][2]);
                codeIndex++;
                for(int i = 0 ; i < IOQ.size() ; i++){
                    if( inputpid == IOQ[i]){
                        RRQ.push_back(IOQ[i]);
                        IOQ.erase(IOQ.begin()+i);
                        break;
                    }
                }
            }
            //        ○ Input으로 주어진 Process 생성 작업의 시행
            else if(stoi(instruction[codeIndex][0]) == cycle && instruction[codeIndex][1] != "INPUT" ){
                Process np = Process(total_pid, instruction[codeIndex][1], feedSize, vmemSize);
                process_list.push_back(np);
                RRQ.push_back(total_pid);
                total_pid++;
                codeIndex++;
            }
        }
        //        ○ 현재 수행중인 Process가 없을 경우 스케줄러에서 수행할 Process 획득 (이 시점에 잔여 time quantum이 없는지 확인)
        if(cur_proc == -1 && !RRQ.empty()){
            int total = 0;
            for(int i = 0 ; i < RRQ.size() ; i++){
                total+=process_list[find(RRQ[i])].CpuCycle;
            }
            if(total == 0){
                for(int i = 0 ; i < RRQ.size() ; i++){
                    process_list[find(RRQ[i])].CpuCycle = feedSize;
                    feedCycle += feedFreq;
                }
            }
            
            for(int i = 0 ; i < RRQ.size() ; i++){
                if(process_list[find(RRQ[i])].CpuCycle <= 0) {
                    RRQ.push_back(RRQ[i]);
                    RRQ.erase(RRQ.begin() + i);
                    i--;
                    continue;
                }
                if(cycle == 55) {
                    cout<<" ";
                }
                cur_proc = RRQ[i];
                RRQ.erase(RRQ.begin()+i);
                break;
            }
            
            FILE *sc = fopen((root_dir+"scheduler.txt").c_str(), "a");
            fprintf (sc, "%d\t%d\t%s \n " , cycle, cur_proc, process_list[find(cur_proc)].name.c_str());
            fclose(sc);
            ctq = 0;
        }
        
        
        //        ○ system.txt 정보 출력
        printSystem(cycle);
    
        
        //        ○ Process 명령 수행
        if(cur_proc == -1) continue;
        ctq++;
        process_list[find(cur_proc)].CpuCycle--;
        
        int opcode = process_list[find(cur_proc)].getOpcode();
        int argument = process_list[find(cur_proc)].getArgument();
        bool end = process_list[find(cur_proc)].updateIndex();
        vector <pair <int, int> > lru_list;
        vector <pair <int, int> >::iterator ll;
        switch (opcode) {
            case MEMALLOCATION:{
                int allid = process_list[find(cur_proc)].memoryAllocation(cycle, argument);
                lru_list.clear();
                phymem.memoryAllocation(cycle,cur_proc, argument, allid, lru_list);
                //process_list[find(cur_proc)].printVmem(cycle);
                if(!lru_list.empty()){
                    for(ll = lru_list.begin() ; ll!= lru_list.end(); ll++){
                        process_list[find(ll->first)].memoryDeAllocation(cycle, ll->second);
                        printMemoey(ll->first, ll->second, MEMRELEASE);
                    }
                }
                printMemoey(cur_proc, allid, MEMALLOCATION);
                break;
            }
            case MEMRELEASE:{
                bool flag = process_list[find(cur_proc)].memoryDeAllocation(cycle, argument);
                if(flag) phymem.memoryDeAllocation(cycle, cur_proc, argument);
               // process_list[find(cur_proc)].printVmem(cycle);
                printMemoey(cur_proc, argument, MEMRELEASE);
                break;
            }
            case MEMACESS:{
                int pagefaultSize =0;
                process_list[find(cur_proc)].memAcess(cycle, argument, pagefaultSize);
                cout <<pagefaultSize;
                if (pagefaultSize>0){
                    cout << pagefaultSize;
                    lru_list.clear();
                    phymem.memoryAllocation(cycle, cur_proc, pagefaultSize, argument, lru_list);
                    printMemoey(cur_proc, argument, MEMALLOCATION);
                    if(!lru_list.empty()){
                        for(ll = lru_list.begin() ; ll!= lru_list.end(); ll++){
                            process_list[find(ll->first)].memoryDeAllocation(cycle, ll->second);
                            printMemoey(ll->first, ll->second, MEMRELEASE);
                        }
                    }
                }
                
                phymem.memoryAcess(cycle, cur_proc, argument);
                
                printMemoey(cur_proc, argument, MEMACESS);
                break;
            }
            case NOMEM:{
                break;
            }
            case IOWAITS:{
                IOQ.push_back(cur_proc);
                break;
            }
            case SLEEP:{
                SleepQ.push_back(cur_proc);
                process_list[find(cur_proc)].sleepCycle = cycle + argument;
                break;
            }
        }
        
        if(end) {
            endProcess(cur_proc);
        }
        else {
            if(opcode == IOWAITS || opcode == SLEEP) cur_proc = -1;
            if(ctq == timeQuantum && cur_proc != -1) {
                RRQ.push_back(cur_proc);
                cur_proc = -1;
            }
            else if (cur_proc != -1 && process_list[find(cur_proc)].CpuCycle == 0){
                RRQ.push_back(cur_proc);
                cur_proc = -1;
            }
        }
        
 
    }
    

}

int find(int pid){
    for(int i = 0 ; i!= process_list.size() ; i++){
        if(process_list[i].pid == pid)
            return i;
    }
    return -1;
}

void endProcess(int pid){
    //메모리 해제
    process_list[find(pid)].meomoryDeAllocationAll();
    set <int>::iterator itt;
    for(itt = process_list[find(pid)].valid_allid.begin() ; itt != process_list[find(pid)].valid_allid.end() ; itt++){
        phymem.memoryDeAllocation(cycle, pid, *itt);
    }
    //프로세스리스트, RRQ, IOQ, SleepQ에서 프로세스 삭제
    process_list.erase(process_list.begin()+find(pid));
    vector <int>::iterator it;
    for(it = RRQ.begin() ; it != RRQ.end() ; it++){
        if(*it == pid){
            it = RRQ.erase(it);
        }
    }
    for(it = IOQ.begin() ; it != IOQ.end() ; it++){
        if(*it == pid){
            it = IOQ.erase(it);
        }
    }
    for(it = SleepQ.begin() ; it != SleepQ.end() ; it++){
        if(*it == pid){
            it = SleepQ.erase(it);
        }
    }
    
    cur_proc = -1;
}
void printQueue(int type){
    FILE *sy = fopen((root_dir+"system.txt").c_str(), "a");
    switch (type) {
        case 0:
            fprintf(sy, "RunQueue: ");
            cout << "RunQueue: ";
            if (RRQ.empty()){
                fprintf(sy, "Empty\n");
                cout << "Empty\n";
            }
            else{
                for(int i=0 ; i != RRQ.size() ; i++){
                    printf("%d(%s) ", RRQ[i], process_list[find(RRQ[i])].name.c_str());
                    fprintf(sy,"%d(%s) ", RRQ[i], process_list[find(RRQ[i])].name.c_str());
                }
                printf("\n");
                fprintf(sy,"\n");
            }
            
            break;
        case 1:
            cout << "SleepQueue: ";
            fprintf(sy,"SleepQueue: ");
            if (RRQ.empty()){
                cout << "Empty\n";
                fprintf(sy, "Empty\n");
            }
            else{
                for(int i=0 ; i != SleepQ.size() ; i++){
                    printf("%d(%s) ", SleepQ[i], process_list[find(SleepQ[i])].name.c_str());
                    
                    fprintf(sy,"%d(%s) ", SleepQ[i], process_list[find(SleepQ[i])].name.c_str());
                }
                fprintf(sy,"\n");
                cout <<"\n";
            }
            break;
        case 2:
            cout << "IOWait List: ";
            fprintf(sy,"IOWait List: ");
            if (RRQ.empty()){
                cout << "Empty\n";
                fprintf(sy,"Empty\n");
            }
            else{
                for(int i=0 ; i != IOQ.size() ; i++){
                    printf("%d(%s) ", IOQ[i], process_list[find(IOQ[i])].name.c_str());
                    
                    fprintf(sy, "%d(%s) ", IOQ[i], process_list[find(IOQ[i])].name.c_str());
                }
                printf("\n");
                
                fprintf(sy, "\n");
            }
            
            break;
            
        default:
            break;
    }
    fclose(sy);
}

void printSystem(int cycle){
    FILE *sy = fopen((root_dir+"system.txt").c_str(), "a");
    
    string temp;
    //Line 1 : CPU cycle, 실행되고 있는 Process의 ID, 실행되고 있는 Code의 정보
    cout <<cycle << " Cycle: ";
    fprintf(sy, "%d Cycle: ", cycle);
    if(cur_proc != -1) {
        string temp =  process_list[find(cur_proc)].printProcess();
        fprintf(sy, "%s", temp.c_str());
    }
    else
        cout << " \n";
    
    //    Line 2 : Run queue(RR queue) 현황
    printQueue(0);
    //      Line 3 : Sleep List 현황
    printQueue(1);
    //    Line 4 : IOWait List 현황
    printQueue(2);
    //    Line 5 : 현재 Physical Memory 현황
    cout << phymem.printMemoryStatus();
    fprintf(sy, "%s", phymem.printMemoryStatus().c_str());
    //    Line 6 : 현재 Physical Memory LRU 현황
    
    fclose(sy);
}

void printProcess(int pid){
    ofstream fp(process_list[find(pid)].file, ios::ate);
    fp << process_list[find(pid)].printProcessToFile(cycle);
}

void printMemoey(int pid, int allid, int type){
    FILE *fp = fopen("/Users/sujinhan/Desktop/OS/OS_study/memory.txt", "a");
    fprintf (fp, "%d\t%d\t%d\t%d\n " , cycle, pid, allid, type);
    fclose(fp);
}


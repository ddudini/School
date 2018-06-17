
#include "global.h"

void printSystem(){
    string temp;
    //Line 1 : CPU cycle, 실행되고 있는 Process의 ID, 실행되고 있는 Code의 정보
    sysout <<cycle << " Cycle: ";
    if(cur_proc != NULL) {
        temp = "Process#" +to_string(cur_proc->pid)+" running code "+cur_proc->name+" line "+ to_string(cur_proc->codeIndex)+"(op "+ to_string(cur_proc->getCurFirstOperand()) +", arg" + to_string(cur_proc->getCurSecondOperand()) +")\n";
    }
    else
        temp = " \n ";
    sysout << temp;
    //    Line 2 : Run queue(RR queue) 현황
    sysout << "RunQueue: ";
    if (RRQ.empty())
        sysout << "Empty\n";
    else{
        for(it = RRQ.begin() ; it != RRQ.end() ; it++){
            temp = to_string(it->pid) +"(" + it->name+")\n";
            sysout << temp;
        }
    }
    //      Line 3 : Sleep List 현황
    sysout << "SleepList: ";
    if (RRQ.empty())
        sysout << "Empty\n";
    else{
        for(it = SleepQ.begin() ; it != SleepQ.end() ; it++){
            temp = to_string(it->pid) +"(" + it->name+")\n";
            sysout << temp;
        }
    }
    //    Line 4 : IOWait List 현황
    sysout << "IOWait List: ";
    if (RRQ.empty())
        sysout << "Empty\n";
    else{
        for(it = IOQ.begin() ; it != IOQ.end() ; it++){
            temp = to_string(it->pid) +"(" + it->name+")\n";
            sysout << temp;
        }
    }
    //    Line 5 : 현재 Physical Memory 현황
    //    Line 6 : 현재 Physical Memory LRU 현황
}

#include "Process.h"
#include <fstream>
#include <iostream>

#define ROOT_DIR "/Users/sujinhan/Desktop/OS/Assignment3/example/"
void Process::updateCycle(int c){
    cycle = c;
}
void Process::sleep(int cycle, int num){
    sleep_cycle = cycle+num;
}
/*
void Process::readfile(string filename){
    filename = ROOT_DIR + filename + ".txt";
    
    ifstream fin(filename);
    fin >> totalInst;
    
    int operand, next;
    for(int i=0 ; i < totalInst ; i++){
        fin >> operand >> next;
        vector <int> temp;
        temp.push_back(operand);
        temp.push_back(next);
        inst.push_back(temp);
    }
    codeIndex = 0;

};
 */
int Process::getOpcode(){
    if(codeIndex >= totalInst) return -1;
    return inst[codeIndex][0];
}
int Process::getArgument(){
    if(codeIndex >= totalInst) return -1;
    return inst[codeIndex][1];
}
int Process::updateInst(){
    codeIndex+=1;
    return codeIndex;
}

Process::Process() {pid = -1;}
Process::Process(int id_, string f){
    cycle = 0;
    sleep_cycle = -1;
    name = f;
    pid = id_;
    codeIndex = 0;
    string filename = ROOT_DIR + name + ".txt";
    ifstream fin(filename);
    fin >> totalInst;
    
    inst = new int *[10];
    for(int i=0 ; i< totalInst ;i++){
        inst[i] = new int [2];
    }
    
    
    for(int i=0 ; i < totalInst ; i++){
        fin >> inst[i][0] >> inst[i][1];
    }
    fin.close();
    
    //printProcessStatus();
    
};


void Process::printProcessStatus(){
    string temp = "Process ID Created: " + to_string(pid) + " Name: " + name + " CodeIndex/TotalIndex:" + to_string(codeIndex) +"/" +  to_string(totalInst) + "\n";
    cout << temp;
    
    for(int i=0 ; i < totalInst ; i++){
        std::cout << inst[i][0] << " " << inst[i][1] << "\n";
    }
}


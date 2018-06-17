#include "Process.hpp"

Process::Process(int id, string n, int size, int vmemSize){
    pid = id;
    name = n;
    CpuCycle = size;
    sleepCycle = -1;
    index = 0;
    total_allid = 0;
    file = "/Users/sujinhan/Desktop/OS/OS_study/" + to_string(pid) +".txt";
    string filename ="/Users/sujinhan/Desktop/OS/OS_study/OS_study/"+ n+".txt";
    ifstream fin(filename);
    fin >> totalInst;
    opcode = vector <int>(totalInst, -1);
    argument = vector <int>(totalInst, -1);
    for(int i=0 ; i < totalInst ; i++){
        fin >> opcode[i] >> argument[i];
    }
    PTB = vector<PageEntry>(vmemSize, PageEntry(-1, 0));
}

Process::Process(const Process &p){
    
    this->total_allid = p.total_allid;
    this->pid = p.pid;
    this->name = p.name;
    this->totalInst = p.totalInst;
    this->sleepCycle = p.sleepCycle;
    this->CpuCycle = p.CpuCycle;
    this->index = p.index;
    this->opcode.resize((int)(p.opcode.size()));
    this->argument.resize((int)(p.argument.size()));
    this->opcode.assign(p.opcode.begin(), p.opcode.end());
    this->argument.assign(p.argument.begin(), p.argument.end());
    this->PTB.resize((int)(p.PTB.size()));
    this->PTB.assign(p.PTB.begin(), p.PTB.end());
}

string Process::printProcess(){
    printf("Process# %d running code %s line %d (%d , %d)\n", pid, name.c_str(), index+1, opcode[index], argument[index]);
    
    string out;
    out = "Process# "+to_string(pid)+" running code "+name+"line "+to_string(index+1)+" (op "+to_string(opcode[index])+" , arg "+to_string(argument[index])+")\n";
    
    return out;
}


int Process::getOpcode(){
    return opcode[index];
}
int Process::getArgument(){
    return argument[index];
}
bool Process::updateIndex(){
    index++;
    return index >= totalInst ? true : false;
}
string Process::printProcessToFile(int cycle){
    // Line 1
    string ou = to_string(cycle) +" Cycle#Instruction op "+to_string(opcode[index-1])+" arg "+to_string(argument[index-1])+"\n";
    // Line 2
    ou+="AllocID";
    vector <PageEntry>::iterator pe;
    int count = 0;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(count % 4 == 0) ou+="|";
        if(pe->allid != -1) ou+= to_string(pe->allid);
        else ou+="-";
        count++;
    }
    ou+="|\n";
    
    // Line 3
    ou+="Valid  ";
    count = 0;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(count % 4 == 0) ou+="|";
        ou+= to_string(pe->valid);
        count++;
    }
    ou+="|\n";
    
    return ou;
}
void Process::printVmem(int cycle){
    // Line 1
    printf("%d Cycle#Instruction op %d arg %d\n" , cycle, opcode[index-1], argument[index-1]);
    // Line 2
    printf("AllocID" );
    vector <PageEntry>::iterator pe;
    int count = 0;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(count % 4 == 0) printf("|");
        if(pe->allid != -1) printf("%d", pe->allid);
        else printf("-");
        count++;
    }
    printf("|\n" );
    // Line 3
    printf("Valid  " );
    count = 0;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(count % 4 == 0) printf("|");
        printf("%d", pe->valid);
        count++;
    }
    printf("|\n");
        
 }
int Process::memoryAllocation(int cycle, int size){
    vector <PageEntry>::iterator pe;
    vector <PageEntry>::iterator end;
    int size2 = 0;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(pe->allid == -1 && pe->valid == 0){
            size2++;
            
        }
        if(size == size2) {
            end = pe;
            break;
        }
    }
    for(pe = end-size+1 ; pe != end+1 ; pe++){
        pe->allid = total_allid;
        pe->valid = 1;
    }
    total_allid++;
    
    printProcessToFile(cycle);
    return total_allid-1;
    
}

bool Process::memoryDeAllocation(int cycle, int allid){
    bool flag = false;
    vector <PageEntry>::iterator pe;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(pe->allid == allid){
            if (pe->valid == 1) flag = true;
            pe->allid = -1;
            pe->valid = 0;
        }
    }
    
    printProcessToFile(cycle);
    return flag;
}

void Process::memAcess(int cycle, int allid, int &pageFaultSize){
    //printVmem(1);
    
    vector <PageEntry>::iterator pe;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(pe->allid == allid){
            if(pe->valid == 0) {
                pe->valid = 1;
                pageFaultSize++;
            }
            else break;
        }
    }
    
    printProcessToFile(cycle);
}

void Process::meomoryDeAllocationAll(){
    vector <PageEntry>::iterator pe;
    for(pe = PTB.begin() ; pe != PTB.end() ; pe++){
        if(pe->valid == 1){
            valid_allid.insert(pe->allid);
            pe->valid = 0;
            pe->allid = -1;
        }
    }
}














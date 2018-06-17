#include "MemorySystem.h"

MemorySystem::MemorySystem(int pmemSize, int frame_size_){
    total_allid = 0;
    max_order = log2(pmemSize);
    frame_size = frame_size_;
    
    for(int i=0 ; i <= max_order ; i++){
        vector <Frame> temp;
        free_list.push_back(temp);
    }
    free_list[max_order].push_back(Frame(pmemSize, 0, -1));
    
    for(int i=0 ; i < pmemSize / frame_size_ ; i++){
        Frame temp = Frame(frame_size_, i, -1);
        memory.push_back(temp);
    }
}
void MemorySystem::printFreeList(){
    cout << "Free Frame List:\n";
    for(int i = 0; i <= max_order ; i++){
        for(int j = 0 ; j < free_list[i].size() ; j++){
            free_list[i][j].printFrame();
        }
    }
}
string MemorySystem::printMemotyStatus(){
    //printFreeList();
    string outt = "|";
    
    vector<Frame>::iterator it;
    for(it = memory.begin() ; it != memory.end()-1 ; it++){
        if(it->allid == -1){
            outt += "--- ";
        }
        else {
            outt += to_string(it->pid) + "#" + to_string(it->allid) + " ";
        }
    }
    
    if(it->allid == -1){
        outt += "---|";
    }
    else {
        outt += to_string(it->pid) + "#" + to_string(it->allid) + "|";
    }
    outt += "\n";
    return outt;
}
int MemorySystem::findSize(int size){
    return ceil(log2(size));
}
bool operator <(const Frame &a, const Frame &b){ //백터 오름차순 정렬!! 5, 4, 3, 2, 1..
    return a.address > b.address;
}
void MemorySystem::memoryAllocation2(int pid, int address, int size){
    for(int i=0 ; i <size+address; i++){
        memory[i].allid = total_allid;
        memory[i].pid = pid;
    }
    total_allid++;
}
int MemorySystem::memoryAllocation(int pid, int pageSize){
    int demandingSize = pageSize * frame_size;
    int index = findSize(demandingSize);
    cout << index;
    int index2 = index;
    if(free_list[index].empty()){
        for(index2 = index ; index2 <= max_order ; index2++){
            if(!free_list[index2].empty()) break;
        }
    }
    cout<<index2;
    sort(free_list[index2].begin(), free_list[index2].end());
    free_list[index2].back().printFrame();
    memoryAllocation2(pid, free_list[index2].back().address, demandingSize);
    free_list[index2].pop_back();
    
    //블락 나누면서 생긴 빈 프레임들 할당.
    int size = pow(2, index);
    int add = free_list[index2].back().address + size;
    for(int i = index ; i < index2 ; i++){
        Frame free_frame = Frame(size, add, -1);
        free_list[i].push_back(free_frame);
        add +=size;
        size *=2;
        cout << i ;
        free_frame.printFrame();
    }
   printFreeList();
    
    return total_allid-1;
}







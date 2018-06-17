#include "MemorySystem.hpp"
MemorySystem::MemorySystem(int pmemSize, int frame_size_){
    pmem_size = pmemSize;
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
    
    memory[pmemSize-1].printflag = false;
    
}



void MemorySystem::printFreeList(){
    cout << "Free Frame List:\n";
    for(int i = 0; i <= max_order ; i++){
        cout << i << "::";
        for(int j = 0 ; j < free_list[i].size() ; j++){
            free_list[i][j].printFrame();
        }
        cout << "\n";
    }
}
string MemorySystem::printMemoryStatus(){
    string outt = "|";
    
    vector<Frame>::iterator it;
    for(it = memory.begin() ; it != memory.end() ; it++){
        if(it->allid == -1){
            if(it->printflag) outt += "---|";
            else outt += "--- ";
        }
        else {
            if(it->printflag) outt += to_string(it->pid) + "#" + to_string(it->allid) + "|";
            else outt += to_string(it->pid) + "#" + to_string(it->allid) + " ";
        }
    }
    outt+="\nLRU:";
    for(int i=LRU.size()-1 ; i>=0 ; i--){
        outt+= " (" + to_string(LRU[i].first) + ":" + to_string(LRU[i].second) + ")";
    }
    outt+="\n\n";

    return outt;
}


int MemorySystem::findSize(int size){
    return ceil(log2(size));
}

bool operator <(const Frame &a, const Frame &b){ //백터 오름차순 정렬!! 5, 4, 3, 2, 1..
    return a.address > b.address;
}

void MemorySystem::memoryAllocation2(int cycle, int pid, int allid, int address, int size){
    memory[address+size-1].printflag = true;
    
    memoryAcess(cycle, pid, allid);
    
    for(int i=address ; i <size+address; i++){
        memory[i].allid = allid;
        memory[i].pid = pid;
    }
}

void MemorySystem::memoryAllocation(int cycle, int pid, int pageSize, int allid, vector <pair <int, int> > &lrulist){
    if (pageSize == 0){
        cout<<"Page Size 0 required";
        return;
    }
    
    int demandingSize = pageSize * frame_size;
    int index = findSize(demandingSize);
    int demandingframeSize = pow(2, index);
    
    int index2 = index;

    bool allocated = false;
    while (!allocated){
        for(index2 = index ; index2 <= max_order ; index2++){
            if (free_list[index2].empty()) continue;
            if(!free_list[index2].empty()) break;
            }
    
        if(index2 == max_order+1){
            memoryDeAllocationByLRU(cycle, lrulist);
            continue;
        }
        
        sort(free_list[index2].begin(), free_list[index2].end());
        int add = free_list[index2].back().address;
        memoryAllocation2(cycle, pid, allid, add, demandingframeSize);
        free_list[index2].pop_back();
        
        //블락 나누면서 생긴 빈 프레임들 할당
        add+=demandingframeSize;
        for(int i = index ; i < index2 ; i++){
            Frame free_frame = Frame(demandingframeSize, add, -1);
            free_list[i].push_back(free_frame);
            add += demandingframeSize;
            demandingframeSize *=2;
            memory[add-1].printflag = true;
        }
        allocated = true;
    }
}

void MemorySystem::memoryAcess(int cycle, int id, int aid){
    pair <int, int> temp = make_pair(id, aid);
    
    int i=0;
    for(; i != LRU.size() ; i++){
        if(LRU[i] == temp){
            LRU.erase(LRU.begin() + i);
            break;
        }
    }

    LRU.insert(LRU.begin(), temp);
}
bool MemorySystem::isBuddy(int num){
    return pmem_size % num == 0 ? true : false;
}
void MemorySystem::memoryDeAllocation(int cycle, int pid, int allid){
    int size = 0;
    int add = -1;
    for(int i=0 ; i < memory.size() ; i++){
        if(memory[i].pid == pid && memory[i].allid == allid){
            add = add == -1? memory[i].address : add;
            size++;
            memory[i].pid = -1;
            memory[i].allid = -1;
        }
    }
    memory[add + size -1].printflag = false;
    int index = findSize(size);
    free_list[index].push_back(Frame(size, add, -1));
    
    for(int i=0 ; i < free_list[index].size() ; i++){
        if( i > max_order) break;
        if(add + size == free_list[index][i].address && isBuddy(add+2*size) ){
            free_list[index].erase(free_list[index].begin()+i-1, free_list[index].begin()+i);
            index++;
            size *=2;
            if(add+size-1 != pmem_size-1 )memory[add+size-1].printflag = false;
            free_list[index].push_back(Frame(add, size, -1));
            i=0;
        }
        else if( add - size == free_list[index][i].address && isBuddy(add+size )){
            free_list[index].erase(free_list[index].begin()+i, free_list[index].begin()+i+1);
            index++;
            add = add-size;
            size *= 2;
             if(add+size-1 != pmem_size-1 ) memory[size+add-1].printflag = false;
            free_list[index].push_back(Frame(size, add, -1));
            i=0;
        }
    }
    //buddy끼리 합치기.
}

void MemorySystem::memoryDeAllocationByLRU(int cycle, vector <pair <int, int> > &lrulist){
    int lru_pid = LRU.back().first;
    int lru_allid = LRU.back().second;
    lrulist.push_back(make_pair(lru_pid, lru_allid));
    LRU.pop_back();
    
    //deleteFromLru(lru_pid, lru_allid);
    memoryDeAllocation(cycle, lru_pid, lru_allid);
}

void MemorySystem::deleteFromLru(int pid, int allid){
    pair <int, int> temp = make_pair(pid, allid);
    
    for(int i = 0 ; i != LRU.size() ; i++){
        if(LRU[i] == temp){
            LRU.erase(LRU.begin() + i);
            break;
        }
    }
}








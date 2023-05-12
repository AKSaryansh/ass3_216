#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <bits/stdc++.h>
#include <iomanip>
#include <ios>

using namespace std;

struct Block{
    long long unsigned int id ; 
    bool modified; 
    bool invalid;
};


struct Cache{



long long unsigned int  blocksize; 
long long unsigned int  L1size;
long long unsigned int  L1ways; 
long long unsigned int  L2size; 
long long unsigned int  L2ways; 
long long unsigned int total_access_time ;
long long unsigned int L1_read;
long long unsigned int L1_readmiss;
long long unsigned int L2_read;
long long unsigned int L2_readmiss;
long long unsigned int L1_write;
long long unsigned int L1_writemiss;
long long unsigned int L2_write;
long long unsigned int L2_writemiss;
long long unsigned int L1_writebacks ;
long long unsigned int L2_writebacks ;
long long unsigned int  L1sets ;
long long unsigned int  L2sets ;
queue<Block> L1_cache[1024];
queue<Block> L2_cache[1024];
// vector<Block> memory ;


Cache(long long unsigned int  blocksizex,
long long unsigned int L1sizex,
long long unsigned int L1waysx,
long long unsigned int L2sizex,
long long unsigned int L2waysx 
){
    blocksize = blocksizex; 
    L1size = L1sizex ;
    L2size = L2sizex ;
    L1ways = L1waysx ;
    L2ways = L2waysx ;
    total_access_time = 0 ; 
    L1_read = 0 ; 
    L2_read = 0 ; 
    L1_readmiss = 0 ; 
    L2_readmiss = 0 ;
    L1_write = 0 ;
    L2_write = 0 ;
    L1_writemiss = 0 ;
    L2_writemiss = 0 ;
    L1sets = L1size / (L1ways * blocksize);
    L2sets = L2size / (L2ways * blocksize);
    // L1_cache = new queue<Block>[L1sets] ; 
    // L2_cache = new queue<Block>[L2sets] ; 

    // initialising vector 
    long long unsigned int  blah = log2(blocksize);
    long long unsigned int  blah2 = 64-blah;
    long long unsigned int  N = (1<<blah2);
    // for (long long unsigned int i = 0 ; i<N ; i++){
    //     // cout <<"hi"<<"\n" ;
    //     Block b ; 
    //     memory.push_back(b);
    //     memory[i].id = i ;
    //     memory[i].modified = false ;
    //     memory[i].invalid = false ;
    // }
    // queue<Block> L1_cache[L1sets];
    // queue<Block> L2_cache[L2sets];
    // vector<Block> memory ;


}


// long long unsigned int const L1sets = L1size / (L1ways * blocksize);
// long long unsigned int const L2sets = L2size / (L2ways * blocksize);
// queue<Block> L1_cache[L1sets];
// queue<Block> L2_cache[L2sets];
// vector<Block> memory ;


void update_memory(Block b){
    return ; 
    // b.modified = false ; 
    // b.invalid = false ; 
    // memory[b.id]= b ;  
}

long long unsigned int compute_address(string a){
    return stoi(a,0,16);
}

bool present(queue<Block> q, long long unsigned int id) {
    while (!q.empty()) {
        Block b = q.front();
        q.pop();
        if (b.id == id) {
            return true;
        }
    }
    return false;
}



Block find_and_remove(queue<Block> q, long long unsigned int id,long long unsigned int set_number,bool is_L2) {
    Block result;
    long long unsigned int found = 0;
    long long unsigned int size = q.size();
    queue<Block> new1;
    for (long long unsigned int i = 0; i < size; i++) {
        Block b = q.front();
        q.pop();
        if (b.id == id && found == 0) {
            result = b;
            found = 1;
        }
        else {
            new1.push(b);
        }
    }
    if (is_L2){
        L2_cache[set_number] = new1;
    }
    else{
        L1_cache[set_number] = new1;
    }
    
    if (found == 1) {
        return result;
    }
    else {
        return Block();
    } 
}



void update_L2(long long unsigned int id, Block b){
    long long unsigned int setnumber = b.id%L2sets;  
    // cout<<"look"<<endl;
    // printq(L2_cache[setnumber]);
    find_and_remove(L2_cache[setnumber], b.id, b.id%L2sets, true);
    // printq(L2_cache[setnumber]);
    // cout<<"look"<<endl ;
    L2_cache[setnumber].push(b);
    // printq(L2_cache[setnumber]);
    // cout <<b.id<<" "<<b.invalid<<endl ;
}

bool remove_L1(long long unsigned int id, Block b){
    long long unsigned int setnumber = id%L1sets; 
    if (present(L1_cache[setnumber], id)){
        Block b = find_and_remove(L1_cache[setnumber], b.id, b.id%L1sets, false);
        if(b.modified == true ){
            L1_writebacks++;
            update_L2(id, b ); 
            return true ;
        } 
    }
    return false ; 
}


void remove_L2(long long unsigned int id, Block rem){
    long long unsigned int setnumber = rem.id%L2sets ; 
    find_and_remove(L2_cache[setnumber], rem.id, setnumber , true);
}



void put_L2(long long unsigned int id, Block b){
    long long unsigned int setnumber = id%L2sets; 
    queue<Block> currq = L2_cache[setnumber]; 
    if (currq.size()==L2ways){
        Block removed = currq.front();
        currq.pop(); 
        // if(removed.invalid){
        //     remove_L1(removed.id, removed, true);
        // }
        bool f = remove_L1(removed.id, removed );
        if (f) {
            L2_writebacks++; 
            remove_L2(removed.id , removed);
        }
        else if (removed.modified){
            L2_writebacks++; 
            remove_L2(removed.id, removed);
        }
        else remove_L2(removed.id,  removed);
        // if (removed.modified){
        //     update_memory(removed) ;
        //     L2_writebacks++;
        //     remove_L1(removed.id, removed, false);
        // }
        // else { 
        //     remove_L1(removed.id, removed, false);
        // }
        currq.push(b);
        L2_cache[setnumber]= currq ; 
        // return ; 
    }
    else L2_cache[setnumber].push(b); 
}



void put_L1(long long unsigned int id, Block b){
    long long unsigned int setnumber = id%L1sets; 
    queue<Block> currq = L1_cache[setnumber]; 
    if (currq.size()==L1ways ){
        Block removed = currq.front();
        currq.pop(); 
        if (removed.modified == true){
            // cout <<"correctly updated inL2"<<endl;
            L2_write++;
            L1_writebacks++;
            update_L2(removed.id, removed);
        }
        currq.push(b);
        L1_cache[setnumber] = currq ; 
        return ; 
    }
    else L1_cache[setnumber].push(b); 
}


//if present, then put the block in L1. else, read from 
void read_L2(long long unsigned int id){
    // total_access_time+=20;
    long long unsigned int setnumber = id%L2sets; 
    if (present(L2_cache[setnumber], id)){
        L2_read++;
        Block b = find_and_remove(L2_cache[setnumber], id, setnumber, true);
        L2_cache[setnumber].push(b);
        b.modified = false ; 
        put_L1(b.id, b);      
        return ; 
    }
    else {
        // total_access_time+=200;
        L2_readmiss++;
        // Block newblock = memory[id];
        Block newblock ; 
        newblock.id = id ; 
        newblock.modified = false ; 
        put_L2(id, newblock);
        L2_read++;
        put_L1(id, newblock);  
        // read_L2(id);
    }
}


//this function processes the read request. if the id is present in the corresp set, then read hit. if not, then read_L2 and then read_L1
void read_L1(long long unsigned int id){
    long long unsigned int setnumber = id%L1sets;  
    if(present(L1_cache[setnumber], id)){
        // total_access_time+=1;
        Block b = find_and_remove(L1_cache[setnumber], id, setnumber, false );
        L1_cache[setnumber].push(b);
        L1_read++;
        return ; 
    }
    else {
        // total_access_time+=1;
        L1_readmiss++;
        read_L2(id);
        L1_read++;
    }
}

// void modify_and_invalidate_L2(long long unsigned int id){
//     long long unsigned int setnumber = id%L2sets; 
//     queue<Block> hey ; 
//     while (!L2_cache[setnumber].empty()) {
//         Block b = L2_cache[setnumber].front();
//         L2_cache[setnumber].pop();  
//         if (b.id == id) {
//             b.invalid = true ;
//             b.modified = true ;      
//         }
//         hey.push(b);
//     }
//     L2_cache[setnumber]=hey ; 
// }


//this function is used when we write in L1. send a signal to L2 to indicate its copy is now invalid.
// void modify_and_invalidate_L2(long long unsigned int id){
//     long long unsigned int setnumber = id%L2sets; 
//     Block b = find_and_remove(L2_cache[setnumber], id, setnumber, true);
//     b.invalid = true ; 
//     b.modified = true ; 
//     L2_cache[setnumber].push(b);
// }


//this function processes the write request. if the required block present in L1, then L1_write++. also, send invalidate signal to L2's copy of that block
void write_L1(long long unsigned int id){
    // total_access_time+=1;
    long long unsigned int setnumber = id%L1sets; 
    if (present(L1_cache[setnumber], id)){
        Block b = find_and_remove(L1_cache[setnumber], id, setnumber , false);
        b.modified = true;
        // b.invalid = false ; 
        // modify_and_invalidate_L2(b.id);
        L1_cache[setnumber].push(b);
        L1_write++; 
        return ; 
    }
    else {
        L1_writemiss++;
        read_L2(id); 
        write_L1(id);
        return ; 
    }
}





};


vector<string> tokenize(string str, char delim) {
    vector<string> vi ; long long unsigned int  length = 0 ; 
    for (int i = 0 ; i < str.size() ; i ++){
        if(str[i] == delim){
            vi.push_back( str.substr(i-length, length)) ; 
            length  =0 ; 
        }
        else{length ++ ; }
    }  
    vi.push_back(str.substr(str.size()  - length , length) ) ; 
    return vi ; 
}



int main(int argc, char* argv[]){
    
    long long unsigned int blksize = stoul(argv[1],nullptr,10) ; 
    long long unsigned int l1_size = stoul(argv[2], nullptr, 10) ; 
    long long unsigned int l1_assoc = stoul(argv[3], nullptr, 10) ; 
   
    long long unsigned int l2_size = stoul(argv[4], nullptr, 10) ; 
    long long unsigned int l2_assoc = stoul(argv[5], nullptr, 10) ;
 
    string filename = argv[6] ; 

    // cout <<"done"<<"\n" ;
    Cache mycache = Cache(blksize, l1_size, l1_assoc, l2_size, l2_assoc) ; 
    // if (argc != 2){
    //     std::cerr << "Required argument: file_name\n";
	// 	return 0;
    // }
   
   string line ;  string word;
    ifstream myfile (filename);   vector<string> row ; 
    
        if (myfile.is_open())
        {
            while ( getline (myfile,line) ) {
                // cout << line << "\n" ; 
                // stringstream ss(line);
                
                // while (getline(ss, word, '\t')) {
                //     // cout << word ;
                //     row.push_back(word);
                // }
                row = tokenize(line, '\t') ; 
                // cout << "hey" << "\n" ; 
                // cout << row[0]  << " "<< row[1] << "\n";  
                long long unsigned int address = stoul(row[1], nullptr, 16);
        long long unsigned int id = address/blksize ;
                if (row[0] == "r") {
                    // cout << "read" << "\n" ; 
                    mycache.read_L1(id) ;  
                } 
                else if (row[0] == "w"){
                    // cout << "write" << "\n" ; 
                    mycache.write_L1(id) ;  
                }
                else{
                    cout << "failed" << "\n" ; 
                }

            } ; 


    // while (getline(file, line)) {
    //     vector<string> row;
        // stringstream ss(line);
        // string word;
        // while (getline(ss, word, '\t')) {
        //     row.push_back(word);
        // }
    //     long long unsigned int address = compute_address(row[1]);
    //     long long unsigned int id = address/blocksize ; 
    //     // long long unsigned int id = stoi(row[1]);
    //     string a = row[0];
    //     char operation = a[0];
    //     if (operation == 'w'){
    //         write_L1(id);
    //     }
    //     else{
    //         read_L1(id);
    //     }
    // } 

    long long unsigned int L1_accestime = 1 ; 
    long long unsigned int L2_accestime = 20 ; 
    long long unsigned int dram_accestime = 200 ; 
    // long long unsigned int total_access_time = (mycache.L1_read + mycache.L1_write)*1 + (mycache.L2_read + mycache.L2_write)*20 + (L2_writebacks)*200;

    cout <<"===== Simulation Results ====="<<"\n" ;
    cout<<"i. Number of L1 reads:                       "<<mycache.L1_read<<"\n";
    cout<<"ii. Number of L1 read misses:                "<<mycache.L1_readmiss<<"\n";
    cout<<"iii. Number of L1 writes:                    "<<mycache.L1_write<<"\n";
    cout<<"iv. Number of L1 write misses:               "<<mycache.L1_writemiss<<"\n";
    cout<<"v. L1 miss rate:                             "<<float(mycache.L1_readmiss + mycache.L1_writemiss)/float(mycache.L1_read + mycache.L1_write)<<"\n";
    cout<<"vi. Number of writebacks from L1 memory:     "<<mycache.L1_writebacks<<"\n";
    cout<<"vii. Number of L2 reads:                     "<<mycache.L2_read<<"\n";
    cout<<"viii. Number of L2 read misses:              "<<mycache.L2_readmiss<<"\n";
    cout<<"ix. Number of L2 writes:                     "<<mycache.L2_write<<"\n";
    cout<<"x. Number of L2 write misses:                "<<mycache.L2_writemiss<<"\n";
    cout<<"xi. L2 miss rate:                            "<<float(mycache.L2_readmiss + mycache.L2_writemiss)/float(mycache.L2_read + mycache.L2_write)<<"\n";
    cout<<"xii. Number of writebacks from L2 memory :   "<<mycache.L2_writebacks<<"\n";
    long long unsigned int L1_accesstime = 1 ;
    long long unsigned int L2_accesstime = 20 ;
    long long unsigned int dram_accesstime = 200 ;
    long long unsigned int total_access_time = ((mycache.L1_read+ mycache.L1_write)*L1_accesstime) + ((mycache.L2_read+ mycache.L2_write)*L2_accesstime)
                                        + ((mycache.L2_writebacks+ mycache.L2_readmiss)*dram_accesstime);


    cout <<"===== Simulation End ====="<<"\n" ;
    cout<<"[ Total Access Time: "<<total_access_time<<" ns ]";
    return 0 ; 
}
}
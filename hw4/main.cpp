#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <bitset>
#include <cmath>
#include <queue>
#include <algorithm>

using namespace std;

unsigned long long shex2lldec(string str){
    str = str.substr(2);
    unsigned long long num = strtoull(str.c_str(), NULL, 16);
    return num;
}

int sbin2idec(string str){
    int result = strtol(str.c_str(), 0, 2);
    return result;
}

int get_exponent(int num){
    int result = 0;
    while(num != 1){
        num = num/2;
        result++;
    }
    return result;
}

int getRandomNumber(int min, int max){
    static const double fraction = 1.0 / (RAND_MAX + 1.0);
    
    return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}



struct Data{
    bool valid = false;
    int tag = 0;
    vector<unsigned long long> block;
    int lru_index = 0;
    bool dirty = false;
};



int main(int argc, char** argv){
    int L1_capacity = 0;
    int L1_associativity = 0;
    int L2_capacity = 0;
    int L2_associativity = 0;
    int block_size = 0; // block size of L1 = block size of L2
    int cache_replacement_policy = 0;
    string trace_file_name = "";

    vector<string> input_arguments;

    for(int i=0; i<argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    for(int i=0; i<input_arguments.size(); i++){
        if(input_arguments[i] == "-c"){
            L2_capacity = stoi(input_arguments[i+1]);
        }
        if(input_arguments[i] == "-a"){
            L2_associativity = stoi(input_arguments[i+1]);
        }
        if(input_arguments[i] == "-b"){
            block_size = stoi(input_arguments[i+1]);
        }
        if(input_arguments[i] == "-lru"){
            cache_replacement_policy = 1;
        }
        if(input_arguments[i] == "-random"){
            cache_replacement_policy = 2;
        }
        if(i == input_arguments.size()-1){
            trace_file_name = input_arguments[i];
        }
    }

    string store_file_name = "" + trace_file_name + "_" + to_string(L2_capacity) + "_" + to_string(L2_associativity) + "_" + to_string(block_size) + ".out";
    // cout << store_file_name << endl;

    

    L1_capacity = L2_capacity/4;
    if(L2_associativity >= 4) L1_associativity = L2_associativity/4;
    else L1_associativity = L2_associativity;

    // cout << "L1_capacity : " << L1_capacity << endl;
    // cout << "L1_associativity : " << L1_associativity << endl;
    // cout << "L1_block_size : " << block_size << endl;
    // cout << "L2_capacity : " << L2_capacity << endl;
    // cout << "L2_associativity : " << L2_associativity << endl;
    // cout << "L2_block_size : " << block_size << endl;
    // cout << "cache_replacement_policy : " << cache_replacement_policy << endl;
    // cout << "trace_file_name : " << trace_file_name << endl << endl;




    /**************************************
        make virtual cache structure
    **************************************/

    int total_address_bit = 64;
    int word_size = 8;
    int num_of_words_in_a_block = block_size/word_size;
    
    int byte_offset_bit = get_exponent(word_size);
    int block_offset_bit = get_exponent(num_of_words_in_a_block);

    int L1_index_bit = 10 + get_exponent(L1_capacity) - get_exponent(block_size) - get_exponent(L1_associativity);
    int L2_index_bit = 10 + get_exponent(L2_capacity) - get_exponent(block_size) - get_exponent(L2_associativity);

    int L1_tag_bit = total_address_bit - L1_index_bit - block_offset_bit - byte_offset_bit;
    int L2_tag_bit = total_address_bit - L2_index_bit - block_offset_bit - byte_offset_bit;

    // cout << "L1_tag_bit : " << L1_tag_bit << endl;
    // cout << "L1_index_bit : " << L1_index_bit << endl;
    // cout << "L2_tag_bit : " << L2_tag_bit << endl;
    // cout << "L2_index_bit : " << L2_index_bit << endl << endl;
    // cout << "block_offset_bit : " << block_offset_bit << endl; 
    
    int L1_way = pow(2, get_exponent(L1_associativity));
    int L2_way = pow(2, get_exponent(L2_associativity));
    
    int L1_set = pow(2, L1_index_bit);
    int L2_set = pow(2, L2_index_bit);

    // cout << "L1_way : " << L1_way << endl;
    // cout << "L1_set : " << L1_set << endl;
    // cout << "L2_way : " << L2_way << endl;
    // cout << "L2_set : " << L2_set << endl << endl;

    // int size = 1 + 1 + num_of_words_in_a_block;

    // ::printf("size : %d\n", size);

    bool valid = false;
    int tag = 0;
    vector<unsigned long long> block;
    block.clear();

    Data d;

    vector<Data> L1_dataset;
    L1_dataset.clear();

    for(int i=0; i<L1_set; i++){
        L1_dataset.push_back(d);
    }

    vector<vector<Data>> L1_cache;
    L1_cache.clear();

    for(int i=0; i<L1_way; i++){
        L1_cache.push_back(L1_dataset);
    }

    vector<Data> L2_dataset;
    L2_dataset.clear();

    for(int i=0; i<L2_set; i++){
        L2_dataset.push_back(d);
    }

    vector<vector<Data>> L2_cache;
    L2_cache.clear();

    for(int i=0; i<L2_way; i++){
        L2_cache.push_back(L2_dataset);
    }




    /**************************************
    **************************************/

    ifstream fin;
    fin.open(trace_file_name);
    string line;
    vector<string> code;
    while(getline(fin, line)){
        code.push_back(line);
    }
    fin.close();
    

    int cnt1 = 0;
    int cnt2 = 0;
    int cnt3 = 0;
    
    int total_access = 0;
    int read_access = 0;
    int write_access = 0;

    int L1_read_miss = 0;
    int L2_read_miss = 0;
    int L1_write_miss = 0;
    int L2_write_miss = 0;

    int L1_clean_eviction = 0;
    int L2_clean_eviction = 0;
    int L1_dirty_eviction = 0;
    int L2_dirty_eviction = 0;

    for(int inst=0; inst<code.size(); inst++){
    // for(int inst=0; inst<3000; inst++){
        string str = code[inst];
        istringstream ss(str);
        vector<string> split_str;
        split_str.clear();
        string buffer;
        while(getline(ss, buffer, ' ')){
            split_str.push_back(buffer);
        }

        int L1_index = 0;
        int L1_tag = 0;
        int L2_index = 0;
        int L2_tag = 0;

        string s_inst = split_str[1];
        // ::printf("str : %s\n", s_inst.c_str());

        unsigned long long ll_inst = shex2lldec(s_inst);
        // ::printf("int : %lld\n", ll_inst);
        // ::printf("hex : 0x%llx\n", ll_inst);
        
        bitset<64> bit(ll_inst);
        string sb_inst = bit.to_string();
        // ::printf("bin : %s\n", sb_inst.c_str());
        
        string inst_L1_index_bit = sb_inst.substr(L1_tag_bit, L1_index_bit);
        L1_index = sbin2idec(inst_L1_index_bit);

        string inst_L1_tag_bit = sb_inst.substr(0, L1_tag_bit);
        L1_tag = sbin2idec(inst_L1_tag_bit);

        string inst_L2_index_bit = sb_inst.substr(L2_tag_bit, L2_index_bit);
        L2_index = sbin2idec(inst_L2_index_bit);
        
        string inst_L2_tag_bit = sb_inst.substr(0, L2_tag_bit);
        L2_tag = sbin2idec(inst_L2_tag_bit);

        // ::printf("L1 tag bit : %s\n", inst_L1_tag_bit.c_str());
        // ::printf("L1 index bit : %s\n", inst_L1_index_bit.c_str());
        // ::printf("L2 tag bit : %s\n", inst_L2_tag_bit.c_str());
        // ::printf("L2 index bit : %s\n", inst_L2_index_bit.c_str());

        // ::printf("L1 index = %d\n", L1_index);
        // ::printf("L1 tag = %d\n", L1_tag);
        // ::printf("L2 index = %d\n", L2_index);
        // ::printf("L2 tag = %d\n", L2_tag);
        

        

        /***********************************************
                        write, read 구현
        ************************************************/
        


        // write part
        if(split_str[0] == "W"){
            write_access++;
            total_access++;

            // 일단 물리 주소에 해당하는 block을 만들어야함.
            // block size에 맞춰서 block을 만듦.
            // 이 때 block은 block_offset_bit를 int로 바꿔서 그 값이 block_index가 되도록 만들어야함!

            string inst_block_index = sb_inst.substr(sb_inst.size() - byte_offset_bit - block_offset_bit, block_offset_bit);
            int block_index = sbin2idec(inst_block_index);
            int pre = block_index;
            int post = num_of_words_in_a_block - block_index;

            vector<unsigned long long> L1_block;
            L1_block.clear();
            for(int i=0; i<pre; i++){
                L1_block.push_back(ll_inst - (pre - i) * word_size);
            }
            for(int i=0; i<post; i++){
                L1_block.push_back(ll_inst + i * word_size);
            }
            
            // for(int i=0; i<L1_block.size(); i++){
            //     ::printf("L1_block[%d] = 0x%llx\n", i, L1_block[i]);
            // }

            vector<unsigned long long> L2_block;
            L2_block.clear();
            for(int i=0; i<pre; i++){
                L2_block.push_back(ll_inst - (pre - i) * word_size);
            }
            for(int i=0; i<post; i++){
                L2_block.push_back(ll_inst + i * word_size);
            }





            // 각 index에 해당하는 Data 구조체가 존재하는지 확인
            // (way 상관없지만 각 way 검사해서 어느 way에 있는지, 어느 way에서부터 없는지 기록하도록) valid로 있는지 없는지 확인
            // 만약 있다면 tag matching까지 해서 한 번에 확인
            
            
            // for L1 cache
            // true : hit, false : miss
            bool L1_state = false;

            bool L1_valid_check = false;
            bool L1_tag_check = false;

            // L1의 hit과 miss를 결정지을 수 있는 경우는 총 세 가지이다
            int L1_case = 0;
            // case1 : L1 valid check == true && L1 tag check == true => L1 hit
            // case2 : L1 valid check == true && L1 tag check == false => L1 miss
            // case3 : L1 valid check == false => L1 miss
            
            for(int i=0; i<L1_cache.size(); i++){ // L1
                if(L1_cache[i][L1_index].valid == true){
                    L1_valid_check = true;
                    break;
                }
                else{
                    L1_valid_check = false;
                }
            }

            if(L1_valid_check){
                for(int i=0; i<L1_cache.size(); i++){
                    if(L1_cache[i][L1_index].tag == L1_tag){
                        L1_tag_check = true;
                        break;
                    }
                    else{
                        L1_tag_check = false;
                    }
                }
            }

            if(L1_valid_check == true && L1_tag_check == true){
                L1_case = 1;
                L1_state = true;
            }
            else if(L1_valid_check == true && L1_tag_check == false){
                L1_case = 2;
                L1_state = false;
            }
            else if(L1_valid_check == false){
                L1_case = 3;
                L1_state = false;
            }

            // for L2 cache
            
            // true : hit, false : miss
            bool L2_state = false;

            bool L2_valid_check = false;
            bool L2_tag_check = false;


            if(L1_state == false){ // L2
                // L2의 hit과 miss를 결정지을 수 있는 경우는 총 세 가지이다
                // case1 : L2 valid check == true && L2 tag check == true => L2 hit
                // case2 : L2 valid check == true && L2 tag check == false => L2 miss
                // case3 : L2 valid check == false => L2 miss
                for(int i=0; i<L2_cache.size(); i++){
                    if(L2_cache[i][L2_index].valid == true){
                        L2_valid_check = true;
                        break;
                    }
                    else{
                        L2_valid_check = false;
                    }
                }

                if(L2_valid_check){
                    for(int i=0; i<L2_cache.size(); i++){
                        if(L2_cache[i][L2_index].tag == L2_tag){
                            L2_tag_check = true;
                            break;
                        }
                        else{
                            L2_tag_check = false;
                        }
                    }
                }

                if(L2_valid_check == true && L2_tag_check == true){
                    L2_state = true;
                }
                else if(L2_valid_check == true && L2_tag_check == false){
                    L2_state = false;
                }
                else if(L2_valid_check == false){
                    L2_state = false;
                }
            }

            int statement = 0;
            // 1 : L1 miss and L2 miss -> L1에 없어서 L2에 확인해봤는데 L2에도 없음
            // 2 : L1 miss and L2 hit -> L1에 없어서 L2에 확인해봤는데 L2에는 있음
            // 3 : L1 hit and L2 hit -> 우리가 원하는 상황
            if(L1_state == false && L2_state == false){
                statement = 1;
            }
            else if(L1_state == false && L2_state == true){
                statement = 2;
            }
            else if(L1_state == true){
                statement = 3;
            }

            
            // 모든 상황 확인 후 L1_miss와 L2_miss 확인

            if(statement == 1){ // L1과 L2 둘 다 miss인 경우
                L1_write_miss++;
                L2_write_miss++;

                if(cache_replacement_policy == 1){ // lru
                    int L1_evict_way = 0;
                    vector<int> buffer;
                    
                    for(int i=0; i<L1_cache.size(); i++){
                        buffer.push_back(L1_cache[i][L1_index].lru_index);
                    }
                    int max = *max_element(buffer.begin(), buffer.end());
                    int min = *min_element(buffer.begin(), buffer.end());

                    if(min != 0){
                        for(int i=0; i<L1_cache.size(); i++){
                            if(L1_cache[i][L1_index].lru_index == min){
                                L1_evict_way = i + 1;
                            }
                        }
                        for(int i=0; i<L1_cache.size(); i++){
                            L1_cache[i][L1_index].lru_index--;
                        }
                        L1_cache[L1_evict_way-1][L1_index].valid = true;
                        L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                        L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                        L1_cache[L1_evict_way-1][L1_index].lru_index = max;
                        L1_dirty_eviction++;
                    }
                    else{
                        L1_evict_way = max + 1;
                        L1_cache[L1_evict_way-1][L1_index].valid = true;
                        L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                        L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                        L1_cache[L1_evict_way-1][L1_index].lru_index = L1_evict_way;
                        L1_clean_eviction++;
                    }
                }
                else{ // random
                    int L1_evict_way = getRandomNumber(1, L1_way);

                    if(L1_cache[L1_evict_way-1][L1_index].dirty == false){
                        L1_clean_eviction++;
                    }
                    else{
                        L1_dirty_eviction++;
                    }
                    L1_cache[L1_evict_way-1][L1_index].valid = true;
                    L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                    L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                    L1_cache[L1_evict_way-1][L1_index].dirty = true;
                }



                if(cache_replacement_policy == 1){ // lru
                    int L2_evict_way = 0;
                    vector<int> buffer;
                    
                    for(int i=0; i<L2_cache.size(); i++){
                        buffer.push_back(L2_cache[i][L2_index].lru_index);
                    }
                    int max = *max_element(buffer.begin(), buffer.end());
                    int min = *min_element(buffer.begin(), buffer.end());

                    if(min != 0){
                        for(int i=0; i<L2_cache.size(); i++){
                            if(L2_cache[i][L2_index].lru_index == min){
                                L2_evict_way = i + 1;
                            }
                        }
                        for(int i=0; i<L2_cache.size(); i++){
                            L2_cache[i][L2_index].lru_index--;
                        }
                        L2_cache[L2_evict_way-1][L2_index].valid = true;
                        L2_cache[L2_evict_way-1][L2_index].tag = L2_tag;
                        L2_cache[L2_evict_way-1][L2_index].block = L2_block;
                        L2_cache[L2_evict_way-1][L2_index].lru_index = max;
                        L2_dirty_eviction++;
                    }
                    else{
                        L2_evict_way = max + 1;
                        L2_cache[L2_evict_way-1][L2_index].valid = true;
                        L2_cache[L2_evict_way-1][L2_index].tag = L2_tag;
                        L2_cache[L2_evict_way-1][L2_index].block = L2_block;
                        L2_cache[L2_evict_way-1][L2_index].lru_index = L2_evict_way;
                        L2_clean_eviction++;
                    }
                }

                else{ // random
                    int L2_evict_way = getRandomNumber(1, L2_way);

                    if(L2_cache[L2_evict_way-1][L2_index].dirty == false){
                        L2_clean_eviction++;
                    }
                    else{
                        L2_dirty_eviction++;
                    }
                    L2_cache[L2_evict_way-1][L2_index].valid = true;
                    L2_cache[L2_evict_way-1][L2_index].tag = L2_tag;
                    L2_cache[L2_evict_way-1][L2_index].block = L2_block;
                    L2_cache[L2_evict_way-1][L2_index].dirty = true;
                }
            }

            else if(statement == 2){ // L1는 miss지만 L2에는 값이 있어서 hit
                L1_write_miss++;
                
                if(cache_replacement_policy == 1){ // lru
                    int L1_evict_way = 0;
                    vector<int> buffer;
                    
                    for(int i=0; i<L1_cache.size(); i++){
                        buffer.push_back(L1_cache[i][L1_index].lru_index);
                    }
                    int max = *max_element(buffer.begin(), buffer.end());
                    int min = *min_element(buffer.begin(), buffer.end());

                    if(min != 0){
                        for(int i=0; i<L1_cache.size(); i++){
                            if(L1_cache[i][L1_index].lru_index == min){
                                L1_evict_way = i + 1;
                            }
                        }
                        for(int i=0; i<L1_cache.size(); i++){
                            L1_cache[i][L1_index].lru_index--;
                        }
                        L1_cache[L1_evict_way-1][L1_index].valid = true;
                        L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                        L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                        L1_cache[L1_evict_way-1][L1_index].lru_index = max;
                        L1_dirty_eviction++;
                    }
                    else{
                        L1_evict_way = max + 1;
                        L1_cache[L1_evict_way-1][L1_index].valid = true;
                        L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                        L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                        L1_cache[L1_evict_way-1][L1_index].lru_index = L1_evict_way;
                        L1_clean_eviction++;
                    }
                }
                else{ // random
                    int L1_evict_way = getRandomNumber(1, L1_way);

                    if(L1_cache[L1_evict_way-1][L1_index].dirty == false){
                        L1_clean_eviction++;
                    }
                    else{
                        L1_dirty_eviction++;
                    }
                    L1_cache[L1_evict_way-1][L1_index].valid = true;
                    L1_cache[L1_evict_way-1][L1_index].tag = L1_tag;
                    L1_cache[L1_evict_way-1][L1_index].block = L1_block;
                    L1_cache[L1_evict_way-1][L1_index].dirty = true;
                }
            }
        }


        

        // read part
        else if(split_str[0] == "R"){
            read_access++;
            total_access++;

                        
            // for L1 cache
            // true : hit, false : miss
            bool L1_state = false;

            bool L1_valid_check = false;
            bool L1_tag_check = false;

            // L1의 hit과 miss를 결정지을 수 있는 경우는 총 세 가지이다
            int L1_case = 0;
            // case1 : L1 valid check == true && L1 tag check == true => L1 hit
            // case2 : L1 valid check == true && L1 tag check == false => L1 miss
            // case3 : L1 valid check == false => L1 miss
            
            for(int i=0; i<L1_cache.size(); i++){ // L1
                if(L1_cache[i][L1_index].valid == true){
                    L1_valid_check = true;
                    break;
                }
                else{
                    L1_valid_check = false;
                }
            }

            if(L1_valid_check){
                for(int i=0; i<L1_cache.size(); i++){
                    if(L1_cache[i][L1_index].tag == L1_tag){
                        L1_tag_check = true;
                        break;
                    }
                    else{
                        L1_tag_check = false;
                    }
                }
            }

            if(L1_valid_check == true && L1_tag_check == true){
                L1_case = 1;
                L1_state = true;
                // cnt1++;
            }
            else if(L1_valid_check == true && L1_tag_check == false){
                L1_case = 2;
                L1_state = false;
                // cnt2++;
            }
            else if(L1_valid_check == false){
                L1_case = 3;
                L1_state = false;
                // cnt3++;
            }

            // for L2 cache
            
            // true : hit, false : miss
            bool L2_state = false;

            bool L2_valid_check = false;
            bool L2_tag_check = false;


            if(L1_state == false){ // L2
                // L2의 hit과 miss를 결정지을 수 있는 경우는 총 세 가지이다
                // case1 : L2 valid check == true && L2 tag check == true => L2 hit
                // case2 : L2 valid check == true && L2 tag check == false => L2 miss
                // case3 : L2 valid check == false => L2 miss
                for(int i=0; i<L2_cache.size(); i++){
                    if(L2_cache[i][L2_index].valid == true){
                        L2_valid_check = true;
                        break;
                    }
                    else{
                        L2_valid_check = false;
                    }
                }

                if(L2_valid_check){
                    for(int i=0; i<L2_cache.size(); i++){
                        if(L2_cache[i][L2_index].tag == L2_tag){
                            L2_tag_check = true;
                            break;
                        }
                        else{
                            L2_tag_check = false;
                        }
                    }
                }

                if(L2_valid_check == true && L2_tag_check == true){
                    L2_state = true;
                }
                else if(L2_valid_check == true && L2_tag_check == false){
                    L2_state = false;
                }
                else if(L2_valid_check == false){
                    L2_state = false;
                }
            }

            int statement = 0;
            // 1 : L1 miss and L2 miss -> L1에 없어서 L2에 확인해봤는데 L2에도 없음
            // 2 : L1 miss and L2 hit -> L1에 없어서 L2에 확인해봤는데 L2에는 있음
            // 3 : L1 hit and L2 hit -> 우리가 원하는 상황
            if(L1_state == false && L2_state == false){
                statement = 1;
                // cnt1++;
                L1_read_miss++;
                L2_read_miss++;
            }
            else if(L1_state == false && L2_state == true){
                statement = 2;
                // cnt2++;
                L1_read_miss++;
            }
            else if(L1_state == true){
                statement = 3;
                // cnt3++;
            }

        }
        
        // ::printf("%d done.\n", inst);
    }

    ::printf("-- General Stats --\n");
    ::printf("L1 capacity: %d\n", L1_capacity);
    ::printf("L1 way: %d\n", L1_way);
    ::printf("L2 capacity: %d\n", L2_capacity);
    ::printf("L2 way: %d\n", L2_way);
    ::printf("Block size: %d\n", block_size);
    ::printf("Total accesses: %d\n", total_access);
    ::printf("Read accesses: %d\n", read_access);
    ::printf("Write accesses: %d\n", write_access);
    ::printf("L1 Read misses: %d\n", L1_read_miss);
    ::printf("L2 Read misses: %d\n", L2_read_miss);
    ::printf("L1 Write misses: %d\n", L1_write_miss);
    ::printf("L2 Write misses: %d\n", L2_write_miss);
    ::printf("L1 Read miss rate: %.2lf%\n", (double)L1_read_miss/(double)read_access*100);
    ::printf("L2 Read miss rate: %.2lf%\n", (double)L2_read_miss/(double)L1_read_miss*100);
    ::printf("L1 Write miss rate: %.2lf%\n", (double)L1_write_miss/(double)write_access*100);
    ::printf("L2 Write miss rate: %.2lf%\n", (double)L2_write_miss/(double)L1_write_miss*100);
    ::printf("L1 clean eviction: %d\n", L1_clean_eviction);
    ::printf("L2 clean eviction: %d\n", L2_clean_eviction);
    ::printf("L1 dirty eviction: %d\n", L1_dirty_eviction);
    ::printf("L2 dirty eviction: %d\n", L2_dirty_eviction);
    // ::printf("%d\n%d\n%d\n", cnt1, cnt2, cnt3);

    ofstream fout;
    fout.open(store_file_name);
    if(fout.is_open()){
        fout << "-- General Stats --" << endl;
        fout << "L1 capacity: " << dec << L1_capacity << endl;
        fout << "L1 way: " << L1_way << endl;
        fout << "L2 capacity: " << L2_capacity << endl;
        fout << "L2 way: " << L2_way << endl;
        fout << "Block size: " << block_size << endl;
        fout << "Total accesses: " << total_access << endl;
        fout << "read accesses: " << read_access << endl;
        fout << "write accesses: " << write_access << endl;
        fout << "L1 Read misses: " << L1_read_miss << endl;
        fout << "L2 Read misses: " << L2_read_miss << endl;
        fout << "L1 Write misses: " << L1_write_miss << endl;
        fout << "L2 Write misses: " << L2_write_miss << endl;
        fout.precision(4);
        fout << "L1 Read miss rate: " << (double)L1_read_miss/(double)read_access*100 << "%" << endl;
        fout << "L2 Read miss rate: " << (double)L2_read_miss/(double)L1_read_miss*100 << "%" << endl;
        fout << "L1 Write miss rate: " << (double)L1_write_miss/(double)write_access*100 << "%" << endl;
        fout << "L2 Write miss rate: " << (double)L2_write_miss/(double)L1_write_miss*100 << "%" << endl;
        fout << "L1 clean eviction: " << L1_clean_eviction << endl;
        fout << "L2 clean eviction: " << L2_clean_eviction << endl;
        fout << "L1 dirty eviction: " << L1_dirty_eviction << endl;
        fout << "L2 dirty eviction: " << L2_dirty_eviction << endl;
    }
    fout.close();
    
    
    
    
    // 확인용 출력 코드
    // for(int i=0; i<L1_cache.size(); i++){
    //     for(int j=0; j<L1_cache[i].size(); j++){
    //         printf("| way[%d] | set[%d] | %s | %d |", i+1, j, L1_cache[i][j].valid ? "true" : "false", L1_cache[i][j].tag);
    //         for(int k=0; k<L1_cache[i][j].block.size(); k++){
    //             printf(" %llu |", L1_cache[i][j].block[k]);
    //         }
    //         printf("\n");
    //     }
    //     printf("\n");
    // }
    // for(int i=0; i<L2_cache.size(); i++){
    //     for(int j=0; j<L2_cache[i].size(); j++){
    //         printf("| way[%d] | set[%d] | %s | %d |", i+1, j, L2_cache[i][j].valid ? "true" : "false", L2_cache[i][j].tag);
    //         for(int k=0; k<L2_cache[i][j].block.size(); k++){
    //             printf(" %llu |", L2_cache[i][j].block[k]);
    //         }
    //         printf("\n");
    //     }
    //     printf("\n");
    // }

    // compile : main -c capacity -a associativity -b block_size -lru 또는 -random trace file
    // 이거 보고서에 꼭 적기 : capacity는 KB 단위만, associativity는 아무 단위 없음, block size는 B 단위만 작성
}
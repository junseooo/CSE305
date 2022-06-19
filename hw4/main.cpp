#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <bitset>
#include <cmath>

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
    
    int L1_set = pow(2, L1_index_bit);
    int L2_set = pow(2, L2_index_bit);

    int L1_way = pow(2, get_exponent(L1_associativity));
    int L2_way = pow(2, get_exponent(L2_associativity));

    cout << "L1_set : " << L1_set << endl;
    cout << "L1_way : " << L1_way << endl;
    cout << "L2_set : " << L2_set << endl;
    cout << "L2_way : " << L2_way << endl << endl;

    int size = 1 + 1 + num_of_words_in_a_block;

    printf("size : %d\n", size);

    unsigned long long L1[L1_way-1][L1_set-1][size-1];
    unsigned long long L2[L2_way-1][L2_set-1][size-1];
    
    int cnt = 0;
    for(int i=0; i<L1_way; i++){
        for(int j=0; j<L1_set; j++){
            for(int k=0; k<size; k++){
                L1[i][j][k] = 0;
                cnt++;
                cout << cnt << " is done" << endl;
            }
        }
    }
    for(int i=0; i<L2_way; i++){
        for(int j=0; j<L2_set; j++){
            for(int k=0; k<size; k++){
                L2[i][j][k] = 0;
            }
        }
    }


    /**************************************
    **************************************/





    // 일단 하나로 코드 짜고 반복문에 넣기!!!

    string str = "W 0x3c8fed220";
    istringstream ss(str);
    vector<string> split_str;
    split_str.clear();
    string buffer;
    while(getline(ss, buffer, ' ')){
        // cout << buffer << endl;
        // printf("%s\n", buffer.c_str());
        split_str.push_back(buffer);
    }

    string s_inst = split_str[1];
    // printf("str : %s\n", s_inst.c_str());

    unsigned long long ll_inst = shex2lldec(s_inst);
    // printf("int : %lld\n", ll_inst);
    // printf("hex : 0x%llx\n", ll_inst);
    
    bitset<64> bit(ll_inst);
    string sb_inst = bit.to_string();
    // printf("bin : %s\n", sb_inst.c_str());
    



    int valid_bit = 0;

    string inst_L1_index_bit = sb_inst.substr(L1_tag_bit, L1_index_bit);
    string inst_L1_tag_bit = sb_inst.substr(0, L1_tag_bit);

    string inst_L2_index_bit = sb_inst.substr(L2_tag_bit, L2_index_bit);
    string inst_L2_tag_bit = sb_inst.substr(0, L2_tag_bit);

    // printf("L1 tag bit : %s\n", inst_L1_tag_bit.c_str());
    // printf("L1 index bit : %s\n", inst_L1_index_bit.c_str());
    // printf("L2 tag bit : %s\n", inst_L2_tag_bit.c_str());
    // printf("L2 index bit : %s\n", inst_L2_index_bit.c_str());

    int L1_index = sbin2idec(inst_L1_index_bit);
    int L1_tag = sbin2idec(inst_L1_tag_bit);
    
    int L2_index = sbin2idec(inst_L2_index_bit);
    int L2_tag = sbin2idec(inst_L2_tag_bit);

    // printf("L1 index = %d\n", L1_index);
    // printf("L1 tag = %d\n", L1_tag);
    // printf("L2 index = %d\n", L2_index);
    // printf("L2 tag = %d\n", L2_tag);
    
    


    /***********************************************
                    write, read 구현
    ************************************************/

    int L1_hit = 0;
    int L1_miss = 0;
    int L2_hit = 0;
    int L2_miss = 0;

    if(split_str[0] == "W"){
        // 먼저 각 way에 각 set에 각 block에 data가 있는지 확인
        bool exist_check = false;
        int a_way_to_be_used = 0;
        for(int i=0; i<L1_way; i++){ // for each way
            if(L1[i][L1_index][0] == 1) exist_check = true;
            else{
                exist_check = false;
                a_way_to_be_used = i;
                break;
            }
        }
        printf("exist_check = %s\n", exist_check ? "true" : "false");
        printf("a_way_to_be_used = %d\n", a_way_to_be_used);
        
        if(exist_check == true){ // no empty block
            if(cache_replacement_policy == 1){ //lru

            }
            else{ // random

            }
        }
        else{ // there is an empty block for any block
            
            // L1
            printf("before : L1[%d][%d][%d] = %lld\n", a_way_to_be_used, L1_index, 0, L1[a_way_to_be_used][L1_index][0]);
            printf("before : L1[%d][%d][%d] = %lld\n", a_way_to_be_used, L1_index, 1, L1[a_way_to_be_used][L1_index][1]);
            int a = 0;
            L1[L1_index][a][a_way_to_be_used] = 1;
            printf("after : L1[%d][%d][%d] = %lld\n", a_way_to_be_used, L1_index, 0, L1[a_way_to_be_used][L1_index][0]);
            printf("after : L1[%d][%d][%d] = %lld\n", a_way_to_be_used, L1_index, 1, L1[a_way_to_be_used][L1_index][1]);

            // printf("before : L1[%d][%d][%d] = %lld\n", L1_index, 1, a_way_to_be_used, L1[L1_index][1][a_way_to_be_used]);
            L1[L1_index][1][a_way_to_be_used] = L1_tag;
            // printf("after : L1[%d][%d][%d] = %lld\n", L1_index, 1, a_way_to_be_used, L1[L1_index][1][a_way_to_be_used]);

            for(int x=0; x<num_of_words_in_a_block; x++){
                // L1[L1_index][2+x][a_way_to_be_used] = ll_inst + 4*x;
                cout << "done" << endl;
            }

            //L2
            // L2[L2_index][0][a_way_to_be_used] = 1;
            // L2[L2_index][1][a_way_to_be_used] = L2_tag;
            // for(int x=0; x<num_of_words_in_a_block; x++){
            //     L2[L2_index][2+x][a_way_to_be_used] = ll_inst + 4*x;
            // }
        }

    }

    else if(split_str[0] == "R"){
        // if there is the data in any blocks

        bool check = false;
        for(int k=0; k<L1_way; k++){ // for each way    
            if(L1[L1_index][0][k] == 1){ // valid bit == 1
                if(L1[L1_index][1][k] == L1_tag){ // tag matching만 되면 무조건 맞는겨?
                    check = true;
                }
                else continue;
            }
            else continue; // valid bit == 0
        }

        if(check == true) L1_hit++;
        else{
            L1_miss++;
            // bool check = false;
            for(int k=0; k<L2_way; k++){ // for each way    
                if(L2[L2_index][0][k] == 1){ // valid bit == 1
                    if(L2[L2_index][1][k] == L2_tag){ // tag matching만 되면 무조건 맞는겨?
                        check = true;
                    }
                    else continue;
                }
                else continue; // valid bit == 0
            }

            if(check == true) L2_hit++;
            else L2_miss++;
        }
    }




    /**********************************************
                        example
    **********************************************/ 

    for(int j=0; j<size; j++){
        printf("L1[%d][%d][%d] = %lld\n", L1_index, j, L1_way-1, L1[L1_index][j][L1_way-1]);
    }
   






    /********************************************
    parsing에 너무 오래 걸리니까 나중에 할 때 풀기
    

    ifstream fin;
    fin.open(trace_file_name);
    string line;
    vector<string> code;
    while(getline(fin, line)){
        code.push_back(line);
    }
    fin.close();
    
    for(int i=0; i<code.size(); i++){
    // for(int i=0; i<3; i++){
        string str = code[i];
        istringstream ss(str);
        vector<string> split_str;
        split_str.clear();
        string buffer;
        while(getline(ss, buffer, ' ')){
            split_str.push_back(buffer);
        }
        if(split_str[0] == "W"){

        }
        else if(split_str[0] == "R"){

        }
    }
    
    // string str = code[0];

    *********************************************/


    // compile : main -c capacity -a associativity -b block_size -lru 또는 -random trace file
    // capacity는 KB 단위만, associativity는 아무 단위 없음, block size는 B 단위만 작성
}
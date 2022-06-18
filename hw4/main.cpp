#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <bitset>

using namespace std;

long long shex2lldec(string str){
    str = str.substr(2);
    long long num = strtoull(str.c_str(), NULL, 16);
    return num;
}

int main(int argc, char** argv){
    int L1_capacity = 0;
    int L1_associativity = 0;
    int L1_block_size = 0;
    int L2_capacity = 0;
    int L2_asscociativity = 0;
    int L2_block_size = 0;
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
            L2_asscociativity = stoi(input_arguments[i+1]);
        }
        if(input_arguments[i] == "-b"){
            L2_block_size = stoi(input_arguments[i+1]);
        }
        if(input_arguments[i] == "-lru"){
            cache_replacement_policy = 1;
        }
        if(input_arguments[i] == "-random"){
            cache_replacement_policy = 2;
        }
        if(i==input_arguments.size()-1){
            trace_file_name = input_arguments[i];
        }
    }

    L1_capacity = L2_capacity/4;
    if(L2_asscociativity >= 4) L1_associativity = L2_asscociativity/4;
    else L1_associativity = L2_asscociativity;
    L1_block_size = L2_block_size;

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


    // 일단 하나로 코드 짜고 반복문에 넣기
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

    long long ll_inst = shex2lldec(s_inst);
    // printf("int : %lld\n", ll_inst);
    // printf("hex : 0x%llx\n", ll_inst);
    
    bitset<32> bit(ll_inst);
    string sb_inst = bit.to_string();
    // printf("bin : %s", sb_inst.c_str());
    
    



    // if(split_str[0] == "W"){

    // }
    // else if(split_str[0] == "R"){

    // }
    

    // cout << "capacity : " << capacity << endl;
    // cout << "associativity : " << asscociativity << endl;
    // cout << "block_size : " << block_size << endl;
    // cout << "cache_replacement_policy : " << cache_replacement_policy << endl;
    // cout << "trace_file_name : " << trace_file_name << endl;



    // compile : main -c capacity -a associativity -b block_size -lru 또는 -random trace file
}
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


class Element{
private:
    int valid_bit;
    string tag_bit;
    int num_of_words_in_a_block;
    vector<long long> block;
public:
    Element();
    Element(int _valid_bit, string _tag_bit, int _num_of_words_in_a_block);
    
    int get_valid_bit();
    string get_tag_bit();

    void set_valid_bit(int valid_bit);
    void set_tag_bit(string tag_bit);
    void set_block_value(long long value);
    
    void get_block();
    
    Element(const Element& rhs){
        this->valid_bit = rhs.valid_bit;
        this->tag_bit = rhs.tag_bit;
        this->num_of_words_in_a_block = rhs.num_of_words_in_a_block;
        this->block = rhs.block;
    }
};
Element::Element(int _valid_bit, string _tag_bit, int _num_of_words_in_a_block){
    valid_bit = _valid_bit;
    tag_bit = _tag_bit;
    num_of_words_in_a_block = _num_of_words_in_a_block;
    block[num_of_words_in_a_block];
}
int Element::get_valid_bit(){
    return valid_bit;
}
void Element::set_valid_bit(int valid_bit){
    this->valid_bit = valid_bit;
}
string Element::get_tag_bit(){
    return tag_bit;
}
void Element::set_tag_bit(string tag_bit){
    this->tag_bit = tag_bit;
}
void Element::set_block_value(long long value){
    for(int i=0; i<this->num_of_words_in_a_block; i++){
        this->block[i] = value + i*4;
    }
}
void Element::get_block(){
    for(int i=0; i<this->num_of_words_in_a_block; i++){
        printf("0x%llx\n", this->block[i]);
    }
}




long long shex2lldec(string str){
    str = str.substr(2);
    long long num = strtoull(str.c_str(), NULL, 16);
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




    /***************************
    make virtual cache structure
    ***************************/

    int byte_offset_bit = 2; // word 단위로 하므로 같은 값 써도 무방
    int num_of_words_in_a_block = block_size/4;
    int block_offset_bit = get_exponent(num_of_words_in_a_block);

    int L1_index_bit = 10 + get_exponent(L1_capacity) - get_exponent(block_size) - get_exponent(L1_associativity);
    int L2_index_bit = 10 + get_exponent(L2_capacity) - get_exponent(block_size) - get_exponent(L2_associativity);

    int L1_tag_bit = 32 - L1_index_bit - block_offset_bit - byte_offset_bit;
    int L2_tag_bit = 32 - L2_index_bit - block_offset_bit - byte_offset_bit;

    // cout << "L1_tag_bit : " << L1_tag_bit << endl;
    // cout << "L1_index_bit : " << L1_index_bit << endl;
    // cout << "L2_tag_bit : " << L2_tag_bit << endl;
    // cout << "L2_index_bit : " << L2_index_bit << endl << endl;
    
    int L1_row = pow(2, L1_index_bit);
    int L2_row = pow(2, L2_index_bit);

    int L1_column = pow(2, get_exponent(L1_associativity));
    int L2_column = pow(2, get_exponent(L2_associativity));

    // cout << "L1_row : " << L1_row << endl;
    // cout << "L1_column : " << L1_column << endl;
    // cout << "L2_row : " << L2_row << endl;
    // cout << "L2_column : " << L2_column << endl;


    /***********************************************
    앞서 구한 row랑 column으로 빈 cache를 만들어 놓고 싶음
    그러고 밑에서 W할 때 빈칸에다가 추가하는 식으로?
    근데 Data 때문에 operator overriding 해야해서 그냥 vector로 다 넣어서 하니까
    vector를 null로 지정한 후 만들려고 했는데 그게 안 됌...

    일단 여기다가 vector<vector<Data>> 형식의 L1, L2 cache를 만드는게 급선무
    ************************************************/



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

    long long ll_inst = shex2lldec(s_inst);
    // printf("int : %lld\n", ll_inst);
    // printf("hex : 0x%llx\n", ll_inst);
    
    bitset<32> bit(ll_inst);
    string sb_inst = bit.to_string();
    printf("bin : %s\n", sb_inst.c_str());
    



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
    int L2_index = sbin2idec(inst_L2_index_bit);

    // printf("L1 index = %d\n", L1_index);
    // printf("L2 index = %d\n", L2_index);
    

    /***********************************************
    여기에 
    ************************************************/


    if(split_str[0] == "W"){
        // 해당 값을 그냥 씀
        // n-way set일 경우 만약 다 찼을 경우, 제일 마지막으로 사용된 값이 없어짐
        
        // 일단 directed map
        Element L1_data(valid_bit, inst_L1_tag_bit, num_of_words_in_a_block);
    }
    else if(split_str[0] == "R"){
        // 일단 읽음
        // 먼저 tag bit가 0이면 miss 후 L2 cache로도 안 감
        // tag bit가 0이 아닐 때
        // hit 이면 hit
        // miss 면 L2 cache로
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
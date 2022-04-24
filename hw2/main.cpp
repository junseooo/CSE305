#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <cstdio>

using namespace std;

string shex2sbin(string shex){
    string sbin = "";
    shex = shex.substr(2);
    for(int i = 0; i<shex.length(); ++i){
        switch(shex[i]){
            case '0': sbin.append ("0000"); break;
            case '1': sbin.append ("0001"); break;
            case '2': sbin.append ("0010"); break;
            case '3': sbin.append ("0011"); break;
            case '4': sbin.append ("0100"); break;
            case '5': sbin.append ("0101"); break;
            case '6': sbin.append ("0110"); break;
            case '7': sbin.append ("0111"); break;
            case '8': sbin.append ("1000"); break;
            case '9': sbin.append ("1001"); break;
            case 'a': sbin.append ("1010"); break;
            case 'b': sbin.append ("1011"); break;
            case 'c': sbin.append ("1100"); break;
            case 'd': sbin.append ("1101"); break;
            case 'e': sbin.append ("1110"); break;
            case 'f': sbin.append ("1111"); break;
        }
    }
    return sbin;
}

string sbin2shex(string str) {
    stringstream ss;
    ss << "0x" << hex << stoll(str, NULL, 2);
    str = ss.str();
    return str;
}

string sdec2shex(string str) {
    stringstream ss;
    ss << "0x" << hex << stoll(str, NULL, 10);
    str = ss.str();
    return str;
}

string zeroExtension(string str){
    if(str.size() == 32) return str;
    else{
        int buffer = 32 - str.size();
        string buffer_str = "";
        for(int i=0; i<buffer; i++){
            buffer_str = buffer_str + "0";
        }
        buffer_str = buffer_str + str;
        return buffer_str;
    }
}

// vector<string> setReg(string str, vector<string> reg){
    // string op = str.substr(0,6);
    // string rs = str.substr(6,11);
    // string rt = str.substr(11,16);
    // string rd = str.substr(16,21);
    // string shamt = str.substr(21,26);
    // string funct = str.substr(26,32);
    // string imm = str.substr(16,32);
    // string tar = str.substr(6,32);

    // op = sbin2shex(op);
    // rs = sbin2shex(rs);
    // rt = sbin2shex(rt);
    // rd = sbin2shex(rd);
    // shamt = sbin2shex(shamt);
    // funct = sbin2shex(funct);
    // imm = sbin2shex(imm);
    // tar = sbin2shex(tar);

    // if(op == "0x0"){ // R format
    //     if(funct == "0x21"){ // addu
            
    //     }
    // }
    

// }

int main(int argc, char** argv){
    

    /******************************************************
    ******************************************************/

    // arguments parsing
    vector<string> input_arguments;

    for(int i=0; i<argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    // data parsing
    int m_option = 0;
    int d_option = 0;
    int n_option = 0;
    string string_sadd;
    string string_eadd;
    string string_num_of_instruction;
    string input_file;
    int sadd;
    int eadd;
    int num_of_instruction;

    for(int i=0; i<input_arguments.size(); i++){
        if(input_arguments[i] == "-m"){
            m_option = 1;
            string buffer_addr = input_arguments[i+1];
            istringstream ss(buffer_addr);
            string buffer;
            vector <string> v;
            while(getline(ss, buffer, ':')){
                v.push_back(buffer);
            }
            string_sadd = v[0];
            string_eadd = v[1];
            string_sadd = string_sadd.substr(2);
            string_eadd = string_eadd.substr(2);            
            sadd = strtol(string_sadd.c_str(), NULL, 16);
            eadd = strtol(string_eadd.c_str(), NULL, 16);
        }

        if(input_arguments[i] == "-d"){
            d_option = 1;
        }
        
        if(input_arguments[i] == "-n"){
            n_option = 1;
            string_num_of_instruction = input_arguments[i+1];
            num_of_instruction = stoi(string_num_of_instruction);
        }
        
        if(i == input_arguments.size() - 1){
            input_file = input_arguments[i];
        }
    }

    // std::cout << "< check options >" << std::endl;
    // std::cout << "sadd : " << sadd << std::endl;
    // std::cout << "eadd : " << eadd << std::endl;
    // std::cout << "num_of_instruction : " << num_of_instruction << std::endl;
    // std::cout << "input_file : " << input_file << std::endl;
    // std::cout << std::endl;


    /******************************************************
    ******************************************************/

    // input file parsing
    ifstream fin;
    fin.open(input_file);
    string line;
    vector<string> code;
    while(getline(fin, line)){
        code.push_back(line);
    }
    fin.close();
    
    // std::cout << "< check code input >" << std::endl;
    // for(auto i : code){
    //     std::cout << i << std::endl;
    // }
    // std::cout << std::endl;


    /******************************************************
    ******************************************************/

    // calculate text and data size
    string shex_tsize = code[0].substr(2);
    string shex_dsize = code[1].substr(2);

    int buf_idec_tsize = (int)strtol(shex_tsize.c_str(), NULL, 16);
    int buf_idec_dsize = (int)strtol(shex_dsize.c_str(), NULL, 16);
    int idec_tsize = buf_idec_tsize/4;
    int idec_dsize = buf_idec_dsize/4;

    // std::cout << "< check text and data size >" << std::endl;
    // std::cout << "idec_tsize : " << idec_tsize << std::endl;
    // std::cout << "idec_dsize : " << idec_dsize << std::endl;
    // std::cout << std::endl;

    // make string hexadecimal code vector
    vector<string> shex_tcode;
    for(int i=2; i<idec_tsize+2; i++){
        shex_tcode.push_back(code[i]);
    }

    // std::cout << "< check shex_tcode >" << std::endl;
    // for(auto i:shex_tcode){
    //     std::cout << i << std::endl;
    // }
    // std::cout << std::endl;

    // make string binary code vector
    vector<string> sbin_tcode;
    for(int i=0; i<shex_tcode.size(); i++){
        string sbin = shex2sbin(shex_tcode[i]);
        string buffer = zeroExtension(sbin);

        sbin_tcode.push_back(buffer);
    }

    std::cout << "< check binary text code >" << std::endl;
    for(auto i : sbin_tcode){
        std::cout << i << std::endl;
    }
    std::cout << std::endl;

    // mapping the address and hexadecimal code
    int tadd = 0x00400000;
    // int dadd = 0x10000000;
    map<int, string> tgroup;
    // map<int, string> dgroup;

    for(int i=0; i<sbin_tcode.size(); i++){
        tgroup.insert({tadd, shex_tcode[i]});
        tadd += 4;
    }

    // std::cout << "< check address >" << std::endl;
    // for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){

    //     std::cout << it->first << " : " << it->second << std::endl;
    // }
    // std::cout << std::endl;


    /******************************************************
    ******************************************************/
    
    
    
    // register condition
    vector<string> reg;
    for(int i=0; i<32; i++){
        reg[i] = "0x0";
    }

    int pc = 0x00400000;
    
    if(d_option == 0){
        std::cout << "Current register values:" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        std::cout << "PC: 0x" << hex << pc << std::endl;
        std::cout << "Registers:" << std::endl;
        for(int i=0; i<reg.size(); i++){
            std::cout << "R" << i << ": " << reg[i] << std::endl;
        }
        std::cout << std::endl;
    }
    // else{
    //     for(int i=0; i<num_of_instruction; i++){ // print each instruction case
    //         std::cout << "Current register values:" << std::endl;
    //         std::cout << "--------------------------------" << std::endl;
    //         setReg(sbin_tcode[i], reg);
    //         std::cout << "PC: 0x" << hex << pc << std::endl;
    //         std::cout << "Registers:" << std::endl;
    //         for(int i=0; i<reg.size(); i++){ // print register vector
    //             std::cout << "R" << i << ": " << reg[i] << std::endl;
    //         }
    //         std::cout << std::endl;
    //         pc+=4;
    //     }
    // }

    // print m option
    if(m_option == 1){
        std::cout << "Memory content [0x" << hex << sadd << "..0x" << hex << eadd << "]:" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        // text data
        if((sadd >= 0x00400000 && sadd < 0x10000000) || sadd >= 0x10000000){
            auto iter = tgroup.find(sadd);
            if(iter == tgroup.end()){
                for(int i=sadd; i<=eadd; i+=4){
                    cout << "0x" << hex << i << ": 0x0" << endl; 
                }
            }
            else{
                int mapping_end_add;
                for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){
                    int idec_add = it->first;
                    // std::cout << idec_add << ": " << it->second << std::endl;
                    if(idec_add > eadd) break;

                    string sdec_add = to_string(idec_add);
                    string shex_add = sdec2shex(sdec_add);
                    std::cout << shex_add << ": " << it->second << std::endl;
                    mapping_end_add = idec_add;
                }
                if(mapping_end_add < eadd){
                    for(int i=mapping_end_add; i<=eadd; i+=4){
                        cout << "0x" << hex << i << ": 0x0" << endl; 
                    }
                }
            }  
        }
    }

    return 0;
}
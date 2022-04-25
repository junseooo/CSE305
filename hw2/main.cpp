#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <cstdio>
#include <bitset>
#include <cmath>

using namespace std;

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

int pc = 0x00400000;
int tadd = 0x00400000;
int dadd = 0x10000000;
map<int, string> tgroup;
map<int, string> dgroup;

vector<string> reg;

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

string zeroExtension(string str, int size){
    string buffer_str = "";
    if(str.size() != 32){
        int buffer = size - str.size();
        for(int i=0; i<buffer; i++){
            buffer_str = buffer_str + "0";
        }
        buffer_str = buffer_str + str;
        return buffer_str;
    }
    else return str;
}

string signExtension(string str, int size){
    string buffer_str = "";
    if(str.size() != 32){
        int buffer = size - str.size();
        for(int i=0; i<buffer; i++){
            buffer_str = buffer_str + str[0];
        }
        buffer_str = buffer_str + str;
        return buffer_str;
    }
    return str;
}

int sbin2signedint(string str){
    int size = str.size();
    int index = size - 1;
    int res=0;

    res = res - (str[0] - '0') * (pow(2, index));
    index -= 1;

    while(1){
        int i=1;
        if(index == -1) break;
        res = res + (str[i] - '0') * (pow(2, index));
        index -= 1;
        i++;
    }
    return res;
}

vector<string> setReg(int cpc, map<int, string> map, vector<string> reg){
    auto iter = map.find(cpc);
    if(iter == map.end()){
        // cout << "there is no correct instruction" << endl;
        return reg;
    }
    else{
        string str = map[cpc];
        // cout << str << endl;
        string op = str.substr(0,6);
        string rs = str.substr(6,5);
        string rt = str.substr(11,5);
        string rd = str.substr(16,5);
        string shamt = str.substr(21,5);
        string funct = str.substr(26,6);
        string imm = str.substr(16,16);
        string tar = str.substr(6,26);

        // cout << "op : " << op << endl;
        // cout << "rs : " << rs << endl;
        // cout << "rt : " << rt << endl;
        // cout << "rd : " << rd << endl;
        // cout << "shamt : " << shamt << endl;
        // cout << "funct : " << funct << endl;
        // cout << "imm : " << imm << endl;
        // cout << "tar : " << tar << endl;
        // cout << endl;

        op = sbin2shex(op);
        funct = sbin2shex(funct);
        imm = sbin2shex(imm);
        tar = sbin2shex(tar);

        // cout << "op : " << op << endl;
        // cout << "rs : " << rs << endl;
        // cout << "rt : " << rt << endl;
        // cout << "rd : " << rd << endl;
        // cout << "shamt : " << shamt << endl;
        // cout << "funct : " << funct << endl;
        // cout << "imm : " << imm << endl;
        // cout << "tar : " << tar << endl;
        // cout << endl;

        int irs = stoi(rs, 0, 2);
        int irt = stoi(rt, 0, 2);
        int ird = stoi(rd, 0, 2);
        int ishamt = stoi(shamt, 0, 2);
        
        int iimm = strtol(imm.c_str(), NULL, 16);
        // int itar = strtol(tar.c_str(), NULL, 26);

        // cout << "int rs index : " << irs << endl;
        // cout << "int rt index : " << irt << endl;
        // cout << "int rd index : " << ird << endl;
        // cout << "int shamt : " << ishamt << endl;
        // cout << "int imm : " << iimm << endl;
        // cout << endl;

        string str_rs_val = reg[irs];
        int int_rs_val = (int)strtol(str_rs_val.c_str(), NULL, 16);

        string str_rt_val = reg[irt];
        int int_rt_val = (int)strtol(str_rt_val.c_str(), NULL, 16);

        if(op == "0x0"){ // R format
            if(funct == "0x21"){ // addu
                unsigned int res = int_rs_val + int_rt_val;
                string shex_res = sdec2shex(to_string(res));

                reg[ird] = shex_res;
                
                pc+=4;
            }
            if(funct == "0x24"){ // and
                bitset<32> res(int_rs_val & int_rt_val);

                string shex_res = sbin2shex(res.to_string());

                reg[ird] = shex_res;
                
                pc+=4;
            }
            if(funct == "0x27"){ // nor
                bitset<32> buf(int_rs_val | int_rt_val);
                bitset<32> res(~buf);

                string shex_res = sbin2shex(res.to_string());

                reg[ird] = shex_res;

                pc+=4;
            }
            if(funct == "0x25"){ // or
                bitset<32> res(int_rs_val | int_rt_val);
                
                string shex_res = sbin2shex(res.to_string());

                reg[ird] = shex_res;

                pc+=4;
            }
            if(funct == "0x2b"){ // sltu            
                if(int_rs_val < int_rt_val) reg[ird] = "0x1";
                else reg[ird] = "0x0";

                pc+=4;
            }
            if(funct == "0x0"){ // sll
                int_rt_val = int_rt_val << ishamt;
                
                string shex_res = sdec2shex(to_string(int_rt_val));

                reg[ird] = shex_res;

                pc+=4;
            }
            if(funct == "0x2"){ // srl
                int_rt_val = int_rt_val >> ishamt;
                
                string shex_res = sdec2shex(to_string(int_rt_val));

                reg[ird] = shex_res;

                pc+=4;
            }
            if(funct == "0x23"){ // subu
                unsigned int res = int_rs_val - int_rt_val;

                string shex_res = sdec2shex(to_string(res));

                reg[ird] = shex_res;

                pc+=4;
            }
            if(funct == "0x8"){ // jr
                pc = int_rs_val;
            }
        }

        // I format
        if(op == "0x9"){ // addiu
            string sbin_imm = shex2sbin(imm);
            sbin_imm = zeroExtension(sbin_imm, 16);
            // cout << sbin_imm << endl;
            if(sbin_imm[0] == '1'){ // minus
                int i_val = sbin2signedint(sbin_imm);
                int res = int_rs_val + i_val;
                string shex_res = sdec2shex(to_string(res));

                reg[irt] = shex_res;

                pc+=4;
            }
            else{
                int res = int_rs_val + iimm;
                string shex_res = sdec2shex(to_string(res));

                reg[irt] = shex_res;

                pc+=4;

            }
        }
        if(op == "0xc"){ // andi
            bitset<32> res(int_rs_val & iimm);

            string shex_res = sbin2shex(res.to_string());

            reg[irt] = shex_res;

            pc+=4;
        }
        if(op == "0x4"){ // beq
            if(int_rs_val == int_rt_val){
                pc+=4;
                // branch
                int int_target_add = pc + iimm * 4;

                pc = int_target_add;
            }
        }
        if(op == "0x5"){ // bne
            pc+=4;
            if(int_rs_val != int_rt_val){
                // branch
                int int_target_add = pc + iimm * 4;

                pc = int_target_add;
            }
        }
        if(op == "0xf"){ // lui
            bitset<16> res(iimm);

            string buffer = res.to_string() + "0000000000000000";
                
            string shex_res = sbin2shex(buffer);

            reg[irt] = shex_res;

            pc+=4;
        }
        if(op == "0x23"){ // lw
            int res = int_rs_val + 4 * iimm;

            string shex_res;

            map = dgroup;

            auto iter = map.find(res);
            if(iter != map.end()){
                string sbin_val = iter->second;
                unsigned long long ull_val = strtoul(sbin_val.c_str(), NULL, 2);
                int i_val = ull_val;
                shex_res = sdec2shex(to_string(i_val));
            }

            reg[irt] = shex_res;

            pc+=4;
        }
        if(op == "0x20"){ // lb
            int res = int_rs_val + 4 * iimm;

            int i_val;

            map = dgroup;

            auto iter = map.find(res);
            if(iter != map.end()){
                string sbin_val = iter->second;
                unsigned long long ull_val = strtoul(sbin_val.c_str(), NULL, 2);
                i_val = ull_val;
            }

            string sbin_val = bitset<32>(i_val).to_string();

            string buffer = sbin_val.substr(24);

            buffer = signExtension(buffer, 32);

            string shex_res = sbin2shex(buffer);

            reg[irt] = shex_res;

            pc+=4;
        }
        if(op == "0xd"){ // ori
            bitset<32> res(int_rs_val | iimm);
                
            string shex_res = sbin2shex(res.to_string());

            reg[irt] = shex_res;

            pc+=4;
        }
        if(op == "0xb"){ // sltiu
            if(int_rs_val < iimm) reg[irt] = "0x1";
            else reg[irt] = "0x0";

            pc+=4;
        }
        if(op == "0x2b"){ // sw
            int target_add = int_rs_val + 4 * iimm;
            
            map = dgroup;

            map.insert({target_add, sdec2shex(to_string(int_rt_val))});

            pc+=4;
        }
        if(op == "0x28"){ // sb
            cout << "sb" << endl;
            cout << "int_rt_val : " << int_rt_val << endl;
            
            string sbin_val = bitset<32>(int_rt_val).to_string();
            cout << "sbin_val : " << sbin_val << endl;

            string buffer = sbin_val.substr(24);
            cout << "buffer : " << buffer << endl;

            buffer = zeroExtension(buffer, 32);
            cout << "buffer : " << buffer << endl;

            // unsigned long long ull_val = strtoul(buffer.c_str(), NULL, 2);
            // int i_val = ull_val;
            // cout << "i_val : " << i_val << endl;

            int target_add = int_rs_val + 4 * iimm;
            cout << "target_add : " << target_add << endl;

            map = dgroup;

            map.insert({target_add, buffer});

            cout << "< check data code address >" << endl;
            auto it = map.begin();
            for(it = map.begin(); it != map.end(); ++it){
                cout << it->first << " : " << it->second << endl;
                // string sbin_code = it->second;
                // unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);

                // cout << dec << it-> first << " : 0x" << hex << ull_code << endl;
            }
            cout << endl;

            pc+=4;

        }
        
        // j format
        if(op == "0x2"){ // j
            tar = tar.substr(2);

            int idec_target_add = (int)strtol(tar.c_str(), NULL, 16);

            int target_add = idec_target_add * 4;

            pc = target_add;
        }
        if(op == "0x3"){ // jal
            pc+=4;
            reg[31] = sdec2shex(to_string(pc)); // $ra store

            tar = tar.substr(2);

            int idec_target_add = (int)strtol(tar.c_str(), NULL, 16);

            int target_add = idec_target_add * 4;

            pc = target_add;
        }
        return reg;
    }
}

/**********************************************************************
***********************************************************************
                            MAIN FUNCTION                            
***********************************************************************
**********************************************************************/

int main(int argc, char** argv){

    // arguments parsing
    vector<string> input_arguments;

    for(int i=0; i<argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    // data parsing
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
            if(input_arguments[i+1].at(0) == '1' || '2')
            string_num_of_instruction = input_arguments[i+1];
            num_of_instruction = stoi(string_num_of_instruction);
        }
        
        if(i == input_arguments.size() - 1){
            input_file = input_arguments[i];
        }
    }

    // cout << "< check options >" << endl;
    // cout << "sadd : " << sadd << endl;
    // cout << "eadd : " << eadd << endl;
    // cout << "num_of_instruction : " << num_of_instruction << endl;
    // cout << "input_file : " << input_file << endl;
    // cout << endl;


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
    
    // cout << "< check code input >" << endl;
    // for(auto i : code){
    //     cout << i << endl;
    // }
    // cout << endl;
    
    // vector<string> reg;
    for(int i=0; i<32; i++){
        reg.push_back("0x0");
    }

    /******************************************************
     * if there is an empty vector code
    ******************************************************/
    if(code.empty()){
        if(n_option == 1){
            cout << "Current register values:" << endl;
                cout << "--------------------------------" << endl;
                cout << "PC: " << sdec2shex(to_string(pc)) << endl;
                cout << "Registers:" << endl;
                for(int i=0; i<reg.size(); i++){
                    cout << "R" << dec << i << ": " << reg[i] << endl;
                }
            cout << endl;
        }
        if(m_option == 1){
            cout << "Memory content [0x" << hex << sadd << "..0x" << hex << eadd << "]:" << endl;
            cout << "--------------------------------" << endl;
            // text data
            if((sadd >= 0x00400000 && sadd < 0x10000000) || sadd >= 0x10000000){
                map<int, string> group;
                if(sadd >= 0x00400000 && sadd < 0x10000000) group = tgroup;
                if(sadd >= 0x10000000) group = dgroup;

                auto iter = group.find(sadd);
                if(iter == group.end()){
                    for(int i=sadd; i<=eadd; i+=4){
                        cout << "0x" << hex << i << ": 0x0" << endl; 
                    }
                }
                else{
                    int mapping_end_add;
                    
                    // 중간에 mapping된 데이터 무시되는거 해결해야함
                    // auto it = group.end();
                    // cout << "last mapping address : " << it -> second << endl;

                    for(map<int, string>::iterator it = group.begin(); it != group.end(); ++it){
                        int idec_add = it->first;
                        // cout << idec_add << ": " << it->second << endl;
                        if(idec_add > eadd) break;

                        string sdec_add = to_string(idec_add);
                        string shex_add = sdec2shex(sdec_add);
                        
                        string sbin_code = it->second;
                        unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);
                        cout << shex_add << ": 0x" << hex << ull_code << endl;
                        mapping_end_add = idec_add;
                    }
                    if(mapping_end_add < eadd){
                        for(int i=mapping_end_add+4; i<=eadd; i+=4){
                            cout << "0x" << hex << i << ": 0x0" << endl;
                        }
                    }
                }
            }
            cout << endl;
        }


        

    }


    

    
    
    /******************************************************
    ******************************************************/

    // calculate text and data size
    string shex_tsize = code[0].substr(2);
    string shex_dsize = code[1].substr(2);

    int buf_idec_tsize = (int)strtol(shex_tsize.c_str(), NULL, 16);
    int buf_idec_dsize = (int)strtol(shex_dsize.c_str(), NULL, 16);
    int idec_tsize = buf_idec_tsize/4;
    int idec_dsize = buf_idec_dsize/4;

    // cout << "< check text and data size >" << endl;
    // cout << "idec_tsize : " << idec_tsize << endl;
    // cout << "idec_dsize : " << idec_dsize << endl;
    // cout << endl;



    // make string hexadecimal code vector
    vector<string> shex_tcode;
    for(int i=2; i<idec_tsize+2; i++){
        shex_tcode.push_back(code[i]);
    }

    // cout << "< check shex_tcode >" << endl;
    // for(auto i:shex_tcode){
    //     cout << i << endl;
    // }
    // cout << endl;

    vector<string> sbin_tcode;
    for(int i=0; i<shex_tcode.size(); i++){
        string sbin = shex2sbin(shex_tcode[i]);
        string buffer = zeroExtension(sbin, 32);

        sbin_tcode.push_back(buffer);
    }
    cout << endl;

    // cout << "< check binary text code >" << endl;
    // for(auto i : sbin_tcode){
    //     cout << i << endl;
    // }
    // cout << endl;

    for(int i=0; i<sbin_tcode.size(); i++){
        tgroup.insert({tadd, sbin_tcode[i]});
        tadd += 4;
    }

    // cout << "< check text code address >" << endl;
    // for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){
    //     string sbin_code = it->second;
    //     unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);

    //     cout << dec << it-> first << " : 0x" << hex << ull_code << endl;
    // }
    // cout << endl;




    // make string hexadecimal code vector
    vector<string> shex_dcode;
    for(int i=2+idec_tsize; i<code.size(); i++){
        shex_dcode.push_back(code[i]);
    }

    // cout << "< check shex_dcode >" << endl;
    // for(auto i:shex_dcode){
    //     cout << i << endl;
    // }
    // cout << endl;

    vector<string> sbin_dcode;
    for(int i=0; i<shex_dcode.size(); i++){
        string sbin = shex2sbin(shex_dcode[i]);
        string buffer = zeroExtension(sbin, 32);

        sbin_dcode.push_back(buffer);
    }

    for(int i=0; i<sbin_dcode.size(); i++){
        dgroup.insert({dadd, sbin_dcode[i]});
        dadd += 4;
    }

    // cout << "< check data code address >" << endl;
    // for(map<int, string>::iterator it = dgroup.begin(); it != dgroup.end(); ++it){
    //     cout << it->first << " : " << it->second << endl;


        // string sbin_code = it->second;
        // unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);

        // cout << dec << it-> first << " : 0x" << hex << ull_code << endl;


    // }
    // cout << endl;

    




    /******************************************************
    ******************************************************/

    if(d_option == 0){ // 어차피 한 번만 출력하면 돼서 m option 따로 빼도 괜찮음
        // set register part
        if(n_option == 1){ // (d,n) = (0,1)
            while(1){
                int i;
                if(i == num_of_instruction) break;
                auto iter = tgroup.find(pc);
                if(iter->second == "") break;

                reg = setReg(pc, tgroup, reg);
                i++;
            }
        }
        else{ // (d,n) = (0,0)
            while(1){
                auto iter = tgroup.find(pc);
                if(iter->second == "") break;
                
                reg = setReg(pc, tgroup, reg);
            }
        }

        cout << "Current register values:" << endl;
        cout << "--------------------------------" << endl;
        cout << "PC: " << sdec2shex(to_string(pc)) << endl;
        cout << "Registers:" << endl;
        for(int i=0; i<reg.size(); i++){
            cout << "R" << dec << i << ": " << reg[i] << endl;
        }
        cout << endl;

        if(m_option == 1){
            cout << "Memory content [0x" << hex << sadd << "..0x" << hex << eadd << "]:" << endl;
            cout << "--------------------------------" << endl;
            // text data
            if((sadd >= 0x00400000 && sadd < 0x10000000) || sadd >= 0x10000000){
                map<int, string> group;
                if(sadd >= 0x00400000 && sadd < 0x10000000) group = tgroup;
                if(sadd >= 0x10000000) group = dgroup;

                auto iter = group.find(sadd);
                if(iter == group.end()){
                    for(int i=sadd; i<=eadd; i+=4){
                        cout << "0x" << hex << i << ": 0x0" << endl; 
                    }
                }
                else{
                    int mapping_end_add;
                    
                    // 중간에 mapping된 데이터 무시되는거 해결해야함
                    // auto it = group.end();
                    // cout << "last mapping address : " << it -> second << endl;

                    for(map<int, string>::iterator it = group.begin(); it != group.end(); ++it){
                        int idec_add = it->first;
                        // cout << idec_add << ": " << it->second << endl;
                        if(idec_add > eadd) break;

                        string sdec_add = to_string(idec_add);
                        string shex_add = sdec2shex(sdec_add);
                        
                        string sbin_code = it->second;
                        unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);
                        cout << shex_add << ": 0x" << hex << ull_code << endl;
                        mapping_end_add = idec_add;
                    }
                    if(mapping_end_add < eadd){
                        for(int i=mapping_end_add+4; i<=eadd; i+=4){
                            cout << "0x" << hex << i << ": 0x0" << endl;
                        }
                    }
                }
            }
            cout << endl;
        }
    }







    else{ // d_option == 1인 경우. instruction 마다 register, memory 모두 출력해야 함
        if(n_option == 1){ // (d,n) = (1,1).    몇 개의 instruction을 decode할지 정해준 경우
            while(1){
                int i;
                if(i == num_of_instruction) break;
                auto iter = tgroup.find(pc);
                if(iter->second == "") break;

                reg = setReg(pc, tgroup, reg);

                cout << "Current register values:" << endl;
                cout << "--------------------------------" << endl;
                cout << "PC: " << sdec2shex(to_string(pc)) << endl;
                cout << "Registers:" << endl;
                for(int i=0; i<reg.size(); i++){
                    cout << "R" << dec << i << ": " << reg[i] << endl;
                }
                cout << endl;

                if(m_option == 1){
                    cout << "Memory content [0x" << hex << sadd << "..0x" << hex << eadd << "]:" << endl;
                    cout << "--------------------------------" << endl;
                    // text data
                    if((sadd >= 0x00400000 && sadd < 0x10000000) || sadd >= 0x10000000){
                        map<int, string> group;
                        if(sadd >= 0x00400000 && sadd < 0x10000000) group = tgroup;
                        if(sadd >= 0x10000000) group = dgroup;
                        auto iter = group.find(sadd);
                        if(iter == group.end()){
                            for(int i=sadd; i<=eadd; i+=4){
                                cout << "0x" << hex << i << ": 0x0" << endl; 
                            }
                        }
                        else{
                            int mapping_end_add;
                            for(map<int, string>::iterator it = group.begin(); it != group.end(); ++it){
                                int idec_add = it->first;
                                if(idec_add > eadd) break;

                                string sdec_add = to_string(idec_add);
                                string shex_add = sdec2shex(sdec_add);

                                string sbin_code = it->second;
                                unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);
                                cout << shex_add << ": 0x" << hex << ull_code << endl;

                                mapping_end_add = idec_add;
                            }
                            if(mapping_end_add < eadd){
                                for(int i=mapping_end_add+4; i<=eadd; i+=4){
                                    cout << "0x" << hex << i << ": 0x0" << endl; 
                                }
                            }
                        }  
                    }
                    cout << endl;
                }
                i++;
            }
        }






        else{ // (d,n) = (1,0)
            while(1){
                auto iter = tgroup.find(pc);
                if(iter->second == "") break;

                reg = setReg(pc, tgroup, reg);
                cout << "Current register values:" << endl;
                cout << "--------------------------------" << endl;
                cout << "PC: " << sdec2shex(to_string(pc)) << endl;
                cout << "Registers:" << endl;
                for(int i=0; i<reg.size(); i++){
                    cout << "R" << dec << i << ": " << reg[i] << endl;
                }
                cout << endl;

                if(m_option == 1){
                    cout << "Memory content [0x" << hex << sadd << "..0x" << hex << eadd << "]:" << endl;
                    cout << "--------------------------------" << endl;
                    // text data
                    if((sadd >= 0x00400000 && sadd < 0x10000000) || sadd >= 0x10000000){
                        map<int, string> group;
                        if(sadd >= 0x00400000 && sadd < 0x10000000) group = tgroup;
                        if(sadd >= 0x10000000) group = dgroup;
                        auto iter = group.find(sadd);
                        if(iter == group.end()){
                            for(int i=sadd; i<=eadd; i+=4){
                                cout << "0x" << hex << i << ": 0x0" << endl; 
                            }
                        }
                        else{
                            int mapping_end_add;
                            for(map<int, string>::iterator it = group.begin(); it != group.end(); ++it){
                                int idec_add = it->first;
                                if(idec_add > eadd) break;

                                string sdec_add = to_string(idec_add);
                                string shex_add = sdec2shex(sdec_add);
                                
                                string sbin_code = it->second;
                                unsigned long long ull_code = strtoul(sbin_code.c_str(), NULL, 2);
                                cout << shex_add << ": 0x" << hex << ull_code << endl;

                                mapping_end_add = idec_add;
                            }
                            if(mapping_end_add < eadd){
                                for(int i=mapping_end_add+4; i<=eadd; i+=4){
                                    cout << "0x" << hex << i << ": 0x0" << endl; 
                                }
                            }
                        }  
                    }
                    cout << endl;
                }
            }
        }
    }

    return 0;
}
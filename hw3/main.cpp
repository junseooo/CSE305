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

int pc = 0x00400000;
int tadd = 0x00400000;
int dadd = 0x10000000;

int prediction_type = 0;
int m_option = 0;
int n_option = 0;
int d_option = 0;
int p_option = 0;

int start_address;
int end_address;
int num_of_instruction;
string input_file;

map<int, string> tgroup;
map<int, string> dgroup;

vector<string> reg;
vector<string> ID_reg;
vector<string> IF_reg;
vector<string> EX_reg;
vector<string> MEM_reg;
vector<string> WB_reg;

map<string, string> IF_ID_state_reg;
map<string, string> ID_EX_state_reg;
map<string, string> EX_MEM_state_reg;
map<string, string> MEM_WB_state_reg;
map<string, string> WB_IF_state_reg;

string toBinary(int n){
    string r;
    while(n!=0) {r=(n%2==0 ?"0":"1")+r; n/=2;}
    return r;
}

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

string sbin2shex(string sbin) {
    stringstream ss;
    ss << "0x" << hex << stoll(sbin, NULL, 2);
    string shex = ss.str();
    return shex;
}

int shex2idec(string str){
    str = str.substr(2);
    int idec = strtol(str.c_str(), NULL, 16);
    return idec;
}

string sdec2shex(string str) {
    if(str == "") return "";
    else{
        stringstream ss;
        ss << "0x" << hex << stoll(str, NULL, 10);
        str = ss.str();
        return str;
    }
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
    unsigned long long ull_val = strtoul(str.c_str(), NULL, 2);
    int i_val = ull_val;
    i_val = i_val - 1;
    string new_str = toBinary(i_val);

    for(int i=0; i<new_str.size(); i++){
        if(new_str[i] == '0') new_str[i] = '1';
        else if(new_str[i] == '1') new_str[i] = '0';
    }
    unsigned long long new_ull_val = strtoul(new_str.c_str(), NULL, 2);
    int new_i_val = new_ull_val;
    return new_i_val * -1;
}

int checkFormat(string sbin){
    string op = sbin.substr(0, 6);
    if(op == "000000") return 1;
    else if(op == "000010" || op == "000011") return 3;
    else return 2;
}

vector<string> splitCode(string sbin, int type){
    vector<string> code_piece;
    if(type == 1){
        string op = sbin.substr(0,6);
        string rs = sbin.substr(6,5);
        string rt = sbin.substr(11,5);
        string rd = sbin.substr(16,5);
        string shamt = sbin.substr(21,5);
        string func = sbin.substr(26,6);
        code_piece.push_back(op);
        code_piece.push_back(rs);
        code_piece.push_back(rt);
        code_piece.push_back(rd);
        code_piece.push_back(shamt);
        code_piece.push_back(func);
    }
    else if(type == 3){
        string op = sbin.substr(0,6);
        string tar = sbin.substr(6,26);
        code_piece.push_back(op);
        code_piece.push_back(tar);
    }
    else if(type == 2){
        string op = sbin.substr(0,6);
        string rs = sbin.substr(6,5);
        string rt = sbin.substr(11,5);
        string imm = sbin.substr(16,16);
        code_piece.push_back(op);
        code_piece.push_back(rs);
        code_piece.push_back(rt);
        code_piece.push_back(imm);
    }
    return code_piece;
}

void IF_stage(int npc){
    IF_reg = ID_reg;
    if(tgroup.find(npc) != tgroup.end()){
        string sbin = tgroup[npc];
        string sb_rs = sbin.substr(6,5);
        string sb_rt = sbin.substr(11,5);

        int rs = stoi(sb_rs, 0, 2);
        int rt = stoi(sb_rt, 0, 2);
        
        IF_ID_state_reg["IF_ID.RegisterRs"] = to_string(rs);
        IF_ID_state_reg["IF_ID.RegisterRt"] = to_string(rt);
        
        int sign = 0;
        if(ID_EX_state_reg["ID_EX.MemRead"] == "1" && (ID_EX_state_reg["ID_EX.RegisterRt"] == IF_ID_state_reg["IF_ID.RegisterRs"] || ID_EX_state_reg["ID_EX.RegisterRt"] == IF_ID_state_reg["IF_ID.RegisterRt"])){
            sign = 1;
        }
        
        if(sign == 0){ // 평소
            IF_ID_state_reg["IF_ID.INST"] = sbin;
            IF_ID_state_reg["IF_ID.CPC"] = to_string(npc);
            npc+=4;
            IF_ID_state_reg["IF_ID.NPC"] = to_string(npc);
        }
        else if(sign == 1){ // 다음 cycle에 stall을 넣어야할 때
            IF_ID_state_reg["IF_ID.CPC"] = to_string(npc);
            IF_ID_state_reg["IF_ID.NPC"] = "0";
        }
    }
    else{
        string cpc = IF_ID_state_reg["IF_ID.CPC"];
        ID_EX_state_reg["ID_EX.CPC"] = "";
        int i_cpc = stoi(cpc);
        int npc = i_cpc + 4;

        ID_EX_state_reg["ID_EX.NPC"] = to_string(npc);
    }
}

void ID_stage(int npc){
    ID_reg = EX_reg;

    string sd_cpc = IF_ID_state_reg["IF_ID.CPC"];
    ID_EX_state_reg["ID_EX.CPC"] = sd_cpc;
    string sd_npc = IF_ID_state_reg["IF_ID.NPC"];
    ID_EX_state_reg["ID_EX.NPC"] = sd_npc;

    string sb_inst = IF_ID_state_reg["IF_ID.INST"];
    int type = checkFormat(sb_inst);
    vector<string> code_piece = splitCode(sb_inst, type);
    string sb_rs;
    string sb_rt;
    string sb_rd;

    if(code_piece.size() == 6){ // R format
        ID_EX_state_reg["ID_EX.op"] = code_piece[0];
        sb_rs = code_piece[1];
        sb_rt = code_piece[2];
        sb_rd = code_piece[3];
        int rs = stoi(sb_rs, 0, 2);
        int rt = stoi(sb_rt, 0, 2);
        int rd = stoi(sb_rd, 0, 2);
        ID_EX_state_reg["ID_EX.RegisterRs"] = to_string(rs);
        ID_EX_state_reg["ID_EX.RegisterRt"] = to_string(rt);
        ID_EX_state_reg["ID_EX.RegisterRd"] = to_string(rd);
        ID_EX_state_reg["ID_EX.shamt"] = code_piece[4];
        ID_EX_state_reg["ID_EX.func"] = code_piece[5];
        ID_EX_state_reg["ID_EX.INST_TYPE"] = "R";
    }
    else if(code_piece.size() == 4) { // I format
        ID_EX_state_reg["ID_EX.op"] = code_piece[0];
        sb_rs = code_piece[1];
        sb_rt = code_piece[2];
        int rs = stoi(sb_rs, 0, 2);
        int rt = stoi(sb_rt, 0, 2);
        ID_EX_state_reg["ID_EX.RegisterRs"] = to_string(rs);
        ID_EX_state_reg["ID_EX.RegisterRt"] = to_string(rt);
        ID_EX_state_reg["ID_EX.imm"] = code_piece[3];
        ID_EX_state_reg["ID_EX.INST_TYPE"] = "I";

        if(code_piece[0] == "100011"){
            ID_EX_state_reg["ID_EX.MemRead"] = "1";
        }
    }
    else{ // J format
        ID_EX_state_reg["ID_EX.op"] = code_piece[0];
        ID_EX_state_reg["ID_EX.tar"] = code_piece[1];
        ID_EX_state_reg["ID_EX.INST_TYPE"] = "J";
    }
}

void EX_stage(int npc){
    EX_reg = MEM_reg;

    string sd_cpc = ID_EX_state_reg["ID_EX.CPC"];
    EX_MEM_state_reg["EX_MEM.CPC"] = sd_cpc;

    string sd_pc = ID_EX_state_reg["ID_EX.NPC"];
    EX_MEM_state_reg["EX_MEM.NPC"] = sd_pc;

    string inst_type = ID_EX_state_reg["ID_EX.INST_TYPE"];
    EX_MEM_state_reg["EX_MEM.INST_TYPE"] = inst_type;

    string sb_op;
    string sd_rs;
    string sd_rt;
    string sd_rd;
    string sb_shamt;
    string sb_func;
    string sb_imm;
    string sb_tar;
    
    if(inst_type == "R"){
        sb_op = ID_EX_state_reg["ID_EX.op"];
        sd_rs = ID_EX_state_reg["ID_EX.RegisterRs"];
        sd_rt = ID_EX_state_reg["ID_EX.RegisterRt"];
        sd_rd = ID_EX_state_reg["ID_EX.RegisterRd"];
        sb_shamt = ID_EX_state_reg["ID_EX.shamt"];
        sb_func = ID_EX_state_reg["ID_EX.func"];

        string sh_op = sbin2shex(sb_op);
        EX_MEM_state_reg["EX_MEM.op"] = sh_op;

        // forwarding
        string forwardA;
        string forwardB;

        if(EX_MEM_state_reg["EX_MEM.RegWrite"] == "1" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRs"]){
            forwardA = "10";
        }
        if(EX_MEM_state_reg["EX_MEM.RegWrite"] == "1" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRt"]){
            forwardB = "10";
        }
        if(MEM_WB_state_reg["MEM_WB.RegWrite"] == "1" && MEM_WB_state_reg["MEM_WB.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != ID_EX_state_reg["ID_EX.RegisterRs"] && MEM_WB_state_reg["MEM_WB.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRs"]){
            forwardA = "01";
        }
        if(MEM_WB_state_reg["MEM_WB.RegWrite"] == "1" && MEM_WB_state_reg["MEM_WB.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != ID_EX_state_reg["ID_EX.RegisterRt"] && MEM_WB_state_reg["MEM_WB.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRt"]){
            forwardB = "01";
        }

        vector<string> buf_reg = reg;

        if(forwardA == "10"){
            buf_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRs"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }
        else if(forwardA == "01"){
            buf_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRs"])] = MEM_WB_state_reg["MEM_WB.ALU_OUT"];
        }

        if(forwardB == "10"){
            buf_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRt"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }
        else if(forwardB == "01"){
            buf_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRt"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }
        
        int rs = stoi(sd_rs);
        int rt = stoi(sd_rt);
        int rd = stoi(sd_rd);

        string s_rs_val = buf_reg[rs];
        int rs_val = shex2idec(s_rs_val);
        string s_rt_val = buf_reg[rt];
        int rt_val = shex2idec(s_rt_val);

        EX_MEM_state_reg["EX_MEM.RegisterRd"] = to_string(rd);
        
        int shamt = stoi(sb_shamt, 0, 2);

        string sh_func = sbin2shex(sb_func);
        EX_MEM_state_reg["EX_MEM.func"] = sh_func;

        if(sh_func == "0x21"){ // addu
            int res = rs_val + rt_val;
            string sh_res = sdec2shex(to_string(res));
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x24"){ // and
            bitset<32> res(rs_val & rt_val);
            string sh_res = sbin2shex(res.to_string());
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x8"){ // jr
            int target_add = rs_val;
            string sd_target_add = to_string(target_add);
            EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_target_add;
        }
        if(sh_func == "0x27"){ // nor
            bitset<32> buf(rs_val | rt_val);
            bitset<32> res(~buf);
            string sh_res = sbin2shex(res.to_string());
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x25"){ // or
            bitset<32> res(rs_val | rt_val);
            string sh_res = sbin2shex(res.to_string());
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x2b"){ // sltu
            if(rs_val < rt_val) EX_MEM_state_reg["EX_MEM.ALU_OUT"] = "0x1";
            else EX_MEM_state_reg["EX_MEM.ALU_OUT"] = "0x0";
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x0"){ // sll
            rt_val = rt_val << shamt;
            string sh_res = sdec2shex(to_string(rt_val));
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x2"){ // srl
            rt_val = rt_val >> shamt;
            string sh_res = sdec2shex(to_string(rt_val));
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_func == "0x23"){ // subu
            unsigned int res = rs_val - rt_val;
            string sh_res = sdec2shex(to_string(res));
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
    }

    else if(inst_type == "I"){
        sb_op = ID_EX_state_reg["ID_EX.op"];
        sd_rs = ID_EX_state_reg["ID_EX.RegisterRs"];
        sd_rt = ID_EX_state_reg["ID_EX.RegisterRt"];
        sb_imm = ID_EX_state_reg["ID_EX.imm"];

        string sh_op = sbin2shex(sb_op);
        EX_MEM_state_reg["EX_MEM.op"] = sh_op;
        
        // forwarding
        string forwardA;
        string forwardB;

        if(EX_MEM_state_reg["EX_MEM.RegWrite"] == "1" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRs"]){
            forwardA = "10";
        }
        if(EX_MEM_state_reg["EX_MEM.RegWrite"] == "1" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRt"]){
            forwardB = "10";
        }
        if(MEM_WB_state_reg["MEM_WB.RegWrite"] == "1" && MEM_WB_state_reg["MEM_WB.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != ID_EX_state_reg["ID_EX.RegisterRs"] && MEM_WB_state_reg["MEM_WB.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRs"]){
            forwardA = "01";
        }
        if(MEM_WB_state_reg["MEM_WB.RegWrite"] == "1" && MEM_WB_state_reg["MEM_WB.RegisterRd"] != "" && EX_MEM_state_reg["EX_MEM.RegisterRd"] != ID_EX_state_reg["ID_EX.RegisterRt"] && MEM_WB_state_reg["MEM_WB.RegisterRd"] == ID_EX_state_reg["ID_EX.RegisterRt"]){
            forwardB = "01";
        }
        if(forwardA == "10"){
            ID_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRs"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }
        else if(forwardA == "01"){
            ID_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRs"])] = MEM_WB_state_reg["MEM_WB.ALU_OUT"];
        }

        if(forwardB == "10"){
            ID_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRt"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }
        else if(forwardB == "01"){
            ID_reg[stoi(ID_EX_state_reg["ID_EX.RegisterRt"])] = EX_MEM_state_reg["EX_MEM.ALU_OUT"];
        }

        sd_rs = ID_EX_state_reg["ID_EX.RegisterRs"];

        int rs = stoi(sd_rs);
        int rt = stoi(sd_rt);

        EX_MEM_state_reg["EX_MEM.RegisterRd"] = to_string(rt);

        string s_rs_val = ID_reg[rs];
        int rs_val = shex2idec(s_rs_val);

        string s_rt_val = ID_reg[rt];
        int rt_val = stoi(s_rt_val);

        int imm = strtol(sb_imm.c_str(), NULL, 2);

        if(sh_op == "0x9"){ // addiu
            string sh_res;
            if(sb_imm[0] == '1'){ // minus
                int i_val = sbin2signedint(sb_imm);
                cout << "i_val : " << i_val << endl;
                int res = rs_val + i_val;
                sh_res = sdec2shex(to_string(res));
            }
            else{
                int res = rs_val + imm;
                sh_res = sdec2shex(to_string(res));
            }
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0xc"){ // andi
            bitset<32> res(rs_val & imm);
            string sh_res = sbin2shex(res.to_string());
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0x4"){ // beq
            if(rs_val == rt_val){
                int cpc = stoi(sd_pc);
                int target_add = cpc + imm * 4;
                string sd_target_add = to_string(target_add);
                EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_target_add;
            }
            else EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_pc;
        }
        if(sh_op == "0x5"){ // bne
            if(rs_val != rt_val){
                int cpc = stoi(sd_pc);
                int target_add = cpc + imm * 4;
                string sd_target_add = to_string(target_add);
                EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_target_add;
            }
            else EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_pc;
        }
        if(sh_op == "0xf"){ // lui
            bitset<16> res(imm);
            string buffer = res.to_string() + "0000000000000000";
            string sh_res = sbin2shex(buffer);
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0x23"){ // lw
            int res = rs_val + 4 * imm;
            string sd_res = to_string(res);
            EX_MEM_state_reg["EX_MEM.LOAD_ADD"] = sd_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0x20"){ // lb
            int res = rs_val + 4 * imm;
            string sd_res = to_string(res);
            EX_MEM_state_reg["EX_MEM.LOAD_ADD"] = sd_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0xd"){ // ori
            bitset<32> res(rs_val | imm);
            string sh_res = sbin2shex(res.to_string());
            EX_MEM_state_reg["EX_MEM.ALU_OUT"] = sh_res;
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0xb"){ // sltiu
            if(rs_val < imm) EX_MEM_state_reg["EX_MEM.ALU_OUT"] = "0x1";
            else EX_MEM_state_reg["EX_MEM.ALU_OUT"] = "0x0";
            EX_MEM_state_reg["EX_MEM.RegWrite"] = "1";
        }
        if(sh_op == "0x2b"){ // sw
            int store_add = rs_val + 4 * imm;
            string sd_store_add = to_string(store_add);
            EX_MEM_state_reg["EX_MEM.STORE_ADD"] = sd_store_add;

            string sb_store_val = bitset<32>(rt_val).to_string();
            EX_MEM_state_reg["EX_MEM.STORE_VAL"] = sb_store_val;
        }
        if(sh_op == "0x28"){ // sb
            string sb_val = bitset<32>(rt_val).to_string();
            string buffer = sb_val.substr(24);
            string sb_store_val = zeroExtension(buffer, 32);
            EX_MEM_state_reg["EX_MEM.STORE_VAL"] = sb_store_val;
            
            int store_add = rs_val + 4 * imm;
            string sd_store_add = to_string(store_add);
            EX_MEM_state_reg["EX_MEM.STORE_ADD"] = sd_store_add;
        }
    }

    else if(inst_type == "J"){
        sb_op = ID_EX_state_reg["ID_EX.op"];
        sb_tar = ID_EX_state_reg["ID_EX.tar"];

        string sh_op = sbin2shex(sb_op);
        EX_MEM_state_reg["EX_MEM.op"] = sh_op;
    
        if(sh_op == "0x2"){ // j
            string sh_tar = sbin2shex(sb_tar);
            sh_tar = sh_tar.substr(2);

            int buffer = (int)strtol(sh_tar.c_str(), NULL, 16);
            int target_add = buffer * 4;
            string sd_target_add = to_string(target_add);
            EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_target_add;
        }
        if(sh_op == "0x3"){ // jal
            reg[31] = sdec2shex(to_string(pc)); // $ra store

            string sh_tar = sbin2shex(sb_tar);
            sh_tar = sh_tar.substr(2);

            int buffer = (int)strtol(sh_tar.c_str(), NULL, 16);
            int target_add = buffer * 4;
            string sd_target_add = to_string(target_add);
            EX_MEM_state_reg["EX_MEM.TARGET_ADD"] = sd_target_add;
        }
    }
}

void MEM_stage(int npc){
    MEM_reg = WB_reg;

    string sd_cpc = EX_MEM_state_reg["EX_MEM.CPC"];
    MEM_WB_state_reg["MEM_WB.CPC"] = sd_cpc;

    string sd_pc = EX_MEM_state_reg["EX_MEM.NPC"];
    MEM_WB_state_reg["MEM_WB.NPC"] = sd_pc;

    string inst_type = EX_MEM_state_reg["EX_MEM.INST_TYPE"];
    MEM_WB_state_reg["MEM_WB.INST_TYPE"] = inst_type;

    string sh_op = EX_MEM_state_reg["EX_MEM.op"];
    MEM_WB_state_reg["MEM_WB.op"] = sh_op;

    string sh_func = EX_MEM_state_reg["EX_MEM.func"];

    string sh_alu_out = EX_MEM_state_reg["EX_MEM.ALU_OUT"];

    string sd_target_add = EX_MEM_state_reg["EX_MEM.TARGET_ADD"];

    string sd_write_reg;

    if(inst_type == "R" || inst_type == "I"){
        sd_write_reg = EX_MEM_state_reg["EX_MEM.RegisterRd"];
        MEM_WB_state_reg["MEM_WB.RegisterRd"] = sd_write_reg;
    }

    string sd_load_add = EX_MEM_state_reg["EX_MEM.LOAD_ADD"];
    string sd_load_reg = EX_MEM_state_reg["EX_MEM.LOAD_REG"];

    string sd_store_add = EX_MEM_state_reg["EX_MEM.STORE_ADD"];
    string sb_store_val = EX_MEM_state_reg["EX_MEM.STORE_VAL"];
    
    if(sh_op == "0x0" && sh_func != "0x8"){ // R format
        MEM_WB_state_reg["MEM_WB.func"] = sh_func;
        MEM_WB_state_reg["MEM_WB.ALU_OUT"] = sh_alu_out;
        MEM_WB_state_reg["MEM_WB.RegWrite"] = "1";
    }

    if(sh_op == "0x9" || sh_op == "0xc" || sh_op == "0xf" || sh_op == "0xd" || sh_op == "0xb"){ // addiu, addu, lui, ori, sltiu
        MEM_WB_state_reg["MEM_WB.ALU_OUT"] = sh_alu_out;
        MEM_WB_state_reg["MEM_WB.RegWrite"] = "1";
    }

    // branch
    if(sh_func == "0x8" || sh_func == "0x4" || sh_func == "0x5" || sh_func == "0x2" || sh_func == "0x3"){ // jr, beq, bne, j, jal
        int target_add = stoi(sd_target_add);
        pc = target_add;
    }
    
    // load
    if(sh_func == "0x23" || sh_func == "0x20"){ // lw, lb
        int load_add = stoi(sd_load_add);
        int load_reg = stoi(sd_load_reg);
        string sh_res;

        if(sh_func == "0x23"){
            auto iter = dgroup.find(load_add);
            if(iter != dgroup.end()){
                string sb_val = iter->second;
                unsigned long long ull_val = strtoul(sb_val.c_str(), NULL, 2);
                int i_val = ull_val;
                sh_res = sdec2shex(to_string(i_val));
            }
        }
        if(sh_func == "0x20"){
            int i_val;
            auto iter = dgroup.find(load_add);

            if(iter != dgroup.end()){
                string sbin_val = iter->second;
                unsigned long long ull_val = strtoul(sbin_val.c_str(), NULL, 2);
                i_val = ull_val;
            }
            string sb_val = bitset<32>(i_val).to_string();
            string buffer = sb_val.substr(24);
            buffer = signExtension(buffer, 32);
            sh_res = sbin2shex(buffer);
        }
        MEM_WB_state_reg["MEM_WB.READ_DATA"] = sh_res;
        MEM_WB_state_reg["MEM_WB.RegWrite"] = "1";
    }

    // store
    if(sh_func == "0x2b" || sh_func == "0x28"){ // sw, sb
        int store_add = stoi(sd_store_add);
        dgroup.insert({store_add, sb_store_val});
    }
}

void WB_stage(int npc){
    WB_reg = reg;
    
    string sd_cpc = MEM_WB_state_reg["MEM_WB.CPC"];
    WB_IF_state_reg["WB_IF.CPC"] = sd_cpc;

    string sd_pc = MEM_WB_state_reg["MEM_WB.NPC"];
    WB_IF_state_reg["WB_IF.NPC"] = sd_pc;

    string sd_write_reg = MEM_WB_state_reg["MEM_WB.RegisterRd"];
    int write_reg = stoi(sd_write_reg);

    string sh_op = MEM_WB_state_reg["MEM_WB.op"];
    string sh_func = MEM_WB_state_reg["MEM_WB.func"];

    string sh_alu_out = MEM_WB_state_reg["MEM_WB.ALU_OUT"];
    string sh_read_data = MEM_WB_state_reg["MEM_WB.READ_DATA"];


    if(MEM_WB_state_reg["MEM_WB.RegWrite"] == "1"){
        WB_reg[write_reg] = sh_alu_out;
    }

    if(sh_func == "0x23" || sh_func == "0x20"){ // lw, lb
        WB_reg[write_reg] = sh_read_data;
    }

    reg = WB_reg;

}

void printMemory(){
    std::cout << "Memory content [0x" << hex << start_address << "..0x" << hex << end_address << "]:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    // text data
    if((start_address >= 0x00400000 && start_address < 0x10000000) || start_address >= 0x10000000){
        map<int, string> group;
        if(start_address >= 0x00400000 && start_address < 0x10000000) group = tgroup;
        if(start_address >= 0x10000000) group = dgroup;

        for(int i=start_address; i<=end_address; i+=4){
            auto iter = group.find(i);
            if(iter == group.end()){
                std::cout << "0x" << hex << i << ": 0x0" << std::endl;
            }
            else{
                string sbin_data_code = iter->second;
                string shex_data_code = sbin2shex(sbin_data_code);
                std::cout << "0x" << hex << i << ": " << shex_data_code << std::endl; 
            }
        }

    }
    std::cout << std::endl;
        
}

void printReg(vector<string> regv, string sd_npc){
    int npc = 0x400000;
    if(sd_npc != "") npc = stoi(sd_npc);
    
    std::cout << "Current register values:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "PC: 0x" << hex << npc << std::endl;
    std::cout << "Registers:" << std::endl;
    for(int i=0; i<32; i++){
        std::cout << "R" << dec << i << ": " << regv[i] << std::endl;
    }
    std::cout << std::endl;
}

void printPC(){
    string sd_IF_pc = IF_ID_state_reg["IF_ID.CPC"];
    string sd_ID_pc = ID_EX_state_reg["ID_EX.CPC"];
    string sd_EX_pc = EX_MEM_state_reg["EX_MEM.CPC"];
    string sd_MEM_pc = MEM_WB_state_reg["MEM_WB.CPC"];
    string sd_WB_pc = WB_IF_state_reg["WB_IF.CPC"];
    
    string IF_pc = sdec2shex(sd_IF_pc);
    string ID_pc = sdec2shex(sd_ID_pc);
    string EX_pc = sdec2shex(sd_EX_pc);
    string MEM_pc = sdec2shex(sd_MEM_pc);
    string WB_pc = sdec2shex(sd_WB_pc);
    std::cout << "Current pipeline PC state:" << std::endl;
    std::cout << "{" << IF_pc << "|" << ID_pc << "|" << EX_pc << "|" << MEM_pc << "|" << WB_pc << "}" << std::endl << std::endl;
}

int main(int argc, char** argv){
    vector<string> input_arguments;

    for(int i=0; i<argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    // data parsing
    for(int i=0; i<input_arguments.size(); i++){
        if(i == 1){
            if(input_arguments[i] == "-atp") prediction_type = 1;
            if(input_arguments[i] == "-atnp") prediction_type = 2;
        }
        if(input_arguments[i] == "-m"){
            m_option = 1;
            string range_of_address = input_arguments[i+1];
            istringstream ss(range_of_address);
            string buffer;
            vector <string> v;
            while(getline(ss, buffer, ':')){
                v.push_back(buffer);
            }
            string s_start_address = v[0];
            string s_end_address = v[1];
            start_address = shex2idec(s_start_address);
            end_address = shex2idec(s_end_address);
        }

        if(input_arguments[i] == "-d"){
            d_option = 1;
        }

        if(input_arguments[i] == "-p"){
            p_option = 1;
        }
        
        if(input_arguments[i] == "-n"){
            n_option = 1;
            string s_num_of_instruction = input_arguments[i+1];
            num_of_instruction = stoi(s_num_of_instruction);
        }
        
        if(i == input_arguments.size() - 1){
            input_file = input_arguments[i];
        }
    }

    // std::cout << "< check options >" << std::endl;
    // std::cout << "prediction type : " << prediction_type << std::endl;
    // std::cout << "m option : " << m_option << std::endl;
    // std::cout << "start address : " << start_address << std::endl;
    // std::cout << "end address : " << end_address << std::endl;
    // std::cout << "d option : " << d_option << std::endl;
    // std::cout << "n option : " << n_option << std::endl;
    // std::cout << "number of instruction : " << num_of_instruction << std::endl;
    // std::cout << "input file name : " << input_file << std::endl;
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


    // calculate text and data size
    string shex_tsize = code[0];
    string shex_dsize = code[1];

    int idec_tsize = shex2idec(shex_tsize);
    int idec_dsize = shex2idec(shex_dsize);
    idec_tsize = idec_tsize/4;
    idec_dsize = idec_dsize/4;

    // std::cout << "< check text and data size >" << std::endl;
    // std::cout << "num of test code : " << idec_tsize << std::endl;
    // std::cout << "num of data code : " << idec_dsize << std::endl;
    // std::cout << std::endl;


    // make string hexadecimal code vector
    vector<string> shex_tcode;
    for(int i=2; i<idec_tsize+2; i++){
        shex_tcode.push_back(code[i]);
    }

    // make string 32bits binary code vector
    vector<string> sbin_tcode;
    for(int i=0; i<shex_tcode.size(); i++){
        string sbin = shex2sbin(shex_tcode[i]);
        sbin = zeroExtension(sbin, 32);
        sbin_tcode.push_back(sbin);
    }

    for(int i=0; i<sbin_tcode.size(); i++){
        tgroup.insert({tadd, sbin_tcode[i]});
        tadd += 4;
    }

    // std::cout << "< check text group mapping >" << std::endl;
    // for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){
    //     std::cout << dec << it->first << ": " << it->second << std::endl;
    // }
    // std::cout << std::endl;


    // make string hexadecimal code vector
    vector<string> shex_dcode;
    for(int i=2+idec_tsize; i<code.size(); i++){
        shex_dcode.push_back(code[i]);
    }

    vector<string> sbin_dcode;
    for(int i=0; i<shex_dcode.size(); i++){
        string sbin = shex2sbin(shex_dcode[i]);
        string buffer = zeroExtension(sbin, 32);

        sbin_dcode.push_back(buffer);
    }

    for(int i=0; i<sbin_dcode.size(); i++){
        dgroup[dadd] = sbin_dcode[i];
        dadd += 4;
    }

    // std::cout << "< check data group mapping >" << std::endl;
    // for(map<int, string>::iterator it = dgroup.begin(); it != dgroup.end(); ++it){
    //     std::cout << dec << it->first << ": " << it->second << std::endl;
    // }
    // std::cout << std::endl;

    /*******************************************************************
    ********************************************************************
                            code parsing finished
    ********************************************************************
    *******************************************************************/

    // set initial register condition
    for(int i=0; i<32; i++){
        reg.push_back("0x0");
        IF_reg.push_back("0x0");
        ID_reg.push_back("0x0");
        EX_reg.push_back("0x0");
        MEM_reg.push_back("0x0");
        WB_reg.push_back("0x0");
    }

    // for(pair<int, string> code : tgroup){
    //     std::cout << code.first << ": " << code.second << std::endl;
    // }

    auto it = tgroup.begin();

    int input_pc = it->first;
    int IF_input_pc = input_pc;
    int cycle = 1;

    while(tgroup.find(IF_input_pc) != tgroup.end()){

        // cout << endl;

        // for(map<string,string>::iterator it = IF_ID_state_reg.begin(); it != IF_ID_state_reg.end(); ++it){
        //     std::cout << it-> first << ": " << it-> second << std::endl;
        // }
        // cout << endl;

        // for(map<string,string>::iterator it = ID_EX_state_reg.begin(); it != ID_EX_state_reg.end(); ++it){
        //     std::cout << it-> first << ": " << it-> second << std::endl;
        // }
        // cout << endl;

        // for(map<string,string>::iterator it = EX_MEM_state_reg.begin(); it != EX_MEM_state_reg.end(); ++it){
        //     std::cout << it-> first << ": " << it-> second << std::endl;
        // }
        // cout << endl;

        // for(map<string,string>::iterator it = MEM_WB_state_reg.begin(); it != MEM_WB_state_reg.end(); ++it){
        //     std::cout << it-> first << ": " << it-> second << std::endl;
        // }
        // cout << endl;

        // for(map<string,string>::iterator it = WB_IF_state_reg.begin(); it != WB_IF_state_reg.end(); ++it){
        //     std::cout << it-> first << ": " << it-> second << std::endl;
        // }
        // cout << endl;

        if(cycle > num_of_instruction) break;

        std::cout << "===== Cycle " << cycle << " =====" << std::endl;

        if(cycle == 1){
            IF_stage(input_pc);
            cycle++;
        }

        else if(cycle == 2){
            string sd_ID_input_pc = IF_ID_state_reg["IF_ID.CPC"];
            int ID_input_pc = stoi(sd_ID_input_pc);
            ID_stage(ID_input_pc);

            string sd_IF_input_pc = IF_ID_state_reg["IF_ID.NPC"];
            IF_input_pc = stoi(sd_IF_input_pc);
            IF_stage(IF_input_pc);

            cycle++;
        }

        else if(cycle == 3){
            string sd_EX_input_pc = ID_EX_state_reg["ID_EX.CPC"];
            int EX_input_pc = stoi(sd_EX_input_pc);
            cout << EX_input_pc << endl;
            EX_stage(EX_input_pc);

            string sd_ID_input_pc = IF_ID_state_reg["IF_ID.CPC"];
            int ID_input_pc = stoi(sd_ID_input_pc);
            ID_stage(ID_input_pc);

            string sd_IF_input_pc = IF_ID_state_reg["IF_ID.NPC"];
            IF_input_pc = stoi(sd_IF_input_pc);
            IF_stage(IF_input_pc);
            
            cycle++;
        }
        
        else if(cycle == 4){
            string sd_MEM_input_pc = EX_MEM_state_reg["EX_MEM.CPC"];
            int MEM_input_pc = stoi(sd_MEM_input_pc);
            MEM_stage(MEM_input_pc);

            string sd_EX_input_pc = ID_EX_state_reg["ID_EX.CPC"];
            int EX_input_pc = stoi(sd_EX_input_pc);
            EX_stage(EX_input_pc);

            string sd_ID_input_pc = IF_ID_state_reg["IF_ID.CPC"];
            int ID_input_pc = stoi(sd_ID_input_pc);
            ID_stage(ID_input_pc);

            string sd_IF_input_pc = IF_ID_state_reg["IF_ID.NPC"];
            IF_input_pc = stoi(sd_IF_input_pc);
            IF_stage(IF_input_pc);

            cycle++;
        }
        
        else if(cycle >= 5){
            string sd_WB_input_pc = MEM_WB_state_reg["MEM_WB.CPC"];
            int WB_input_pc = stoi(sd_WB_input_pc);
            WB_stage(WB_input_pc);

            string sd_MEM_input_pc = EX_MEM_state_reg["EX_MEM.CPC"];
            int MEM_input_pc = stoi(sd_MEM_input_pc);
            MEM_stage(MEM_input_pc);

            string sd_EX_input_pc = ID_EX_state_reg["ID_EX.CPC"];
            int EX_input_pc = stoi(sd_EX_input_pc);
            EX_stage(EX_input_pc);

            string sd_ID_input_pc = IF_ID_state_reg["IF_ID.CPC"];
            int ID_input_pc = stoi(sd_ID_input_pc);
            ID_stage(ID_input_pc);

            string sd_IF_input_pc = IF_ID_state_reg["IF_ID.NPC"];
            int IF_input_pc = stoi(sd_IF_input_pc);
            IF_stage(IF_input_pc);

            cycle++;
        }

        if(p_option == 1) printPC();
        if(d_option == 1) printReg(reg, IF_ID_state_reg["IF_ID.NPC"]);
        if(m_option == 1) printMemory();
    }

    cout << "===== Completion cycle: " << cycle-1 << " =====" << endl << endl;
    printPC();
    printReg(reg, IF_ID_state_reg["IF_ID.NPC"]);

    // test execution : runfile -atp -m 0x00400000:0x00400010 -d -p -n 3 sample.o
    return 0;
}
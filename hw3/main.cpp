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
map<int, string> tgroup;
map<int, string> dgroup;

vector<string> reg;

vector<string> IF_ID_state_reg;
vector<string> ID_EX_state_reg;
vector<string> EX_MEM_state_reg;
vector<string> MEM_WB_state_reg;

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
    int idec = strtol(str.c_str(), NULL, 16);
    return idec;
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

void IF_stage(int pc){
    pc+=4;
    IF_ID_state_reg.push_back(to_string(pc));
    string shex = tgroup[pc];
    string sbin = shex2sbin(shex);
    sbin = zeroExtension(sbin, 32);
    int type = checkFormat(sbin);
    vector<string> code_piece = splitCode(sbin, type);
    for(int i=0; i<code_piece.size(); i++){
        IF_ID_state_reg.push_back(code_piece[i]);
    }
}

void ID_stage(){
    
}

void EX_stage(){
    
}

void MEM_stage(){
    
}

void WB_stage(){
    
}


int main(){
    string code = "0x24020400";
    
    return 0;
}
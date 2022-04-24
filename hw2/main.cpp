#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <cstdio>

using namespace std;

string shex2sbin(string sHex){
    string sReturn = "";
    sHex = sHex.substr(2);
    for(int i = 0; i<sHex.length(); ++i){
        switch (sHex[i]){
            case '0': sReturn.append ("0000"); break;
            case '1': sReturn.append ("0001"); break;
            case '2': sReturn.append ("0010"); break;
            case '3': sReturn.append ("0011"); break;
            case '4': sReturn.append ("0100"); break;
            case '5': sReturn.append ("0101"); break;
            case '6': sReturn.append ("0110"); break;
            case '7': sReturn.append ("0111"); break;
            case '8': sReturn.append ("1000"); break;
            case '9': sReturn.append ("1001"); break;
            case 'a': sReturn.append ("1010"); break;
            case 'b': sReturn.append ("1011"); break;
            case 'c': sReturn.append ("1100"); break;
            case 'd': sReturn.append ("1101"); break;
            case 'e': sReturn.append ("1110"); break;
            case 'f': sReturn.append ("1111"); break;
        }
    }
    return sReturn;
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

// void amulator(string str, int ptr){

// }

int main(int argc, char** argv){
    
    /******************
     ARGUMENTS PARSING
    ******************/

    int m_option = 0;
    int d_option = 0;
    int n_option = 0;

    vector<string> input_arguments;
    string string_start_address;
    string string_end_address;
    string string_num_of_instruction;
    string input_file;

    // arguments parsing
    for(int i = 0; i < argc; i++){
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
            string_start_address = v[0];
            string_end_address = v[1];
        }

        if(input_arguments[i] == "-d"){
            d_option = 1;
        }
        
        if(input_arguments[i] == "-n"){
            n_option = 1;
            string_num_of_instruction = input_arguments[i+1];
        }
        
        if(i == input_arguments.size() - 1){
            input_file = input_arguments[i];
        }
    }

    string_start_address = string_start_address.substr(2);
    string_end_address = string_end_address.substr(2);
    int start_address = stoi(string_start_address);
    int end_address = stoi(string_end_address);
    int num_of_instruction = stoi(string_num_of_instruction);

    // cout << "< check options >" << endl;
    // cout << "start_address : " << start_address << endl;
    // cout << "end_address : " << end_address << endl;
    // cout << "num_of_instruction : " << num_of_instruction << endl;
    // cout << "input_file : " << input_file << endl;
    // cout << endl;

    /******************
     input file parsing
    ******************/
    
    string R0;
    string R1;
    string R2;
    string R3;
    string R4;
    string R5;
    string R6;
    string R7;
    string R8;
    string R9;
    string R10;
    string R11;
    string R12;
    string R13;
    string R14;
    string R15;
    string R16;
    string R17;
    string R18;
    string R19;
    string R20;
    string R21;
    string R22;
    string R23;
    string R24;
    string R25;
    string R26;
    string R27;
    string R28;
    string R29;
    string R30;
    string R31;

    int tadd = 0x00400000;
    int dadd = 0x10000000;
    int PC = 0x00400000;

    ifstream fin;
    fin.open(input_file);
    string line;
    vector<string> code;
    while(getline(fin, line)){
        code.push_back(line);
    }
    fin.close();

    // for(auto i : code){
    //     cout << i << endl;
    // }


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
    
    vector<string> shex_tcode;
    for(int i=2; i<idec_tsize+2; i++){
        shex_tcode.push_back(code[i]);
    }

    cout << "< check shex_tcode >" << endl;
    for(auto i:shex_tcode){
        cout << i << endl;
    }
    cout << endl;

    vector<string> sbin_tcode;
    for(int i=0; i<shex_tcode.size(); i++){
        string sbin = shex2sbin(shex_tcode[i]);
        string buffer = zeroExtension(sbin);

        sbin_tcode.push_back(buffer);
    }

    cout << "< check binary text code >" << endl;
    for(auto i : sbin_tcode){
        cout << i << endl;
    }
    cout << endl;

    map<int, string> tgroup;
    map<int, string> dgroup;

    for(int i=0; i<sbin_tcode.size(); i++){
        tadd += 4;
        tgroup.insert({tadd, shex_tcode[i]});
    }

    cout << "< check address >" << endl;
    for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){

        cout << it->first << " : " << it->second << endl;
    }
    cout << endl;
    
    // dgroup.insert({str[i], dadd});

    cout << "< CONSOLE HERE !!! >" << endl;
    cout << "Current register values:" << endl;
    cout << "--------------------------------" << endl;
    cout << "PC:" << PC << endl;
    cout << "Registers:" << endl;
    cout << endl;
    cout << "Memory content [" << start_address << ".." << end_address << "]:" << endl;
    cout << "--------------------------------" << endl;
    for(map<int, string>::iterator it = tgroup.begin(); it != tgroup.end(); ++it){
        int idec_address = it->first;
        string sdec_address = to_string(idec_address);
        string shex_address = sdec2shex(sdec_address);
        
        cout << shex_address << ": " << it->second << endl;
    }
    
    cout << endl;


    /**************************
     MAKE EMULATOR with option
    **************************/

    return 0;
}
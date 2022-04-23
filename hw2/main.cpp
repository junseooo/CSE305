#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

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
    string addr1;
    string addr2;
    string num_of_instruction;
    string input_file;

    // arguments parsing
    for(int i = 0; i < argc; i++){
        string str(argv[i]);
        input_arguments.push_back(str);
    }

    // for(int i=0; i<input_arguments.size(); i++){
    //     cout << i << "번 째 원소 : " << input_arguments[i] << endl;
    // }

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
            addr1 = v[0];
            addr2 = v[1];
        }

        if(input_arguments[i] == "-d"){
            d_option = 1;
        }
        
        if(input_arguments[i] == "-n"){
            n_option = 1;
            num_of_instruction = input_arguments[i+1];
        }
        
        if(i == input_arguments.size() - 1){
            input_file = input_arguments[i];
        }
    }

    // cout << "addr1 : " << addr1 << endl;
    // cout << "addr2 : " << addr2 << endl;
    // cout << "input_file : " << input_file << endl;

    /**************************
     MAKE EMULATOR with option
    **************************/

    /******************
     input file parsing
    ******************/

    int tadd = 0x00400000;
    int dadd = 0x10000000;
    int PC = 0x00400000;
    int tsize = 0;
    int dsize = 0;
    
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

    vector<string> bincode;
    tsize = stoi(code[0].substr(2));
    dsize = stoi(code[1].substr(2));
    for(int i=2; i<code.size(); i++){
        string sbin = shex2sbin(code[i]);
        string buffer = zeroExtension(sbin);

        bincode.push_back(buffer);
    }
    cout << endl;
    cout << endl;

    for(auto i : bincode){
        cout << i << endl;
    }

    return 0;
}

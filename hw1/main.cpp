#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <map>

using namespace std;

map<string, int> m;

vector<string> Split(string str){
    string str_copy;
    vector<string> v;
    v.clear();
    int i=0;
    istringstream ss(str);
    while(ss>>str_copy){
        v.push_back(str_copy);
        i++;
    }
    return v;
}

int findIndex(vector<string> v, string str){
    auto it = find(v.begin(), v.end(), str);
    return it - v.begin();
}

char* toBinary(unsigned int n, int num) {
    char *str = (char *) malloc(30);
    str[num] = '\0';
    for (int i = num - 1; i >= 0; i--) {
        str[num - 1 - i] = ((n >> i) & 1) + '0';
    }
    return str;
}

int checkFormat(string str){ // R, I, J 인지 format 확인
    if(str == "addu" || str == "and" || str == "jr" || str == "nor" || str == "or" || str == "sltu" || str == "sll" || str == "srl" || str == "subu") return 1;
    else if(str == "addiu" || str == "andi" || str == "beq" || str == "bne" || str == "lui" || str == "lw" || str == "ori" || str == "sltiu" || str == "sw" || str == "lb" || str == "sb" || str == "la") return 2;
    else if(str == "j" || str == "jal") return 3;
    else return 0;
}

string instructionConversion(vector<string> v){
    string op;
    int op_dec; // string으로 받은 16진수를 10진수로 변환하기 위한 변수
    string rs;
    string rt;
    string rd;
    string shamt;

    string func; // for F
    string imm; // for I
    string address; // for I
    string offset; // for I
    string target; // for J

    string result;

    if(v[0].find(":") != string::npos){ // 첫번째 원소에 ":"이 존재하면 두번째 원소를 instruction으로 확인
        
    }

    if(checkFormat(v[0]) == 1){ // R format
        op = "0";
        result += toBinary(stoi(op), 6);

        if(v[0] == "addu") func = "0x21";
        else if(v[0] == "and") func = "0x24";
        else if(v[0] == "nor") func = "0x27";
        else if(v[0] == "or") func = "0x25";
        else if(v[0] == "sltu") func = "0x2b";
        else if(v[0] == "subu") func = "0x23";
        else if(v[0] == "jr") {
            string buf_rs = v[1];
            rs = buf_rs.substr(1);

            result += toBinary(stoi(rs), 5);
            result += toBinary(stoi("0"), 15);
            func = "8";
            result += toBinary(stoi(func), 6);

            return result;
        }
        else if(v[0] == "sll"){
            string buf_rd = v[1];
            rd = buf_rd.substr(1, buf_rd.size()-2);
            string buf_rt = v[2];
            rt = buf_rt.substr(1, buf_rt.size()-2);
            shamt = v[3];

            result += toBinary(stoi("0"), 5);
            result += toBinary(stoi(rt), 5);
            result += toBinary(stoi(rd), 5);
            result += toBinary(stoi(shamt), 5);
            result += toBinary(stoi("0"), 6);

            return result;
        }
        else if(v[0] == "srl"){
            string buf_rd = v[1];
            rd = buf_rd.substr(1, buf_rd.size()-2);
            string buf_rt = v[2];
            rt = buf_rt.substr(1, buf_rt.size()-2);
            shamt = v[3];

            result += toBinary(stoi("0"), 5);
            result += toBinary(stoi(rt), 5);
            result += toBinary(stoi(rd), 5);
            result += toBinary(stoi(shamt), 5);
            result += toBinary(stoi("2"), 6);

            return result;
        }

        string buf_rd = v[1];
        rd = buf_rd.substr(1, buf_rd.size()-2);
        string buf_rs = v[2];
        rs = buf_rs.substr(1, buf_rs.size()-2);
        string buf_rt = v[3];
        rt = buf_rt.substr(1);

        result += toBinary(stoi(rs), 5);
        result += toBinary(stoi(rt), 5);
        result += toBinary(stoi(rd), 5);
        result += toBinary(stoi("0"), 5);
        int func_dec = (int)strtol(func.c_str(), NULL, 16);
        result += toBinary(func_dec, 6);

        return result;
    }
    else if(checkFormat(v[0]) == 2){ // I format
        if(v.size() == 3){ // instruction 구성이 3개로만 이루어진 경우
            if(v[0] == "lui"){ // upper 16-bit에 imm(숫자)를 load
                op = "0xf";
                string buf_rt = v[1];
                rt = buf_rt.substr(1, buf_rt.size()-2);
                imm = v[2]; // 숫자

                int op_dec = (int)strtol(op.c_str(), NULL, 16);
                result += toBinary(op_dec, 6);
                result += toBinary(stoi("0"), 5);
                result += toBinary(stoi(rt), 5);
                result += toBinary(stoi(imm), 16);
                
                return result;
            }
            else if(v[0] == "la"){
                auto address = m.find(v[2]);
                if(address != m.end()){
                    int target_add = address->second;
                    string add_binary = toBinary(target_add, 32);
                    string upadd = add_binary.substr(0, 16);
                    string lowadd = add_binary.substr(16);
                    if(lowadd == "0000000000000000"){ // lui
                        op = "0xf";
                        string buf_rt = v[1];
                        rt = buf_rt.substr(1, buf_rt.size()-2);

                        int op_dec = (int)strtol(op.c_str(), NULL, 16);
                        result += toBinary(op_dec, 6);
                        result += toBinary(stoi("0"), 5);
                        result += toBinary(stoi(rt), 5);
                        result += upadd;
                        
                        return result;
                    }
                    else{
                        // lui
                        op = "0xf";
                        string buf_rt = v[1];
                        rt = buf_rt.substr(1, buf_rt.size()-2);

                        int op_dec = (int)strtol(op.c_str(), NULL, 16);
                        result += toBinary(op_dec, 6);
                        result += toBinary(stoi("0"), 5);
                        result += toBinary(stoi(rt), 5);
                        result += upadd;
                        result += "\n";

                        // ori
                        string op2 = "0xd";
                        int op_dec2 = (int)strtol(op2.c_str(), NULL, 16);
                        result += toBinary(op_dec2, 6);
                        result += toBinary(stoi(rt), 5);
                        result += toBinary(stoi(rt), 5);
                        result += lowadd;
                        
                        return result;
                    }
                }
                return result;
            }
            else if(v[0] == "lw") op = "0x23";
            else if(v[0] == "lb") op = "0x20";
            else if(v[0] == "sw") op = "0x2b";
            else if(v[0] == "sb") op = "0x28";

            string buf_rt = v[1];
            rt = buf_rt.substr(1, buf_rt.size()-1);

            address = v[2];
            int index = address.find("(");
            offset = address.substr(0, index);
            string buf_add = address.substr(index+1);
            rs = buf_add.substr(1, buf_add.size()-2);

            int op_dec = (int)strtol(op.c_str(), NULL, 16);
            result += toBinary(op_dec, 6);
            result += toBinary(stoi(rs), 5);
            result += toBinary(stoi(rt), 5);
            result += toBinary(stoi(offset), 16);
            
            return result;
        }
        else if(v.size() == 4){ // instruction 구성이 4개로 이루어진 경우
            if(v[0] == "addiu") op = "9";
            else if(v[0] == "andi") op = "0xc";
            else if(v[0] == "ori") op = "0xd";
            else if(v[0] == "sltiu") op = "0xb";
            else if(v[0] == "beq") {
                op = "4";
                string buf_rt = v[1];
                rt = buf_rt.substr(1, buf_rt.size()-2);
                string buf_rs = v[2];
                rs = buf_rs.substr(1, buf_rs.size()-2);
                imm = v[3];
                auto address = m.find(imm);
                if(address != m.end()){
                    int imm_add = address->second;
                    result += toBinary(imm_add, 16);
                }
                return result;
            }
            else if(v[0] == "bne") {
                op = "5";
                string buf_rt = v[1];
                rt = buf_rt.substr(1, buf_rt.size()-2);
                string buf_rs = v[2];
                rs = buf_rs.substr(1, buf_rs.size()-2);
                imm = v[3];
                auto address = m.find(imm);
                if(address != m.end()){
                    int imm_add = address->second;
                    result += toBinary(imm_add, 16);
                }
                return result;
            }

            string buf_rt = v[1];
            rt = buf_rt.substr(1, buf_rt.size()-2);
            string buf_rs = v[2];
            rs = buf_rs.substr(1, buf_rs.size()-2);
            imm = v[3];
            int imm_dec;
            int op_dec;

            if(op.find("0x") != string::npos){
                op_dec = (int)strtol(op.c_str(), NULL, 16);
                result += toBinary(op_dec, 6);
            }
            else{
                result += toBinary(stoi(op), 6);
            }
            result += toBinary(stoi(rs), 5);
            result += toBinary(stoi(rt), 5);
            if(imm.find("0x") != string::npos){
                imm_dec = (int)strtol(imm.c_str(), NULL, 16);
                result += toBinary(imm_dec, 16);
            }
            else{
                result += toBinary(stoi(imm), 16);
            }

            return result;
        }
    }
    else if(checkFormat(v[0]) == 3){ // J format
        if(v[0] == "j") op = "2";
        else if(v[0] == "jal") op = "3";
        
        target = v[1];
        result += toBinary(stoi(op), 6);
        
        auto address = m.find(target);
        if(address != m.end()){
            int target_add = address->second;
            int target_add_buf = target_add / 4;
            result += toBinary(target_add_buf, 26);
        }

        return result;
    }
    return result;
}

string string_to_hex(string str) {
    stringstream ss;
    ss << "0x" << hex << stoll(str, NULL, 2);
    str = ss.str();
    return str;
}

int main(){
    vector<string> code;
    vector<string> dcode; // data part의 코드
    vector<string> tcode; // text part의 코드
    vector<string> dlabel; // data part의 라벨 모음
    vector<string> tlabel; // text part의 라벨 모음
    code.clear();
    dcode.clear();
    tcode.clear();
    dlabel.clear();
    tlabel.clear();
    // data와 text의 코드를 따로 저장함
    // 코드에서 특정 라벨에 접근할 때 그 주소를 반환할 수 있게끔 해야함

    int dadd = 0x10000000; // 8-bit로 표현한 16진수. -> binary로 표현 시 32-bit으로 표현 가능
    int tadd = 0x00400000;
    
    // cin으로 파일 읽어오기
    // istream 머시기 그거 써서 파일명 동일하게 만들어서 출력파일 만들기 .o 파일로
    cout << "$ ";
    string command;
    getline(cin, command);

    istringstream start(command);
    vector<string> commandParsing;
    while(start >> command){
        commandParsing.push_back(command);
    }
    string inputFile = commandParsing[1];
    string fileName = inputFile;
    string outputFile;
    cout << endl << "input file name : " << inputFile << endl;
    
    if (commandParsing[0] == "./runfile") {
        fileName.pop_back();
        fileName.pop_back();
        string outputFile = fileName + ".o";
        cout << "output file name : " << outputFile << endl << endl;
        ofstream fout;
        fout.open(outputFile);
        fout.close();

        cout << "START HERE" << endl << endl;
    
        ifstream fin;
        fin.open(inputFile);
        string line;
        
        while(getline(fin, line)){
            // cout << line << endl;
            int i=0;
            while(line[i] == ' '){
                line.erase(line.begin());
            }
            code.push_back(line);
        }
        fin.close();

        // 전체 코드 잘 들어갔는지 확인
        cout << "< check total sample code input >" << endl;
        for(int i=0; i<code.size(); i++){
            cout << code[i] << endl;
        }
        cout << endl;

        // 전체 코드 중에서 data code만 따로 저장
        // 빈칸일 경우 dcode에 안 넣기
        int didx = findIndex(code, ".data");
        int tidx = findIndex(code, ".text");
        // cout << "didx : " << didx << endl;
        // cout << "tidx : " << tidx << endl;
        // cout << "code.size() : " << code.size() << endl;
        // cout << endl;
        
        cout << "< check data code input > " << endl;
        int dblank = 0;
        int dsize = tidx - didx - 1;
        cout << "dsize : " << dsize << endl;
        for(int i=didx+1; i<dsize+1; i++){
            if(code[i].empty() != true){
                dcode.push_back(code[i]);
            }
            else{
                dblank++;
                continue;
            }
        }
        dsize -= dblank;
        cout << "new dsize : " << dsize << endl;
        string input_dsize = string_to_hex(toBinary(dsize, 16));
        cout << "dsize in hex : " << input_dsize << endl;
        fout.open(outputFile);
        if(fout.is_open()){
            fout << input_dsize << endl;
        }
        fout.close();
        for(int i=0; i<dsize; i++){
            cout << dcode[i] << endl;
        }
        cout << endl;

        // 전체 코드 중에서 test code만 따로 저장
        // 빈칸일 경우 tcode에 안 넣기
        cout << "< check test code input > " << endl;
        int tblank = 0;
        int tsize = code.size() - tidx - 1;
        cout << "tsize : " << tsize << endl;
        for(int i=tidx+1; i<code.size(); i++){
            if(code[i].empty() != true) {
                tcode.push_back(code[i]);
            }
            else {
                tblank++;
                continue;
            }
        }
        tsize -= tblank;
        cout << "new tsize : " << tsize << endl;
        string input_tsize = string_to_hex(toBinary(tsize, 16));
        cout << "tsize in hex : " << input_tsize << endl;
        fout.open(outputFile);
        if(fout.is_open()){
            fout << input_tsize << endl;
        }
        fout.close();
        for(int i=0; i<tsize; i++){
            cout << tcode[i] << endl;
        }
        cout << endl;
        
        // label의 경우 뒤에 값이 존재하면 주소 부여. 뒤에 값이 없는 경우 주소 부여 x.
        // ex) "main:" 의 경우 뒤에 아무것도 없으므로 주소 부여 x
        // ex) "var: .word 5" 의 경우 뒤에 값이 있으므로 주소 부여해야함.
        // text에서 sum: 다음에 있는 instruction의 유무 판단 후 주소 부여해야함.
        /********************** data read **********************/
        // ":" 있을 때 label 저장하는 벡터에 저장해두기.
        // 나중에 write file 할 때 label이 나오면 그 주소값에 접근할 수 있게끔.

        for(int i=0; i<dsize; i++){
            if(dcode[i].find(":") != string::npos){ // ":"가 존재할 경우
                int index = dcode[i].find(":");
                string buffer_str = dcode[i].substr(index+1);
                if(buffer_str.empty() != true){ // ":"가 존재하는데 그 뒤에 문자열이 차있을 경우
                    string label = dcode[i].substr(0, index);
                    m.insert({label, dadd}); // label 이름(string)과 주소(int)를 map으로 저장
                    dadd+=4;
                }
                else continue; // ":"가 존재하는데 그 뒤에 문자열이 비었을 경우
            }
        }
        
        /********************** text read **********************/ // data read와 비슷한 과정
        for(int i=0; i<tsize; i++){
            if(tcode[i].find(":") != string::npos){ // ":"가 존재할 경우
                int index = tcode[i].find(":");
                string buffer_str = tcode[i].substr(index+1);
                if(buffer_str.empty() != true){ // ":" 가 존재하는데 그 뒤에 instruction이 있을 경우
                    string label = tcode[i].substr(0, index);
                    m.insert({label, tadd});
                    tadd+=4;
                }
                else{ // ":" 뒤에 문자열이 더 없는 경우 - 다음 instruction의 주소와 같은 값으로 저장
                    string label = tcode[i].substr(0, index);
                    m.insert({label, tadd});
                }
            }
            else{ // ":"가 없는 경우. 즉, 그냥 instruction일 경우
                m.insert({tcode[i], tadd});
                tadd+=4;
            }
        }

        /********************** write file **********************/

        cout << "< check address >" << endl;
        for(map<string, int>::iterator it = m.begin(); it != m.end(); ++it){
            cout << it->first << " : " << it->second << endl;
        }
        cout << endl;

        cout << "< check result >" << endl;
        string value;
        fout.open(outputFile);
        for(int i=0; i<tsize; i++){
            string result;
            vector<string> v = Split(tcode[i]);
            if(tcode[i].find(":") != string::npos){ // ":" 가 있을 경우
                int index = tcode[i].find(":");
                string str_buf = tcode[i].substr(index+1);
                if(str_buf.empty() != true){ // ":" 뒤에 instruction이 존재할 경우
                    vector<string> v_buf = Split(str_buf);
                    value = instructionConversion(v_buf);
                    result = string_to_hex(value);
                    cout << tcode[i] << " : " << result << endl;
                }
                else continue; // ":" 뒤에 intruction이 없을 경우
            }
            else{
                value = instructionConversion(v);
                cout << tcode[i] << " : " << value << endl;
                result = string_to_hex(value);
                cout << tcode[i] << " : " << result << endl;
            }
            if(fout.is_open()){
                fout << result << endl;
            }
        }
        
        cout << endl << "< one more data check >" << endl;
        cout << "dsize : " << dsize << endl;
        for(int i=0; i<dsize; i++){
            cout << dcode[i] << endl;
            vector<string> v = Split(dcode[i]);
            // for(int j=0; j<v.size(); j++){
            //     cout << v[j] << endl;
            // }
            int index = v.size();
            // cout << index << endl;
            string data_buf = v[index-1];
            string bin_data_buf;
            cout << data_buf << endl;
            if(data_buf.find("0x") != string::npos){
                int data_buf_dec = (int)strtol(data_buf.c_str(), NULL, 16);
                bin_data_buf = toBinary(data_buf_dec, 32);
            }
            else{
                bin_data_buf = toBinary(stoi(data_buf), 32);
            }
            cout << bin_data_buf << endl;
            cout << string_to_hex(bin_data_buf) << endl;
            fout << string_to_hex(bin_data_buf) << endl;
        }
        fout.close();
    }
    else cout << "this is not an appropriate command." << endl;

    return 0;
}
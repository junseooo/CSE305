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

int checkFormat(string str){ // R, I, J ���� format Ȯ��
    if(str == "addu" || str == "and" || str == "jr" || str == "nor" || str == "or" || str == "sltu" || str == "sll" || str == "srl" || str == "subu") return 1;
    else if(str == "addiu" || str == "andi" || str == "beq" || str == "bne" || str == "lui" || str == "lw" || str == "ori" || str == "sltiu" || str == "sw" || str == "lb" || str == "sb" || str == "la") return 2;
    else if(str == "j" || str == "jal") return 3;
    else return 0;
}

string instructionConversion(vector<string> v){
    string op;
    int op_dec; // string���� ���� 16������ 10������ ��ȯ�ϱ� ���� ����
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

    if(v[0].find(":") != string::npos){ // ù��° ���ҿ� ":"�� �����ϸ� �ι�° ���Ҹ� instruction���� Ȯ��
        
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
        if(v.size() == 3){ // instruction ������ 3���θ� �̷���� ���
            if(v[0] == "lui"){ // upper 16-bit�� imm(����)�� load
                op = "0xf";
                string buf_rt = v[1];
                rt = buf_rt.substr(1, buf_rt.size()-2);
                imm = v[2]; // ����

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
        else if(v.size() == 4){ // instruction ������ 4���� �̷���� ���
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

int main(int argc, char** argv){
    vector<string> code;
    vector<string> dcode; // data part�� �ڵ�
    vector<string> tcode; // text part�� �ڵ�
    vector<string> dlabel; // data part�� �� ����
    vector<string> tlabel; // text part�� �� ����
    code.clear();
    dcode.clear();
    tcode.clear();
    dlabel.clear();
    tlabel.clear();
    // data�� text�� �ڵ带 ���� ������
    // �ڵ忡�� Ư�� �󺧿� ������ �� �� �ּҸ� ��ȯ�� �� �ְԲ� �ؾ���

    int dadd = 0x10000000; // 8-bit�� ǥ���� 16����. -> binary�� ǥ�� �� 32-bit���� ǥ�� ����
    int tadd = 0x00400000;

    string inputFile = argv[1];

    string fileName = inputFile;

    cout << endl << "input file name : " << inputFile << endl;
    
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

    // ��ü �ڵ� �� ������ Ȯ��
    cout << "< check total sample code input >" << endl;
    for(int i=0; i<code.size(); i++){
        cout << code[i] << endl;
    }
    cout << endl;

    // ��ü �ڵ� �߿��� data code�� ���� ����
    // ��ĭ�� ��� dcode�� �� �ֱ�
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

    // ��ü �ڵ� �߿��� test code�� ���� ����
    // ��ĭ�� ��� tcode�� �� �ֱ�
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
    
    // label�� ��� �ڿ� ���� �����ϸ� �ּ� �ο�. �ڿ� ���� ���� ��� �ּ� �ο� x.
    // ex) "main:" �� ��� �ڿ� �ƹ��͵� �����Ƿ� �ּ� �ο� x
    // ex) "var: .word 5" �� ��� �ڿ� ���� �����Ƿ� �ּ� �ο��ؾ���.
    // text���� sum: ������ �ִ� instruction�� ���� �Ǵ� �� �ּ� �ο��ؾ���.
    /********************** data read **********************/
    // ":" ���� �� label �����ϴ� ���Ϳ� �����صα�.
    // ���߿� write file �� �� label�� ������ �� �ּҰ��� ������ �� �ְԲ�.

    for(int i=0; i<dsize; i++){
        if(dcode[i].find(":") != string::npos){ // ":"�� ������ ���
            int index = dcode[i].find(":");
            string buffer_str = dcode[i].substr(index+1);
            if(buffer_str.empty() != true){ // ":"�� �����ϴµ� �� �ڿ� ���ڿ��� ������ ���
                string label = dcode[i].substr(0, index);
                m.insert({label, dadd}); // label �̸�(string)�� �ּ�(int)�� map���� ����
                dadd+=4;
            }
            else continue; // ":"�� �����ϴµ� �� �ڿ� ���ڿ��� ����� ���
        }
    }
    
    /********************** text read **********************/ // data read�� ����� ����
    for(int i=0; i<tsize; i++){
        if(tcode[i].find(":") != string::npos){ // ":"�� ������ ���
            int index = tcode[i].find(":");
            string buffer_str = tcode[i].substr(index+1);
            if(buffer_str.empty() != true){ // ":" �� �����ϴµ� �� �ڿ� instruction�� ���� ���
                string label = tcode[i].substr(0, index);
                m.insert({label, tadd});
                tadd+=4;
            }
            else{ // ":" �ڿ� ���ڿ��� �� ���� ��� - ���� instruction�� �ּҿ� ���� ������ ����
                string label = tcode[i].substr(0, index);
                m.insert({label, tadd});
            }
        }
        else{ // ":"�� ���� ���. ��, �׳� instruction�� ���
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
        if(tcode[i].find(":") != string::npos){ // ":" �� ���� ���
            int index = tcode[i].find(":");
            string str_buf = tcode[i].substr(index+1);
            if(str_buf.empty() != true){ // ":" �ڿ� instruction�� ������ ���
                vector<string> v_buf = Split(str_buf);
                value = instructionConversion(v_buf);
                result = string_to_hex(value);
                cout << tcode[i] << " : " << result << endl;
            }
            else continue; // ":" �ڿ� intruction�� ���� ���
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


    return 0;
}
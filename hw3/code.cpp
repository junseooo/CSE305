#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////function/////////////////////////////////////////////////////////////////////////////////////
int binary_to_int(std::string binary_str)
{ ///동작 ok
    int a = 1;
    int num = 0;
    for (int i = binary_str.length() - 1; i >= 0; i--)
    {
        for (int j = 0; j < binary_str.length() - i - 1; j++)
        {
            a *= 2;
        }
        if (binary_str[i] == '1')
        {
            num += a;
        }
        a = 1;
    }
    return num;
}

std::string int_to_hex(int num)
{ ///16진수 string으로 변환 //동작 ok
    std::stringstream ss;
    std::string str;
    ss << std::hex << num;
    str = ss.str();
    return str;
}

int change_instruction(std::string instruction, int start, int final)
{ /// 동작 ok
    std::string bit_num;
    for (auto iter = instruction.begin() + start; iter != instruction.begin() + final; iter++)
    {
        bit_num.push_back(*iter);
    }
    int int_num = binary_to_int(bit_num);
    return int_num;
}

std::vector<int> change_to_R_format_instruction(std::vector<std::string> string_vec_instruction)
{ //동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int int_rs = change_instruction(instruction, 6, 11);
    int int_rt = change_instruction(instruction, 11, 16);
    int int_rd = change_instruction(instruction, 16, 21);
    int int_shamt = change_instruction(instruction, 21, 26);
    int int_funct = change_instruction(instruction, 26, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(int_rs);
    decimal_instruction.push_back(int_rt);
    decimal_instruction.push_back(int_rd);
    decimal_instruction.push_back(int_shamt);
    decimal_instruction.push_back(int_funct);
    return decimal_instruction;
}

std::vector<int> change_to_I_format_instruction(std::vector<std::string> string_vec_instruction)
{ ///동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int int_rs = change_instruction(instruction, 6, 11);
    int int_rt = change_instruction(instruction, 11, 16);
    int int_immediate = change_instruction(instruction, 16, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(int_rs);
    decimal_instruction.push_back(int_rt);
    decimal_instruction.push_back(int_immediate);
    return decimal_instruction;
}

std::vector<int> change_to_J_format_instruction(std::vector<std::string> string_vec_instruction)
{ ///동작 ok
    std::string instruction;
    for (int i = 0; i < string_vec_instruction.size(); i++)
    {
        instruction += string_vec_instruction[i];
    }
    std::vector<int> decimal_instruction;
    int int_opcode = change_instruction(instruction, 0, 6);
    int jump_target = change_instruction(instruction, 6, 32);
    decimal_instruction.push_back(int_opcode);
    decimal_instruction.push_back(jump_target);
    return decimal_instruction;
}

std::vector<int> find_address_index_for_branch(std::vector<std::vector<int>> instruction_or_data_address, int address_in_regi)
{
    std::vector<int> index;
    for (auto iter = instruction_or_data_address.begin(); iter != instruction_or_data_address.end(); iter++)
    {
        for (int n = 0; n < (*iter).size(); n++)
        {
            if ((*iter)[n] == address_in_regi)
            {
                index.push_back(iter - instruction_or_data_address.begin());
                index.push_back(n);
                return index;
            }
        }
    }
}

std::string value_sign_extension(std::string data, int quantity)
{ ///checked
    std::string num_extension;
    if (data.front() == '0')
    {
        for (int num = 0; num < quantity; num++)
        {
            num_extension.push_back('0');
        }
    }
    else
    {
        for (int num = 0; num < quantity; num++)
        {
            num_extension.push_back('1');
        }
    }
    return num_extension + data;
}

std::bitset<32> change_to_bitset(std::string instruction_32bit)
{ ///checked
    std::bitset<32> bitset_instruction = 0;
    for (int n = 0; n < 32; n++)
    {
        if (instruction_32bit[31 - n] == '1')
        {
            bitset_instruction.set(n);
        }
    }
    return bitset_instruction;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Myformats
{
public:
    Myformats()
    {
        opcode = 0;
        instruction_name = "none";
    }
    virtual ~Myformats() {};

    int opcode;
    std::string instruction_name;
};

class R_format : public Myformats
{
public:
    std::string type_name = "R";
    int rs, rt, rd, shamt, funct;


    std::bitset<32> operation(std::vector<std::bitset<32>>& my_register)
    { ////register 상태와 pc table 받아야함.
        int rs_value = int(my_register[rs].to_ullong()); //ullong
        int rt_value = int(my_register[rt].to_ullong()); //ullong
        int rd_value = 0;

        if (instruction_name == "addu")
        {
            rd_value = rs_value + rt_value;
            return std::bitset<32>(rd_value);
        } ////checked

        if (instruction_name == "and")
        {
            return (my_register[rs] & my_register[rt]);
        } ///checked

        if (instruction_name == "nor")
        {
            return (~(my_register[rs] | my_register[rt]));
        } ///checked

        if (instruction_name == "or")
        {
            return (my_register[rs] | my_register[rt]);
        } ///checked

        if (instruction_name == "sltu")
        {
            if (rs_value < rt_value)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        } ///checked

        if (instruction_name == "sll")
        {
            return (my_register[rt] << shamt);
        } ///checked

        if (instruction_name == "srl")
        {
            return (my_register[rt] >> shamt);
        } ///checked

        if (instruction_name == "subu")
        {
            rd_value = rs_value - rt_value;
            return std::bitset<32>(rd_value);
        } ///checked

        if (instruction_name == "jr") {//////////////////////////////////////////rs로 분기하는것 좀더 생각필요
            return my_register[rs];
        }
    }

    std::vector<int> operation_jr(std::vector<std::bitset<32>> my_register, std::vector<std::vector<int>> instruction_address)
    { ///pc_num에서 rs안에 들어있는 주소와 같은 값의 index를 구한다.

        ///return my_register[rs];
        int decimal_num = int(my_register[rs].to_ullong());
        std::vector<int> index = find_address_index_for_branch(instruction_address, decimal_num);
        return index;
    } ///checked 만약 찾지못하면??


    R_format()
    {
        rs = rt = rd = shamt = funct = 0;
    }
    R_format(std::string name, int op, int funct_num)
    {
        rs = rt = rd = shamt = funct = 0;
        instruction_name = name;
        opcode = op;
        funct = funct_num;
    }
    virtual ~R_format() {};
};

class I_format : public Myformats
{
public:
    std::string type_name = "I";
    int rs, rt, immediate_or_address;
    int32_t rt_value = 0;

    std::vector<int> operation_beq(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& instruction_address, int big_index, int small_index)
    { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() == my_register[rt].to_string())
        {
            int32_t target_address = instruction_address[big_index][small_index] + 4 + immediate_or_address * 4;
            std::vector<int> branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else
        {
            return { -1, 0 }; //rs와 rt가 다르면 -1 리턴.
        }
    } ///checked

    std::vector<int> operation_bne(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& instruction_address, int big_index, int small_index)
    { ///분기해야할 pc index찾아주기
        if (my_register[rs].to_string() != my_register[rt].to_string())
        {
            int target_address = instruction_address[big_index][small_index] + 4 + immediate_or_address * 4;
            std::vector<int> branch_idx = find_address_index_for_branch(instruction_address, target_address);
            return branch_idx;
        }
        else
        {
            return { -1, 0 }; //rs와 rt가 다르면 -1 리턴.
        }
    } ///checked

    std::bitset<32> operation_lw_or_lb(std::vector<std::vector<std::string>>& data_memory, std::vector<int> address_index)
    {
        if (instruction_name == "lw")
        {
            std::string word;
            for (int a = 0; a < 4; a++) {

                word += data_memory[address_index[0]][a];
            }
            return change_to_bitset(word);
            //my_register[rt] = change_to_bitset(word); ////std::vector<std::bitset<32> my_register[0] = 15; 하면 2진수로 들어가는가 체크체크하기
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            std::string num_extension = value_sign_extension(one_byte_data, 24);
            return change_to_bitset(num_extension);
            //my_register[rt] = change_to_bitset(num_extension);
        }
    } //checked
    void operation_sw_or_sb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<std::string>>& data_memory, std::vector<int> address_index)
    {
        rt_value = int32_t(my_register[rt].to_ullong());
        if (instruction_name == "sw")
        {
            std::string a = std::bitset<32>(rt_value).to_string();
            std::string temp;
            std::vector<std::string> cont;
            for (int num = 0; num < 4; num++) {
                for (int num2 = 0 + num * 8; num2 < 8 + num * 8; num2++) {
                    temp.push_back(a[num2]);
                }
                cont.push_back(temp);
                temp.clear();
            }
            data_memory[address_index[0]] = cont;
        }
        else
        {
            std::string one_byte_data;
            for (int n = 0; n < 8; n++)
            {
                one_byte_data.push_back(data_memory[address_index[0]][address_index[1]][n]);
            }
            data_memory[address_index[0]][address_index[1]] = one_byte_data;
        }
    }

    std::vector<int> address_idx_cal_lw_lb_sw_sb(std::vector<std::bitset<32>>& my_register, std::vector<std::vector<int>>& data_address) {
        int32_t rs_value = int32_t(my_register[rs].to_ullong());
        int32_t address = rs_value + immediate_or_address;
        std::vector<int> address_index = find_address_index_for_branch(data_address, address);
        return address_index;
    }///data memory와 data address 크기 늘려주기. assign만 되면 된다.

    std::bitset<32> operation(std::vector<std::bitset<32>>& my_register)
    {
        int32_t rs_value = int32_t(my_register[rs].to_ullong());
        int32_t rt_value = int32_t(my_register[rt].to_ullong());

        if (instruction_name == "addiu")
        {                                                                                                        ///////rs와 sign-extened된 immediate의 합을 rt에 저장.
            std::string extension = value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16); ///32비트 extension된 immediate
            std::bitset<32> bitset_extension = change_to_bitset(extension);
            int32_t rt_value = (int32_t)bitset_extension.to_ullong() + rs_value;
            return rt_value;
            //my_register[rt] = std::bitset<32>(rt_value);
            //return;
        } ////checked

        if (instruction_name == "andi")
        {
            return (my_register[rs] & std::bitset<32>(immediate_or_address));
            //my_register[rt] = (my_register[rs] & std::bitset<32>(immediate_or_address));
            //return;
        } ///checked

        if (instruction_name == "lui")
        {
            std::bitset<32> container;
            std::bitset<16> imm_16bit(immediate_or_address);
            for (int idx = 0; idx < 32; idx++)
            {
                if (idx < 16)
                {
                    container[idx] = 0;
                    //my_register[rt][idx] = 0;
                }
                else
                {
                    container[idx] = imm_16bit[idx - 16];
                    //my_register[rt][idx] = imm_16bit[idx - 16];
                }
            }
            return container;
        } ///checked

        if (instruction_name == "ori")
        {
            return (my_register[rs] | std::bitset<32>(immediate_or_address));
            //my_register[rt] = (my_register[rs] | std::bitset<32>(immediate_or_address));
            //return;
        } ///checked

        if (instruction_name == "sltiu")
        { ///rs가 sign_extened imm보다 작으면 rt 1, 아니면 0
            std::string a = (value_sign_extension(std::bitset<16>(immediate_or_address).to_string(), 16));
            std::bitset<32> b = change_to_bitset(a);
            if (rs_value < int32_t(b.to_ullong()))
            {
                return 1;
                //my_register[rt] = 1;
                //return;
            }
            else
            {
                return 0;
                //my_register[rt] = 0;
                //return;
            }
        } ///checked
    }

    I_format()
    {
        rs = rt = immediate_or_address = 0;
    }
    I_format(std::string name, int op_num)
    {
        rs = rt = immediate_or_address = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~I_format() {};
};

class J_format : public Myformats
{
public:
    std::string type_name = "J";
    int jump_target;
    std::string j_operation(int pc)
    { ///checked
        jump_target = jump_target * 4;

        std::bitset<28> jump_target_28bit(jump_target);
        std::string str_jump_target_28bit = jump_target_28bit.to_string();

        std::bitset<32> pc_32bit(pc);
        std::string str_pc_32bit = pc_32bit.to_string();

        std::string upper_pc_4bit;
        for (int i = 0; i < 4; i++)
        {
            upper_pc_4bit.push_back(str_pc_32bit[i]);
        }

        std::string jump_target_32bit = upper_pc_4bit + str_jump_target_28bit;
        return jump_target_32bit;
    }

    J_format()
    {
        jump_target = 0;
    }
    J_format(std::string name, int op_num)
    {
        jump_target = 0;
        instruction_name = name;
        opcode = op_num;
    }
    virtual ~J_format() {};
};


class IF_ID {
public:
    int pc = 0;
    std::vector<int> index = { -1,-1 };
    IF_ID() {}
};

class ID_EX {
public:
    int pc = 0;
    std::vector<int> index = { -1, -1 };
    std::string instruction_type = "none";
    std::string instruction_name = "none";
    R_format* r_formats = nullptr; //어떤 format의 인스턴스인지 확인하기 위해서
    I_format* i_formats = nullptr;
    J_format* j_formats = nullptr;
    bool register_write = false;
    int rs = -1;
    int rt = -1;
    int rd = -1;
    ID_EX() {}
};

class EX_MEM {
public:
    std::string instruction_type = "none";
    std::string instruction_name = "none";
    int pc = 0;
    std::vector<int> index = { -1,-1 };
    bool register_write = false;
    int rs = -1;
    int rt = -1;
    int rd = -1;
    std::bitset<32> ALU_result = 0;
    I_format* i_formats = nullptr;
    EX_MEM() {}

};
class MEM_WB {
public:
    int pc = 0;
    std::vector<int> index{ -1,-1 };
    std::string instruction_name = "none";
    std::string instruction_type = "none";
    std::bitset<32> ALU_result = 0; //arithmatic인 경우 사용
    bool register_write = false;
    int rs = -1;
    int rt = -1;
    int rd = -1;
    std::bitset<32> data = 0; //lw,lb인 경우 사용
    MEM_WB() {}
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::string command;
    getline(std::cin, command);

    std::istringstream start_command(command);
    std::vector<std::string> command_parsing;
    while (start_command >> command)
    {
        command_parsing.push_back(command);
    }

    if (command_parsing[0] == "./runfile")
    {
        bool exist_m = false;
        bool exist_d = false;
        bool exist_n = false;
        bool exist_p = false;
        int memory_n;
        int command_n_instruction;
        for (int n = 0; n < command_parsing.size(); n++)
        {
            if (command_parsing[n] == "-m")
            {
                exist_m = true;
                memory_n = n + 1;
                continue;
            }
            if (command_parsing[n] == "-d")
            {
                exist_d = true;
                continue;
            }
            if (command_parsing[n] == "-n")
            {
                exist_n = true;
                command_n_instruction = stoi(command_parsing[n + 1]);
                continue;
            }
            if (command_parsing[n] == "-p") {
                exist_p = true;
                continue;
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////0-31 reg 초기화 및 pc 설정///////////////////////////////////////////////////////////////

        std::vector<std::bitset<32>> my_register(32, 0);


        int start_instruction_address = 0x400000; ///txt 영역 시작주소 (pc)
        int end_instruction_address;
        std::vector<int> byte_instruction_address(4);
        byte_instruction_address[0] = start_instruction_address;
        std::vector<std::vector<int>> instruction_address;
        instruction_address.push_back(byte_instruction_address); ////////////////asdfasdfasdf

        int start_data_address = 0x10000000; ///data영역 시작주소
        int end_data_address;
        std::vector<int> byte_data_address;
        std::vector<std::vector<int>> data_address;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        R_format addu("addu", 0, 0x21), _and("and", 0, 0x24), jr("jr", 0, 8), nor("nor", 0, 0x27), _or("or", 0, 0x25), sltu("sltu", 0, 0x2b), sll("sll", 0, 0), srl("srl", 0, 2), subu("subu", 0, 0x23);
        I_format addiu("addiu", 9), andi("andi", 0xc), beq("beq", 4), bne("bne", 5), lui("lui", 0xf), lw("lw", 0x23), ori("ori", 0xd), sltiu("sltiu", 0xb), sw("sw", 0x2b), lb("lb", 0x20), sb("sb", 0x28);
        J_format j("j", 2), jal("jal", 3);

        std::vector<R_format> R{ addu, _and, jr, nor, _or, sltu, sll, srl, subu };
        std::vector<I_format> I{ addiu, andi, beq, bne, lui, lw, ori, sltiu, sw, lb, sb };
        std::vector<J_format> J{ j, jal };
        std::vector<R_format> R_buffer{};
        std::vector<I_format> I_buffer{};
        std::vector<J_format> J_buffer{};

        std::string one_byte_instruction;
        std::vector<std::string> four_byte_instruction;
        std::vector<std::vector<std::string>> instruction_memory;
        instruction_memory.clear();

        std::string one_byte_data;
        std::vector<std::string> four_byte_data;
        std::vector<std::vector<std::string>> data_memory;
        data_memory.clear();

        int txt_section_size;
        int data_section_size;

        std::ifstream reading;
        reading.open(command_parsing.back());
        if (reading.is_open())
        {
            int line_num = 0;                           ///object파일의 몇번째 줄인지 확인
            int end_line_instruction_memory_plus_1 = 0; ////instruction의 끝 줄 확인
            std::string hex_num;                        ////getline을 사용
            int64_t decimal_num;
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //// 바이너리 파일 읽어오기
            while (!reading.eof())
            {
                getline(reading, hex_num);
                const char* transform = hex_num.c_str();
                decimal_num = strtoul(transform, NULL, 16); ////16진수 10진수로 바꾸기
                if (line_num == 0)
                { ////text section size 설정
                    txt_section_size = decimal_num;
                    end_instruction_address = start_instruction_address + txt_section_size;
                    end_line_instruction_memory_plus_1 = 2 + (txt_section_size / 4); /// input file에서 txt size, data size를 포함한 line number
                    line_num++;
                    continue;
                }
                if (line_num == 1)
                { ////data section size 설정
                    data_section_size = decimal_num;
                    end_data_address = start_data_address + data_section_size;
                    line_num++;
                    continue;
                }
                if (line_num > 1)
                {
                    std::bitset<32> bit_num(decimal_num);          //////32비트로 나타내기
                    std::string bit_oneline = bit_num.to_string(); ///// 32비트 이진수를 string으로

                    if (line_num < end_line_instruction_memory_plus_1)
                    {
                        for (int i = 0; i < 4; i++)
                        {
                            for (int n = 0 + i * 8; n < 8 + i * 8; n++)
                            {
                                one_byte_instruction.push_back(bit_oneline[n]);
                            }
                            four_byte_instruction.push_back(one_byte_instruction);
                            one_byte_instruction.clear();
                        }
                        instruction_memory.push_back(four_byte_instruction);
                        four_byte_instruction.clear();

                        if (line_num == 2)
                        {
                            instruction_address[0][1] = byte_instruction_address[0] + 1;
                            instruction_address[0][2] = byte_instruction_address[0] + 2;
                            instruction_address[0][3] = byte_instruction_address[0] + 3;
                        }
                        if (line_num > 2)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                byte_instruction_address.push_back(start_instruction_address + 4 * (line_num - 2) + i);
                            }
                            instruction_address.push_back(byte_instruction_address); /////pc 설정 (10진수)
                        }
                        byte_instruction_address.clear();
                        line_num++;
                        continue;
                    }

                    else
                    {
                        if (line_num == end_line_instruction_memory_plus_1) {
                            int end_address = (instruction_address.back())[3];
                            std::vector<int> end = { end_address + 1,end_address + 2, end_address + 3, end_address + 4 };
                            instruction_address.push_back(end);
                        }

                        if (line_num < end_line_instruction_memory_plus_1 + data_section_size / 4)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                for (int n = 0 + i * 8; n < 8 + i * 8; n++)
                                {
                                    one_byte_data.push_back(bit_oneline[n]);
                                }
                                four_byte_data.push_back(one_byte_data);
                                one_byte_data.clear();
                            }
                            data_memory.push_back(four_byte_data);
                            four_byte_data.clear();

                            for (int i = 0; i < 4; i++)
                            {
                                byte_data_address.push_back(start_data_address + 4 * (line_num - end_line_instruction_memory_plus_1) + i);
                            }
                            data_address.push_back(byte_data_address); ///data memory address 설정
                            byte_data_address.clear();
                            line_num++;
                            continue;
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }
            reading.close();
        }

        /////////////////////////////////////////////////instruction memory, data memory 생성됨 ///////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////.///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int instruction_memory_big_index = 0;
        int instruction_memory_small_index = 0;
        int data_memory_big_index = 0;
        int data_memory_small_index = 0;
        std::vector<std::bitset<32>> hazard_register;
        std::vector<int*> index = { &instruction_memory_big_index, &instruction_memory_small_index };

        /////////////////////////////////////////////////////////pipeline test 코드///////////////////////////////////////////////////////////////////

        bool data_hazard = false;
        bool load_use_hazard = false;

        //////////////////////////////////////////////////////////////////////////// IF/ID 상태레지스터 ///////////////////////////////////
        std::vector<std::string> instruction_32bit_string = { "none" };
        int IF_pc = 0;
        std::vector<int> IF_index = { -1,-1 };

        IF_ID IF_ID_reg;
        IF_ID* IF_ID_ptr = &IF_ID_reg;
        //////////////////////////////////////////////////////////////////////////// ID STAGE 레지스터, ID/EX 상태레지스터 ///////////////////////////////////
        std::string ID_instruction_name = "none";
        std::string ID_instruction_type = "none";
        R_format* ID_r_format = nullptr;
        I_format* ID_i_format = nullptr;
        J_format* ID_j_format = nullptr;
        bool ID_register_write = false;
        int ID_rs = 0;
        int ID_rt = 0;
        int ID_imm = 0;
        int ID_rd = 0;
        int ID_shamt = 0;
        int ID_pc = 0;

        ID_EX ID_EX_reg;
        ID_EX* ID_EX_ptr = &ID_EX_reg;
        //////////////////////////////////////////////////////////////////////////// EX STAGE 레지스터, EX/MEM 상태레지스터 ////////////////////////////////////////////////////
        std::bitset<32> EX_result = 0;
        EX_MEM EX_MEM_reg;
        //////////////////////////////////////////////////////////////////////////// MEM STAGE 레지스터, MEM/WB 상태레지스터////////////////////////////////////////////////////////
        std::bitset<32> MEM_result_for_lw = 0;
        MEM_WB MEM_WB_reg;
        ////////////////////////////////////////////////////////////////////////////
        std::deque<std::vector<std::string>> instruction_pipeline = { {"none"}, {"none"}, {"none"}, {"none"}, {"none"} };
        std::string start_signal = "start";
        ID_EX ID_EX_stall;
        IF_ID IF_ID_stall;

        int cycle = 0;
        while (instruction_memory_big_index < instruction_address.size()) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            ////////////////////////////////////////////////////////////IF STAGE 시작 //////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////Fetching 단계에서는 pc+4와 instruction memory에 접근해서 instruction을 꺼내옴.//////////
            if (instruction_memory_big_index == instruction_address.size() -1) {
                if (MEM_WB_reg.pc == 0) {
                    break;
                }
                instruction_32bit_string = { "" };
                std::string temp;
                for (int a = 0; a < 4; a++) {
                    for (int i = 0; i < 8; i++) {
                        temp.push_back('0');
                    }
                    instruction_32bit_string.push_back(temp);
                    temp.clear();
                }
                instruction_pipeline.pop_back();
                instruction_pipeline.push_front(instruction_32bit_string);

                instruction_memory_big_index--;
                IF_pc = 0;
                IF_index = { -1,-1 };

            }

            else {
                if (load_use_hazard == false) {
                    instruction_pipeline.pop_back();

                    instruction_32bit_string = instruction_memory[instruction_memory_big_index];             /////instruction memory에 접근 + 32bit instruction 가져옴.
                    instruction_pipeline.push_front(instruction_32bit_string);

                    IF_pc = instruction_address[instruction_memory_big_index][instruction_memory_small_index];
                    IF_index = { instruction_memory_big_index, instruction_memory_small_index };

                    //std::cout << "IF START" << std::endl;
                    //std::cout << "index = " << "{" << IF_index[0] << "," << IF_index[1] << "}" << std::endl;
                }
            }

            cycle++;
            ////////////////////////////////////////////////////////////ID STAGE 시작 //////////////////////////////////////////////////////////////////////////


            if (load_use_hazard == false) {
                if (IF_ID_reg.pc != 0) {
                    //std::cout << "ID START" << std::endl;

                    std::vector<int> int_instruction;
                    std::string ID_stage_type_checking = "none";
                    ID_register_write = false; // 초기화 시켜주기 위해서

                    if (ID_stage_type_checking == "none") {
                        int_instruction = change_to_R_format_instruction(instruction_pipeline[1]);
                        for (auto r = R.begin(); r != R.end(); r++) {
                            if ((r->opcode) == int_instruction[0] && (r->funct) == int_instruction[5]) {
                                //std::cout << r->instruction_name << std::endl; ////test////
                                R_format* instruction = new R_format(r->instruction_name, r->opcode, r->funct);

                                ID_rs = instruction->rs = int_instruction[1];
                                ID_rt = instruction->rt = int_instruction[2];
                                ID_rd = instruction->rd = int_instruction[3];
                                ID_shamt = instruction->shamt = int_instruction[4];
                                ID_instruction_type = (instruction->type_name);
                                ID_instruction_name = (instruction->instruction_name);
                                //R_buffer.push_back(instruction);
                                ID_r_format = instruction; //&R_buffer.back();
                                ID_i_format = nullptr;
                                ID_j_format = nullptr;
                                if (instruction->rd != -1) {
                                    ID_register_write = true;
                                }
                                if (instruction->instruction_name == "jr") {
                                    std::vector<int> index = instruction->operation_jr(my_register, instruction_address);
                                    instruction_memory_big_index = index[0];
                                    IF_ID_ptr = &IF_ID_stall;
                                }
                                break;
                            }
                        }
                    }

                    if (ID_stage_type_checking == "none") {
                        int_instruction = change_to_I_format_instruction(instruction_pipeline[1]);
                        for (auto i = I.begin(); i != I.end(); i++) {
                            if (i->opcode == int_instruction[0]) {
                                //std::cout << i->instruction_name << std::endl; ////test////
                                I_format* instruction = new I_format(i->instruction_name, i->opcode);
                                ID_rs = instruction->rs = int_instruction[1];
                                ID_rt = instruction->rt = int_instruction[2];
                                ID_imm = instruction->immediate_or_address = int_instruction[3];
                                ID_instruction_type = (instruction->type_name);
                                ID_instruction_name = (instruction->instruction_name);
                                //I_buffer.push_back(instruction);
                                ID_r_format = nullptr;
                                ID_i_format = instruction;
                                ID_j_format = nullptr;

                                if (instruction->rt != -1) {
                                    if (ID_instruction_name != "sw" && ID_instruction_name != "sb") {
                                        ID_register_write = true;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    if (ID_stage_type_checking == "none") {
                        ///////////////////////////////////////////////////////j foramt은 id stage에서 종료됨///////////////////////////////////////////////////////////
                        int_instruction = change_to_J_format_instruction(instruction_pipeline[1]);
                        for (auto j = J.begin(); j != J.end(); j++) {
                            if (j->opcode == int_instruction[0]) {
                                //std::cout << j->instruction_name << std::endl;
                                J_format* instruction = new J_format(j->instruction_name, j->opcode);

                                ID_imm = instruction->jump_target = int_instruction[1];
                                ID_instruction_type = (instruction->type_name);
                                ID_instruction_name = (instruction->instruction_name);
                                //J_buffer.push_back(instruction);
                                ID_r_format = nullptr;
                                ID_i_format = nullptr;
                                ID_j_format = instruction;
                                if (instruction->instruction_name == "j" || instruction->instruction_name == "jal") {
                                    if (instruction->instruction_name == "jal") {
                                        std::vector<int> new_idx = { instruction_memory_big_index, instruction_memory_small_index }; ///돌아올 pc를 저장해야한다.
                                        my_register[31] = instruction_address[new_idx[0] + 1][0]; ///ra에 다음 실행할 ins의 주소를 저장. [new_idx[1]
                                    }
                                    instruction->j_operation(IF_ID_reg.pc);
                                }
                                break;
                            }
                        }
                    }
                    start_signal = "not start";
                }
            }



            ////////////////////////////////////////////////////////////EX STAGE 시작 //////////////////////////////////////////////////////////////////////////

            if (load_use_hazard == false) {
                if (ID_EX_reg.pc != 0) {
                    //std::cout << "EX START" << std::endl;
                    //std::cout << ID_EX_reg.instruction_name << std::endl;
                    std::vector<std::bitset<32>>* register_ptr = &my_register;
                    if (data_hazard == true) {
                        register_ptr = &hazard_register;
                    }


                    if (ID_EX_reg.instruction_type == "R") {
                        EX_result = (*(ID_EX_reg.r_formats)).operation(*register_ptr); ///jr 처리해주기
                    }
                    if (ID_EX_reg.instruction_type == "I") {

                        if (ID_EX_reg.instruction_name == "lw" || ID_EX_reg.instruction_name == "lb" || ID_EX_reg.instruction_name == "sw" || ID_EX_reg.instruction_name == "sb") {
                            std::vector<int> new_index = (*(ID_EX_reg.i_formats)).address_idx_cal_lw_lb_sw_sb(*register_ptr, data_address);
                            //EX_MEM_reg.BRANCH_TARGET = new_index;

                            //std::cout << ID_EX_reg.instruction_name << "'s ex_stage(calculation address) complete" << std::endl;
                            int address = data_address[new_index[0]][new_index[1]]; //address찾았음
                            EX_result = std::bitset<32>(address);
                        }
                        else {
                            if (ID_EX_reg.instruction_name == "beq" || ID_EX_reg.instruction_name == "bne") {
                                std::vector<int> new_index;
                                if (ID_EX_reg.instruction_name == "beq") {
                                    new_index = (*(ID_EX_reg.i_formats)).operation_beq(*register_ptr, instruction_address, ID_EX_reg.index[0], ID_EX_reg.index[1]);
                                }
                                if (ID_EX_reg.instruction_name == "bne") {
                                    new_index = (*(ID_EX_reg.i_formats)).operation_bne(*register_ptr, instruction_address, ID_EX_reg.index[0], ID_EX_reg.index[1]);
                                }
                                if (new_index[0] != -1) { //beq가 성립한다는 뜻
                                    int address = instruction_address[new_index[0]][new_index[1]];///new_index의 주소 리턴
                                    EX_result = std::bitset<32>(address);
                                }
                                else {

                                }
                            }
                            else {
                                EX_result = (*(ID_EX_reg.i_formats)).operation(*register_ptr); ///->operation(*register_ptr);
                            }
                        }
                    }
                    if (ID_EX_reg.instruction_type == "J") {

                    }
                }
            }

            ////////////////////////////////////////////////////////////MEM STAGE 시작 //////////////////////////////////////////////////////////////////////////

            if (EX_MEM_reg.pc != 0) {
                //std::cout << "MEM START" << std::endl;
                //std::cout << EX_MEM_reg.instruction_name << std::endl;
                if (EX_MEM_reg.instruction_type == "I") {
                    int32_t address = int32_t(EX_MEM_reg.ALU_result.to_ullong());
                    std::vector<int> address_index = find_address_index_for_branch(data_address, address);
                    //////////////////////////////////////////////////////// lw, lb는 wb stage에서 register에 써줘야함////////////////////////////////////////////////////
                    if (EX_MEM_reg.instruction_name == "lw" || EX_MEM_reg.instruction_name == "lb") {
                        std::bitset<32> word = (*(EX_MEM_reg.i_formats)).operation_lw_or_lb(data_memory, address_index); ///data를 가져왔음.

                        //std::cout << EX_MEM_reg.instruction_name << " is started " << std::endl;
                        //std::cout << "the index of address is " << address_index[0] << "," << address_index[1] << std::endl;

                        MEM_result_for_lw = word; ///////////  string으로 된 32bit data memory의 주소 리턴.
                    }
                    //////////////////////////////////////////////////////// sw, sb는 mem stage에서 종료///////////////////////////////////////////////////////////////////
                    if (EX_MEM_reg.instruction_name == "sw" || EX_MEM_reg.instruction_name == "sb") {
                        (*(EX_MEM_reg.i_formats)).operation_sw_or_sb(my_register, data_memory, address_index);
                        if (EX_MEM_reg.instruction_name == "sw") {
                            //std::cout << "sw complete" << std::endl;
                        }
                        else {
                            //std::cout << "sb complete" << std::endl;
                        }
                    }
                }
                else {
                    //std::cout << "mem_stage of " << EX_MEM_reg.instruction_name << ". This instruction doesn't need memory access" << std::endl;
                }
            }

            ////////////////////////////////////////////////////////////WB STAGE 시작 //////////////////////////////////////////////////////////////////////////
            if (MEM_WB_reg.pc != 0) {
                if (MEM_WB_reg.instruction_type == "R") {
                    int rd = MEM_WB_reg.rd;
                    my_register[rd] = MEM_WB_reg.ALU_result; // e
                }
                if (MEM_WB_reg.instruction_type == "I") {
                    int rt = MEM_WB_reg.rt;
                    if (MEM_WB_reg.instruction_name == "lw" || MEM_WB_reg.instruction_name == "lb") {
                        my_register[rt] = MEM_WB_reg.data;
                    }
                    if (MEM_WB_reg.instruction_name == "sw" || MEM_WB_reg.instruction_name == "lb") {
                    }
                    else {
                        my_register[rt] = MEM_WB_reg.ALU_result;
                    }
                }
                if (MEM_WB_reg.instruction_type == "J") {
                }
            }

            //////////////////////////////////////////////////////////상태레지스터 업데이트//////////////////////////////////////////////////////////////////////////
            //std::cout << "-------------------------------------------------STATE REGISTER UPDATE-------------------------------------------------" << std::endl;
            data_hazard = false; //해저드 초기화

            int end_pc = MEM_WB_reg.pc;
            MEM_WB_reg.pc = EX_MEM_reg.pc;
            MEM_WB_reg.index = EX_MEM_reg.index;
            MEM_WB_reg.instruction_name = EX_MEM_reg.instruction_name;
            MEM_WB_reg.instruction_type = EX_MEM_reg.instruction_type;
            MEM_WB_reg.ALU_result = EX_MEM_reg.ALU_result;
            MEM_WB_reg.rt = EX_MEM_reg.rt;
            MEM_WB_reg.rd = EX_MEM_reg.rd;
            MEM_WB_reg.data = MEM_result_for_lw; // MEM STAGE의 결과값 저장.
            MEM_WB_reg.register_write = EX_MEM_reg.register_write;

            EX_MEM_reg.pc = ID_EX_ptr->pc;
            EX_MEM_reg.index = ID_EX_ptr->index;
            EX_MEM_reg.instruction_name = ID_EX_ptr->instruction_name;
            EX_MEM_reg.instruction_type = ID_EX_ptr->instruction_type;
            EX_MEM_reg.rs = ID_EX_ptr->rs;
            EX_MEM_reg.rt = ID_EX_ptr->rt;
            EX_MEM_reg.rd = ID_EX_ptr->rd;
            EX_MEM_reg.ALU_result = EX_result; // EX stage의 결과값을 저장.
            EX_MEM_reg.i_formats = ID_EX_ptr->i_formats;
            EX_MEM_reg.register_write = ID_EX_ptr->register_write;

            if (load_use_hazard == true) { // load use hazard stall을 위해
                ID_EX_ptr = &ID_EX_reg;
                IF_ID_ptr = &IF_ID_reg;
                load_use_hazard = false;
            }

            ID_EX_ptr->pc = IF_ID_ptr->pc;
            ID_EX_ptr->index = IF_ID_ptr->index;
            ID_EX_ptr->instruction_type = ID_instruction_type;
            ID_EX_ptr->instruction_name = ID_instruction_name;
            ID_EX_ptr->rs = ID_rs;
            ID_EX_ptr->rt = ID_rt;
            ID_EX_ptr->rd = ID_rd;
            ID_EX_ptr->r_formats = ID_r_format; //어떤 format의 인스턴스인지 확인하기 위해서
            ID_EX_ptr->i_formats = ID_i_format;
            ID_EX_ptr->j_formats = ID_j_format;
            ID_EX_ptr->register_write = ID_register_write;

            IF_ID_reg.pc = IF_pc;
            IF_ID_reg.index = IF_index;

            instruction_memory_big_index++; //pc+4와 동일

            ///해저드체킹
            /////EX/MEM TO EX FORWARDING
            //std::cout << "-------------------------------------------------hazard checking-------------------------------------------------" << std::endl;
            bool threetime = false;
            if (MEM_WB_reg.register_write == true) {
                if (MEM_WB_reg.instruction_type == "R") {
                    if (MEM_WB_reg.rd != -1) {
                        if (EX_MEM_reg.rd != ID_EX_reg.rs || EX_MEM_reg.rd != ID_EX_reg.rt) {////////////////////////  ///MEM/WB to EX FORWARDING 
                            if (EX_MEM_reg.rd != ID_EX_reg.rs) {
                                if (MEM_WB_reg.rd == ID_EX_reg.rs) {
                                    //std::cout << "MEM/WB FORWARDING, ID/EX rs 데이터 해저드 발생" << std::endl;
                                    data_hazard = true;
                                    hazard_register = my_register;
                                    hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;
                                }
                            }
                            if (EX_MEM_reg.rd != ID_EX_reg.rt) {
                                if (MEM_WB_reg.rd == ID_EX_reg.rt) {
                                    //std::cout << "MEM/WB FORWARDING, 데이터 해저드 발생" << std::endl;
                                    data_hazard = true;
                                    hazard_register = my_register;
                                    hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;
                                }
                            }
                        }
                        if (MEM_WB_reg.rd == EX_MEM_reg.rs && EX_MEM_reg.rs == ID_EX_reg.rs) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;

                        }
                        if (MEM_WB_reg.rd == EX_MEM_reg.rs && EX_MEM_reg.rs == ID_EX_reg.rt) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;

                        }
                        if (MEM_WB_reg.rd == EX_MEM_reg.rt && EX_MEM_reg.rt == ID_EX_reg.rs) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;

                        }
                        if (MEM_WB_reg.rd == EX_MEM_reg.rt && EX_MEM_reg.rt == ID_EX_reg.rt) {
                            //::cout << "같은 레지스터가 연속 3번 사용됨" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;

                        }
                    }
                }

                if (MEM_WB_reg.instruction_type == "I") {
                    if (MEM_WB_reg.rt != -1) {
                        if (EX_MEM_reg.rt != ID_EX_reg.rs) { /// mem/wb to ex forwarding
                            if (MEM_WB_reg.rt == ID_EX_reg.rs) {
                                //std::cout << "MEM/WB FORWARDING, ID/EX rs 데이터 해저드 발생" << std::endl;
                                data_hazard = true;
                                hazard_register = my_register;
                                hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;
                            }
                        }
                        if (EX_MEM_reg.rt != ID_EX_reg.rt) {
                            if (MEM_WB_reg.rt == ID_EX_reg.rt) { /// mem/wb to ex forwarding
                                //std::cout << "MEM/WB FORWARDING, 데이터 해저드 발생" << std::endl;
                                data_hazard = true;
                                hazard_register = my_register;
                                hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;
                            }
                        }

                        if (MEM_WB_reg.rt == EX_MEM_reg.rs && EX_MEM_reg.rs == ID_EX_reg.rs) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨, MEM/WB의 EX결과값전달" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;
                        }
                        if (MEM_WB_reg.rt == EX_MEM_reg.rs && EX_MEM_reg.rs == ID_EX_reg.rt) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨, MEM/WB의 EX결과값전달" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;
                        }
                        if (MEM_WB_reg.rt == EX_MEM_reg.rt && EX_MEM_reg.rt == ID_EX_reg.rs) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨, MEM/WB의 EX결과값전달" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rs] = MEM_WB_reg.ALU_result;

                        }
                        if (MEM_WB_reg.rt == EX_MEM_reg.rt && EX_MEM_reg.rt == ID_EX_reg.rt) {
                            //std::cout << "같은 레지스터가 연속 3번 사용됨, MEM/WB의 EX결과값전달" << std::endl;
                            data_hazard = true;
                            threetime = true;
                            hazard_register = my_register;
                            hazard_register[ID_EX_reg.rt] = MEM_WB_reg.ALU_result;

                        }
                        ///////////////////////////////////////////////////
                    }
                }
            }

            if (EX_MEM_reg.register_write == true) {
                if (EX_MEM_reg.instruction_type == "R") {
                    if (EX_MEM_reg.rd != -1) {
                        if (EX_MEM_reg.rd == ID_EX_reg.rt || EX_MEM_reg.rd == ID_EX_reg.rs) {
                           // std::cout << "EX/MEM rt, ID/EX rt rs 데이터 해저드 발생" << std::endl;
                            data_hazard = true;
                            if (threetime == false) {
                                hazard_register = my_register;
                            }

                            if (EX_MEM_reg.rd == ID_EX_reg.rt) {
                                hazard_register[ID_EX_reg.rt] = EX_MEM_reg.ALU_result;
                            }
                            if (EX_MEM_reg.rd == ID_EX_reg.rs) {
                                hazard_register[ID_EX_reg.rs] = EX_MEM_reg.ALU_result;
                            }
                        }
                    }
                }

                bool lw_sw_seq = false;
                if (EX_MEM_reg.instruction_type == "I") {//////////////////////addiu, andi, lui,ori, sltiu 인 경우
                    if (EX_MEM_reg.rt != -1) {
                        if (EX_MEM_reg.rt == ID_EX_reg.rt || EX_MEM_reg.rt == ID_EX_reg.rs) {
                            //std::cout << "EX/MEM rt, ID/EX rt rs 데이터 해저드 발생" << std::endl;
                            if (EX_MEM_reg.instruction_name == "lw" || EX_MEM_reg.instruction_name == "lb") { ///load use hazard
                                if (ID_EX_reg.instruction_name != "sw" && ID_EX_reg.instruction_name == "sb"){
                                    //std::cout << "load use hazard" << std::endl;
                                    load_use_hazard = true;
                                    ID_EX_ptr = &ID_EX_stall;
                                    IF_ID_ptr = &IF_ID_stall;
                                    instruction_memory_big_index--; //
                                }
                                else {
                                    lw_sw_seq = true;
                                    //std::cout << "lw/lb + sw/sb 나옴" << std::endl;
                                }
                            }
                            if (lw_sw_seq != true) {////lw,sw 연속으로 나오지 않을때 load use hazard.
                                data_hazard = true;
                                hazard_register = my_register;
                                if (EX_MEM_reg.rt == ID_EX_reg.rt) {
                                    hazard_register[ID_EX_reg.rt] = EX_MEM_reg.ALU_result;
                                }
                                if (EX_MEM_reg.rt == ID_EX_reg.rs) {
                                    hazard_register[ID_EX_reg.rs] = EX_MEM_reg.ALU_result;
                                }
                            }
                        }
                    }

                }
                if (exist_d == true) {
                    std::cout << "===== Completion cycle: " << cycle << " ====" << std::endl;
                    std::wcout << std::endl;
                }
                if (exist_p == true) {

                    std::cout << "Current pipeline PC state" << std::endl;
                    std::cout << "{" << "0x" << int_to_hex(IF_ID_reg.pc) << "|" << "0x" << int_to_hex(ID_EX_reg.pc) << "|" << "0x" << int_to_hex(EX_MEM_reg.pc) << "|" << "0x" << int_to_hex(MEM_WB_reg.pc) << "|" << "0x" << int_to_hex(end_pc) << "}" << std::endl;
                }
                if (exist_d == true)
                {
                    std::cout << "Current register values: " << std::endl;
                    std::cout << "PC: 0x" << int_to_hex(instruction_address[instruction_memory_big_index][0]) << std::endl;
                    std::cout << "Register:" << std::endl;
                    for (int n = 0; n < my_register.size(); n++)
                    {
                        std::cout << "R" << n << ":"
                            << " 0x" << int_to_hex(int(my_register[n].to_ullong())) << std::endl;
                    }
                }
            }

        }
  

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////.///////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (exist_d == false) {
            std::cout << "===== Completion cycle: " << cycle << " ====" << std::endl;
            std::wcout << std::endl;
        }
        if (exist_p == false) {

            std::cout << "Current pipeline PC state" << std::endl;
            std::cout << "{" << "|" << "|" << "|" << "|" << "}" << std::endl;
        }
        if (exist_d == false)
        {
            std::cout << "Current register values: " << std::endl;
            std::cout << "PC: 0x" << int_to_hex(instruction_address[instruction_memory_big_index][0]) << std::endl;
            std::cout << "Registers:" << std::endl;
            for (int n = 0; n < my_register.size(); n++)
            {
                std::cout << "R" << n << ":"
                    << " 0x" << int_to_hex(int(my_register[n].to_ullong())) << std::endl;
            }
        }

        if (exist_m == true)
        {
            std::string address_range = command_parsing[memory_n];
            int token_index;
            for (int n = 0; n < address_range.size(); n++)
            {
                if (address_range[n] == ':')
                {
                    token_index = n;
                }
            }
            std::string first_address;
            std::string second_address;
            for (int n = 0; n < token_index; n++)
            {
                first_address.push_back(address_range[n]);
            }
            for (int n = token_index + 1; n < address_range.size(); n++)
            {
                second_address.push_back(address_range[n]);
            }

            const char* first = first_address.c_str();
            int d_first_address = strtol(first, NULL, 16);

            const char* second = second_address.c_str();
            int d_second_address = strtol(second, NULL, 16);

            std::cout << "Memory content [" << first_address << ".." << second_address << "]" << std::endl;
            std::cout << "Current register value: " << std::endl;
            std::cout << "---------------------------------------------------------------" << std::endl;
            for (int n = d_first_address; n < d_second_address + 1; n += 4)
            {
                std::cout << "0x" << int_to_hex(n) << ":";
                if (n >= start_data_address && n <= end_data_address)
                {
                    std::vector<int> index = find_address_index_for_branch(data_address, n);
                    std::vector<std::string> temp = data_memory[index[0]];
                    std::cout << temp[0] + temp[1] + temp[2] + temp[3] << std::endl;
                }
                else
                {
                    if (n >= start_instruction_address && n <= end_instruction_address)
                    {
                        std::vector<int> index = find_address_index_for_branch(instruction_address, n);
                        std::vector<std::string> temp = instruction_memory[index[0]];
                        std::cout << temp[0] + temp[1] + temp[2] + temp[3] << std::endl;
                    }
                    else
                    {
                        std::cout << "0x0" << std::endl;
                    }
                }
            }
        }
    }
}
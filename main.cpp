#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <langinfo.h>

using byte_t = unsigned char;

std::vector<ZyanU8> readFile(const std::string& file) {
    std::ifstream input(file, std::ios::binary);

    // copies all data into buffer
    std::vector<ZyanU8> buffer(std::istreambuf_iterator<char>(input), {});
    /*std::cout<<std::hex;
    for(auto&& it : buffer)
    {
       std::cout<<it;
    }
    std::cout<<std::endl;*/
    return buffer;
}

bool writeToFile(const std::string& file, std::vector<ZyanU8> const& data ) {
    std::ofstream out(file, std::ios::binary);

    for(auto&& it : data)
    {
       out<<it;
    }
    return true;
}

bool write_to_file(std::string const& path, std::vector<std::string> const& data)
{
    std::ofstream myfile{path};
    myfile << "use64"<<"\n";
  /*  myfile <<"format ELF64"<<"\n";
    myfile <<"section '.text' executable"<<"\n";
    myfile <<"public _start"<<"\n";
    myfile <<"_start:"<<"\n";
*/
    for(auto&& line : data)
    {
        myfile << line<<"\n";
    }
    myfile.close();
}

std::vector<std::string> add_jmp(std::vector<std::string> const& data)
{
    if(data.empty())
        return {};

    std::vector<std::string> ret;
    ret.emplace_back("jmp label0");
    for(int i = 0; i < data.size(); i++)
    {
        std::string new_str;
        new_str = "label" + std::to_string(i)+ ": " + data[i];
        if(i < data.size() - 1)
            new_str += '\n' + std::string("jmp ") + "label" + std::to_string(i + 1);
        else
            new_str += '\n' + std::string("jmp ") + "labelEnd";
        ret.push_back(new_str);
//        if(i < data.size() - 1)
//        ret.push_back(std::string("jmp ") + "label" + std::to_string(i + 1));
    }
    ret.emplace_back("labelEnd:");
    return ret;
}

std::vector<std::string> add_junk(std::vector<std::string> const& data) {

    if(data.size() < 3)
        return {};

    std::vector<std::string> ret = data;

    std::srand(std::time(nullptr));

    int junk_count = std::rand() % (ret.size() - 2);


    for(int i = 0; i < junk_count; i++) {
        int junk_position = std::rand() % (ret.size() - 2);
        //добавить перепрыгивание этой функции
        ret.insert(std::begin(ret) + junk_position + 1, "DB 0x12\n");
    }

//    int junk_position = std::rand() % (ret.size() - 2);
//    ret.insert(std::begin(ret) + junk_position + 1, "number DB 0x12\n");


    return ret;
}

std::vector<std::string> shuffle(std::vector<std::string> const& data) {
    std::vector<std::string> ret = data;
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(ret)+1, std::end(ret)-1, rng);
    return  ret;
}

void printHex(std::vector<ZyanU8> const& data)
{
    std::cerr<<std::hex;

    for(auto&& it : data)
    {
        std::remove_reference<decltype(data)>::type::value_type mask = 0b11110000;
        std::cerr<<((it & mask) >> 4);
        std::cerr<<(it & (~mask));
        std::cerr<<" ";
    }
    std::cerr<<std::endl;
}

int key = 1; // если 8 то вставить 9, в деоде если  после xor 9 cor 8 ==9, то значит было число после xor 9

std::vector<ZyanU8> encode(std::vector<ZyanU8> const& data)
{

    auto encoded = data;

    std::transform(std::cbegin(data), std::cend(data), std::begin(encoded), [](auto&& it){
        return it + key;
    });
    return encoded;
}

void generate_shellcode(std::vector<ZyanU8> const& data)
{

    std::ofstream myfile{"shellcode.asm"};

    myfile <<"format ELF64"<<"\n";
    myfile <<"section '.text' executable"<<"\n";
    myfile <<"public _start"<<"\n";
    myfile <<"_start:"<<"\n";
    myfile <<"len equ " << data.size()<<"\n";
    myfile <<"keys.xor1 equ " << key<<"\n";
    myfile <<"encode_setup:"<<std::endl;
    myfile <<"xor rcx, rcx"<<std::endl;
    myfile <<"lea rsi, [payload]"<<std::endl;
    myfile <<"encode:"<<std::endl;
    myfile <<"mov al, byte [rsi+rcx]"<<std::endl;
    myfile <<"sub al, keys.xor1"<<std::endl;
    myfile <<"mov byte [rsi+rcx], al"<<std::endl;
    myfile <<"inc rcx"<<std::endl;
    myfile <<"cmp rcx, len"<<std::endl;
    myfile <<"jne encode"<<std::endl;
    myfile <<"jmp  payload"<<std::endl;
//------
    myfile <<"section '.data' executable writeable"<<std::endl;
//--

    myfile <<"payload:"<<std::endl;

    myfile << "db ";
    for(auto it = std::cbegin(data); it != std::cend(data) - 1; it++)
    {
        myfile << static_cast<int>(*it)<<", ";
    }
    myfile <<  static_cast<int>(data.back())<<'\n';

    myfile.close();

    system("fasm shellcode.asm");
    system("ld shellcode.o -o shellcode.out");
}

int main()
{
    auto bin = readFile("/home/mugutdinov/vp-client-nova/graduating/simple.bin");
//    printHex(bin);
//    auto data = encode(bin);
//    printHex(data);
//    generate_shellcode(data);

    auto&& data = &bin[0];

    // Initialize decoder context
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = 0x007FFFFFFF400000;
    ZyanUSize offset = 0;
    const ZyanUSize length = /*sizeof(data);*/ bin.size();

    std::vector<std::string> instr_v;
    ZydisDecodedInstruction instruction;
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data/*bin.data()*/ + offset, length - offset,
                                                 &instruction)))
    {
        // Print current instruction pointer.
//        printf("%016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human readable format
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
                                        runtime_address);


//        puts(buffer);
        instr_v.emplace_back(buffer);
        offset += instruction.length;
        runtime_address += instruction.length;
    }


    for(auto&& i : instr_v)
    {
        std::cout<<i<<std::endl;
    }

    instr_v = add_jmp(instr_v);
    instr_v = shuffle(instr_v);
    instr_v = add_junk(instr_v);

    write_to_file("list.asm", instr_v);

    system("fasm list.asm");
//    system("ld list.o -o list.out");


    //    printHex(bin);
    bin = readFile("list.bin");
    auto encoded = encode(bin);
    printHex(encoded);
    generate_shellcode(encoded);

    //    writeToFile("/home/mugutdinov/vp-client-nova/graduating/simple_same.bin", data);

    return 0;
}


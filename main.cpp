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
#include <sstream>

std::string merge(std::vector<std::string> const& listening)
{
    std::string ret;
    for(auto&& line : listening)
    {
        ret += line;
    }
    return ret;
}

std::vector<std::string> createCodeForExecutable(std::vector<std::string> const& code, std::vector<std::string> const& data)
{
    std::vector<std::string> ret;

    auto add_line = [&ret](std::string const& line){
        ret.emplace_back(line + "\n");
    };

#ifdef __linux__
    add_line("format ELF64");
    add_line("section '.text' executable");
    add_line("public _start");
    add_line("_start:");

#elif _WIN32
    // windows code goes here
#else

#endif

    for(auto&& line : code)
    {
        add_line(line);
    }

    if(!data.empty())
    {
        add_line("section '.data' executable writeable");

        for(auto&& line : data)
        {
            add_line(line);
        }
    }

    return ret;
}

void fasm(std::string const& filename)
{
    system(std::string{"fasm " + filename}.c_str());
}
void build(std::string const& filename)
{
    auto dot_position = filename.find('.');
    if(dot_position == std::string::npos)
    {
        std::cerr<<"Error: could not find dot in filename"<<std::endl;
        return;
    }

    auto name = filename.substr(0, dot_position);
    std::string fasm = "fasm " + name + ".asm";
    system(fasm.c_str());

    std::string ld = "ld " + name + ".o " + "-o " + name + ".out";
    system("ld shellcode.o -o shellcode.out");
}

std::vector<ZyanU8> readFile(const std::string& file) {
    std::ifstream input(file, std::ios::binary);

    // copies all data into buffer
    std::vector<ZyanU8> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

bool writeToBinFile(const std::string& file, std::vector<ZyanU8> const& data ) {
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

    for(auto&& line : data)
    {
        myfile << line<<"\n";
    }
    myfile.close();
    return true;
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

void generateShellcode(std::vector<ZyanU8> const& payload)
{
    std::string filename = "shellcode.asm";
    std::ofstream myfile{filename};

    std::vector<std::string> code;
    code.emplace_back("len equ " + std::to_string(payload.size()));
    code.emplace_back("keys.xor1 equ " + std::to_string(key));
    code.emplace_back("encode_setup:");
    code.emplace_back("xor rcx, rcx");
    code.emplace_back("lea rsi, [payload]");
    code.emplace_back("encode:");
    code.emplace_back("mov al, byte [rsi+rcx]");
    code.emplace_back("sub al, keys.xor1");
    code.emplace_back("mov byte [rsi+rcx], al");
    code.emplace_back("inc rcx");
    code.emplace_back("cmp rcx, len");
    code.emplace_back("jne encode");
    code.emplace_back("jmp payload");

    std::vector<std::string> data;
    data.emplace_back("payload:");

    std::string dpayload = "db ";
    for(auto it = std::cbegin(payload); it != std::cend(payload) - 1; it++)
    {
        dpayload += std::to_string(static_cast<int>(*it)) + ", ";
    }
    dpayload += std::to_string(static_cast<int>(payload.back()));
    data.push_back(dpayload);

    auto listening = createCodeForExecutable(code, data);

    myfile<<merge(listening);
    myfile.close();

    build(filename);
}

std::vector<std::string> decode(std::vector<ZyanU8> const& bin)
{
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
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
                                        runtime_address);

        instr_v.emplace_back(buffer);
        offset += instruction.length;
        runtime_address += instruction.length;
    }

    return instr_v;
}

int main()
{
    auto bin = readFile("/home/mugutdinov/vp-client-nova/graduating/simple.bin");
//    printHex(bin);
//    auto data = encode(bin);
//    printHex(data);
//    generateShellcode(data);

    auto instr_v = decode(bin);

    for(auto&& i : instr_v)
    {
        std::cout<<i<<std::endl;
    }

    instr_v = add_jmp(instr_v);
    instr_v = shuffle(instr_v);
    instr_v = add_junk(instr_v);

    write_to_file("list.asm", instr_v);

    fasm("list.asm");
//    system("ld list.o -o list.out");


    //    printHex(bin);
    bin = readFile("list.bin");
    auto encoded = encode(bin);
    printHex(encoded);
    generateShellcode(encoded);

    //    writeToFile("/home/mugutdinov/vp-client-nova/graduating/simple_same.bin", data);

    return 0;
}


#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

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


bool write_to_file(std::string const& path, std::vector<std::string> const& data)
{
    std::ofstream myfile{path};
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
std::vector<std::string> shuffle(std::vector<std::string> const& data) {
    std::vector<std::string> ret = data;
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(ret)+1, std::end(ret)-1, rng);
    return  ret;
}

int main()
{

//
   /* ZyanU8 data[] =
            {
                    0x66,0x81,0xC4,0xFC,0x03,0x00,0x00,
                    0x90,0x90,0x90,
                    0x31, 0xdb, 0x64, 0x8b, 0x7b, 0x30, 0x8b, 0x7f,
                    0x0c, 0x8b, 0x7f, 0x1c, 0x8b, 0x47, 0x08, 0x8b,
                    0x77, 0x20, 0x8b, 0x3f, 0x80, 0x7e, 0x0c, 0x33,
                    0x75, 0xf2, 0x89, 0xc7, 0x03, 0x78, 0x3c, 0x8b,
                    0x57, 0x78, 0x01, 0xc2, 0x8b, 0x7a, 0x20, 0x01,
                    0xc7, 0x89, 0xdd, 0x8b, 0x34, 0xaf, 0x01, 0xc6,
                    0x45, 0x81, 0x3e, 0x43, 0x72, 0x65, 0x61, 0x75,
                    0xf2, 0x81, 0x7e, 0x08, 0x6f, 0x63, 0x65, 0x73,
                    0x75, 0xe9, 0x8b, 0x7a, 0x24, 0x01, 0xc7, 0x66,
                    0x8b, 0x2c, 0x6f, 0x8b, 0x7a, 0x1c, 0x01, 0xc7,
                    0x8b, 0x7c, 0xaf, 0xfc, 0x01, 0xc7, 0x89, 0xd9,
                    0xb1, 0xff, 0x53, 0xe2, 0xfd, 0x68, 0x63, 0x61,
                    0x6c, 0x63, 0x89, 0xe2, 0x52, 0x52, 0x53, 0x53,
                    0x53, 0x53, 0x53, 0x53, 0x52, 0x53, 0xff, 0xd7
            };*/
//   auto&& data = &bin[0];

     /*ZyanU8 data[] =
             {
                     0x66,0x81,0xC4,0xFC,0x03,0x00,0x00,
                     0x90,0x90,0x90,
                     0x31, 0xdb, 0x64, 0x8b, 0x7b, 0x30, 0x8b, 0x7f,
                     0x0c, 0x8b, 0x7f, 0x1c, 0x8b, 0x47, 0x08, 0x8b,
                     0x77, 0x20, 0x8b, 0x3f, 0x80, 0x7e, 0x0c, 0x33,
                     0x75, 0xf2, 0x89, 0xc7, 0x03, 0x78, 0x3c, 0x8b,
                     0x57, 0x78, 0x01, 0xc2, 0x8b, 0x7a, 0x20, 0x01,
                     0xc7, 0x89, 0xdd, 0x8b, 0x34, 0xaf, 0x01, 0xc6,
                     0x45, 0x81, 0x3e, 0x43, 0x72, 0x65, 0x61, 0x75,
                     0xf2, 0x81, 0x7e, 0x08, 0x6f, 0x63, 0x65, 0x73,
                     0x75, 0xe9, 0x8b, 0x7a, 0x24, 0x01, 0xc7, 0x66,
                     0x8b, 0x2c, 0x6f, 0x8b, 0x7a, 0x1c, 0x01, 0xc7,
                     0x8b, 0x7c, 0xaf, 0xfc, 0x01, 0xc7, 0x89, 0xd9,
                     0xb1, 0xff, 0x53, 0xe2, 0xfd, 0x68, 0x63, 0x61,
                     0x6c, 0x63, 0x89, 0xe2, 0x52, 0x52, 0x53, 0x53,
                     0x53, 0x53, 0x53, 0x53, 0x52, 0x53, 0xff, 0xd7
             };*/

    auto bin = readFile("/home/mugutdinov/vp-client-nova/graduating/simple.bin");
    auto&& data = &bin[0];
//    ZyanU8 data[] =  { 0xB8, 0x01, 0x00, 0xB8, 0x02, 0x00, 0xB8, 0x03, 0x00, 0xB8, 0x04, 0x00, 0xB8,
//                       0x05, 0x00, 0xB8, 0x06, 0x00, 0xB8, 0x07, 0x00};
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
    write_to_file("list.asm", instr_v);

//    system("");
}


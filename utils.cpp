//
// Created by mugutdinov on 21.06.2021.
//

#include "utils.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <Zydis/Zydis.h>

std::string merge(std::vector<std::string> const& listing)
{
    std::string ret;
    for(auto&& line : listing)
    {
        ret += line;
    }
    return ret;
}

std::vector<std::string> createCodeFor64(std::vector<std::string> const& code )
{
    auto ret = code;
    ret.insert(std::begin(ret),"use64");
    return ret;
}
std::vector<std::string> createCodeFor64(ProgramListing const& listing )
{
    auto total_listing = listing.code_before_start;
    total_listing.insert(std::end(total_listing), std::begin(listing.code_after_start), std::end(listing.code_after_start));
    total_listing.insert(std::end(total_listing), std::begin(listing.data), std::end(listing.data));
    return createCodeFor64(total_listing);
}

std::vector<std::string> createCodeForExecutable(ProgramListing const& listing, bool place_data_in_code)
{
    std::vector<std::string> ret;

    auto add_line = [&ret](std::string const& line){
        ret.emplace_back(line /*+ "\n"*/);
    };

#ifdef __linux__
    add_line("format ELF64");
    add_line("section '.text' executable");
    add_line("public _start");
//    add_line("_start:");

#elif _WIN32

    add_line("format PE64 GUI; at 0xfe0000");
    add_line("include 'C:\\reverse\\fasm\\include\\WIN64AX.INC'");
    add_line("section '.code' code readable writable executable");
    add_line("entry start");
    // windows code goes here
#endif

    for(auto&& line : listing.code_before_start)
    {
        add_line(line);
    }

#ifdef __linux__
    add_line("_start:");

#elif _WIN32
    add_line("start:");
    // windows code goes here
#endif

    for(auto&& line : listing.code_after_start)
    {
        add_line(line);
    }
//#ifdef _WIN32
//    add_line(".end start");
//#endif

    if(!listing.data.empty())
    {
        if(!place_data_in_code)
        {
            add_line("section '.data' executable writeable");
        }

        for(auto&& line : listing.data)
        {
            add_line(line);
        }
    }
#ifdef _WIN32
//    add_line(".end start");
#endif


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

#ifdef __linux__
    std::string ld = "ld " + name + ".o " + "-o " + name + ".out";
    system(ld.c_str());
#endif

}

std::vector<byte> readFile(const std::string& file) {
    std::ifstream input(file, std::ios::binary);

    // copies all data into buffer
    std::vector<byte> buffer(std::istreambuf_iterator<char>(input), {});
    return buffer;
}

bool writeBinFile(const std::string& file, std::vector<byte> const& data ) {
    std::ofstream out(file, std::ios::binary);

    for(auto&& it : data)
    {
        out<<it;
    }
    return true;
}

bool writeTextFile(std::string const& path, std::vector<std::string> const& data)
{
    std::ofstream myfile{path};

    for(auto&& line : data)
    {
        myfile << line<<"\n";
    }
    myfile.close();
    return true;
}

void printHex(std::vector<byte> const& data)
{
    std::cerr<<std::hex;

    for(auto&& it : data)
    {
        std::remove_reference<decltype(data)>::type::value_type mask = 0b11110000;
        std::cerr<<"0x";
        std::cerr<<((it & mask) >> 4);
        std::cerr<<(it & (~mask));
        std::cerr<<", ";
    }
    std::cerr<<std::endl;
}

std::vector<std::string> decodeBin(std::vector<byte> const& bin)
{
    auto&& data = &bin[0];

    // Initialize decoder context
    ZydisDecoder decoder;
//    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
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




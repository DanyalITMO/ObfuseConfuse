#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include "Obfuscators/Text/Jumper.h"
#include "Obfuscators/Text/Junk.h"
#include "Obfuscators/Text/Shuffler.h"
#include "Obfuscators/Bin/AbstractBinObfuscator.h"
#include "Obfuscators/Bin/TwoXor.h"
#include "utils.h"

int main()
{

//    writeBinFile("C:/reverse/graduating/simple_bytes.bin", {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33});
    auto bin = readFile("C:/reverse/graduating/simple.bin");
//    auto bin = readFile("C:/reverse/graduating/simple_bytes.bin");

    auto instr_v = decodeBin(bin);

    for(auto&& i : instr_v)
    {
        std::cout<<i<<std::endl;
    }

    std::vector<std::unique_ptr<AbstractTextObfuscator>> textObfuscators;
    textObfuscators.push_back(std::make_unique<Jumper>());
    textObfuscators.push_back(std::make_unique<Shuffler>());
    textObfuscators.push_back(std::make_unique<Junk>());

    for(auto&& it : textObfuscators)
    {
        instr_v = it->process(instr_v);
    }

    writeTextFile("list.asm", createCodeFor64(instr_v));

    fasm("list.asm");

    bin = readFile("list.bin");

//    std::vector<std::unique_ptr<AbstractBinObfuscator>> binObfuscators;
    std::unique_ptr<TwoXor> binObfuscator{std::make_unique<TwoXor>()};
    auto encoded = binObfuscator->encode(bin);
    printHex(encoded);

    auto separated_listing = binObfuscator->addAsmStub(encoded);
    auto listening = createCodeForExecutable(separated_listing, true);
//    std::vector<std::string> c = code_data.first;
//    c.insert(std::end(c), std::begin(code_data.second), std::end(code_data.second));
//    auto listening = createCodeFor64(c);

    writeTextFile("shellcode.asm", listening);
    build("shellcode.asm");
//    auto decoded = decode(encoded);

    return 0;
}


//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_ABSTRACTBINOBFUSCATOR_H
#define TESTDISASM_ABSTRACTBINOBFUSCATOR_H


#include <vector>
#include <string>

using byte = uint8_t;

struct ProgramListing
{
    std::vector<std::string> code_before_start;
    std::vector<std::string> code_after_start;
    std::vector<std::string> data;
};
class AbstractBinObfuscator {
public:
    AbstractBinObfuscator() = default;
    virtual std::vector<byte> encode(std::vector<byte> const&) = 0;
    virtual std::vector<byte> decode(std::vector<byte> const&) = 0;
    virtual ProgramListing addAsmStub(std::vector<byte> const&) = 0;
};


#endif //TESTDISASM_ABSTRACTBINOBFUSCATOR_H

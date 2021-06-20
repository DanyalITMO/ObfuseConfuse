//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_ABSTRACTBINOBFUSCATOR_H
#define TESTDISASM_ABSTRACTBINOBFUSCATOR_H


#include <vector>
#include <string>

using byte = uint8_t;
class AbstractBinObfuscator {
public:
    AbstractBinObfuscator() = default;
    virtual std::vector<byte> encode(std::vector<byte> const&) = 0;
    virtual std::vector<byte> decode(std::vector<byte> const&) = 0;
    virtual std::pair<std::vector<std::string>, std::vector<std::string>> addAsmStub(std::vector<byte> const&) = 0;
};


#endif //TESTDISASM_ABSTRACTBINOBFUSCATOR_H

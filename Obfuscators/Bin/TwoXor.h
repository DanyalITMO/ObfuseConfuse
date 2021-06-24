//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_TWOXOR_H
#define TESTDISASM_TWOXOR_H

#include "AbstractBinObfuscator.h"

class TwoXor : public AbstractBinObfuscator {
public:
    TwoXor() = default;
    std::vector<byte> encode(std::vector<byte> const &data) override;
    std::vector<byte> decode(std::vector<byte> const &data) override;
    ProgramListing addAsmStub(std::vector<byte> const &payload) override;

private:
    int key1 = 8;
    int key2 = 9;
};


#endif //TESTDISASM_TWOXOR_H

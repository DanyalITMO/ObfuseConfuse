//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_JUNK_H
#define TESTDISASM_JUNK_H


#include "AbstractTextObfuscator.h"

class Junk: public AbstractTextObfuscator {
public:
    Junk() = default;
    std::vector<std::string> process(std::vector<std::string>  const&) override;
};


#endif //TESTDISASM_JUNK_H

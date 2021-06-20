//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_SHUFFLER_H
#define TESTDISASM_SHUFFLER_H


#include "AbstractTextObfuscator.h"

class Shuffler: public AbstractTextObfuscator {
    std::vector<std::string> process(std::vector<std::string> const&) override;
};


#endif //TESTDISASM_SHUFFLER_H

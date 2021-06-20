//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_JUMPER_H
#define TESTDISASM_JUMPER_H

#include "AbstractTextObfuscator.h"

class Jumper : public AbstractTextObfuscator {
public:
    Jumper() = default;
    std::vector<std::string> process(std::vector<std::string>  const&) override;
};


#endif //TESTDISASM_JUMPER_H

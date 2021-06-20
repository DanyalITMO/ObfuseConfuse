//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_ABSTRACTTEXTOBFUSCATOR_H
#define TESTDISASM_ABSTRACTTEXTOBFUSCATOR_H


#include <string>
#include <vector>

class AbstractTextObfuscator {
public:
    AbstractTextObfuscator() = default;
    virtual std::vector<std::string> process(std::vector<std::string>  const&) = 0;
};


#endif //TESTDISASM_ABSTRACTTEXTOBFUSCATOR_H

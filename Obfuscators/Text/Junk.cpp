//
// Created by mugutdinov on 21.06.2021.
//

#include <ctime>
#include "Junk.h"

std::vector<std::string> Junk::process(std::vector<std::string> const& data)
{

    if(data.size() < 3)
        return {};

    std::vector<std::string> ret = data;

    std::srand(std::time(nullptr));

    int junk_count = std::rand() % (ret.size() - 2);


    for(int i = 0; i < junk_count; i++) {
        int junk_position = std::rand() % (ret.size() - 2);
        std::string label = "l_" + std::string(__func__) + std::to_string(i);
        std::string new_str = "jmp " + label + "\n";
        new_str += "DB 0x12\n";
        new_str += label + ": ";

        ret.insert(std::begin(ret) + junk_position + 1, new_str);

    }

//    int junk_position = std::rand() % (ret.size() - 2);
//    ret.insert(std::begin(ret) + junk_position + 1, "number DB 0x12\n");


    return ret;
}
//
// Created by mugutdinov on 21.06.2021.
//

#include <random>
#include <algorithm>
#include "Shuffler.h"


std::vector<std::string> Shuffler::process(std::vector<std::string> const& data)
{
    std::vector<std::string> ret = data;
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(ret)+1, std::end(ret)-1, rng);
    return  ret;

}
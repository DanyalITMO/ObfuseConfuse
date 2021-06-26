//
// Created by mugutdinov on 21.06.2021.
//

#ifndef TESTDISASM_UTILS_H
#define TESTDISASM_UTILS_H

#include <vector>
#include <string>
#include " Obfuscators/Bin/AbstractBinObfuscator.h"

std::vector<std::string> createCodeFor64(std::vector<std::string> const&  code );

std::vector<std::string> createCodeForExecutable(ProgramListing const& listing, bool place_data_in_code = false);

void fasm(std::string const& filename);

void build(std::string const& filename);

std::vector<byte> readFile(const std::string& file);

bool writeBinFile(const std::string& file, std::vector<byte> const& data );

bool writeTextFile(std::string const& path, std::vector<std::string> const& data);

void printHex(std::vector<byte> const& data);

std::vector<std::string> decodeBin(std::vector<byte> const& bin);

#endif //TESTDISASM_UTILS_H

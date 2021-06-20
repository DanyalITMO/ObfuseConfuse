//
// Created by mugutdinov on 21.06.2021.
//

#include <iostream>
#include "TwoXor.h"

std::vector<byte> TwoXor::encode(std::vector<byte> const& data)
{
    std::vector<byte> encoded;

    for(auto it :data)
    {
        if(it == key1)
        {
            encoded.push_back(it ^ key2);
            encoded.push_back(1);
        }
        else if(it == key2)
        {
            encoded.push_back(it ^ key1);
            encoded.push_back(2);
        }
        else
        {
            encoded.push_back(it ^ key1);
        }

    }
    return encoded;
}

std::vector<byte> TwoXor::decode(const std::vector<byte> &data)
{
    std::vector<byte> decoded;

    for(auto it = std::begin(data); it != std::end(data); it++)
    {
        if(*it == (key1 ^ key2))
        {
            auto variant = *(it+1);
            if(variant == 1)
            {
                decoded.push_back(*it ^ key2);
            }
            else if(variant == 2)
            {
                decoded.push_back(*it ^ key1);
            }
            else
            {
                std::cerr<<"Error decoding variant";
            }
            it++;
        }
        else
        {
            decoded.push_back(*it ^ key1);
        }
    }

    return decoded;
}

std::pair<std::vector<std::string>, std::vector<std::string>>  TwoXor::addAsmStub(const std::vector<byte>& payload)
{
    std::vector<std::string> code;
    code.emplace_back("keys.xor1 equ " + std::to_string(key1));
    code.emplace_back("keys.xor2 equ " + std::to_string(key2));
    code.emplace_back("xor_keys equ keys.xor1 xor keys.xor2");
    code.emplace_back("len equ " + std::to_string(payload.size()));
    code.emplace_back(";rsi base, rbx - offset from, rdx count");
    code.emplace_back("shiftBytes:");
    code.emplace_back("push rax");
    code.emplace_back("push rbx");
    code.emplace_back("push rdx");
    code.emplace_back("push rcx");
    code.emplace_back("xor rcx, rcx");
    code.emplace_back("loop_point:");
    code.emplace_back("mov al, byte [rsi+rbx]");
    code.emplace_back("XCHG al, byte [rsi+rbx+1]");
    code.emplace_back("mov byte [rsi+rbx], al");
    code.emplace_back("inc rbx");
    code.emplace_back("inc rcx");
    code.emplace_back("cmp rcx, rdx");
    code.emplace_back("jnz loop_point");
    code.emplace_back("pop rcx");
    code.emplace_back("pop rdx");
    code.emplace_back("pop rbx");
    code.emplace_back("pop rax");
    code.emplace_back("ret");
    code.emplace_back("_start:");
    code.emplace_back("encode_setup:");
    code.emplace_back("xor rcx, rcx");
    code.emplace_back("lea rsi, [payload]");
    code.emplace_back("mov r8, len");
    code.emplace_back("encode:");
    code.emplace_back("mov al, byte [rsi+rcx]");
    code.emplace_back("cmp al, xor_keys");
    code.emplace_back("je variant");
    code.emplace_back("jne basic");
    code.emplace_back("variant:");
    code.emplace_back("mov ah, byte [rsi+rcx+1]");
    code.emplace_back("cmp ah, 1");
    code.emplace_back("je first");
    code.emplace_back("jne second");
    code.emplace_back("first:");
    code.emplace_back("xor al, keys.xor2");
    code.emplace_back("jmp end_of_variant");
    code.emplace_back("second:");
    code.emplace_back("xor al, keys.xor1");
    code.emplace_back("jmp end_of_variant");
    code.emplace_back("end_of_variant:");
    code.emplace_back("mov byte [rsi+rcx], al");
    code.emplace_back("mov byte [rsi+rcx+1], 0x90");
    code.emplace_back(";rsi base, rbx - offset from, rdx count");
    code.emplace_back("mov rbx, rcx");
    code.emplace_back("inc rbx ;from next byte");
    code.emplace_back("mov rdx, len");
    code.emplace_back("sub rdx, rcx");
    code.emplace_back("sub rdx, 2 ; -1 for count from zero, -1 for rcx");
    code.emplace_back("call shiftBytes");
    code.emplace_back("dec r8");
    code.emplace_back("jmp end_of_cycle");
    code.emplace_back("basic:");
    code.emplace_back("xor al, keys.xor1");
    code.emplace_back("mov byte [rsi+rcx], al");
    code.emplace_back("jmp end_of_cycle");
    code.emplace_back("end_of_cycle:");
    code.emplace_back("inc rcx");
    code.emplace_back("cmp rcx, r8");
    code.emplace_back("jne encode");
    code.emplace_back("jmp payload");

    std::vector<std::string> data;
    data.emplace_back("payload:");

    std::string dpayload = "db ";
    for(auto it = std::cbegin(payload); it != std::cend(payload) - 1; it++)
    {
        dpayload += std::to_string(static_cast<int>(*it)) + ", ";
    }
    dpayload += std::to_string(static_cast<int>(payload.back()));
    data.push_back(dpayload);

    return {code, data};
}
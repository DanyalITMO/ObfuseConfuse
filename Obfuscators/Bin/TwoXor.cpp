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
//std::vector<std::string>,  std::vector<std::string>
ProgramListing TwoXor::addAsmStub(const std::vector<byte>& payload)
{
    ProgramListing listing;
//    std::vector<std::string> code;
    auto&& code_before = listing.code_before_start;
    code_before.emplace_back("keys.xor1 equ " + std::to_string(key1));
    code_before.emplace_back("keys.xor2 equ " + std::to_string(key2));
    code_before.emplace_back("xor_keys equ keys.xor1 xor keys.xor2");
    code_before.emplace_back("len equ " + std::to_string(payload.size()));
    code_before.emplace_back(";rsi base, rbx - offset from, rdx count");
    code_before.emplace_back("shiftBytes:");
    code_before.emplace_back("push rax");
    code_before.emplace_back("push rbx");
    code_before.emplace_back("push rdx");
    code_before.emplace_back("push rcx");
    code_before.emplace_back("xor rcx, rcx");
    code_before.emplace_back("loop_point:");
    code_before.emplace_back("mov al, byte [rsi+rbx]");
    code_before.emplace_back("XCHG al, byte [rsi+rbx+1]");
    code_before.emplace_back("mov byte [rsi+rbx], al");
    code_before.emplace_back("inc rbx");
    code_before.emplace_back("inc rcx");
    code_before.emplace_back("cmp rcx, rdx");
    code_before.emplace_back("jnz loop_point");
    code_before.emplace_back("pop rcx");
    code_before.emplace_back("pop rdx");
    code_before.emplace_back("pop rbx");
    code_before.emplace_back("pop rax");
    code_before.emplace_back("ret");

    auto&& code_after = listing.code_after_start;

//    code_after.emplace_back("start:");
    code_after.emplace_back("encode_setup:");

    code_after.emplace_back("jmp short call_decoder");
    code_after.emplace_back("decoder:");
    code_after.emplace_back("pop rsi");


    code_after.emplace_back("xor rcx, rcx");


//    code_after.emplace_back("mov rsi, payload");

//    code_after.emplace_back("push payload");
//    code_after.emplace_back("pop rsi");

//    code_after.emplace_back("call next");
//    code_after.emplace_back("next:");
//    code_after.emplace_back("pop rsi");
//    code_after.emplace_back("add rsi, 0x60");

//    code_after.emplace_back("lea rsi, [rip]");
//    code_after.emplace_back("add rsi, 0x60");

/*
    49:C7C0 34000000         | mov r8,34                               | 34:'4'
    code_after.emplace_back("mov r8, len");
*/
    code_after.emplace_back("push len");
    code_after.emplace_back("pop r8");

    code_after.emplace_back("encode:");
    code_after.emplace_back("mov al, byte [rsi+rcx]");
    code_after.emplace_back("cmp al, xor_keys");
    code_after.emplace_back("je variant");
    code_after.emplace_back("jne basic");
    code_after.emplace_back("variant:");
    code_after.emplace_back("mov ah, byte [rsi+rcx+1]");
    code_after.emplace_back("cmp ah, 1");
    code_after.emplace_back("je first");
    code_after.emplace_back("jne second");
    code_after.emplace_back("first:");
    code_after.emplace_back("xor al, keys.xor2");
    code_after.emplace_back("jmp end_of_variant");
    code_after.emplace_back("second:");
    code_after.emplace_back("xor al, keys.xor1");
//    code_after.emplace_back("jmp end_of_variant"); что бы избежать нуля на нулевом прыжке
    code_after.emplace_back("end_of_variant:");
    code_after.emplace_back("mov byte [rsi+rcx], al");
    code_after.emplace_back("mov byte [rsi+rcx+1], 0x90");
    code_after.emplace_back(";rsi base, rbx - offset from, rdx count");
    code_after.emplace_back("mov rbx, rcx");
    code_after.emplace_back("inc rbx ;from next byte");

//    code_after.emplace_back("mov rdx, len");
    code_after.emplace_back("push len");
    code_after.emplace_back("pop rdx");

    code_after.emplace_back("sub rdx, rcx");
    code_after.emplace_back("sub rdx, 2 ; -1 for count from zero, -1 for rcx");
    code_after.emplace_back("call shiftBytes");
    code_after.emplace_back("dec r8");
    code_after.emplace_back("jmp end_of_cycle");
    code_after.emplace_back("basic:");
    code_after.emplace_back("xor al, keys.xor1");
    code_after.emplace_back("mov byte [rsi+rcx], al");
//    code_after.emplace_back("jmp end_of_cycle"); что бы избежать нуля на нулевом прыжке
    code_after.emplace_back("end_of_cycle:");
    code_after.emplace_back("inc rcx");
    code_after.emplace_back("cmp rcx, r8");
    code_after.emplace_back("jne encode");
    code_after.emplace_back("jmp rsi");



    code_after.emplace_back("call_decoder:");
    code_after.emplace_back("call decoder");


    auto&& data = listing.data;
    data.emplace_back("payload:");

    std::string dpayload = "db ";
    for(auto it = std::cbegin(payload); it != std::cend(payload) - 1; it++)
    {
        dpayload += std::to_string(static_cast<int>(*it)) + ", ";
    }
    dpayload += std::to_string(static_cast<int>(payload.back()));
    data.push_back(dpayload);

    return listing;
}
#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>

int main()
{
    ZyanU8 data[] =
            {
                    0x66,0x81,0xC4,0xFC,0x03,0x00,0x00,
                    0x90,0x90,0x90,
                    0x31, 0xdb, 0x64, 0x8b, 0x7b, 0x30, 0x8b, 0x7f,
                    0x0c, 0x8b, 0x7f, 0x1c, 0x8b, 0x47, 0x08, 0x8b,
                    0x77, 0x20, 0x8b, 0x3f, 0x80, 0x7e, 0x0c, 0x33,
                    0x75, 0xf2, 0x89, 0xc7, 0x03, 0x78, 0x3c, 0x8b,
                    0x57, 0x78, 0x01, 0xc2, 0x8b, 0x7a, 0x20, 0x01,
                    0xc7, 0x89, 0xdd, 0x8b, 0x34, 0xaf, 0x01, 0xc6,
                    0x45, 0x81, 0x3e, 0x43, 0x72, 0x65, 0x61, 0x75,
                    0xf2, 0x81, 0x7e, 0x08, 0x6f, 0x63, 0x65, 0x73,
                    0x75, 0xe9, 0x8b, 0x7a, 0x24, 0x01, 0xc7, 0x66,
                    0x8b, 0x2c, 0x6f, 0x8b, 0x7a, 0x1c, 0x01, 0xc7,
                    0x8b, 0x7c, 0xaf, 0xfc, 0x01, 0xc7, 0x89, 0xd9,
                    0xb1, 0xff, 0x53, 0xe2, 0xfd, 0x68, 0x63, 0x61,
                    0x6c, 0x63, 0x89, 0xe2, 0x52, 0x52, 0x53, 0x53,
                    0x53, 0x53, 0x53, 0x53, 0x52, 0x53, 0xff, 0xd7
            };

    // Initialize decoder context
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = 0x007FFFFFFF400000;
    ZyanUSize offset = 0;
    const ZyanUSize length = sizeof(data);
    ZydisDecodedInstruction instruction;
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, length - offset,
                                                 &instruction)))
    {
        // Print current instruction pointer.
//        printf("%016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human readable format
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
                                        runtime_address);


        puts(buffer);

        offset += instruction.length;
        runtime_address += instruction.length;
    }
}


#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <sys/mman.h>
#include <fstream>

using namespace std;

struct Instruction {
    std::string mnemonic;
    std::string operands;
    size_t length;
};

unordered_map<uint8_t, std::string> opcodeTable = {
    {0x89, "MOV"},
    {0xE9, "JMP"},
    {0xFF, "CALL"},
    {0xC3, "RET"},
    {0x50, "PUSH"},
    {0x83, "ADD"},
    {0x01, "ADD"},
    {0x48, "MOV"},
};

std::string toHex(int32_t value) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << value;
    return ss.str();
}

Instruction decodeMOV(const uint8_t* binary, size_t& offset) {
    Instruction instr;
    instr.mnemonic = "MOV";
    uint8_t modrm = binary[offset + 1];
    uint8_t reg = (modrm >> 3) & 0x7;
    uint8_t rm = modrm & 0x7;
    instr.operands = "R" + to_string(reg) + ", R" + to_string(rm);
    instr.length = 2;
    offset += instr.length;
    return instr;
}

Instruction decodeJMP(const uint8_t* binary, size_t& offset) {
    Instruction instr;
    instr.mnemonic = "JMP";
    int32_t jump_offset = *reinterpret_cast<const int32_t*>(&binary[offset + 1]);
    instr.operands = "0x" + toHex(jump_offset);
    instr.length = 5;
    offset += instr.length;
    return instr;
}

Instruction decodeCALL(const uint8_t* binary, size_t& offset) {
    Instruction instr;
    instr.mnemonic = "CALL";
    int32_t call_offset = *reinterpret_cast<const int32_t*>(&binary[offset + 1]);
    instr.operands = "0x" + toHex(call_offset);
    instr.length = 5;
    offset += instr.length;
    return instr;
}

Instruction decodeRET(const uint8_t* binary, size_t& offset) {
    Instruction instr;
    instr.mnemonic = "RET";
    instr.operands = "";
    instr.length = 1;
    offset += instr.length;
    return instr;
}

Instruction decodePUSH(const uint8_t* binary, size_t& offset) {
    Instruction instr;
    instr.mnemonic = "PUSH";
    instr.operands = "R" + to_string(binary[offset + 1] & 0x7);
    instr.length = 2;
    offset += instr.length;
    return instr;
}

void disassemble(const uint8_t* binary, size_t binary_size) {
    size_t offset = 0;
    while (offset < binary_size) {
        uint8_t opcode = binary[offset];

        Instruction instr;
        switch (opcode) {
            case 0x89:
                instr = decodeMOV(binary, offset);
                break;
            case 0xE9:
                instr = decodeJMP(binary, offset);
                break;
            case 0xFF:
                instr = decodeCALL(binary, offset);
                break;
            case 0xC3:
                instr = decodeRET(binary, offset);
                break;
            case 0x50:
                instr = decodePUSH(binary, offset);
                break;
            default:
                instr.mnemonic = "UNKNOWN";
                instr.operands = "N/A";
                instr.length = 1;
                offset += 1;
                break;
        }

        cout << instr.mnemonic << " " << instr.operands << endl;
    }
}

uint8_t* readBinaryFile(const string& filename, size_t& size) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    size = file.tellg();
    file.seekg(0, std::ios::beg);

    uint8_t* buffer = new uint8_t[size];
    file.read(reinterpret_cast<char*>(buffer), size);

    file.close();

    return buffer;
}

void handleCommandLine(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: disassembler <binary_file>" << endl;
        exit(EXIT_FAILURE);
    }

    string filename = argv[1];
    size_t file_size;
    uint8_t* binary = readBinaryFile(filename, file_size);

    cout << "Disassembling file: " << filename << endl;
    disassemble(binary, file_size);

    delete[] binary;
}

int main(int argc, char* argv[]) {
    handleCommandLine(argc, argv);
    return 0;
}

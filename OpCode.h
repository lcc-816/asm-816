//
//  OpCode.h
//  65calc02
//
//  Created by Kelvin Sherlock on 9/13/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#ifndef __OpCode_h__
#define __OpCode_h__

#include "Instruction.h"
#include "Machine.h"

#include <stdint.h>

#include "const_vector.h"

class OpCode : public Instruction
{
public:    

    static const_vector<OpCode> OpCodes(Machine machine); // 256 entries.
    
    
    OpCode() = default;
    OpCode(const OpCode&) = default;
    //OpCode(OpCode&&) = default;
    
    OpCode(Instruction instr, AddressMode addressMode);
    OpCode(Machine machine, Mnemonic m, AddressMode mode);
    OpCode(Machine machine, uint8_t opcode);
    

    OpCode &operator=(const OpCode &) = default;
    //OpCode &operator=(OpCode &&) = default;
    
#ifdef __OBJC__
    NSString *formatOperand(uint32_t operand, uint16_t address, bool longM = false, bool longX = false) const;
    NSString *formatHex(uint32_t operand, bool longM = false, bool longX = false) const;
    
#endif
    
    uint8_t opcode() const;
    AddressMode addressMode() const;
    unsigned bytes() const;
    unsigned bytes(bool longM, bool longX) const;
    
    unsigned cycles() const; // version for long m/x?
    
    
    static int opcode(Machine machine, Mnemonic mnemonic, AddressMode addressMode);
    static int opcode(Instruction instruction, AddressMode addressMode);
    
    
    bool reads_a() const {
        return _attributes & 0x01;
    }

    bool writes_a() const {
        return _attributes & 0x02;
    }

    bool reads_x() const {
        return _attributes & 0x04;
    }
    
    bool writes_x() const {
        return _attributes & 0x08;
    }
    
    bool reads_y() const {
        return _attributes & 0x10;
    }
    
    bool writes_y() const {
        return _attributes & 0x20;
    }
    
    bool reads_s() const {
        return _attributes & 0x40;
    }
    
    bool writes_s() const {
        return _attributes & 0x80;
    }

    bool reads_d() const {
        return _attributes & 0x100;
    }
    
    bool writes_d() const {
        return _attributes & 0x200;
    }

    bool reads_b() const {
        return _attributes & 0x400;
    }
    
    bool writes_b() const {
        return _attributes & 0x800;
    }

    bool reads_k() const {
        return _attributes & 0x1000;
    }
    
    bool writes_k() const {
        return _attributes & 0x2000;
    }

    bool reads_pc() const {
        return _attributes & 0x4000;
    }
    
    bool writes_pc() const {
        return _attributes & 0x8000;
    }

    bool reads_zp() const {
        return _attributes & 0x10000;
    }
    
    bool writes_zp() const {
        return _attributes & 0x20000;
    }

    
private:
    friend class Instruction;
    
    static class OpCode m6502_opcodes[256];
    static class OpCode m65c02_opcodes[256];
    static class OpCode m65ce02_opcodes[256];
    static class OpCode mw65c02_opcodes[256];
    static class OpCode mr65c02_opcodes[256];
    static class OpCode m65816_opcodes[256];
    
    static void InitHashTable();
    
    OpCode(unsigned OpCode, Machine machine, Mnemonic mnemonic, uint_least32_t addressModes, AddressMode addressMode, unsigned bytes, unsigned cycles, uint32_t attributes = 0);
    
    unsigned _opcode = 0;
    AddressMode _addressMode = kUndefinedAddressMode;
    unsigned _bytes = 0;
    unsigned _cycles = 0;
    uint32_t _attributes = 0;
};


inline OpCode::OpCode(unsigned opcode, Machine machine, Mnemonic mnemonic, uint_least32_t addressModes, AddressMode addressMode, unsigned bytes, unsigned cycles, uint32_t attributes) :
    Instruction(machine, mnemonic, addressModes),
    _opcode(opcode),
    _addressMode(addressMode),
    _bytes(bytes),
    _cycles(cycles),
    _attributes(attributes)
{
}


inline uint8_t OpCode::opcode() const
{
    return _opcode;
}

inline AddressMode OpCode::addressMode() const
{
    return _addressMode;
}

inline unsigned OpCode::bytes() const
{
    return _bytes;
}

inline unsigned OpCode::cycles() const
{
    return _cycles;
}

inline int OpCode::opcode(Instruction instruction, AddressMode addressMode)
{
    return opcode(instruction.machine(), instruction.mnemonic(), addressMode);
}


#endif

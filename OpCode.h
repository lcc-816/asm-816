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
    
    
    OpCode();
    OpCode(const OpCode& OpCode);
    OpCode(Instruction instr, AddressMode addressMode);
    OpCode(Machine machine, Mnemonic m, AddressMode mode);
    OpCode(Machine machine, uint8_t opcode);
    

    
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
    
    
private:
    friend class Instruction;
    
    static class OpCode m6502_opcodes[256];
    static class OpCode m65c02_opcodes[256];
    static class OpCode mw65c02_opcodes[256];
    static class OpCode mr65c02_opcodes[256];
    static class OpCode m65816_opcodes[256];
    
    static void InitHashTable();
    
    OpCode(unsigned OpCode, Machine machine, Mnemonic mnemonic, AddressMode addressMode, unsigned bytes, unsigned cycles);
    
    unsigned _opcode;
    AddressMode _addressMode;
    unsigned _bytes;
    unsigned _cycles;
};


inline OpCode::OpCode(unsigned opcode, Machine machine, Mnemonic mnemonic, AddressMode addressMode, unsigned bytes, unsigned cycles) :
    Instruction(machine, mnemonic, 1 << addressMode),
    _opcode(opcode),
    _addressMode(addressMode),
    _bytes(bytes),
    _cycles(cycles)
{
}


inline OpCode::OpCode() :
    _opcode(0), 
    _addressMode(kUndefinedAddressMode), 
    _bytes(0), 
    _cycles(0)
{
}

inline OpCode::OpCode(const OpCode& opcode)
{
    *this = opcode;
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

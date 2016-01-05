//
//  Mnemonic.h
//  65calc02
//
//  Created by Kelvin Sherlock on 9/5/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#ifndef __Mnemonic_h__
#define __Mnemonic_h__

#include "Machine.h"

#include <string>
#include <stdint.h>

#include "const_vector.h"

class Instruction {
    

    
private:
    Machine _machine = kUndefinedMachine;
    Mnemonic _mnemonic = kUndefinedMnemonic;
    uint_least32_t _addressModes = 0;  
    
public:
    // static methods
    static const_vector<Instruction> Instructions(Machine machine);    
    static bool coerce(Instruction &instruction, AddressMode &addressMode, bool explicitAddress);
    
    // constructors
    Instruction(Machine machine, const std::string &s) : Instruction(machine, s.c_str()) {}
    Instruction() = default;
    Instruction(const Instruction & rhs) = default;
    Instruction(Machine machine, const char *name);
    Instruction(Machine machine, Mnemonic mnemonic);
    
    Instruction &operator=(const Instruction&) = default;

    // -- instance methods
    Machine machine() const;
    Mnemonic mnemonic() const;
    uint_least32_t addressModes() const;
    
    bool hasAddressMode(AddressMode mode) const;
    bool isRelative() const;
    
    
    explicit operator bool() const;
    
    const char *toString() const;
    unsigned bytes(AddressMode addressMode, bool longM = false, bool longXY = false) const;

    
#ifdef __COREFOUNDATION_CFSTRING__
    CFStringRef toCFString() const
    {
        return MnemonicToCFString(_mnemonic);
    }
#endif
    
#ifdef __OBJC__
    NSString *toNSString() const
    {
        return MnemonicToNSString(_mnemonic);
    }
    
    NSString *formatOperand(uint32_t operand, AddressMode addressMode, uint16_t address, bool longM = false, bool longX = false);
#endif
    
private:

    friend class OpCode;
    
    static Instruction m6502_instructions[];
    static Instruction m65c02_instructions[];
    static Instruction m65ce02_instructions[];
    static Instruction mw65c02_instructions[];
    static Instruction mr65c02_instructions[];
    static Instruction m65816_instructions[];

    static Mnemonic m6502_mnemonics[];
    static Mnemonic m65c02_mnemonics[];
    static Mnemonic m65ce02_mnemonics[];
    static Mnemonic mw65c02_mnemonics[];
    static Mnemonic mr65c02_mnemonics[];
    static Mnemonic m65816_mnemonics[];
    
    
    
    Instruction(Machine machine, Mnemonic mnemonic, uint_least32_t addressModes);


};




inline Instruction::Instruction(Machine machine, Mnemonic mnemonic, uint_least32_t addressModes) :
_machine(machine), _mnemonic(mnemonic), _addressModes(addressModes)
{
}


inline Machine Instruction::machine() const
{
    return _machine;
}

inline Mnemonic Instruction::mnemonic() const
{
    return _mnemonic;
}

inline uint_least32_t Instruction::addressModes() const
{
    return _addressModes;
}

inline bool Instruction::hasAddressMode(AddressMode mode) const
{
    return _addressModes & (1 << mode);
}

inline bool Instruction::isRelative() const
{
    return _addressModes & ((1 << relative) | (1 << relative_long));
}


inline Instruction::operator bool() const
{
    return _mnemonic != kUndefinedMnemonic;
}

inline const char *Instruction::toString() const
{ 
    return MnemonicToString(_mnemonic); 
}


#endif
